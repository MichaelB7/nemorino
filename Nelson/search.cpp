#include <stdlib.h>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <math.h>
#include <cstring>
#include <thread>
#include <chrono>
#include "search.h"
#include "hashtables.h"
#include "evaluation.h"
#include "tbprobe.h"

void Search::Reset() {
	while (thread_pool != nullptr && Stop.load() && thread_pool->tasks_active() > 0);
	BestMove.move = MOVE_NONE;
	BestMove.score = VALUE_ZERO;
	NodeCount = 0;
	QNodeCount = 0;
	MaxDepth = 0;
	PonderMode.store(false);
	threadLocalData.History.age();
	searchMoves.clear();
	threadLocalData.cmHistory.age();
	threadLocalData.followupHistory.age();
	threadLocalData.killerManager.clear();
	for (int i = 0; i < PV_MAX_LENGTH; ++i) PVMoves[i] = MOVE_NONE;
}

void Search::NewGame() {
	Reset();
	for (int i = 0; i < 12; ++i) {
		for (int j = 0; j < 64; ++j)counterMove[i][j] = MOVE_NONE;
	}
	threadLocalData.cmHistory.initialize();
	threadLocalData.History.initialize();
	threadLocalData.followupHistory.initialize();
}

std::string Search::PrincipalVariation(Position & pos, int depth) {
	std::stringstream ss;
	int i = 0;
	ponderMove = MOVE_NONE;
	//First get PV from PV array...
	for (; i < depth && i < PV_MAX_LENGTH; ++i) {
		if (PVMoves[i] == MOVE_NONE || !pos.validateMove(PVMoves[i])) break;
		Position next(pos);
		if (!next.ApplyMove(PVMoves[i])) break;
		pos = next;
		if (i > 0) ss << " ";
		ss << toString(PVMoves[i]);
		if (i == 1) ponderMove = PVMoves[i];
	}
	//...then continue with moves from transposition table
	for (; i < depth; ++i) {
		Move hashmove = settings::parameter.HelperThreads == 0 ? tt::hashmove<tt::UNSAFE>(pos.GetHash()) : tt::hashmove<tt::THREAD_SAFE>(pos.GetHash());
		if (hashmove == MOVE_NONE || !pos.validateMove(hashmove)) break;
		Position next(pos);
		if (!next.ApplyMove(hashmove)) break;
		pos = next;
		if (i > 0) ss << " ";
		ss << toString(hashmove);
		if (i == 1) ponderMove = hashmove;
	}
	return ss.str();
}

//Creates the "thinking output" while running in UCI or XBoard mode
void Search::info(Position & pos, int pvIndx, SearchResultType srt) {
	if (UciOutput) {
		Position npos(pos);
		npos.copy(pos);
		std::string srtString;
		if (srt == SearchResultType::FAIL_LOW) srtString = " upperbound"; else if (srt == SearchResultType::FAIL_HIGH) srtString = " lowerbound";
		uint64_t effectiveTBHits = tbHits > 1 ? tbHits * (settings::parameter.HelperThreads + 1) : tbHits;
		if (abs(int(BestMove.score)) <= int(VALUE_MATE_THRESHOLD))
			sync_cout << "info depth " << _depth << " seldepth " << std::max(MaxDepth, _depth) << " multipv " << pvIndx + 1 << " score cp " << (int)BestMove.score << srtString << " nodes " << (settings::parameter.HelperThreads + 1) * NodeCount
			<< " nps " << (settings::parameter.HelperThreads + 1) * NodeCount * 1000 / _thinkTime << " hashfull " << tt::GetHashFull()
			<< " tbhits " << effectiveTBHits
			<< " time " << _thinkTime
			<< " pv " << PrincipalVariation(npos, _depth) << sync_endl;
		else {
			int pliesToMate;
			if (int(BestMove.score) > 0) pliesToMate = VALUE_MATE - BestMove.score + 1; else pliesToMate = -BestMove.score - VALUE_MATE;
			sync_cout << "info depth " << _depth << " seldepth " << std::max(MaxDepth, _depth) << " multipv " << pvIndx + 1 << " score mate " << pliesToMate / 2 << srtString << " nodes " << (settings::parameter.HelperThreads + 1) * NodeCount
				<< " nps " << (settings::parameter.HelperThreads + 1) * NodeCount * 1000 / _thinkTime << " hashfull " << tt::GetHashFull()
				<< " tbhits " << effectiveTBHits
				<< " time " << _thinkTime
				<< " pv " << PrincipalVariation(npos, _depth) << sync_endl;
		}
	}
}

void Search::debugInfo(std::string info)
{
	if (UciOutput) sync_cout << "info string " << info << sync_endl;
}

Move Search::GetBestBookMove(Position & pos, ValuatedMove * moves, int moveCount) {
	if (settings::options.getBool(settings::OPTION_OWN_BOOK) && BookFile != nullptr) {
		if (book == nullptr) book = new polyglot::Book(*BookFile);
		book->probe(pos, true, moves, moveCount);
	}
	return MOVE_NONE;
}

Search::Search() {
	for (int i = 0; i < 12; ++i)
		for (int j = 0; j < 64; ++j)
			counterMove[i][j] = MOVE_NONE;
	BestMove.move = MOVE_NONE;
	BestMove.score = VALUE_NOTYETDETERMINED;
	threadLocalData.cmHistory.initialize();
	threadLocalData.followupHistory.initialize();
	threadLocalData.History.initialize();
	_thinkTime = 0;
	for (int i = 0; i < PV_MAX_LENGTH; ++i) PVMoves[i] = MOVE_NONE;
}

Search::~Search() {
	if (book != nullptr) {
		delete book;
		book = nullptr;
	}
	if (BookFile != nullptr) {
		delete BookFile;
		BookFile = nullptr;
	}
	if (XAnalysisOutput != nullptr) {
		delete XAnalysisOutput;
		XAnalysisOutput = nullptr;
	}
}

ValuatedMove Search::Think(Position & pos) {
	std::lock_guard<std::mutex> lgStart(mtxSearch);
	//slave threads
	std::vector<std::thread> subThreads;
	//Initialize Engine before starting the new search
	_thinkTime = 1; //to avoid divide by 0 errors
	ponderMove = MOVE_NONE;
	Value score = VALUE_ZERO;
	ValuatedMove lastBestMove = VALUATED_MOVE_NONE;
	rootPosition = pos;
	rootPosition.ResetPliesFromRoot();
	settings::parameter.EngineSide = rootPosition.GetSideToMove();
	tt::newSearch();
	//Get all root moves
	ValuatedMove* generatedMoves = rootPosition.GenerateMoves<LEGAL>();
	rootMoveCount = rootPosition.GeneratedMoveCount();
	if (rootMoveCount == 0) {
		BestMove.move = MOVE_NONE;
		BestMove.score = VALUE_ZERO;
		info(rootPosition, 0, SearchResultType::UNIQUE_MOVE);
		utils::debugInfo("No valid move!");
		return BestMove;
	}
	if (rootMoveCount == 1) {
		BestMove = *generatedMoves; //if there is only one legal move save time and return move immediately (although there is no score assigned)
		utils::debugInfo("Only one valid move!");
		goto END;
	}
	//check if book is available
	if (settings::options.getBool(settings::OPTION_OWN_BOOK) && BookFile != nullptr) {
		if (book == nullptr) book = new polyglot::Book(*BookFile);
		//Currently engine isn't looking for the best book move, but selects on of the available bookmoves by random, with entries weight used to define the
		//probability 
		Move bookMove = book->probe(rootPosition, false, generatedMoves, rootMoveCount);
		if (bookMove != MOVE_NONE) {
			BestMove.move = bookMove;
			BestMove.score = VALUE_ZERO;
			utils::debugInfo("Book move");
			//Try to find a suitable move for pondering
			Position next(rootPosition);
			if (next.ApplyMove(bookMove)) {
				ValuatedMove* replies = next.GenerateMoves<LEGAL>();
				ponderMove = book->probe(next, true, replies, next.GeneratedMoveCount());
				if (ponderMove == MOVE_NONE && next.GeneratedMoveCount() > 0) ponderMove = replies->move;
			}
			info(rootPosition, 0, SearchResultType::BOOK_MOVE);
			goto END;
		}
	}
	//If a search move list is provided replace root moves by search moves
	if (searchMoves.size()) {
		rootMoveCount = int(searchMoves.size());
		for (int i = 0; i < rootMoveCount; ++i) rootMoves[i].move = searchMoves[i];
		searchMoves.clear();
	}
	else {
		memcpy(rootMoves, generatedMoves, rootMoveCount * sizeof(ValuatedMove));
	}
	//Root probing of tablebases. This is done as suggested by SF: Keep only the winning, resp. drawing, moves in the move list
	//and then search normally. This way the engine will play "better" than by simply choosing the "best" tablebase move (which is
	//the move which minimizes the number until drawPlyCount is reset without changing the result
	tbHits = 0;
	probeTB = tablebases::MaxCardinality > 0;
	if (rootPosition.GetMaterialTableEntry()->IsTablebaseEntry()) {
		probeTB = false;
		tablebases::RootMoves tbMoves;
		for (int i = 0; i < rootMoveCount; ++i) tbMoves.emplace_back(rootMoves[i].move);
		if (tablebases::rank_root_moves(rootPosition, tbMoves)) {
			++tbHits;
			for (int i = 0; i < rootMoveCount; ++i) {
				rootMoves[i].move = tbMoves[i].pv[0];
				rootMoves[i].score = tbMoves[i].tbScore;
				BestMove = rootMoves[0]; //if tablebase probe only returns one move => play it and done!
				info(rootPosition, 0, SearchResultType::TABLEBASE_MOVE);
				//utils::debugInfo("Tablebase move", toString(BestMove.move));
				goto END;
			}
		}
	}
	SetRootMoveBoni();
	Contempt = rootPosition.GetSideToMove() == WHITE ? Eval(settings::parameter.Contempt, settings::parameter.Contempt / 2)
		: Eval(-settings::parameter.Contempt, -settings::parameter.Contempt / 2);
	//Initialize PV-Array
	std::fill_n(PVMoves, PV_MAX_LENGTH, MOVE_NONE);
	//Special logic to get static evaluation via UCI: if go depth 0 is requested simply return static evaluation
	if (timeManager.GetMaxDepth() == 0) {
		BestMove.move = MOVE_NONE;
		BestMove.score = rootPosition.evaluate();
		if (abs(int(BestMove.score)) <= int(VALUE_MATE_THRESHOLD)) sync_cout << "info score cp " << (int)BestMove.score << sync_endl;
		else {
			int pliesToMate;
			if (int(BestMove.score) > 0) pliesToMate = VALUE_MATE - BestMove.score; else pliesToMate = -BestMove.score - VALUE_MATE;
			sync_cout << "info score mate " << pliesToMate / 2 << sync_endl;
		}
		return BestMove;
	}
	Stop.store(false);
	if (settings::parameter.HelperThreads) {
		if (thread_pool == nullptr) thread_pool = new ThreadPool(settings::parameter.HelperThreads);
		else if (static_cast<int>(thread_pool->size()) != settings::parameter.HelperThreads) {
			delete thread_pool;
			thread_pool = new ThreadPool(settings::parameter.HelperThreads);
		}
		for (int i = 0; i < settings::parameter.HelperThreads; ++i)
			thread_pool->enqueue(std::bind(&Search::startHelper, this, std::placeholders::_1));
	}
	threadLocalData.id = 0;
	//Iterativ Deepening Loop
	for (_depth = 1; _depth < timeManager.GetMaxDepth(); ++_depth) {
		Value alpha, beta, delta = Value(20);
		for (int pvIndx = 0; pvIndx < MultiPv && pvIndx < rootMoveCount; ++pvIndx) {
			if (_depth >= 5 && MultiPv == 1 && std::abs(int16_t(score)) < VALUE_KNOWN_WIN) {
				//set aspiration window
				alpha = std::max(score - delta, -VALUE_INFINITE);
				beta = std::min(score + delta, VALUE_INFINITE);
			}
			else {
				alpha = -VALUE_MATE;
				beta = VALUE_MATE;
			}
			while (true) {
				CHECK(rootPosition.GetPliesFromRoot() == 0)
					if (settings::parameter.HelperThreads > 0)
						score = SearchRoot<ThreadType::MASTER>(alpha, beta, rootPosition, _depth, rootMoves, PVMoves, threadLocalData, pvIndx);
					else
						score = SearchRoot<ThreadType::SINGLE>(alpha, beta, rootPosition, _depth, rootMoves, PVMoves, threadLocalData, pvIndx);
				CHECK(rootPosition.GetPliesFromRoot() == 0)
					//Best move is already in first place, this is assured by SearchRoot
					//therefore we sort only the other moves
					std::stable_sort(rootMoves + pvIndx + 1, &rootMoves[rootMoveCount], sortByScore);
				if (Stopped()) {
					break;
				}
				if (score <= alpha) {
					//fail-low
					beta = (alpha + beta) / 2;
					alpha = std::max(score - delta, -VALUE_INFINITE);
					info(rootPosition, pvIndx, SearchResultType::FAIL_LOW);
					//inform timemanager to assigne more time
					if (!PonderMode.load()) timeManager.reportFailLow();
				}
				else if (score >= beta && rootMoves[pvIndx].move == lastBestMove.move) {
					//Iteration completed
					BestMove = rootMoves[pvIndx];
					score = BestMove.score;
					break;
				}
				else if (score >= beta) {
					//fail-high
					alpha = (alpha + beta) / 2;
					beta = std::min(score + delta, VALUE_INFINITE);
					info(rootPosition, pvIndx, SearchResultType::FAIL_HIGH);
				}
				else {
					//Iteration completed
					BestMove = rootMoves[pvIndx];
					score = BestMove.score;
					break;
				}
				if (std::abs(int16_t(score)) >= VALUE_KNOWN_WIN) {
					alpha = -VALUE_MATE;
					beta = VALUE_MATE;
				}
				else delta += delta * 2;
			}
			Time_t tNow = now();
			_thinkTime = std::max(tNow - timeManager.GetStartTime(), int64_t(1));
			if (!Stopped()) {
				//check if new deeper iteration shall be started
				if (!timeManager.ContinueSearch(_depth, BestMove, NodeCount, tNow, PonderMode)) {
					Stop.store(true);
				}
			}
			else debugInfo("Iteration cancelled!");
			//send information to GUI
			info(rootPosition, pvIndx);
			if (Stopped()) break;
		}
		if (Stopped()) break;
		lastBestMove = BestMove;
	}
	Stop.store(true);
END://when pondering engine must not return a best move before opponent moved => therefore let main thread wait	
	bool infoSent = false;
	while (PonderMode.load()) {
		if (!infoSent) utils::debugInfo("Waiting for opponent..");
		infoSent = true;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	if (PVMoves[0] != MOVE_NONE && PVMoves[0] != BestMove.move) {
		std::stringstream ss;
		ss << "PV Move: " << toString(PVMoves[0]) << " Bestmove: " << toString(BestMove.move);
		utils::debugInfo(ss.str());
		BestMove.move = PVMoves[0];
	}
	//If for some reason search did not find a best move return the  first one (to avoid loss and it's anyway the best guess then)
	if (BestMove.move == MOVE_NONE) BestMove = rootMoves[0];
	return BestMove;
}

//slave thread
void Search::startHelper(int id) {
#ifdef _DEBUG
	sync_cout << "Helper task " << id << " started" << sync_endl;
#endif // _DEBUG
	WinProcGroup::bindThisThread(id);
	int depth = 1;
	Move* PVMovesLocal = new Move[PV_MAX_LENGTH];
	ValuatedMove lastBestMove = VALUATED_MOVE_NONE;
	ValuatedMove* moves = new ValuatedMove[MAX_MOVE_COUNT];
	memcpy(moves, rootMoves, MAX_MOVE_COUNT * sizeof(ValuatedMove));
	ThreadData* h = new ThreadData;
	h->id = id;
	//Iterative Deepening Loop
	Value score = VALUE_ZERO;
	while (!Stop.load() && depth < MAX_DEPTH) {
		Value alpha, beta, delta = Value(20);
		if (depth >= 5) {
			//set aspiration window
			alpha = std::max(score - delta, -VALUE_INFINITE);
			beta = std::min(score + delta, VALUE_INFINITE);
		}
		else {
			alpha = -VALUE_MATE;
			beta = VALUE_MATE;
		}
		while (true && !Stop.load()) {
			CHECK(rootPosition.GetPliesFromRoot() == 0)
				score = SearchRoot<ThreadType::SLAVE>(alpha, beta, rootPosition, depth, moves, PVMovesLocal, *h);
			CHECK(rootPosition.GetPliesFromRoot() == 0)
				if (score <= alpha) {
					//fail-low
					beta = (alpha + beta) / 2;
					alpha = std::max(score - delta, -VALUE_INFINITE);
				}
				else if (score >= beta && moves[0].move == lastBestMove.move) {
					//Iteration completed
					score = moves[0].score;
					break;
				}
				else if (score >= beta) {
					//fail-high
					alpha = (alpha + beta) / 2;
					beta = std::min(score + delta, VALUE_INFINITE);
				}
				else {
					//Iteration completed
					score = moves[0].score;
					break;
				}
			if (std::abs(int16_t(score)) >= VALUE_KNOWN_WIN) {
				alpha = -VALUE_MATE;
				beta = VALUE_MATE;
			}
			else delta += delta * 2;
		}
		lastBestMove = moves[0];
		++depth;
	}
	delete h;
	delete[] moves;
	delete[] PVMovesLocal;
#ifdef _DEBUG
	sync_cout << "Helper task " << id << " done" << sync_endl;
#endif // _DEBUG
}

bool Search::isQuiet(Position & pos) {
	Value evaluationDiff = pos.GetStaticEval() - QSearch<ThreadType::SINGLE>(-VALUE_MATE, VALUE_MATE, pos, 0, threadLocalData);
	return std::abs(int16_t(evaluationDiff)) <= 30;
}

Value Search::qscore(Position * pos)
{
	return QSearch<ThreadType::SINGLE>(-VALUE_MATE, VALUE_MATE, *pos, 0, threadLocalData);
}

void Search::SetRootMoveBoni()
{
	rootMoveBoni.clear();
	for (auto rootMove : rootMoves) {
		rootMoveBoni.insert({ rootMove.move, VALUE_ZERO });
		if (type(rootMove.move) == MoveType::PROMOTION) rootMoveBoni[rootMove.move] = Value(-10 * (int)promotionType(rootMove.move));
		//else if (type(rootMove.move) == MoveType::CASTLING) rootMoveBoni[rootMove.move] = Value(20);
		//else if ((from(rootMove.move) >> 3) == 7 * (int)rootPosition.GetSideToMove()) {
		//	PieceType pt = GetPieceType(rootPosition.GetPieceOnSquare(from(rootMove.move)));
		//	if (pt == PieceType::BISHOP || pt == PieceType::KNIGHT) {
		//		//check if piece has been developed before
		//		bool devMove = true;
		//		Position * pos = &rootPosition;
		//		while (devMove && pos->GetLastAppliedMove()) {
		//			devMove = from(pos->GetLastAppliedMove()) != from(rootMove.move);
		//			pos = pos->Previous();
		//		}
		//		if (devMove) rootMoveBoni[rootMove.move] = Value(10);
		//	}
		//}

	}
}

void Search::updateCutoffStats(ThreadData & tlData, const Move cutoffMove, int depth, Position & pos, int moveIndex) {
	if (moveIndex == -1 || pos.IsQuiet(cutoffMove)) {
		Piece movingPiece = pos.GetPieceOnSquare(from(cutoffMove));
		Square toSquare = to(cutoffMove);
		if (moveIndex >= 0) {
			tlData.killerManager.store(pos, cutoffMove);
		}
		Value v = Value(depth * depth);
		tlData.History.update(-depth * tlData.History.getValue(movingPiece, cutoffMove) / 64, movingPiece, cutoffMove);
		tlData.History.update(v, movingPiece, cutoffMove);
		Piece prevPiece = BLANK;
		Square prevTo = OUTSIDE;
		Piece prev2Piece = BLANK;
		Square prev2To = OUTSIDE;
		//Piece ownPrevPiece = BLANK;
		//Square ownPrevTo = OUTSIDE;
		Move lastApplied;
		if ((lastApplied = FixCastlingMove(pos.GetLastAppliedMove())) != MOVE_NONE) {
			prevTo = to(lastApplied);
			prevPiece = pos.GetPieceOnSquare(prevTo);
			counterMove[int(pos.GetPieceOnSquare(prevTo))][prevTo] = cutoffMove;
			tlData.cmHistory.update(-depth * tlData.cmHistory.getValue(prevPiece, prevTo, movingPiece, toSquare) / 64, prevPiece, prevTo, movingPiece, toSquare);
			tlData.cmHistory.update(v, prevPiece, prevTo, movingPiece, toSquare);
			Move lastApplied2;
			if (pos.Previous() && (lastApplied2 = FixCastlingMove(pos.Previous()->GetLastAppliedMove())) != MOVE_NONE) {
				prev2To = to(lastApplied2);
				prev2Piece = pos.Previous()->GetPieceOnSquare(prev2To);
				tlData.followupHistory.update(-depth * tlData.followupHistory.getValue(prev2Piece, prev2To, movingPiece, toSquare) / 64, prev2Piece, prev2To, movingPiece, toSquare);
				tlData.followupHistory.update(v, prev2Piece, prev2To, movingPiece, toSquare);
			}
		}
		if (moveIndex > 0) {
			int moveCount;
			ValuatedMove* alreadyProcessedQuiets = pos.GetMoves(moveCount);
			moveCount = std::min(moveIndex, moveCount);
			for (int i = 0; i < moveCount; ++i) {
				if (alreadyProcessedQuiets->move != cutoffMove && pos.IsQuiet(alreadyProcessedQuiets->move)) {
					Move alreadyProcessedMove = FixCastlingMove(alreadyProcessedQuiets->move);
					movingPiece = pos.GetPieceOnSquare(from(alreadyProcessedMove));
					toSquare = to(alreadyProcessedMove);
					tlData.History.update(-depth * tlData.History.getValue(movingPiece, alreadyProcessedMove) / 64, movingPiece, alreadyProcessedMove);
					tlData.History.update(-v, movingPiece, alreadyProcessedMove);
					if (pos.GetLastAppliedMove() != MOVE_NONE) {
						tlData.cmHistory.update(-depth * tlData.cmHistory.getValue(prevPiece, prevTo, movingPiece, toSquare) / 64, prevPiece, prevTo, movingPiece, toSquare);
						tlData.cmHistory.update(-v, prevPiece, prevTo, movingPiece, toSquare);
					}
					if (prev2To != Square::OUTSIDE) {
						tlData.followupHistory.update(-depth * tlData.followupHistory.getValue(prev2Piece, prev2To, movingPiece, toSquare) / 64, prev2Piece, prev2To, movingPiece, toSquare);
						tlData.followupHistory.update(-v, prev2Piece, prev2To, movingPiece, toSquare);
					}
				}
				//if (ownPrevTo != OUTSIDE) {
				//	cmHistory.update(-v, ownPrevPiece, ownPrevTo, movingPiece, toSquare);
				//}
				alreadyProcessedQuiets++;
			}
		}
	}
}

ThreadPool::ThreadPool(size_t numberOfThreads)
{
	start(numberOfThreads);
}

ThreadPool::~ThreadPool()
{
	stop();
}

void ThreadPool::enqueue(Task t)
{
	{
		std::unique_lock<std::mutex> lock(mtxStartTask);
		tasks.emplace(t);
	}
	cvStartTask.notify_one();
}

void ThreadPool::start(size_t numberOfThreads)
{
	for (auto i = 0u; i < numberOfThreads; ++i) {
		threads.emplace_back([=] {
			while (true) {
				Task task;
				{
					std::unique_lock<std::mutex> lock(mtxStartTask);
					cvStartTask.wait(lock, [=] { return shutdown || !tasks.empty(); });

					if (shutdown) break;
					task = std::move(tasks.front());
					tasks.pop();
				}
				active.fetch_add(1);
				task(i + 1);
				active.fetch_sub(1);
			}
			});
	}
}

void ThreadPool::stop() noexcept
{
	{
		std::unique_lock<std::mutex> lock(mtxStartTask);
		shutdown = true;
	}
	cvStartTask.notify_all();

	for (auto& th : threads) th.join();
	threads.clear();
}
