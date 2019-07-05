#include <sstream>
#include <string>
#include <cstring>
#include <iomanip>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <ctype.h>
#include <regex>
#include <cstddef>
#include "position.h"
#include "material.h"
#include "settings.h"
#include "hashtables.h"
#include "evaluation.h"
#include "tbprobe.h"

static const std::string PieceToChar("QqRrBbNnPpKk ");

Position::Position()
{
	setFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

Position::Position(std::string fen)
{
	setFromFEN(fen);
}

Position::Position(Position &pos) {
	std::memcpy(this, &pos, offsetof(Position, previous));
	material = pos.GetMaterialTableEntry();
	pawn = pos.GetPawnEntry();
	previous = &pos;
}

Position::~Position()
{
}

void Position::copy(const Position &pos) {
	memcpy(this->attacks, pos.attacks, 64 * sizeof(Bitboard));
	memcpy(this->attacksByPt, pos.attacksByPt, 12 * sizeof(Bitboard));
	this->attackedByUs = pos.attackedByUs;
	this->attackedByThem = pos.attackedByThem;
	this->lastAppliedMove = pos.lastAppliedMove;
	this->StaticEval = pos.StaticEval;
	this->bbPinned[Color::WHITE] = pos.bbPinned[Color::WHITE];
	this->bbPinned[Color::BLACK] = pos.bbPinned[Color::BLACK];
	this->bbPinner[Color::WHITE] = pos.bbPinner[Color::WHITE];
	this->bbPinner[Color::BLACK] = pos.bbPinner[Color::BLACK];
}

int Position::AppliedMovesBeforeRoot = 0;

bool Position::ApplyMove(Move move) {
	pliesFromRoot++;
	Square fromSquare = from(move);
	Square toSquare = to(move);
	Piece moving = Board[fromSquare];
	capturedInLastMove = Board[toSquare];
	remove(fromSquare);
	MoveType moveType = type(move);
	Piece convertedTo;
	switch (moveType) {
	case NORMAL:
		set<false>(moving, toSquare);
		//update epField
		if ((GetPieceType(moving) == PAWN)
			&& (abs(int(toSquare) - int(fromSquare)) == 16)
			&& (GetEPAttackersForToField(toSquare) & PieceBB(PAWN, Color(SideToMove ^ 1))))
		{
			SetEPSquare(Square(toSquare - PawnStep()));
		}
		else
			SetEPSquare(OUTSIDE);
		if (GetPieceType(moving) == PAWN || capturedInLastMove != BLANK) {
			DrawPlyCount = 0;
			if (GetPieceType(moving) == PAWN) {
				PawnKey ^= (ZobristKeys[moving][fromSquare]);
				PawnKey ^= (ZobristKeys[moving][toSquare]);
			}
			if (GetPieceType(capturedInLastMove) == PAWN) {
				PawnKey ^= ZobristKeys[capturedInLastMove][toSquare];
			}
		}
		else
			++DrawPlyCount;
		//update castlings
		updateCastleFlags(fromSquare, toSquare);
		//Adjust material key
		if (capturedInLastMove != BLANK) {
			if (MaterialKey == MATERIAL_KEY_UNUSUAL) {
				if (checkMaterialIsUnusual()) {
					material->Evaluation = calculateMaterialEval(*this);
					material->SetIsTableBaseEntry(popcount(OccupiedByColor[WHITE] | OccupiedByColor[BLACK]) <= tablebases::MaxCardinality);
				}
				else MaterialKey = calculateMaterialKey();
			}
			else MaterialKey -= materialKeyFactors[capturedInLastMove];
			material = probe(MaterialKey);
		}
		if (kingSquares[SideToMove] == fromSquare) kingSquares[SideToMove] = toSquare;
		break;
	case ENPASSANT:
		set<true>(moving, toSquare);
		remove(Square(toSquare - PawnStep()));
		capturedInLastMove = Piece(BPAWN - SideToMove);
		if (MaterialKey == MATERIAL_KEY_UNUSUAL) {
			material->Evaluation = calculateMaterialEval(*this);
		}
		else {
			MaterialKey -= materialKeyFactors[BPAWN - SideToMove];
			material = probe(MaterialKey);
		}
		SetEPSquare(OUTSIDE);
		DrawPlyCount = 0;
		PawnKey ^= ZobristKeys[moving][fromSquare];
		PawnKey ^= ZobristKeys[moving][toSquare];
		PawnKey ^= ZobristKeys[BPAWN - SideToMove][toSquare - PawnStep()];
		break;
	case PROMOTION:
		convertedTo = GetPiece(promotionType(move), SideToMove);
		set<false>(convertedTo, toSquare);
		updateCastleFlags(fromSquare, toSquare);
		SetEPSquare(OUTSIDE);
		DrawPlyCount = 0;
		PawnKey ^= ZobristKeys[moving][fromSquare];
		//Adjust Material Key
		if (checkMaterialIsUnusual()) {
			MaterialKey = MATERIAL_KEY_UNUSUAL;
			material = initUnusual(*this);
		}
		else {
			if (MaterialKey == MATERIAL_KEY_UNUSUAL) MaterialKey = calculateMaterialKey();
			else
				MaterialKey = MaterialKey - materialKeyFactors[WPAWN + SideToMove] - materialKeyFactors[capturedInLastMove] + materialKeyFactors[convertedTo];
			material = probe(MaterialKey);
			material->SetIsTableBaseEntry(popcount(OccupiedByColor[WHITE] | OccupiedByColor[BLACK]) <= tablebases::MaxCardinality);
		}
		break;
	case CASTLING:
		if (toSquare == G1 + (SideToMove * 56) || (toSquare == InitialRookSquare[2 * SideToMove])) {
			//short castling
			remove(InitialRookSquare[2 * SideToMove]); //remove rook
			toSquare = Square(G1 + (SideToMove * 56));
			set<true>(moving, toSquare); //Place king
			set<true>(Piece(WROOK + SideToMove),
				Square(toSquare - 1)); //Place rook
		}
		else {
			//long castling
			remove(InitialRookSquare[2 * SideToMove + 1]); //remove rook
			toSquare = Square(C1 + (SideToMove * 56));
			set<true>(moving, toSquare); //Place king
			set<true>(Piece(WROOK + SideToMove),
				Square(toSquare + 1)); //Place rook
		}
		RemoveCastlingOption(CastleFlag(W0_0 << (2 * SideToMove)));
		RemoveCastlingOption(CastleFlag(W0_0_0 << (2 * SideToMove)));
		SetEPSquare(OUTSIDE);
		++DrawPlyCount;
		kingSquares[SideToMove] = toSquare;
		break;
	}
	assert(kingSquares[WHITE] == lsb(PieceBB(KING, WHITE)));
	assert(kingSquares[BLACK] == lsb(PieceBB(KING, BLACK)));
	tt::prefetch(Hash); //for null move
	SwitchSideToMove();
	tt::prefetch(Hash);
	movepointer = 0;
	attackedByUs = calculateAttacks(SideToMove);
	//calculatePinned();
	attackedByThem = calculateAttacks(Color(SideToMove ^ 1));
	//assert((checkMaterialIsUnusual() && MaterialKey == MATERIAL_KEY_UNUSUAL) || MaterialKey == calculateMaterialKey());
	//assert(PawnKey == calculatePawnKey());
	CalculatePinnedPieces();
	if (pawn->Key != PawnKey) pawn = pawn::probe(*this);
	lastAppliedMove = move;
	if (material->IsTheoreticalDraw()) result = Result::DRAW;
	return !(attackedByUs & PieceBB(KING, Color(SideToMove ^ 1)));
	//if (attackedByUs & PieceBB(KING, Color(SideToMove ^ 1))) return false;
	//attackedByThem = calculateAttacks(Color(SideToMove ^1));
	//return true;
}

void Position::AddUnderPromotions() {
	if (canPromote) {
		movepointer--;
		int moveCount = movepointer;
		for (int i = 0; i < moveCount; ++i) {
			Move pmove = moves[i].move;
			if (type(pmove) == PROMOTION) {
				AddMove(createMove<PROMOTION>(from(pmove), to(pmove), KNIGHT));
				AddMove(createMove<PROMOTION>(from(pmove), to(pmove), ROOK));
				AddMove(createMove<PROMOTION>(from(pmove), to(pmove), BISHOP));
			}
		}
		AddNullMove();
	}
}

Move Position::NextMove() {
	if (generationPhases[generationPhase] == NONE) return MOVE_NONE;
	Move move;
	do {
		processedMoveGenerationPhases |= 1 << (int)generationPhases[generationPhase];
		if (moveIterationPointer < 0) {
			phaseStartIndex = movepointer - (movepointer != 0);
			switch (generationPhases[generationPhase]) {
			case KILLER:
				moveIterationPointer = 0;
				break;
			case NON_LOOSING_CAPTURES:
				GenerateMoves<NON_LOOSING_CAPTURES>();
				evaluateByCaptureScore();
				moveIterationPointer = 0;
				break;
			case LOOSING_CAPTURES:
				GenerateMoves<LOOSING_CAPTURES>();
				evaluateByCaptureScore(phaseStartIndex);
				moveIterationPointer = 0;
				break;
				//case QUIETS:
				//	GenerateMoves<QUIETS>();
				//	evaluateByHistory(phaseStartIndex);
				//	shellSort(moves + phaseStartIndex, movepointer - phaseStartIndex - 1);
				//	moveIterationPointer = 0;
				//	break;
			case QUIETS_POSITIVE:
				GenerateMoves<QUIETS>();
				evaluateByHistory(phaseStartIndex);
				firstNegative = std::partition(moves + phaseStartIndex, &moves[movepointer - 1], positiveScore);
				insertionSort(moves + phaseStartIndex, firstNegative);
				moveIterationPointer = 0;
				break;
			case CHECK_EVASION:
				GenerateMoves<CHECK_EVASION>();
				evaluateCheckEvasions(phaseStartIndex);
				moveIterationPointer = 0;
				break;
			case QUIET_CHECKS:
				GenerateMoves<QUIET_CHECKS>();
				//evaluateBySEE(phaseStartIndex);
				//insertionSort(moves + phaseStartIndex, moves + (movepointer - 1));
				moveIterationPointer = 0;
				break;
			case UNDERPROMOTION:
				if (canPromote) {
					movepointer--;
					int moveCount = movepointer;
					for (int i = 0; i < moveCount; ++i) {
						Move pmove = moves[i].move;
						if (type(pmove) == PROMOTION) {
							AddMove(createMove<PROMOTION>(from(pmove), to(pmove), KNIGHT));
							AddMove(createMove<PROMOTION>(from(pmove), to(pmove), ROOK));
							AddMove(createMove<PROMOTION>(from(pmove), to(pmove), BISHOP));
						}
					}
					AddNullMove();
					phaseStartIndex = moveCount;
					moveIterationPointer = 0;
					break;
				}
				else return MOVE_NONE;
			case FORKS:
				GenerateForks(true);
				moveIterationPointer = 0;
				break;
			case FORKS_NO_CHECKS:
				GenerateForks(false);
				moveIterationPointer = 0;
				break;
			case ALL:
				GenerateMoves<ALL>();
				moveIterationPointer = 0;
				break;
			default:
				break;
			}

		}
		switch (generationPhases[generationPhase]) {
		case HASHMOVE:
			++generationPhase;
			moveIterationPointer = -1;
			if (validateMove(hashMove)) return hashMove;
			break;
		case KILLER:
			while (killerManager && moveIterationPointer < killer::NB_KILLER) {
				Move killerMove = killerManager->getMove(*this, moveIterationPointer);
				++moveIterationPointer;
				//if (killerMove != MOVE_NONE && validateMove(killerMove))  return killerMove;
				if (killerMove != MOVE_NONE && killerMove != hashMove && validateMove(killerMove)) return killerMove;
			}
			++generationPhase;
			moveIterationPointer = -1;
			break;
		case NON_LOOSING_CAPTURES:
			move = getBestMove(phaseStartIndex + moveIterationPointer);
			if (move) {
				++moveIterationPointer;
				goto end_post_hash;
			}
			else {
				++generationPhase;
				moveIterationPointer = -1;
			}
			break;
		case LOOSING_CAPTURES: case QUIETS_NEGATIVE:
			move = getBestMove(phaseStartIndex + moveIterationPointer);
			if (move) {
				++moveIterationPointer;
				goto end_post_killer;
			}
			else {
				++generationPhase;
				moveIterationPointer = -1;
			}
			break;
			//case QUIETS:
			//	move = moves[phaseStartIndex + moveIterationPointer].move;
			//	if (move) {
			//		++moveIterationPointer;
			//		goto end_post_killer;
			//	}
			//	else {
			//		++generationPhase;
			//		moveIterationPointer = -1;
			//	}
			//	break;
		case CHECK_EVASION: case QUIET_CHECKS: case FORKS: case FORKS_NO_CHECKS:
#pragma warning(suppress: 6385)
			move = moves[phaseStartIndex + moveIterationPointer].move;
			if (move) {
				++moveIterationPointer;
				goto end_post_hash;
			}
			else {
				++generationPhase;
				moveIterationPointer = -1;
			}
			break;
		case QUIETS_POSITIVE:
			move = moves[phaseStartIndex + moveIterationPointer].move;
			if (move == firstNegative->move) {
				++generationPhase;
			}
			else {
				++moveIterationPointer;
				goto end_post_killer;
			}
			break;
		case REPEAT_ALL: case ALL:
#pragma warning(suppress: 6385)
			move = moves[moveIterationPointer].move;
			++moveIterationPointer;
			generationPhase += (moveIterationPointer >= movepointer);
			goto end;
		case UNDERPROMOTION:
#pragma warning(suppress: 6385)
			move = moves[phaseStartIndex + moveIterationPointer].move;
			++moveIterationPointer;
			generationPhase += (phaseStartIndex + moveIterationPointer >= movepointer);
			goto end_post_hash;
		default:
			assert(true);
		}
	} while (generationPhases[generationPhase] != NONE);
	return MOVE_NONE;
end_post_killer:
	if (killerManager != nullptr && (processedMoveGenerationPhases & (1 << (int)MoveGenerationType::KILLER)) != 0) {
		if (killerManager->isKiller(*this, move)) return NextMove();
	}
end_post_hash:
	if (hashMove && move == hashMove) return NextMove();
end:
	return move;
}

//Insertion Sort (copied from SF)
void Position::insertionSort(ValuatedMove* first, ValuatedMove* last)
{
	ValuatedMove tmp, *p, *q;
	for (p = first + 1; p < last; ++p)
	{
		tmp = *p;
		for (q = p; q != first && *(q - 1) < tmp; --q) *q = *(q - 1);
		*q = tmp;
	}
}

void Position::shellSort(ValuatedMove* vm, int count) {
	const int gaps[] = { 57, 23, 10, 4, 1 };
	for (int gap : gaps) {
		for (int i = 0; i < count - gap; i++) {
			int j = i + gap;
			ValuatedMove tmp = vm[j];
			while (j >= gap && tmp.score > vm[j - gap].score) {
				vm[j] = vm[j - gap];
				j -= gap;
			}
			vm[j] = tmp;
		}
	}
}

void Position::evaluateByCaptureScore(int startIndex) {
	for (int i = startIndex; i < movepointer - 1; ++i) {
		moves[i].score = settings::parameter.CAPTURE_SCORES[GetPieceType(Board[from(moves[i].move)])][GetPieceType(Board[to(moves[i].move)])] + 2 * (type(moves[i].move) == PROMOTION);
	}
}

void Position::evaluateByMVVLVA(int startIndex) {
	for (int i = startIndex; i < movepointer - 1; ++i) {
		moves[i].score = settings::parameter.PieceValues[GetPieceType(Board[to(moves[i].move)])].mgScore - 150 * relativeRank(GetSideToMove(), Rank(to(moves[i].move) >> 3));
	}
}

void Position::evaluateBySEE(int startIndex) {
	if (movepointer - 2 == startIndex)
		moves[startIndex].score = settings::parameter.PieceValues[QUEEN].mgScore; //No need for SEE if there is only one move to be evaluated
	else
		for (int i = startIndex; i < movepointer - 1; ++i) moves[i].score = SEE(moves[i].move);
}

void Position::evaluateCheckEvasions(int startIndex) {
	ValuatedMove * firstQuiet = std::partition(moves + startIndex, &moves[movepointer - 1], [this](ValuatedMove m) { return IsTactical(m); });
	bool quiets = false;
	int quietsIndex = startIndex;
	for (int i = startIndex; i < movepointer - 1; ++i) {
		quiets = quiets || (moves[i].move == firstQuiet->move);
		if (quiets && history) {
			Piece p = Board[from(moves[i].move)];
			moves[i].score = history->getValue(p, moves[i].move);
		}
		else {
			moves[i].score = settings::parameter.CAPTURE_SCORES[GetPieceType(Board[from(moves[i].move)])][GetPieceType(Board[to(moves[i].move)])] + 2 * (type(moves[i].move) == PROMOTION);
			quietsIndex++;
		}
	}
	if (quietsIndex > startIndex + 1) insertionSort(moves + startIndex, moves + quietsIndex - 1);
	if (movepointer - 2 > quietsIndex) insertionSort(moves + quietsIndex, &moves[movepointer - 1]);
}

Move Position::GetCounterMove(Move(&counterMoves)[12][64]) {
	if (lastAppliedMove != MOVE_NONE) {
		Square lastTo = to(lastAppliedMove);
		return counterMoves[int(Board[lastTo])][lastTo];
	}
	return MOVE_NONE;
}

void Position::evaluateByHistory(int startIndex) {
	Move lastMoves[2] = { MOVE_NONE, MOVE_NONE };
	Piece lastMovingPieces[2] = { Piece::BLANK, Piece::BLANK };
	if (lastAppliedMove != MOVE_NONE) {
		lastMoves[0] = FixCastlingMove(lastAppliedMove);
		lastMovingPieces[0] = Board[to(lastMoves[0])];
		if (Previous() && Previous()->lastAppliedMove != MOVE_NONE) {
			lastMoves[1] = FixCastlingMove(Previous()->lastAppliedMove);
			lastMovingPieces[1] = Previous()->Board[to(lastMoves[1])];
		}
	}
	Bitboard bbNewlyAttacked = lastAppliedMove == MOVE_NONE ? EMPTY : (~(Previous()->attackedByUs)) & AttackedByThem();
	for (int i = startIndex; i < movepointer - 1; ++i) {
		if (moves[i].move == counterMove) {
			moves[i].score = VALUE_MATE;
		}
		else
			if (history) {
				Move fixedMove = FixCastlingMove(moves[i].move);
				Square toSquare = to(fixedMove);
				Piece p = Board[from(fixedMove)];
				moves[i].score = Value(history->getValue(p, fixedMove) - ChebishevDistance(toSquare, KingSquare(Color(SideToMove ^ 1)))); //History + king tropism if equal
				if (lastMoves[0] && cmHistory) {
					moves[i].score += 2 * cmHistory->getValue(lastMovingPieces[0], to(lastMoves[0]), p, toSquare);
					if (lastMoves[1]) moves[i].score += 2 * followupHistory->getValue(lastMovingPieces[1], to(lastMoves[1]), p, toSquare);
				}
				Bitboard toBB = ToBitboard(toSquare);
				if (ToBitboard(from(moves[i].move)) & bbNewlyAttacked) moves[i].score = Value(moves[i].score + 100);
				if ((toBB & (attackedByUs | ~attackedByThem)) != EMPTY)
					moves[i].score = Value(moves[i].score + 500);
				else if ((p < WPAWN) && (toBB & AttacksByPieceType(Color(SideToMove ^ 1), PAWN)) != 0) {
					moves[i].score = Value(moves[i].score - 500);
				}
				assert(moves[i].score < VALUE_MATE);
			}
			else moves[i].score = VALUE_DRAW;
	}
}

Move Position::getBestMove(int startIndex) {
	ValuatedMove bestmove = moves[startIndex];
	for (int i = startIndex + 1; i < movepointer - 1; ++i) {
		if (bestmove < moves[i]) {
			moves[startIndex] = moves[i];
			moves[i] = bestmove;
			bestmove = moves[startIndex];
		}
	}
	return moves[startIndex].score > -VALUE_MATE ? moves[startIndex].move : MOVE_NONE;
}

template<bool SquareIsEmpty> void Position::set(const Piece piece, const Square square) {
	Bitboard squareBB = 1ull << square;
	Piece captured;
	if (!SquareIsEmpty && (captured = Board[square]) != BLANK) {
		OccupiedByPieceType[GetPieceType(captured)] &= ~squareBB;
		OccupiedByColor[GetColor(captured)] &= ~squareBB;
		Hash ^= ZobristKeys[captured][square];
		PsqEval -= settings::parameter.PSQT[captured][square];
	}
	OccupiedByPieceType[GetPieceType(piece)] |= squareBB;
	OccupiedByColor[GetColor(piece)] |= squareBB;
	Board[square] = piece;
	PsqEval += settings::parameter.PSQT[piece][square];
	Hash ^= ZobristKeys[piece][square];
}

void Position::remove(const Square square) {
	Bitboard NotSquareBB = ~(1ull << square);
	Piece piece = Board[square];
	Board[square] = BLANK;
	PsqEval -= settings::parameter.PSQT[piece][square];
	OccupiedByPieceType[GetPieceType(piece)] &= NotSquareBB;
	OccupiedByColor[GetColor(piece)] &= NotSquareBB;
	Hash ^= ZobristKeys[piece][square];
}

void Position::updateCastleFlags(Square fromSquare, Square toSquare) {
	assert(GetPieceType(Board[fromSquare]) != KING || std::abs((int)from - (int)to) != 2 || Chess960);
	if (GetCastlesForColor(SideToMove) != CastleFlag::NoCastles) {
		if (fromSquare == InitialKingSquare[SideToMove]) {
			RemoveCastlingOption(CastleFlag(W0_0 << (2 * SideToMove)));
			RemoveCastlingOption(CastleFlag(W0_0_0 << (2 * SideToMove)));
			SetCastlingLost(SideToMove);
		}
		else if (fromSquare == InitialRookSquare[2 * SideToMove] || toSquare == InitialRookSquare[2 * SideToMove]) {
			RemoveCastlingOption(CastleFlag(W0_0 << (2 * SideToMove)));
			if (GetCastlesForColor(SideToMove) == CastleFlag::NoCastles) SetCastlingLost(SideToMove);
		}
		else if (fromSquare == InitialRookSquare[2 * SideToMove + 1] || toSquare == InitialRookSquare[2 * SideToMove + 1]) {
			RemoveCastlingOption(CastleFlag(W0_0_0 << (2 * SideToMove)));
			if (GetCastlesForColor(SideToMove) == CastleFlag::NoCastles) SetCastlingLost(SideToMove);
		}
	}
}

Bitboard Position::calculateAttacks(Color color) {
	Bitboard occupied = OccupiedBB();
	attacksByPt[GetPiece(ROOK, color)] = 0ull;
	Bitboard rookSliders = PieceBB(ROOK, color);
	Bitboard bbAttacks = EMPTY;
	dblAttacked[color] = EMPTY;
	while (rookSliders) {
		Square sq = lsb(rookSliders);
		Bitboard a = RookTargets(sq, occupied);
		dblAttacked[color] |= bbAttacks & a;
		attacks[sq] = a;
		bbAttacks |= a;
		attacksByPt[GetPiece(ROOK, color)] |= attacks[sq];
		rookSliders &= rookSliders - 1;
	}
	attacksByPt[GetPiece(BISHOP, color)] = 0ull;
	Bitboard bishopSliders = PieceBB(BISHOP, color);
	while (bishopSliders) {
		Square sq = lsb(bishopSliders);
		Bitboard a = BishopTargets(sq, occupied);
		dblAttacked[color] |= bbAttacks & a;
		attacks[sq] = a;
		bbAttacks |= a;
		attacksByPt[GetPiece(BISHOP, color)] |= attacks[sq];
		bishopSliders &= bishopSliders - 1;
	}
	attacksByPt[GetPiece(QUEEN, color)] = 0ull;
	Bitboard queenSliders = PieceBB(QUEEN, color);
	while (queenSliders) {
		Square sq = lsb(queenSliders);
		Bitboard a = RookTargets(sq, occupied);
		a |= BishopTargets(sq, occupied);
		dblAttacked[color] |= bbAttacks & a;
		attacks[sq] = a;
		bbAttacks |= a;
		attacksByPt[GetPiece(QUEEN, color)] |= attacks[sq];
		queenSliders &= queenSliders - 1;
	}
	attacksByPt[GetPiece(KNIGHT, color)] = 0ull;
	Bitboard knights = PieceBB(KNIGHT, color);
	while (knights) {
		Square sq = lsb(knights);
		Bitboard a = KnightAttacks[sq];
		dblAttacked[color] |= bbAttacks & a;
		attacks[sq] = a;
		bbAttacks |= a;
		attacksByPt[GetPiece(KNIGHT, color)] |= attacks[sq];
		knights &= knights - 1;
	}
	Square kingSquare = kingSquares[color];
	attacks[kingSquare] = KingAttacks[kingSquare];
	dblAttacked[color] |= bbAttacks & attacks[kingSquare];
	bbAttacks |= attacks[kingSquare];
	attacksByPt[GetPiece(KING, color)] = attacks[kingSquare];
	attacksByPt[GetPiece(PAWN, color)] = 0ull;
	Bitboard pawns = PieceBB(PAWN, color);
	while (pawns) {
		Square sq = lsb(pawns);
		Bitboard a = PawnAttacks[color][sq];
		dblAttacked[color] |= bbAttacks & a;
		attacks[sq] = a;
		bbAttacks |= a;
		attacksByPt[GetPiece(PAWN, color)] |= attacks[sq];
		pawns &= pawns - 1;
	}
	//dblAttacked[color] |= BatteryAttacks(color);
	return bbAttacks;
}

//Battery attacks are squares attacked by rooks or queens backed by a Slider behind
Bitboard Position::BatteryAttacks(Color attacking_color) const {
	Bitboard bbXRay = EMPTY;
	Bitboard bbRooks = OccupiedByColor[attacking_color] & (OccupiedByPieceType[QUEEN] | OccupiedByPieceType[ROOK]);
	Bitboard bbExclRooks = OccupiedBB() & ~bbRooks;
	while (bbRooks) {
		Square s = lsb(bbRooks);
		bbXRay |= RookTargets(s, bbExclRooks) & ~attacks[s];
		bbRooks &= bbRooks - 1;
	}
	Bitboard bbBishops = OccupiedByColor[attacking_color] & (OccupiedByPieceType[QUEEN] | OccupiedByPieceType[BISHOP]);
	Bitboard bbExclQueen = OccupiedBB() & ~PieceBB(QUEEN, attacking_color);
	while (bbBishops) {
		Square s = lsb(bbBishops);
		bbXRay |= BishopTargets(s, bbExclQueen) & ~attacks[s];
		bbBishops &= bbBishops - 1;
	}
	return bbXRay;
}

Bitboard Position::checkBlocker(Color colorOfBlocker, Color kingColor) {
	return (PinnedPieces(kingColor) & OccupiedByColor[colorOfBlocker]);
}

//Calculates a bitboard of attackers to a given square, but filtering pieces by occupancy mask 
Bitboard Position::AttacksOfField(const Square targetField, const Bitboard occupanyMask) const
{
	//sliding attacks
	Bitboard attacksOfField = RookTargets(targetField, occupanyMask) & (OccupiedByPieceType[ROOK] | OccupiedByPieceType[QUEEN]);
	attacksOfField |= BishopTargets(targetField, occupanyMask) & (OccupiedByPieceType[BISHOP] | OccupiedByPieceType[QUEEN]);
	attacksOfField &= occupanyMask;
	//non-sliding attacks
	attacksOfField |= KnightAttacks[targetField] & OccupiedByPieceType[KNIGHT];
	attacksOfField |= KingAttacks[targetField] & OccupiedByPieceType[KING];
	Bitboard targetBB = ToBitboard(targetField);
	attacksOfField |= ((targetBB >> 7) & NOT_A_FILE) & PieceBB(PAWN, WHITE);
	attacksOfField |= ((targetBB >> 9) & NOT_H_FILE) & PieceBB(PAWN, WHITE);
	attacksOfField |= ((targetBB << 7) & NOT_H_FILE) & PieceBB(PAWN, BLACK);
	attacksOfField |= ((targetBB << 9) & NOT_A_FILE) & PieceBB(PAWN, BLACK);
	return attacksOfField & occupanyMask;
}

Bitboard Position::AttacksOfField(const Square targetField, const Color attackingSide) const {
	//sliding attacks
	Bitboard attacksOfField = SlidingAttacksRookTo[targetField] & (OccupiedByPieceType[ROOK] | OccupiedByPieceType[QUEEN]);
	attacksOfField |= SlidingAttacksBishopTo[targetField] & (OccupiedByPieceType[BISHOP] | OccupiedByPieceType[QUEEN]);
	attacksOfField &= OccupiedByColor[attackingSide];
	//Check for blockers
	Bitboard tmpAttacks = attacksOfField;
	while (tmpAttacks != 0)
	{
		Square from = lsb(tmpAttacks);
		Bitboard blocker = InBetweenFields[from][targetField] & OccupiedBB();
		if (blocker) attacksOfField &= ~ToBitboard(from);
		tmpAttacks &= tmpAttacks - 1;
	}
	//non-sliding attacks
	attacksOfField |= KnightAttacks[targetField] & OccupiedByPieceType[KNIGHT];
	attacksOfField |= KingAttacks[targetField] & OccupiedByPieceType[KING];
	Bitboard targetBB = ToBitboard(targetField);
	attacksOfField |= ((targetBB >> 7) & NOT_A_FILE) & PieceBB(PAWN, WHITE);
	attacksOfField |= ((targetBB >> 9) & NOT_H_FILE) & PieceBB(PAWN, WHITE);
	attacksOfField |= ((targetBB << 7) & NOT_H_FILE) & PieceBB(PAWN, BLACK);
	attacksOfField |= ((targetBB << 9) & NOT_A_FILE) & PieceBB(PAWN, BLACK);
	attacksOfField &= OccupiedByColor[attackingSide];
	return attacksOfField;
}


PieceType Position::getAndResetLeastValuableAttacker(Square toSquare, Bitboard attackers, Bitboard& occupied, Bitboard& attadef, Bitboard& mayXray) const {
	Bitboard leastAttackers = attackers & PieceTypeBB(PAWN);
	if (!leastAttackers) leastAttackers = attackers & (PieceTypeBB(KNIGHT) | PieceTypeBB(BISHOP));
	if (!leastAttackers) leastAttackers = attackers & PieceTypeBB(ROOK);
	if (!leastAttackers) leastAttackers = attackers & PieceTypeBB(QUEEN);
	if (!leastAttackers) leastAttackers = attackers & PieceTypeBB(KING);
	assert(leastAttackers);
	leastAttackers &= (0 - leastAttackers);
	Square leastAttackerSquare = lsb(leastAttackers);
	occupied &= ~leastAttackers;
	attadef &= ~leastAttackers;
	//Check if there is an xray attack now 
	Bitboard shadowed = ShadowedFields[toSquare][leastAttackerSquare];
	Bitboard xray = mayXray & shadowed;
	while (xray) {
		Square xraySquare = lsb(xray);
		if ((InBetweenFields[xraySquare][toSquare] & occupied) == 0) {
			attadef |= ToBitboard(xraySquare);
			mayXray &= ~attadef;
			break;
		}
		xray &= xray - 1;
	}
	return GetPieceType(Board[leastAttackerSquare]);
}

//SEE method, which returns without exact value, when it's sure that value is positive (then VALUE_KNOWN_WIN is returned)
Value Position::SEE_Sign(Move move) const {
	Square fromSquare = from(move);
	Square toSquare = to(move);
	if (settings::parameter.PieceValues[GetPieceType(Board[fromSquare])].mgScore <= settings::parameter.PieceValues[GetPieceType(Board[toSquare])].mgScore && type(move) != PROMOTION) return VALUE_KNOWN_WIN;
	return SEE(move);
}

//This SEE implementation is a mixture of stockfish and Computer chess sample implementation
//(see https://chessprogramming.wikispaces.com/SEE+-+The+Swap+Algorithm)
Value Position::SEE(Move move) const
{
	if (type(move) == CASTLING) return VALUE_ZERO;

	Value gain[32];
	int d = 1;

	Square fromSquare = from(move);
	Square toSquare = to(move);
	gain[0] = settings::parameter.PieceValues[GetPieceType(Board[toSquare])].mgScore;
	Color side = GetColor(Board[fromSquare]);
	Bitboard fromBB = ToBitboard(fromSquare);
	Bitboard occ = OccupiedBB() & (~fromBB) &(~ToBitboard(toSquare));

	if (type(move) == ENPASSANT)
	{
		occ ^= toSquare - 8 * (1 - 2 * side);
		gain[0] = settings::parameter.PieceValues[PAWN].mgScore;
	}

	// Get Attackers ofto Square
	Bitboard attadef = AttacksOfField(toSquare, occ);
	// Get potential xray attackers
	Bitboard mayXRay = SlidingAttacksRookTo[toSquare] & (OccupiedByPieceType[ROOK] | OccupiedByPieceType[QUEEN]);
	mayXRay |= SlidingAttacksBishopTo[toSquare] & (OccupiedByPieceType[BISHOP] | OccupiedByPieceType[QUEEN]);
	mayXRay &= occ;
	mayXRay &= ~attadef;

	// If there are no attackers we are done (we had a simple capture of a hanging piece)
	side = Color(side ^ 1);
	Bitboard attackers = attadef & OccupiedByColor[side];

	//Consider pinned pieces
	if ((attackers & PinnedPieces(side)) != EMPTY && (bbPinner[side] & occ) != EMPTY)
		attackers &= ~PinnedPieces(side);

	if (!attackers) return gain[0];

	PieceType capturingPiece = GetPieceType(Board[fromSquare]);

	do {
		assert(d < 32);

		// Add the new entry to the swap list
		gain[d] = -gain[d - 1] + settings::parameter.PieceValues[capturingPiece].mgScore;

		// Locate and remove the next least valuable attacker
		capturingPiece = getAndResetLeastValuableAttacker(toSquare, attackers, occ, attadef, mayXRay);
		side = Color(side ^ 1);
		attackers = attadef & OccupiedByColor[side];
		//Consider pinned pieces
		if ((attackers & PinnedPieces(side)) != EMPTY && (bbPinner[side] & occ) != EMPTY)
			attackers &= ~PinnedPieces(side);
		++d;

	} while (attackers && (capturingPiece != KING || (--d, false))); // Stop before a king capture

	if (capturingPiece == PAWN && (toSquare <= H1 || toSquare >= A8)) {
		gain[d - 1] += settings::parameter.PieceValues[QUEEN].mgScore - settings::parameter.PieceValues[PAWN].mgScore;
	}

	// find the best achievable score by minimaxing
	while (--d) gain[d - 1] = std::min(-gain[d], gain[d - 1]);

	return gain[0];
}


void Position::setFromFEN(const std::string& fen) {
	material = nullptr;
	std::fill_n(Board, 64, BLANK);
	OccupiedByColor[WHITE] = OccupiedByColor[BLACK] = 0ull;
	std::fill_n(OccupiedByPieceType, 6, 0ull);
	EPSquare = OUTSIDE;
	SideToMove = WHITE;
	DrawPlyCount = 0;
	AppliedMovesBeforeRoot = 0;
	Hash = ZobristMoveColor;
	PsqEval = EVAL_ZERO;
	std::istringstream ss(fen);
	ss >> std::noskipws;
	unsigned char token;

	//Piece placement
	size_t piece;
	char square = A8;
	while ((ss >> token) && !isspace(token)) {
		if (isdigit(token))
			square = square + (token - '0');
		else if (token == '/')
			square -= 16;
		else if ((piece = PieceToChar.find(token)) != std::string::npos) {
			set<true>((Piece)piece, (Square)square);
			square++;
		}
	}

	kingSquares[WHITE] = lsb(PieceBB(KING, WHITE));
	kingSquares[BLACK] = lsb(PieceBB(KING, BLACK));

	//Side to Move
	ss >> token;
	if (token != 'w') SwitchSideToMove();

	//Castles
	CastlingOptions = 0;
	ss >> token;
	while ((ss >> token) && !isspace(token)) {

		if (token == 'K') {
			AddCastlingOption(W0_0);
			InitialKingSquareBB[WHITE] = PieceBB(KING, WHITE);
			InitialKingSquare[WHITE] = lsb(InitialKingSquareBB[WHITE]);
			for (Square k = InitialKingSquare[WHITE]; k <= H1; ++k) {
				if (Board[k] == WROOK) {
					InitialRookSquare[0] = k;
					break;
				}
			}
		}
		else if (token == 'Q') {
			AddCastlingOption(W0_0_0);
			InitialKingSquareBB[WHITE] = PieceBB(KING, WHITE);
			InitialKingSquare[WHITE] = lsb(InitialKingSquareBB[WHITE]);
#pragma warning(suppress: 6295)
			for (Square k = InitialKingSquare[WHITE]; k >= A1; --k) {
				if (Board[k] == WROOK) {
					InitialRookSquare[1] = k;
					break;
				}
			}
		}
		else if (token == 'k') {
			AddCastlingOption(B0_0);
			InitialKingSquareBB[BLACK] = PieceBB(KING, BLACK);
			InitialKingSquare[BLACK] = lsb(InitialKingSquareBB[BLACK]);
			for (Square k = InitialKingSquare[BLACK]; k <= H8; ++k) {
				if (Board[k] == BROOK) {
					InitialRookSquare[2] = k;
					break;
				}
			}
		}
		else if (token == 'q') {
			AddCastlingOption(B0_0_0);
			InitialKingSquareBB[BLACK] = PieceBB(KING, BLACK);
			InitialKingSquare[BLACK] = lsb(InitialKingSquareBB[BLACK]);
			for (Square k = InitialKingSquare[BLACK]; k >= A8; --k) {
				if (Board[k] == BROOK) {
					InitialRookSquare[3] = k;
					break;
				}
			}

		}
		else if (token == '-') continue;
		else {
			if (token >= 'A' && token <= 'H') {
				File kingFile = File(kingSquares[WHITE] & 7);
				InitialKingSquareBB[WHITE] = PieceBB(KING, WHITE);
				InitialKingSquare[WHITE] = lsb(InitialKingSquareBB[WHITE]);
				File rookFile = File((int)token - (int)'A');
				if (rookFile < kingFile) {
					AddCastlingOption(W0_0_0);
					InitialRookSquare[1] = Square(rookFile);
				}
				else {
					AddCastlingOption(W0_0);
					InitialRookSquare[0] = Square(rookFile);
				}
			}
			else if (token >= 'a' && token <= 'h') {
				File kingFile = File(kingSquares[BLACK] & 7);
				InitialKingSquareBB[BLACK] = PieceBB(KING, BLACK);
				InitialKingSquare[BLACK] = lsb(InitialKingSquareBB[BLACK]);
				File rookFile = File((int)token - (int)'a');
				if (rookFile < kingFile) {
					AddCastlingOption(B0_0_0);
					InitialRookSquare[3] = Square(rookFile + 56);
				}
				else {
					AddCastlingOption(B0_0);
					InitialRookSquare[2] = Square(rookFile + 56);
				}
			}
		}
	}
	if (CastlingOptions & 15) {
		Chess960 = Chess960 || (InitialKingSquare[WHITE] != E1) || (InitialRookSquare[0] != H1) || (InitialRookSquare[1] != A1);
		for (int i = 0; i < 4; ++i) InitialRookSquareBB[i] = 1ull << InitialRookSquare[i];
		Square kt[4] = { G1, C1, G8, C8 };
		Square rt[4] = { F1, D1, F8, D8 };
		for (int i = 0; i < 4; ++i) {
			SquaresToBeEmpty[i] = 0ull;
			SquaresToBeUnattacked[i] = 0ull;
			Square ks = lsb(InitialKingSquareBB[i / 2]);
			for (int j = std::min(ks, kt[i]); j <= std::max(ks, kt[i]); ++j) SquaresToBeUnattacked[i] |= 1ull << j;
			for (int j = std::min(InitialRookSquare[i], rt[i]); j <= std::max(InitialRookSquare[i], rt[i]); ++j) {
				SquaresToBeEmpty[i] |= 1ull << j;
			}
			for (int j = std::min(ks, kt[i]); j <= std::max(ks, kt[i]); ++j) {
				SquaresToBeEmpty[i] |= 1ull << j;
			}
			SquaresToBeEmpty[i] &= ~InitialKingSquareBB[i / 2];
			SquaresToBeEmpty[i] &= ~InitialRookSquareBB[i];
		}
	}

	//EP-square
	char col, row;
	if (((ss >> col) && (col >= 'a' && col <= 'h'))
		&& ((ss >> row) && (row == '3' || row == '6'))) {
		EPSquare = (Square)(8 * (row - '0' - 1) + col - 'a');
		Square to;
		if (SideToMove == BLACK && EPSquare <= H4) {
			to = (Square)(EPSquare + 8);
			if ((GetEPAttackersForToField(to) & PieceBB(PAWN, BLACK)) == 0)
				EPSquare = OUTSIDE;
		}
		else if (SideToMove == WHITE && EPSquare >= A5) {
			to = (Square)(EPSquare - 8);
			if ((GetEPAttackersForToField(to) & PieceBB(PAWN, WHITE)) == 0)
				EPSquare = OUTSIDE;
		}
		if (EPSquare != OUTSIDE) Hash ^= ZobristEnPassant[EPSquare & 7];
	}
	std::string dpc;
	ss >> std::skipws >> dpc;
	if (dpc.length() > 0) {
		DrawPlyCount = (unsigned char)atoi(dpc.c_str());
	}
	std::fill_n(attacks, 64, 0ull);
	PawnKey = calculatePawnKey();
	pawn = pawn::probe(*this);
	if (checkMaterialIsUnusual()) {
		MaterialKey = MATERIAL_KEY_UNUSUAL;
		material = initUnusual(*this);
	}
	else {
		MaterialKey = calculateMaterialKey();
		material = probe(MaterialKey);
	}
	attackedByThem = calculateAttacks(Color(SideToMove ^ 1));
	attackedByUs = calculateAttacks(SideToMove);
	CalculatePinnedPieces();
	pliesFromRoot = 0;
}

std::string Position::fen() const {

	int emptyCnt;
	std::ostringstream ss;

	for (Rank r = Rank8; r >= Rank1; --r) {
		for (File f = FileA; f <= FileH; ++f) {
			for (emptyCnt = 0; f <= FileH && Board[createSquare(r, f)] == BLANK; ++f)
				++emptyCnt;
			if (emptyCnt)
				ss << emptyCnt;

			if (f <= FileH)
				ss << PieceToChar[Board[createSquare(r, f)]];
		}

		if (r > Rank1)
			ss << '/';
	}

	ss << (SideToMove == WHITE ? " w " : " b ");

	if (!(CastlingOptions & 15))
		ss << '-';
	else if (Chess960) {
		if (W0_0 & CastlingOptions) {
			Bitboard sideOfKing = 0ull;
			for (int i = InitialKingSquare[WHITE] + 1; i < 8; ++i) sideOfKing |= 1ull << i;
			sideOfKing &= ~InitialRookSquareBB[0];
			if (sideOfKing & PieceBB(ROOK, WHITE)) ss << char((int)'A' + InitialRookSquare[0]); else ss << 'K';
		}
		if (W0_0_0 & CastlingOptions) {
			Bitboard sideOfKing = 0ull;
			for (int i = InitialKingSquare[WHITE] + 1; i >= 0; --i) sideOfKing |= 1ull << i;
			sideOfKing &= ~InitialRookSquareBB[1];
			if (sideOfKing & PieceBB(ROOK, WHITE)) ss << char((int)'A' + InitialRookSquare[1]); else ss << 'Q';
		}
		if (B0_0 & CastlingOptions) {
			Bitboard sideOfKing = 0ull;
			for (int i = InitialKingSquare[BLACK] + 1; i < 64; ++i) sideOfKing |= 1ull << i;
			sideOfKing &= ~InitialRookSquareBB[2];
			if (sideOfKing & PieceBB(ROOK, BLACK)) ss << char((int)'a' + (InitialRookSquare[2] & 7)); else ss << 'k';
		}
		if (B0_0_0 & CastlingOptions) {
			Bitboard sideOfKing = 0ull;
			for (int i = InitialKingSquare[BLACK] + 1; i >= A8; --i) sideOfKing |= 1ull << i;
			sideOfKing &= ~InitialRookSquareBB[3];
			if (sideOfKing & PieceBB(ROOK, BLACK)) ss << char((int)'a' + (InitialRookSquare[3] & 7)); else ss << 'q';
		}
	}
	else {
		if (W0_0 & CastlingOptions)
			ss << 'K';
		if (W0_0_0 & CastlingOptions)
			ss << 'Q';
		if (B0_0 & CastlingOptions)
			ss << 'k';
		if (B0_0_0 & CastlingOptions)
			ss << 'q';
	}
	ss << (EPSquare == OUTSIDE ? " - " : " " + toString(EPSquare) + " ")
		<< int(DrawPlyCount) << " "
		//<< 1 + (_ply - (_sideToMove == BLACK)) / 2;
		<< "1";
	return ss.str();
}

std::string Position::print() {
	std::ostringstream ss;

	ss << "\n +---+---+---+---+---+---+---+---+\n";

	for (int r = 7; r >= 0; --r) {
		for (int f = 0; f <= 7; ++f)
			ss << " | " << PieceToChar[Board[8 * r + f]];

		ss << " |\n +---+---+---+---+---+---+---+---+\n";
	}
	ss << "\nChecked:         " << std::boolalpha << Checked() << std::noboolalpha
		<< "\nEvaluation:      " << (int)this->evaluate()
		<< "\nFen:             " << fen()
		<< "\nHash:            " << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << Hash
		//<< "\nNormalized Hash: " << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << GetNormalizedHash()
		<< "\nMaterial Key:    " << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << MaterialKey
		<< "\n";
	return ss.str();
}

std::string Position::printGeneratedMoves() {
	std::ostringstream ss;
	for (int i = 0; i < movepointer - 1; ++i) {
		ss << toString(moves[i].move) << "\t" << (int)moves[i].score << "\n";
	}
	return ss.str();
}

MaterialKey_t Position::calculateMaterialKey() const {
	MaterialKey_t key = MATERIAL_KEY_OFFSET;
	for (int i = WQUEEN; i <= BPAWN; ++i)
		key += materialKeyFactors[i] * popcount(PieceBB(GetPieceType(Piece(i)), Color(i & 1)));
	return key;
}

PawnKey_t Position::calculatePawnKey() const {
	PawnKey_t key = 0;
	Bitboard pawns = PieceBB(PAWN, WHITE);
	while (pawns) {
		key ^= ZobristKeys[WPAWN][lsb(pawns)];
		pawns &= pawns - 1;
	}
	pawns = PieceBB(PAWN, BLACK);
	while (pawns) {
		key ^= ZobristKeys[BPAWN][lsb(pawns)];
		pawns &= pawns - 1;
	}
	return key;
}

Result Position::GetResult() {
	//if (!result) {
	//	bool checked = Checked();
	//	if (checked && !CheckValidMoveExists<true>()) result = MATE;
	//	else if (!checked && !CheckValidMoveExists<false>()) result = DRAW;
	//	else if (DrawPlyCount >= 100 || checkRepetition()) result = DRAW;
	//	else result = OPEN;
	//}
	if (result == Result::RESULT_UNKNOWN) {
		if (Checked()) {
			if (CheckValidMoveExists<true>()) result = Result::OPEN; else result = Result::MATE;
		}
		else {
			if (CheckValidMoveExists<false>()) result = Result::OPEN; else result = Result::DRAW;
		}
		if (result == Result::OPEN && (DrawPlyCount >= 100 || checkRepetition())) 
			result = Result::DRAW;
	}
	return result;
}

DetailedResult Position::GetDetailedResult() {
	GetResult();
	if (result == Result::OPEN) return DetailedResult::NO_RESULT;
	else if (result == Result::MATE) {
		return GetSideToMove() == WHITE ? DetailedResult::BLACK_MATES : DetailedResult::WHITE_MATES;
	}
	else {
		if (DrawPlyCount >= 100) return DetailedResult::DRAW_50_MOVES;
		else if (GetMaterialTableEntry()->IsTheoreticalDraw()) return DetailedResult::DRAW_MATERIAL;
		else if (!Checked() && !CheckValidMoveExists<false>()) return DetailedResult::DRAW_STALEMATE;
		else {
			//Check for 3 fold repetition
			int repCounter = 0;
			Position * prev = Previous();
			for (int i = 0; i < (std::min(pliesFromRoot + AppliedMovesBeforeRoot, int(DrawPlyCount)) >> 1); ++i) {
				prev = prev->Previous();
				if (prev->GetHash() == GetHash()) {
					repCounter++;
					if (repCounter > 1) return DetailedResult::DRAW_REPETITION;
					prev = prev->Previous();
				}
			}
		}
	}
	return DetailedResult::NO_RESULT;
}

bool Position::checkRepetition() const {
	Position * prev = Previous();
	for (int i = 0; i < (std::min(pliesFromRoot + AppliedMovesBeforeRoot, int(DrawPlyCount)) >> 1); ++i) {
		prev = prev->Previous();
		if (prev->GetHash() == GetHash())
			return true;
		prev = prev->Previous();
	}
	return false;
}

bool Position::hasRepetition() const {
	const Position * pos = this;
	while (pos && pos->GetDrawPlyCount() > 0) {
		if (pos->checkRepetition()) return true;
		pos = pos->Previous();
	}
	return false;
}

//Hashmoves, countermoves, ... aren't really reliable => therefore check if it is a valid move
bool Position::validateMove(Move move) {
	Square fromSquare = from(move);
	Piece movingPiece = Board[fromSquare];
	Square toSquare = to(move);
	bool valid = (movingPiece != BLANK) && (GetColor(movingPiece) == SideToMove) //from field is occuppied by piece of correct color
		&& ((Board[toSquare] == BLANK) || (GetColor(Board[toSquare]) != SideToMove));
	if (valid) {
		PieceType pt = GetPieceType(movingPiece);
		if (pt == PAWN) {
			switch (type(move)) {
			case NORMAL:
				valid = !(ToBitboard(toSquare) & RANKS[7 - 7 * SideToMove]) && (((int(toSquare) - int(fromSquare)) == PawnStep() && Board[toSquare] == BLANK) ||
					(((int(toSquare) - int(fromSquare)) == 2 * PawnStep()) && ((fromSquare >> 3) == (1 + 5 * SideToMove)) && Board[toSquare] == BLANK && Board[toSquare - PawnStep()] == BLANK)
					|| (attacks[fromSquare] & OccupiedByColor[SideToMove ^ 1] & ToBitboard(toSquare)));
				break;
			case PROMOTION:
				valid = (ToBitboard(toSquare) & RANKS[7 - 7 * SideToMove]) &&
					(((int(toSquare) - int(fromSquare)) == PawnStep() && Board[toSquare] == BLANK) || (attacks[fromSquare] & OccupiedByColor[SideToMove ^ 1] & ToBitboard(toSquare)));
				break;
			case ENPASSANT:
				valid = toSquare == EPSquare;
				break;
			default:
				return false;
			}
		}
		else if (pt == KING && type(move) == CASTLING) {
			valid = (CastlingOptions & CastlesbyColor[SideToMove]) != 0
				&& ((InBetweenFields[fromSquare][InitialRookSquare[2 * SideToMove + (toSquare < fromSquare)]] & OccupiedBB()) == 0)
				&& (((InBetweenFields[fromSquare][toSquare] | ToBitboard(toSquare)) & attackedByThem) == 0)
				&& (InitialRookSquareBB[2 * SideToMove + (toSquare < fromSquare)] & PieceBB(ROOK, SideToMove))
				&& !Checked();
		}
		else if (type(move) == NORMAL) valid = (attacks[fromSquare] & ToBitboard(toSquare)) != 0;
		else valid = false;
	}
	return valid;
}

Move Position::validMove(Move proposedMove)
{
	ValuatedMove * vmoves = GenerateMoves<LEGAL>();
	int movecount = GeneratedMoveCount();
	for (int i = 0; i < movecount; ++i) {
		if (vmoves[i].move == proposedMove) return proposedMove;
	}
	return movecount > 0 ? vmoves[0].move : MOVE_NONE;
}
bool Position::givesCheck(Move move)
{
	Square fromSquare = from(move);
	Square toSquare = to(move);
	Square kingSquare = kingSquares[SideToMove ^ 1];
	MoveType moveType = type(move);
	PieceType pieceType;
	if (moveType == MoveType::NORMAL)
		pieceType = GetPieceType(GetPieceOnSquare(fromSquare));
	else if (moveType == MoveType::PROMOTION)
		pieceType = promotionType(move);
	else if (moveType == MoveType::ENPASSANT) pieceType = PieceType::PAWN;
	else {
		toSquare = File(toSquare & 7) == File::FileG ? Square(toSquare - 1) : Square(toSquare + 1);
		pieceType = ROOK; //Castling
	}
	//Check direct checks
	switch (pieceType)
	{
	case KNIGHT:
		if (ToBitboard(toSquare) & KnightAttacks[kingSquare]) return true;
		break;
	case PAWN:
		if (PawnAttacks[SideToMove][toSquare] & ToBitboard(kingSquare)) return true;
		break;
	case BISHOP:
		if (BishopTargets(toSquare, OccupiedBB() & ~ToBitboard(fromSquare)) & PieceBB(KING, Color(SideToMove ^ 1))) return true;
		break;
	case ROOK:
		if (RookTargets(toSquare, OccupiedBB() & ~ToBitboard(fromSquare)) & PieceBB(KING, Color(SideToMove ^ 1))) return true;
		break;
	case QUEEN:
		if ((RookTargets(toSquare, OccupiedBB() & ~ToBitboard(fromSquare)) & PieceBB(KING, Color(SideToMove ^ 1))) || (BishopTargets(toSquare, OccupiedBB() & ~ToBitboard(fromSquare)) & PieceBB(KING, Color(SideToMove ^ 1)))) return true;
		break;
	default:
		break;
	}
	//now check for discovered check
	Bitboard dc = PinnedPieces(Color(SideToMove ^ 1));
	if (dc) {
		if (moveType == MoveType::ENPASSANT) {
			//in EP-captures 2 "from"-Moves have to be checked for discovered (from-square and square of captured pawn)
			if ((ToBitboard(fromSquare) & dc) != EMPTY) { //capturing pawn was pinned
				if ((ToBitboard(toSquare) & RaysBySquares[fromSquare][kingSquare]) == EMPTY) return true; //pin wasn't along capturing diagonal
			}
			Square capturedPawnSquare = Square(toSquare - 8 + 16 * int(SideToMove));
			if ((ToBitboard(capturedPawnSquare) & dc) == EMPTY) return false; //captured pawn isn't pinned
			if ((ToBitboard(toSquare) & RaysBySquares[capturedPawnSquare][kingSquare]) != EMPTY) return false; //captured pawn was pinned, but capturing pawn is now blocking
			return true;
		}
		else {
			if (moveType == MoveType::CASTLING || (ToBitboard(fromSquare) & dc) == EMPTY) return false;
			return (ToBitboard(toSquare) & RaysBySquares[fromSquare][kingSquare]) == EMPTY;
		}
	}
	return false;
}

bool Position::oppositeColoredBishops() const
{
	return popcount(PieceBB(BISHOP, WHITE)) == 1 && popcount(PieceBB(BISHOP, BLACK)) == 1 && popcount(PieceTypeBB(BISHOP) & DARKSQUARES) == 1;
}

bool Position::validateMove(ExtendedMove move) {
	Square fromSquare = from(move.move);
	return Board[fromSquare] == move.piece && validateMove(move.move);
}


void Position::NullMove(Square epsquare, Move lastApplied) {
	SwitchSideToMove();
	SetEPSquare(epsquare);
	lastAppliedMove = lastApplied;
	Bitboard tmp = attackedByThem;
	attackedByThem = attackedByUs;
	attackedByUs = tmp;
	if (StaticEval != VALUE_NOTYETDETERMINED) {
		StaticEval = -StaticEval + 2 * settings::parameter.BONUS_TEMPO.getScore(material->Phase);
	}
}

void Position::deleteParents() {
	if (previous != nullptr) previous->deleteParents();
	delete(previous);
}

bool Position::checkMaterialIsUnusual() const {
	return popcount(PieceBB(QUEEN, WHITE)) > 1
		|| popcount(PieceBB(QUEEN, BLACK)) > 1
		|| popcount(PieceBB(ROOK, WHITE)) > 2
		|| popcount(PieceBB(ROOK, BLACK)) > 2
		|| popcount(PieceBB(BISHOP, WHITE)) > 2
		|| popcount(PieceBB(BISHOP, BLACK)) > 2
		|| popcount(PieceBB(KNIGHT, WHITE)) > 2
		|| popcount(PieceBB(KNIGHT, BLACK)) > 2;
}

ValuatedMove * Position::GenerateForks(bool withChecks)
{
	movepointer -= (movepointer != 0);
	ValuatedMove * firstMove = &moves[movepointer];
	Bitboard knights = PieceBB(KNIGHT, SideToMove);
	Bitboard targets;
	Bitboard forkTargets;
	if (withChecks) {
		targets = ~OccupiedBB() & ~attackedByThem;
		forkTargets = PieceBB(QUEEN, Color(SideToMove ^ 1)) | PieceBB(ROOK, Color(SideToMove ^ 1)) | PieceBB(KING, Color(SideToMove ^ 1));
		forkTargets |= ColorBB(Color(SideToMove ^ 1)) & ~attackedByThem;
	}
	else {
		targets = ~OccupiedBB() & ~attackedByThem & ~KnightAttacks[kingSquares[SideToMove ^ 1]];
		forkTargets = PieceBB(QUEEN, Color(SideToMove ^ 1)) | PieceBB(ROOK, Color(SideToMove ^ 1));
		forkTargets |= ColorBB(Color(SideToMove ^ 1)) & ~attackedByThem & ~PieceTypeBB(KING);
	}
	int forkTargetCount = popcount(forkTargets);
	if (forkTargetCount > 1) {
		while (knights) {
			Square fromSquare = lsb(knights);
			Bitboard knightTargets = KnightAttacks[fromSquare] & targets;
			while (knightTargets) {
				Square toSquare = lsb(knightTargets);
				if (popcount(KnightAttacks[toSquare] & forkTargets) > 1) {
					AddMove(createMove(fromSquare, toSquare));
				}
				knightTargets &= knightTargets - 1;
			}
			knights &= knights - 1;
		}
	}
	AddNullMove();
	return firstMove;
}

bool Position::mateThread() const
{
	Bitboard bbEscapeSquares = GetAttacksFrom(kingSquares[SideToMove ^ 1]) & ~ColorBB(Color(SideToMove ^ 1)) & ~attackedByUs;
	int countEscapeSquares = popcount(bbEscapeSquares);
	return (countEscapeSquares <= 1); //At most one escape square
		//|| (countEscapeSquares == 2  && (bbEscapeSquares & (Rank1 | Rank8)) == bbEscapeSquares); //Backrank mate
}

std::string Position::toSan(Move move) {
	Square toSquare = to(move);
	Square fromSquare = from(move);
	if (type(move) == CASTLING) {
		if (toSquare > fromSquare) return "O-O"; else return "O-O-O";
	}
	PieceType pt = GetPieceType(Board[from(move)]);
	bool isCapture = (Board[to(move)] != BLANK) || (type(move) == PROMOTION);
	if (pt == PAWN) {
		if (isCapture || type(move) == ENPASSANT) {
			if (type(move) == PROMOTION) {
				char ch[] = { toChar(File(fromSquare & 7)), 'x', toChar(File(toSquare & 7)), toChar(Rank(toSquare >> 3)), '=', "QRBN"[promotionType(move)], 0 };
				return ch;
			}
			else {
				char ch[] = { toChar(File(fromSquare & 7)), 'x', toChar(File(toSquare & 7)), toChar(Rank(toSquare >> 3)), 0 };
				return ch;
			}
		}
		else {
			if (type(move) == PROMOTION) {
				char ch[] = { toChar(File(toSquare & 7)), toChar(Rank(toSquare >> 3)), '=', "QRBN"[promotionType(move)], 0 };
				return ch;
			}
			else {
				char ch[] = { toChar(File(toSquare & 7)), toChar(Rank(toSquare >> 3)), 0 };
				return ch;
			}
		}
	}
	else if (pt == KING) {
		if (isCapture) {
			char ch[] = { 'K', 'x', toChar(File(toSquare & 7)), toChar(Rank(toSquare >> 3)), 0 };
			return ch;
		}
		else {
			char ch[] = { 'K', toChar(File(toSquare & 7)), toChar(Rank(toSquare >> 3)), 0 };
			return ch;
		}
	}
	//Check if diambiguation is needed
	Move dMove = MOVE_NONE;
	ValuatedMove * legalMoves = GenerateMoves<LEGAL>();
	while (legalMoves->move) {
		if (legalMoves->move != move && to(legalMoves->move) == toSquare && GetPieceType(Board[from(legalMoves->move)]) == pt) {
			dMove = legalMoves->move;
			break;
		}
		legalMoves++;
	}
	char ch[6];
	ch[0] = "QRBN"[pt];
	int indx = 1;
	if (dMove != MOVE_NONE) {
		if ((from(dMove) & 7) == (fromSquare & 7)) ch[indx] = toChar(Rank(from(dMove) >> 3)); else ch[indx] = toChar(File(from(dMove) & 7));
		indx++;
	}
	if (isCapture) {
		ch[indx] = 'x';
		indx++;
	}
	ch[indx] = toChar(File(toSquare & 7));
	indx++;
	ch[indx] = toChar(Rank(toSquare >> 3));
	indx++;
	ch[indx] = 0;
	return ch;
}

Move Position::parseSan(std::string move) {
	ValuatedMove * legalMoves = GenerateMoves<LEGAL>();
	while (legalMoves->move) {
		if (move.find(toSan(legalMoves->move)) != std::string::npos) return legalMoves->move;
	}
	return MOVE_NONE;
}

std::string Position::printEvaluation() {
	if (material->EvaluationFunction == &evaluateDefault) {
		return printDefaultEvaluation(*this);
	}
	else {
		std::stringstream ss;
		ss << "Special Evaluation Function used!" << std::endl;
		Value score = evaluate();
		if (SideToMove == BLACK) score = -score;
		ss << "Total evaluation: " << static_cast<int>(score) << " (white side)" << std::endl;
		return ss.str();
	}
}

void Position::CalculatePinnedPieces()
{
	for (int colorOfKing = 0; colorOfKing < 2; ++colorOfKing) {
		bbPinned[colorOfKing] = EMPTY;
		bbPinner[colorOfKing] = EMPTY;
		Square kingSquare = kingSquares[colorOfKing];
		Bitboard pinner = (OccupiedByPieceType[ROOK] | OccupiedByPieceType[QUEEN]) & SlidingAttacksRookTo[kingSquare];
		pinner |= (OccupiedByPieceType[BISHOP] | OccupiedByPieceType[QUEEN]) & SlidingAttacksBishopTo[kingSquare];
		pinner &= OccupiedByColor[colorOfKing ^ 1];
		Bitboard occ = OccupiedBB();
		while (pinner)
		{
			Bitboard blocker = InBetweenFields[lsb(pinner)][kingSquare] & occ;
			if (popcount(blocker) == 1) bbPinned[colorOfKing] |= blocker;
			bbPinner[colorOfKing] |= isolateLSB(pinner);
			pinner &= pinner - 1;
		}
	}
}
