#pragma once
#include <string>
#include <vector>
#include "types.h"
#include "board.h"
#include "material.h"
#include "hashtables.h"


struct Evaluation;

const MoveGenerationType generationPhases[26] = { HASHMOVE, NON_LOOSING_CAPTURES, KILLER, LOOSING_CAPTURES, QUIETS_POSITIVE, QUIETS_NEGATIVE, UNDERPROMOTION, NONE, //Main Search Phases
HASHMOVE, NON_LOOSING_CAPTURES, LOOSING_CAPTURES, NONE,                                   //QSearch Phases
HASHMOVE, CHECK_EVASION, UNDERPROMOTION, NONE, //Check Evasion
HASHMOVE, NON_LOOSING_CAPTURES, LOOSING_CAPTURES, QUIET_CHECKS, UNDERPROMOTION, NONE, //QSearch with Checks
REPEAT_ALL, NONE,
ALL, NONE };

//Indexed by StagedMoveGenerationType
const int generationPhaseOffset[] = { 0, //Main Search
8, //QSearch
12, //Check Evasion
16, //QSearch with Checks
22, //Repeat
24  //All moves
};
/* Represents a chess position and provides information about lots of characteristics of this position
   The position is represented by 8 Bitboards (2 for squares occupied by color, and 6 for each piece type)
   Further there is a redundant 64 byte array for fast lookup which piece is on a given square
   Each position contains a pointer to the previous position, which is created when copying the position
   Applying a move is done by first copying the position and then calling ApplyMove
*/
struct Position
{
public:
	//Creates the starting position 
	Position();
	//Creates a position based on the given FEN string
	explicit Position(std::string fen);
	/*Creates a copy of a position.
	  ATTENTION: Only parts of the position are copied. To have a full copy a further call to copy() method is needed
	*/
	Position(Position &pos);
	~Position();

	//Moves already played within a game before searched position is reached
	static int AppliedMovesBeforeRoot;
	//Access methods to the positions bitboards
	Bitboard PieceBB(const PieceType pt, const Color c) const;
	Bitboard ColorBB(const Color c) const;
	Bitboard ColorBB(const int c) const;
	Bitboard PieceTypeBB(const PieceType pt) const;
	Bitboard OccupiedBB() const;
	inline Bitboard MajorPieceBB(const Color c) const { return (OccupiedByPieceType[QUEEN] | OccupiedByPieceType[ROOK]) & OccupiedByColor[static_cast<int>(c)]; }
	inline Bitboard MinorPieceBB(const Color c) const { return (OccupiedByPieceType[BISHOP] | OccupiedByPieceType[KNIGHT]) & OccupiedByColor[static_cast<int>(c)]; }
	inline Bitboard NonPawnMaterial(const Color c) const { return (OccupiedByPieceType[QUEEN] | OccupiedByPieceType[ROOK] | OccupiedByPieceType[BISHOP] | OccupiedByPieceType[KNIGHT]) & OccupiedByColor[static_cast<int>(c)]; }
	//debug helpers
	std::string print();
	std::string printGeneratedMoves();
	//returns current position as FEN string
	std::string fen() const;
	//initializes the position from given FEN string
	void setFromFEN(const std::string& fen);
	//Applies a pseudo-legal move and returns true if move is legal
	bool ApplyMove(Move move);
	//"Undo move" by returning pointer to previous position
	inline Position * Previous() const { return previous; }
	//Generate moves and store it in moves array
	template<MoveGenerationType MGT> ValuatedMove * GenerateMoves();
	//returns Zobrist Hash key of position
	inline uint64_t GetHash() const { return Hash; }
	inline MaterialKey_t GetMaterialKey() const { return MaterialKey; }
	inline uint64_t GetMaterialHash() const { return MaterialKey != MATERIAL_KEY_UNUSUAL ? MaterialKey * 14695981039346656037ull : GetMaterialHashUnusual(); }
	inline PawnKey_t GetPawnKey() const { return PawnKey; }
	inline Eval GetPsqEval() const { return PsqEval; }
	/* The position struct provides staged move generation. To make use of it the staged move generation has to be initialized first by calling InitializeMoveIterator.
	   Then every call to NextMove() will return the next move until MOVE_NONE is returned */
	   //Initialize staged move generation, by providing the necessary information for move ordering
	template<StagedMoveGenerationType SMGT> void InitializeMoveIterator(HistoryManager *history, MoveSequenceHistoryManager *counterMoveHistory, MoveSequenceHistoryManager *followupHistory, killer::Manager * km, Move counter, Move hashmove = MOVE_NONE);
	//Get next move. If MOVE_NONE is returned end of move list is reached
	Move NextMove();
	//SEE (Static Exchange Evaluation): The implementation is copied from Chess Programming Wiki (https://chessprogramming.wikispaces.com/SEE+-+The+Swap+Algorithm)
	Value SEE(Move move) const;
	//SEE method, which returns without exact value, when it's sure that value is positive (then VALUE_KNOWN_WIN is returned)
	Value SEE_Sign(Move move) const;
	//returns true if SideTo Move is in check. Must not be called when it's unclear whether opponent's attack map is already determined
	inline bool Checked() const { return (attackedByThem & PieceBB(KING, SideToMove)) != EMPTY; }
	//Static evaluation function for unusual material (no pre-calculated material values available in Material Table)
	friend Evaluation evaluateFromScratch(Position &pos);
	//Calls the static evaluation function (it will call the evaluation even, if the StaticEval value is already different from VALUE_NOTYETEVALUATED)
	inline Value evaluate();
	//Just for debugging
	std::string printEvaluation();
	//Evaluates final positions
	inline Value evaluateFinalPosition();
	inline int GeneratedMoveCount() const { return movepointer - 1; }
	//Returns the number of plies applied from the root position of the search
	inline int GetPliesFromRoot() const { return pliesFromRoot; }
	inline Color GetSideToMove() const { return SideToMove; }
	inline Piece GetPieceOnSquare(Square square) const { return Board[square]; }
	inline Square GetEPSquare() const { return EPSquare; }
	//gives access to the moves of the currently processed stage
	inline ValuatedMove * GetMovesOfCurrentPhase() { return &moves[phaseStartIndex]; }
	//Within staged move generation this method returns the index of the current move within the current stage. This is needed for
	//updating the history table
	inline int GetMoveNumberInPhase() const { return moveIterationPointer; }
	inline Value GetMaterialScore() const { return material->Score(); }
	inline MaterialTableEntry * GetMaterialTableEntry() const { return material; }
	inline pawn::Entry * GetPawnEntry() const { return pawn; }
	inline Value GetPawnScore() const { return pawn->Score.getScore(material->Phase); }
	inline void InitMaterialPointer() { material = probe(MaterialKey); }
	inline Eval PawnStructureScore() const { return pawn->Score; }
	//checks if the position is final and returns the result
	Result GetResult();
	//for xboard protocol support it's helpful, to not only know that a position is a DRAW but also why it's a draw. Therefore this additional
	//method will return a more detailed result value
	DetailedResult GetDetailedResult();
	//returns a bitboard indicating the squares attacked by the piece on the given square 
	inline Bitboard GetAttacksFrom(Square square) const { return attacks[square]; }
	inline void SetPrevious(Position &pos) { previous = &pos; }
	inline void SetPrevious(Position *pos) { previous = pos; }
	//Should be called before search starts
	inline void ResetPliesFromRoot() { pliesFromRoot = 0; }
	inline Bitboard AttacksByPieceType(Color color, PieceType pieceType) const;
	inline Bitboard AttacksExcludingPieceType(Color color, PieceType excludedPieceType) const;
	inline Bitboard AttacksByColor(Color color) const { return (SideToMove == color) * attackedByUs + (SideToMove != color) * attackedByThem; }
	inline Bitboard AttackedByThem() const { return attackedByThem; }
	inline Bitboard AttackedByUs() const { return attackedByUs; }
	//checks if the position is already repeated (if one of the ancestors has the same zobrist hash). This is no check for 3-fold repetition!
	bool checkRepetition() const;
	//checks if there are any repetitions in prior moves
	bool hasRepetition() const;
	inline void SwitchSideToMove() { SideToMove = Color(SideToMove ^ 1); Hash ^= ZobristMoveColor; }
	inline unsigned char GetDrawPlyCount() const { return DrawPlyCount; }
	//applies a null move to the given position (there is no copy/make for null move), the EPSquare is the only information which has to be restored afterwards
	void NullMove(Square epsquare = OUTSIDE, Move lastApplied = MOVE_NONE);
	//delete all ancestors of the current positions and frees the assigned memory
	void deleteParents();
	//returns the last move applied, which lead to this position
	inline Move GetLastAppliedMove() const { return lastAppliedMove; }
	//get's the piece, which moved in the last applied move
	inline Piece GetPreviousMovingPiece() const { if (previous && lastAppliedMove != MOVE_NONE) return previous->GetPieceOnSquare(to(lastAppliedMove)); else return BLANK; }
	//returns the piece, which has been captured by the last applied move
	inline Piece getCapturedInLastMove() const { if (lastAppliedMove == MOVE_NONE) return Piece::BLANK; else return capturedInLastMove; }
	//checks if a move is quiet (move is neither capture, nor promotion)
	inline bool IsQuiet(const Move move) const { return (Board[to(move)] == BLANK) && (type(move) == NORMAL || type(move) == CASTLING); }
	//checks if a move is quiet and not a castling move (used in search for pruning decisions)
	inline bool IsQuietAndNoCastles(const Move move) const { return type(move) == NORMAL && Board[to(move)] == BLANK; }
	//checks if a move is a tactical (cpture or promotion) move
	inline bool IsTactical(const Move& move) const { return Board[to(move)] != BLANK || type(move) == ENPASSANT || type(move) == PROMOTION; }
	inline bool IsTactical(const ValuatedMove& move) const { return IsTactical(move.move); }
	inline bool IsCapture(const Move& move) const { return Board[to(move)] != BLANK || type(move) == ENPASSANT; }
	//checks if a move is a winning capture (winning here includes equal captures and promotions, NxB and BxN are included as well) - Currently not used!
	inline bool IsWinningCapture(const ValuatedMove& move) const;
	//returns the current value of StaticEval - doesn't check if evaluate has been executed
	inline Value GetStaticEval() const { return StaticEval; }
	inline void SetStaticEval(Value evaluation) { StaticEval = evaluation; }
	inline PieceType GetMostValuablePieceType(Color col) const;
	inline PieceType GetMostValuableAttackedPieceType() const;
	inline bool PawnOn7thRank() { return (PieceBB(PAWN, SideToMove) & RANKS[6 - 5 * SideToMove]) != 0; } //Side to Move has pawn on 7th Rank
	inline bool IsAdvancedPawnPush(Move move) const { return GetPieceType(Board[from(move)]) == PAWN && (SideToMove == WHITE ? to(move) >= A5 : to(move) <= H4); }
	/*Make a full copy of the current position. To get a full copy first the copy constructor has to be called and then method copy has to be called:
		position copiedPosition(pos);
		copiedPosition.copy(pos);
	  This method should always be used when a copy is neede without applying a move */
	void copy(const Position &pos);
	inline bool CastlingAllowed(CastleFlag castling) const { return (CastlingOptions & castling) != 0; }
	inline unsigned GetCastles() const { return CastlingOptions & 15; }
	inline CastleFlag GetCastlesForColor(Color color) const { return static_cast<CastleFlag>(CastlingOptions & ((W0_0 | W0_0_0) << (2 * color))); }
	inline bool HasCastlingLost(const Color col) const { return (CastlingOptions & (CastleFlag::W_LOST_CASTLING << (int)col)) != 0; }
	//creates the SAN (standard algebraic notation) representation of a move
	std::string toSan(Move move);
	//parses a move in SAN notation
	Move parseSan(std::string move);
	Move GetCounterMove(Move(&counterMoves)[12][64]);
	inline bool Improved() { return previous == nullptr || previous->previous == nullptr || StaticEval >= previous->previous->StaticEval; }
	inline bool Worsening() { return previous != nullptr && previous->previous != nullptr && StaticEval <= previous->previous->StaticEval - Value(10); }
	//During staged move generation first only queen promotions are generated. When all other moves are generated and processed under promotions will be added
	void AddUnderPromotions();
	inline ValuatedMove * GetMoves(int & moveCount) { moveCount = movepointer - 1; return moves; }
	inline void ResetMoveGeneration() { movepointer = 0; moves[0].move = MOVE_NONE; moves[0].score = VALUE_NOTYETDETERMINED; }
	//Some moves (like moves from transposition tables) have to be validated (checked for legality) before being applied
	bool validateMove(Move move);
	//For pruning decisions it's necessary to identify whether or not all special movee (like killer,..) are already returned
	inline bool QuietMoveGenerationPhaseStarted() const { return generationPhases[generationPhase] >= QUIETS_POSITIVE; }
	inline bool MoveGenerationPhasePassed(MoveGenerationType phase) const { return (processedMoveGenerationPhases & (1 << (int)phase)) != 0; }
	inline MoveGenerationType GetMoveGenerationPhase() const { return generationPhases[generationPhase]; }
	//Validate a move and return it if validated, else return another valid move
	Move validMove(Move proposedMove);
	//Checks if a move gives check
	bool givesCheck(Move move);
	//Get Pinned Pieces
	inline Bitboard PinnedPieces(Color colorOfKing) const { return bbPinned[colorOfKing]; }
	void CalculatePinnedPieces();
	inline Square KingSquare(Color color) const { return kingSquares[color]; }
	//Check for opposite colored bishops
	bool oppositeColoredBishops() const;
	//Check if there is a mate threat (a possibility that there might be a quiet move giving mate)
	bool mateThread() const;
	//Bitboard of squares attacked by more than one piece
	inline Bitboard dblAttacks(Color color) const { return dblAttacked[color]; }
	//CHeck if kings are on opposed wings 
	inline bool KingOnOpposedWings() const { return ((CastlingOptions & 15) == 0) && std::abs((kingSquares[WHITE] & 7) - (kingSquares[BLACK] & 7)) > 2; }
	Bitboard BatteryAttacks(Color attacking_color) const;
	inline bool IsKvK() const { return MaterialKey == MATERIAL_KEY_KvK; }

private:
	//These are the members, which are copied by the copy constructor and contain the full information
	Bitboard OccupiedByColor[2];
	Bitboard OccupiedByPieceType[6];
	//Zobrist hash hey
	uint64_t Hash = ZobristMoveColor;
	//Material table key
	MaterialKey_t MaterialKey;
	//Pawn hash key
	PawnKey_t PawnKey;
	Square EPSquare;
	//Castling Options (see enum in types.h)
	unsigned char CastlingOptions;
	unsigned char DrawPlyCount;
	Color SideToMove;
	int pliesFromRoot;
	Piece Board[64];
	Eval PsqEval;
	//King Squares
	Square kingSquares[2];

	//Pointer to the previous position
	Position * previous = nullptr;

	//These members are only calculated when needed
	//Pointer to the relevant entry in the Material table
	MaterialTableEntry * material;
	//Pointer to the relevant entry in the pawn hash table
	pawn::Entry * pawn;
	//array to store the generated moves
	ValuatedMove moves[MAX_MOVE_COUNT];
	//number of generated moves in moves array
	int movepointer = 0;
	//Attack array - index is Square number, value is a bitboard indicating all squares attacked by a piece on that square
	Bitboard attacks[64];
	//Attack bitboard containing all squares attacked by the side not to move
	Bitboard attackedByThem;
	//Attack bitboard containing all squares attacked by the side to move
	Bitboard attackedByUs;
	//Attack bitboard containing all squares attacked by at least 2 pieces (indexed by attacking color)
	Bitboard dblAttacked[2] = { EMPTY, EMPTY };
	//Attack bitboard containing all attacks by a certain Piece Type
	Bitboard attacksByPt[12];
	//Bitboards of pieces pinned to king of given Color: bbPinned[0] contains white an black pieces "pinned" to white king
	Bitboard bbPinned[2] = { EMPTY, EMPTY };
	Bitboard bbPinner[2];

	//indices needed to manage staged move generation
	int moveIterationPointer;
	int phaseStartIndex;
	int generationPhase;
	//Result of position (value will be OPEN unless position is final)
	Result result = Result::RESULT_UNKNOWN;
	//Static evaluation of position
	Value StaticEval = VALUE_NOTYETDETERMINED;
	//Information needed for move ordering during staged move generation
	Move hashMove = MOVE_NONE;
	killer::Manager *killerManager;
	Move lastAppliedMove = MOVE_NONE;
	Piece capturedInLastMove = BLANK;
	HistoryManager * history;
	MoveSequenceHistoryManager * cmHistory;
	MoveSequenceHistoryManager * followupHistory;
	Move counterMove = MOVE_NONE;
	ValuatedMove * firstNegative;
	bool canPromote = false;
	uint32_t processedMoveGenerationPhases;
	//Place a piece on Squarre square and update bitboards and Hash key
	template<bool SquareIsEmpty> void set(const Piece piece, const Square square);
	void remove(const Square square);

	inline uint64_t GetMaterialHashUnusual() const {
		uint64_t mhash = 0;
		for (int c = 0; c <= 1; ++c) {
			for (int pt = 0; pt <= 5; ++pt) {
				mhash ^= ZobristKeys[2 * pt + c][popcount(PieceBB(static_cast<PieceType>(pt), static_cast<Color>(c)))];
			}
		}
		return mhash;
	}
	inline void AddCastlingOption(const CastleFlag castleFlag) { Hash ^= ZobristCastles[CastlingOptions & 15]; CastlingOptions |= castleFlag; Hash ^= ZobristCastles[CastlingOptions & 15]; }
	inline void RemoveCastlingOption(const CastleFlag castleFlag) { Hash ^= ZobristCastles[CastlingOptions & 15]; CastlingOptions &= ~castleFlag; Hash ^= ZobristCastles[CastlingOptions & 15]; }
	inline void SetCastlingLost(const Color col) { CastlingOptions |= CastleFlag::W_LOST_CASTLING << (int)col; }
	inline void SetEPSquare(const Square square) {
		if (EPSquare != square) {
			if (EPSquare != OUTSIDE) {
				Hash ^= ZobristEnPassant[EPSquare & 7];
				EPSquare = square;
				if (EPSquare != OUTSIDE)
					Hash ^= ZobristEnPassant[EPSquare & 7];
			}
			else {
				EPSquare = square;
				Hash ^= ZobristEnPassant[EPSquare & 7];
			}
		}
	}
	inline int PawnStep() const { return 8 - 16 * SideToMove; }
	//Add a move to the move list and increment movepointer
	inline void AddMove(Move move) {
		moves[movepointer].move = move;
		moves[movepointer].score = VALUE_NOTYETDETERMINED;
		++movepointer;
	}
	//Adds MOVE_NONE at the end of the move list
	inline void AddNullMove() { moves[movepointer].move = MOVE_NONE; moves[movepointer].score = VALUE_NOTYETDETERMINED; ++movepointer; }
	//Updates Castle Flags after a move from fromSquare to toSquare has been applied, must not be called for castling moves
	void updateCastleFlags(Square fromSquare, Square toSquare);
	//Calculates the attack bitboards for all pieces of one side
	Bitboard calculateAttacks(Color color);
	//Calculates Bitboards of pieces blocking a check. If colorOfBlocker = kingColor, these are the pinned pieces, else these are candidates for discovered checks
	Bitboard checkBlocker(Color colorOfBlocker, Color kingColor);
	//Calculates the material key of this position
	MaterialKey_t calculateMaterialKey() const;
	//Calculates the Pawn Key of this position
	PawnKey_t calculatePawnKey() const;
	//different move evaluation methods (used for move ordering):
	void evaluateByCaptureScore(int startIndex = 0);
	void evaluateByMVVLVA(int startIndex = 0);
	void evaluateBySEE(int startIndex);
	void evaluateCheckEvasions(int startIndex);
	void evaluateByHistory(int startIndex);
	//Get's the best evaluated move from the move list starting at start Index
	Move getBestMove(int startIndex);
	void insertionSort(ValuatedMove* begin, ValuatedMove* end);
	void shellSort(ValuatedMove* vm, int count);
	PieceType getAndResetLeastValuableAttacker(Square toSquare, Bitboard stmAttackers, Bitboard& occupied, Bitboard& attackers, Bitboard& mayXray) const;
	//return a bitboard of squares with pieces attacking the targetField
	Bitboard AttacksOfField(const Square targetField, const Bitboard occupied) const;
	Bitboard AttacksOfField(const Square targetField, const Color attackingSide) const;
	//Checks is a oseudo-legal move is valid
	inline bool isValid(Move move) { Position next(*this); return next.ApplyMove(move); }
	//Checks if a move (e.g. from killer move list) is a valid move
	bool validateMove(ExtendedMove move);
	//Checks if at least one valid move exists - ATTENTION must not be called on a newly initialized position where attackedByThem isn't calculated yet!!
	template<bool CHECKED> bool CheckValidMoveExists();
	//Checks for unusual Material (this means one side has more than one Queen or more than 2 rooks, knights or bishop)
	bool checkMaterialIsUnusual() const;
	//Generates quiet and tactical moves like forks
	ValuatedMove* GenerateForks(bool withChecks);

};

template<> inline ValuatedMove* Position::GenerateMoves<QUIET_CHECKS>();
template<> inline ValuatedMove* Position::GenerateMoves<LEGAL>();
template<> inline ValuatedMove* Position::GenerateMoves<FORKS>();
template<> inline ValuatedMove* Position::GenerateMoves<FORKS_NO_CHECKS>();

Move parseMoveInUCINotation(const std::string& uciMove, const Position& pos);


inline Bitboard Position::PieceBB(const PieceType pt, const Color c) const { return OccupiedByColor[c] & OccupiedByPieceType[pt]; }
inline Bitboard Position::ColorBB(const Color c) const { return OccupiedByColor[c]; }
inline Bitboard Position::ColorBB(const int c) const { return OccupiedByColor[c]; }
inline Bitboard Position::OccupiedBB() const { return OccupiedByColor[WHITE] | OccupiedByColor[BLACK]; }
inline Bitboard Position::PieceTypeBB(const PieceType pt) const { return OccupiedByPieceType[pt]; }

inline bool Position::IsWinningCapture(const ValuatedMove& move) const {
	return (Board[to(move.move)] != BLANK && (settings::parameter.PieceValues[GetPieceType(Board[from(move.move)])].mgScore - settings::parameter.PieceValues[GetPieceType(Board[to(move.move)])].mgScore) < settings::parameter.PieceValues[PAWN].mgScore)
		|| type(move.move) == ENPASSANT || type(move.move) == PROMOTION;
}

inline PieceType Position::GetMostValuablePieceType(Color color) const {
	if (MaterialKey != MATERIAL_KEY_UNUSUAL) return material->GetMostExpensivePiece(color);
	else {
		for (PieceType pt = QUEEN; pt < KING; ++pt) {
			if (PieceBB(pt, color)) {
				assert(MaterialKey == MATERIAL_KEY_UNUSUAL || pt == material->GetMostExpensivePiece(color));
				return pt;
			}
		}
		return KING;
	}
}

inline PieceType Position::GetMostValuableAttackedPieceType() const {
	Color col = Color(SideToMove ^ 1);
	PieceType ptstart = MaterialKey != MATERIAL_KEY_UNUSUAL ? material->GetMostExpensivePiece(col) : QUEEN;
	for (PieceType pt = ptstart; pt < KING; ++pt) {
		if (PieceBB(pt, col) & attackedByUs) return pt;
	}
	return KING;
}

inline Value Position::evaluate() {
	if (StaticEval != VALUE_NOTYETDETERMINED)
		return StaticEval;
	if (GetResult() == Result::OPEN) {
		 return StaticEval = material->EvaluationFunction(*this) + settings::parameter.BONUS_TEMPO.getScore(material->Phase);
	}
	else if (result == Result::DRAW) return StaticEval = VALUE_DRAW;
	else
		return StaticEval = Value((2 - int(result)) * (VALUE_MATE - pliesFromRoot));
}

inline Value Position::evaluateFinalPosition() {
	if (result == Result::DRAW) return VALUE_DRAW;
	else
		return Value((2 - int(result)) * (VALUE_MATE - pliesFromRoot));
}

//Tries to find one valid move as fast as possible
template<bool CHECKED> bool Position::CheckValidMoveExists() {
	assert(attackedByThem); //should have been already calculated
	//Start with king (Castling need not be considered - as there is always another legal move available with castling
	//In Chess960 this might be different)
	Square kingSquare = kingSquares[SideToMove];
	Bitboard kingTargets = KingAttacks[kingSquare] & ~OccupiedByColor[SideToMove] & ~attackedByThem;
	if (CHECKED && kingTargets) {
		if (popcount(kingTargets) > 2) return true; //unfortunately 8/5p2/5kp1/8/4p3/R4n2/1r3K2/4q3 w - - shows that king itself can even block 2 sliders
		else {
			//unfortunately king could be "blocker" e.g. in 8/8/1R2k3/K7/8/8/8/8 w square f6 is not attacked by white
			//however if king moves to f6 it's still check
			Square to = lsb(kingTargets);
			kingTargets &= kingTargets - 1;
			if (isValid(createMove(kingSquare, to)) || (kingTargets && isValid(createMove(kingSquare, lsb(kingTargets))))) return true;
		}
	}
	else if (kingTargets) return true; //No need to check
	if (CHECKED) {
		Bitboard checker = AttacksOfField(kingSquare, Color(SideToMove ^ 1));
		if (popcount(checker) != 1) return false; //double check and no king move => MATE
		//All valid moves are now either capturing the checker or blocking the check
		Bitboard blockingSquares = checker | InBetweenFields[kingSquare][lsb(checker)];
		Bitboard pinned = checkBlocker(SideToMove, SideToMove);
		//Sliders and knights can't move if pinned (as we are in check) => therefore only check the unpinned pieces
		Bitboard sliderAndKnight = OccupiedByColor[SideToMove] & ~OccupiedByPieceType[KING] & ~OccupiedByPieceType[PAWN] & ~pinned;
		while (sliderAndKnight) {
			if (attacks[lsb(sliderAndKnight)] & ~OccupiedByColor[SideToMove] & blockingSquares) return true;
			sliderAndKnight &= sliderAndKnight - 1;
		}
		//Pawns
		Bitboard singleStepTargets;
		Bitboard pawns = PieceBB(PAWN, SideToMove) & ~pinned;
		if (SideToMove == WHITE) {
			if ((singleStepTargets = ((pawns << 8) & ~OccupiedBB())) & blockingSquares) return true;
			if (((singleStepTargets & RANK3) << 8) & ~OccupiedBB() & blockingSquares) return true;
		}
		else {
			if ((singleStepTargets = ((pawns >> 8) & ~OccupiedBB())) & blockingSquares) return true;
			if (((singleStepTargets&RANK6) >> 8) & ~OccupiedBB() & blockingSquares) return true;
		}
		//Pawn captures
		while (pawns) {
			Square from = lsb(pawns);
			if (checker & attacks[from]) return true;
			pawns &= pawns - 1;
		}
	}
	else {
		//Now we need the pinned pieces
		Bitboard pinned = checkBlocker(SideToMove, SideToMove);
		//Now first check all unpinned pieces
		Bitboard sliderAndKnight = OccupiedByColor[SideToMove] & ~OccupiedByPieceType[KING] & ~OccupiedByPieceType[PAWN] & ~pinned;
		while (sliderAndKnight) {
			if (attacks[lsb(sliderAndKnight)] & ~OccupiedByColor[SideToMove]) return true;
			sliderAndKnight &= sliderAndKnight - 1;
		}
		//Pawns
		Bitboard pawns = PieceBB(PAWN, SideToMove) & ~pinned;
		Bitboard pawnTargets;
		//normal pawn move
		if (SideToMove == WHITE) pawnTargets = (pawns << 8) & ~OccupiedBB(); else pawnTargets = (pawns >> 8) & ~OccupiedBB();
		if (pawnTargets) return true;
		//pawn capture
		if (SideToMove == WHITE) pawnTargets = (((pawns << 9) & NOT_A_FILE) | ((pawns << 7) & NOT_H_FILE)) & OccupiedByColor[SideToMove ^ 1];
		else pawnTargets = (((pawns >> 9) & NOT_H_FILE) | ((pawns >> 7) & NOT_A_FILE)) & OccupiedByColor[SideToMove ^ 1];
		if (pawnTargets) return true;
		//Now let's deal with pinned pieces
		Bitboard pinnedSlider = (PieceBB(QUEEN, SideToMove) | PieceBB(ROOK, SideToMove) | PieceBB(BISHOP, SideToMove)) & pinned;
		while (pinnedSlider) {
			Square from = lsb(pinnedSlider);
			if (attacks[from] & (InBetweenFields[from][kingSquare] | ShadowedFields[kingSquare][from])) return true;
			pinnedSlider &= pinnedSlider - 1;
		}
		//pinned knights must not move and pinned kings don't exist => remains pinned pawns
		Bitboard pinnedPawns = PieceBB(PAWN, SideToMove) & pinned;
		Bitboard pinnedPawnsAllowedToMove = pinnedPawns & FILES[kingSquare & 7];
		if (pinnedPawnsAllowedToMove) {
			if (SideToMove == WHITE && ((pinnedPawnsAllowedToMove << 8) & ~OccupiedBB())) return true;
			else if (SideToMove == BLACK && ((pinnedPawnsAllowedToMove >> 8) & ~OccupiedBB())) return true;
		}
		//Now there remains only pinned pawn captures
		while (pinnedPawns) {
			Square from = lsb(pinnedPawns);
			pawnTargets = ColorBB(SideToMove ^ 1) & attacks[from];
			while (pawnTargets) {
				if (isolateLSB(pawnTargets) & (InBetweenFields[from][kingSquare] | ShadowedFields[kingSquare][from])) return true;
				pawnTargets &= pawnTargets - 1;
			}
			pinnedPawns &= pinnedPawns - 1;
		}
	}
	//ep-captures are difficult as 3 squares are involved => therefore simply apply and check it
	Bitboard epAttacker;
	if (EPSquare != OUTSIDE && (epAttacker = (GetEPAttackersForToField(EPSquare - PawnStep()) & PieceBB(PAWN, SideToMove)))) {
		while (epAttacker) {
			if (isValid(createMove<ENPASSANT>(lsb(epAttacker), EPSquare))) return true;
			epAttacker &= epAttacker - 1;
		}
	}
	return false;
}


// Generates all legal moves (by first generating all pseudo-legal moves and then eliminating all invalid moves
//Should only be used at the root as implementation is slow!
template<> ValuatedMove* Position::GenerateMoves<LEGAL>() {
	GenerateMoves<ALL>();
	for (int i = 0; i < movepointer - 1; ++i) {
		if (!isValid(moves[i].move)) {
			moves[i] = moves[movepointer - 2];
			--movepointer;
			--i;
		}
	}
	return &moves[0];
}

//Generates all quiet moves giving check
template<> ValuatedMove* Position::GenerateMoves<QUIET_CHECKS>() {
	movepointer -= (movepointer != 0);
	ValuatedMove * firstMove = &moves[movepointer];
	//There are 2 options to give check: Either give check with the moving piece, or a discovered check by
	//moving a check blocking piece
	Square opposedKingSquare = kingSquares[SideToMove ^ 1];
	//1. Discovered Checks
	Bitboard discoveredCheckCandidates = checkBlocker(SideToMove, Color(SideToMove ^ 1));
	Bitboard targets = ~OccupiedBB();
	//1a Sliders
	Bitboard sliders = (PieceBB(ROOK, SideToMove) | PieceBB(QUEEN, SideToMove) | PieceBB(BISHOP, SideToMove)) & discoveredCheckCandidates;
	while (sliders) {
		Square from = lsb(sliders);
		Bitboard sliderTargets = attacks[from] & targets & (~InBetweenFields[opposedKingSquare][from] & ~ShadowedFields[opposedKingSquare][from]);
		while (sliderTargets) {
			AddMove(createMove(from, lsb(sliderTargets)));
			sliderTargets &= sliderTargets - 1;
		}
		sliders &= sliders - 1;
	}
	//1b Knights
	Bitboard knights = PieceBB(KNIGHT, SideToMove) & discoveredCheckCandidates;
	while (knights) {
		Square from = lsb(knights);
		Bitboard knightTargets = KnightAttacks[from] & targets;
		while (knightTargets) {
			AddMove(createMove(from, lsb(knightTargets)));
			knightTargets &= knightTargets - 1;
		}
		knights &= knights - 1;
	}
	//1c Kings
	Square kingSquare = kingSquares[SideToMove];
	if (discoveredCheckCandidates & PieceBB(KING, SideToMove)) {
		Bitboard kingTargets = KingAttacks[kingSquare] & ~attackedByThem & targets & (~InBetweenFields[opposedKingSquare][kingSquare] & ~ShadowedFields[opposedKingSquare][kingSquare]);
		while (kingTargets) {
			AddMove(createMove(kingSquare, lsb(kingTargets)));
			kingTargets &= kingTargets - 1;
		}
	}
	//1d Pawns
	Bitboard pawns = PieceBB(PAWN, SideToMove) & discoveredCheckCandidates;
	if (pawns) {
		Bitboard singleStepTargets;
		Bitboard doubleStepTargets;
		if (SideToMove == WHITE) {
			singleStepTargets = (pawns << 8) & ~RANK8 & targets;
			doubleStepTargets = ((singleStepTargets & RANK3) << 8) & targets;
		}
		else {
			singleStepTargets = (pawns >> 8) & ~RANK1 & targets;
			doubleStepTargets = ((singleStepTargets & RANK6) >> 8) & targets;
		}
		while (singleStepTargets) {
			Square to = lsb(singleStepTargets);
			Square from = Square(to - PawnStep());
			Bitboard stillBlocking = InBetweenFields[from][opposedKingSquare] | ShadowedFields[opposedKingSquare][from];
			if (!(stillBlocking & ToBitboard(to))) AddMove(createMove(from, to));
			singleStepTargets &= singleStepTargets - 1;
		}
		while (doubleStepTargets) {
			Square to = lsb(doubleStepTargets);
			Square from = Square(to - 2 * PawnStep());
			Bitboard stillBlocking = InBetweenFields[from][opposedKingSquare] | ShadowedFields[opposedKingSquare][from];
			if (!(stillBlocking & ToBitboard(to))) AddMove(createMove(from, to));
			doubleStepTargets &= doubleStepTargets - 1;
		}
	}
	//2. Normal checks
	//2a "Rooks"
	Bitboard rookAttackstoKing = RookTargets(opposedKingSquare, OccupiedBB()) & targets;
	Bitboard rooks = (PieceBB(ROOK, SideToMove) | PieceBB(QUEEN, SideToMove)) & ~discoveredCheckCandidates;
	while (rooks) {
		Square from = lsb(rooks);
		Bitboard rookTargets = attacks[from] & rookAttackstoKing;
		while (rookTargets) {
			AddMove(createMove(from, lsb(rookTargets)));
			rookTargets &= rookTargets - 1;
		}
		rooks &= rooks - 1;
	}
	//2b "Bishops"
	Bitboard bishopAttackstoKing = BishopTargets(opposedKingSquare, OccupiedBB()) & targets;
	Bitboard bishops = (PieceBB(BISHOP, SideToMove) | PieceBB(QUEEN, SideToMove)) & ~discoveredCheckCandidates;
	while (bishops) {
		Square from = lsb(bishops);
		Bitboard bishopTargets = attacks[from] & bishopAttackstoKing;
		while (bishopTargets) {
			AddMove(createMove(from, lsb(bishopTargets)));
			bishopTargets &= bishopTargets - 1;
		}
		bishops &= bishops - 1;
	}
	//2c Knights
	Bitboard knightAttacksToKing = KnightAttacks[opposedKingSquare];
	knights = PieceBB(KNIGHT, SideToMove) & ~discoveredCheckCandidates;
	while (knights) {
		Square from = lsb(knights);
		Bitboard knightTargets = KnightAttacks[from] & knightAttacksToKing & targets;
		while (knightTargets) {
			AddMove(createMove(from, lsb(knightTargets)));
			knightTargets &= knightTargets - 1;
		}
		knights &= knights - 1;
	}
	//2d Pawn
	Bitboard pawnTargets;
	Bitboard pawnFrom;
	Bitboard dblPawnFrom;
	if (SideToMove == WHITE) {
		pawnTargets = targets & (((PieceBB(KING, Color(SideToMove ^ 1)) >> 9) & NOT_H_FILE) | ((PieceBB(KING, Color(SideToMove ^ 1)) >> 7) & NOT_A_FILE));
		pawnFrom = pawnTargets >> 8;
		dblPawnFrom = ((pawnFrom & targets & RANK3) >> 8) & PieceBB(PAWN, WHITE);
		pawnFrom &= PieceBB(PAWN, WHITE);
	}
	else {
		pawnTargets = targets & (((PieceBB(KING, Color(SideToMove ^ 1)) << 7) & NOT_H_FILE) | ((PieceBB(KING, Color(SideToMove ^ 1)) << 9) & NOT_A_FILE));
		pawnFrom = pawnTargets << 8;
		dblPawnFrom = ((pawnFrom & targets & RANK6) << 8) & PieceBB(PAWN, BLACK);
		pawnFrom &= PieceBB(PAWN, BLACK);
	}
	while (pawnFrom) {
		Square from = lsb(pawnFrom);
		AddMove(createMove(from, from + PawnStep()));
		pawnFrom &= pawnFrom - 1;
	}
	while (dblPawnFrom) {
		Square from = lsb(dblPawnFrom);
		AddMove(createMove(from, from + 2 * PawnStep()));
		dblPawnFrom &= dblPawnFrom - 1;
	}
	//2e Castles
	if (CastlingOptions & 15 & CastlesbyColor[SideToMove]) //King is on initial square
	{
		//King-side castles
		if ((CastlingOptions & 15 & (1 << (2 * SideToMove))) //Short castle allowed
			&& (InitialRookSquareBB[2 * SideToMove] & PieceBB(ROOK, SideToMove)) //Rook on initial square
			&& !(SquaresToBeEmpty[2 * SideToMove] & OccupiedBB()) //Fields between Rook and King are empty
			&& !(SquaresToBeUnattacked[2 * SideToMove] & attackedByThem) //Fields passed by the king are unattacked
			&& (RookTargets(opposedKingSquare, ~targets & ~PieceBB(KING, SideToMove)) & RookSquareAfterCastling[2 * SideToMove])) //Rook is giving check after castling
		{
			if (Chess960) AddMove(createMove<CASTLING>(kingSquare, InitialRookSquare[2 * SideToMove])); else AddMove(createMove<CASTLING>(kingSquare, Square(G1 + SideToMove * 56)));
		}
		//Queen-side castles
		if ((CastlingOptions & 15 & (1 << (2 * SideToMove + 1))) //Short castle allowed
			&& (InitialRookSquareBB[2 * SideToMove + 1] & PieceBB(ROOK, SideToMove)) //Rook on initial square
			&& !(SquaresToBeEmpty[2 * SideToMove + 1] & OccupiedBB()) //Fields between Rook and King are empty
			&& !(SquaresToBeUnattacked[2 * SideToMove + 1] & attackedByThem) //Fields passed by the king are unattacked
			&& (RookTargets(opposedKingSquare, ~targets & ~PieceBB(KING, SideToMove)) & RookSquareAfterCastling[2 * SideToMove + 1])) //Rook is giving check after castling
		{
			if (Chess960) AddMove(createMove<CASTLING>(kingSquare, InitialRookSquare[2 * SideToMove + 1])); else AddMove(createMove<CASTLING>(kingSquare, Square(C1 + SideToMove * 56)));
		}
	}
	AddNullMove();
	return firstMove;
}

template<> inline ValuatedMove* Position::GenerateMoves<FORKS>() {
	return GenerateForks(true);
}

template<> inline ValuatedMove* Position::GenerateMoves<FORKS_NO_CHECKS>() {
	return GenerateForks(false);
}

template<MoveGenerationType MGT> ValuatedMove * Position::GenerateMoves() {
	if (MGT == ALL || MGT == CHECK_EVASION) movepointer = 0; else movepointer -= (movepointer != 0);
	ValuatedMove * firstMove = &moves[movepointer];
	//Rooksliders
	Bitboard targets;
	if (MGT == ALL || MGT == TACTICAL || MGT == QUIETS || MGT == CHECK_EVASION) {
		Square kingSquare = kingSquares[SideToMove];
		Bitboard checkBlocker = 0;
		bool doubleCheck = false;
		if (MGT == CHECK_EVASION) {
			Bitboard checker = AttacksOfField(kingSquare, Color(SideToMove ^ 1));
			if (popcount(checker) == 1) {
				checkBlocker = checker | InBetweenFields[kingSquare][lsb(checker)];
			}
			else doubleCheck = true;
		}
		Bitboard empty = ~ColorBB(BLACK) & ~ColorBB(WHITE);
		if (MGT == ALL) targets = ~ColorBB(SideToMove);
		else if (MGT == TACTICAL) targets = ColorBB(SideToMove ^ 1);
		else if (MGT == QUIETS) targets = empty;
		else if (MGT == CHECK_EVASION && !doubleCheck) targets = ~ColorBB(SideToMove) & checkBlocker;
		else targets = 0;
		//Kings
		Bitboard kingTargets;
		if (MGT == CHECK_EVASION) kingTargets = KingAttacks[kingSquare] & ~OccupiedByColor[SideToMove] & ~attackedByThem;
		else kingTargets = KingAttacks[kingSquare] & targets & ~attackedByThem;
		while (kingTargets) {
			AddMove(createMove(kingSquare, lsb(kingTargets)));
			kingTargets &= kingTargets - 1;
		}
		if (MGT == ALL || MGT == QUIETS) {
			if (CastlingOptions & 15 & CastlesbyColor[SideToMove]) //King is on initial square
			{
				//King-side castles
				if ((CastlingOptions & 15 & (1 << (2 * SideToMove))) //Short castle allowed
					&& (InitialRookSquareBB[2 * SideToMove] & PieceBB(PieceType::ROOK, SideToMove)) //Rook on initial square
					&& !(SquaresToBeEmpty[2 * SideToMove] & OccupiedBB()) //Fields between Rook and King are empty
					&& !(SquaresToBeUnattacked[2 * SideToMove] & attackedByThem)) //Fields passed by the king are unattacked
				{
					if (Chess960) AddMove(createMove<CASTLING>(kingSquare, InitialRookSquare[2 * SideToMove]));
					else AddMove(createMove<CASTLING>(kingSquare, Square(G1 + SideToMove * 56)));
				}
				//Queen-side castles
				if ((CastlingOptions & 15 & (1 << (2 * SideToMove + 1))) //Short castle allowed
					&& (InitialRookSquareBB[2 * SideToMove + 1] & PieceBB(ROOK, SideToMove)) //Rook on initial square
					&& !(SquaresToBeEmpty[2 * SideToMove + 1] & OccupiedBB()) //Fields between Rook and King are empty
					&& !(SquaresToBeUnattacked[2 * SideToMove + 1] & attackedByThem)) //Fields passed by the king are unattacked
				{
					if (Chess960) AddMove(createMove<CASTLING>(kingSquare, InitialRookSquare[2 * SideToMove + 1]));
					else AddMove(createMove<CASTLING>(kingSquare, Square(C1 + SideToMove * 56)));
				}
			}
		}
		if (!doubleCheck) {
			Bitboard sliders = PieceBB(PieceType::ROOK, SideToMove) | PieceBB(PieceType::QUEEN, SideToMove) | PieceBB(PieceType::BISHOP, SideToMove);
			while (sliders) {
				Square from = lsb(sliders);
				Bitboard sliderTargets = attacks[from] & targets;
				while (sliderTargets) {
					AddMove(createMove(from, lsb(sliderTargets)));
					sliderTargets &= sliderTargets - 1;
				}
				sliders &= sliders - 1;
			}
			//Knights
			Bitboard knights = PieceBB(KNIGHT, SideToMove);
			while (knights) {
				Square from = lsb(knights);
				Bitboard knightTargets;
				if (MGT == CHECK_EVASION) knightTargets = attacks[from] & targets & checkBlocker; else knightTargets = attacks[from] & targets;
				while (knightTargets) {
					AddMove(createMove(from, lsb(knightTargets)));
					knightTargets &= knightTargets - 1;
				}
				knights &= knights - 1;
			}
			//Pawns
			Bitboard pawns = PieceBB(PAWN, SideToMove);
			//Captures
			if (MGT == ALL || MGT == TACTICAL || MGT == CHECK_EVASION) {
				while (pawns) {
					Square from = lsb(pawns);
					Bitboard pawnTargets;
					if (MGT == CHECK_EVASION) pawnTargets = ColorBB(SideToMove ^ 1) & attacks[from] & ~RANK1and8 & checkBlocker; else pawnTargets = ColorBB(SideToMove ^ 1) & attacks[from] & ~RANK1and8;
					while (pawnTargets) {
						AddMove(createMove(from, lsb(pawnTargets)));
						pawnTargets &= pawnTargets - 1;
					}
					//Promotion Captures
					if (MGT == CHECK_EVASION) pawnTargets = ColorBB(SideToMove ^ 1) & attacks[from] & RANK1and8 & checkBlocker; else pawnTargets = ColorBB(SideToMove ^ 1) & attacks[from] & RANK1and8;
					while (pawnTargets) {
						Square to = lsb(pawnTargets);
						AddMove(createMove<PROMOTION>(from, to, QUEEN));
						canPromote = true;
						pawnTargets &= pawnTargets - 1;
					}
					pawns &= pawns - 1;
				}
			}
			Bitboard singleStepTarget = 0;
			Bitboard doubleSteptarget = 0;
			Bitboard promotionTarget = 0;
			if (SideToMove == WHITE) {
				if (MGT == ALL || MGT == QUIETS) {
					singleStepTarget = (PieceBB(PAWN, WHITE) << 8) & empty & ~RANK8;
					doubleSteptarget = ((singleStepTarget & RANK3) << 8) & empty;
				}
				else if (MGT == CHECK_EVASION) {
					singleStepTarget = (PieceBB(PAWN, WHITE) << 8) & empty & ~RANK8;
					doubleSteptarget = ((singleStepTarget & RANK3) << 8) & empty & checkBlocker;
					singleStepTarget &= checkBlocker;
				}
				if (MGT == ALL || MGT == TACTICAL) promotionTarget = (PieceBB(PAWN, WHITE) << 8) & empty & RANK8;
				else if (MGT == CHECK_EVASION) promotionTarget = (PieceBB(PAWN, WHITE) << 8) & empty & RANK8 & checkBlocker;
			}
			else {
				if (MGT == ALL || MGT == QUIETS) {
					singleStepTarget = (PieceBB(PAWN, BLACK) >> 8) & empty & ~RANK1;
					doubleSteptarget = ((singleStepTarget & RANK6) >> 8) & empty;
				}
				else if (MGT == CHECK_EVASION) {
					singleStepTarget = (PieceBB(PAWN, BLACK) >> 8) & empty & ~RANK1;
					doubleSteptarget = ((singleStepTarget & RANK6) >> 8) & empty & checkBlocker;
					singleStepTarget &= checkBlocker;
				}
				if (MGT == ALL || MGT == TACTICAL) promotionTarget = (PieceBB(PAWN, BLACK) >> 8) & empty & RANK1;
				else if (MGT == CHECK_EVASION) promotionTarget = (PieceBB(PAWN, BLACK) >> 8) & empty & RANK1 & checkBlocker;
			}
			if (MGT == ALL || MGT == QUIETS || MGT == CHECK_EVASION) {
				while (singleStepTarget) {
					Square to = lsb(singleStepTarget);
					AddMove(createMove(to - PawnStep(), to));
					singleStepTarget &= singleStepTarget - 1;
				}
				while (doubleSteptarget) {
					Square to = lsb(doubleSteptarget);
					AddMove(createMove(to - 2 * PawnStep(), to));
					doubleSteptarget &= doubleSteptarget - 1;
				}
			}
			//Promotions
			if (MGT == ALL || MGT == TACTICAL || MGT == CHECK_EVASION) {
				while (promotionTarget) {
					Square to = lsb(promotionTarget);
					Square from = Square(to - PawnStep());
					AddMove(createMove<PROMOTION>(from, to, QUEEN));
					canPromote = true;
					promotionTarget &= promotionTarget - 1;
				}
				Bitboard epAttacker;
				if (EPSquare != OUTSIDE && (epAttacker = (GetEPAttackersForToField(EPSquare - PawnStep()) & PieceBB(PAWN, SideToMove)))) {
					while (epAttacker) {
						AddMove(createMove<ENPASSANT>(lsb(epAttacker), EPSquare));
						epAttacker &= epAttacker - 1;
					}
				}
			}
		}
		if (MGT == ALL && canPromote) {
			int moveCount = movepointer;
			for (int i = 0; i < moveCount; ++i) {
				Move pmove = moves[i].move;
				if (type(pmove) == PROMOTION) {
					AddMove(createMove<PROMOTION>(from(pmove), to(pmove), KNIGHT));
					AddMove(createMove<PROMOTION>(from(pmove), to(pmove), ROOK));
					AddMove(createMove<PROMOTION>(from(pmove), to(pmove), BISHOP));
				}
			}
		}
	}
	else { //Winning, Equal and loosing captures
		Bitboard sliders;
		Bitboard hanging = ColorBB(Color(SideToMove ^ 1)) & ~AttackedByThem();
		if (MGT == WINNING_CAPTURES || MGT == NON_LOOSING_CAPTURES) {
			Bitboard promotionTarget;
			if (SideToMove == WHITE)  promotionTarget = (PieceBB(PAWN, WHITE) << 8) & ~OccupiedBB() & RANK8; else promotionTarget = (PieceBB(PAWN, BLACK) >> 8) & ~OccupiedBB() & RANK1;
			while (promotionTarget) {
				Square to = lsb(promotionTarget);
				Square from = Square(to - PawnStep());
				AddMove(createMove<PROMOTION>(from, to, QUEEN));
				canPromote = true;
				promotionTarget &= promotionTarget - 1;
			}
			//King Captures are always winning as kings can only capture uncovered pieces
			Square kingSquare = kingSquares[SideToMove];
			Bitboard kingTargets = KingAttacks[kingSquare] & ColorBB(SideToMove ^ 1) & ~attackedByThem;
			while (kingTargets) {
				AddMove(createMove(kingSquare, lsb(kingTargets)));
				kingTargets &= kingTargets - 1;
			}
		}
		//Pawn Captures
		if (MGT == WINNING_CAPTURES) {
			Bitboard pawns = PieceBB(PAWN, SideToMove);
			while (pawns) {
				Square from = lsb(pawns);
				Bitboard pawnTargets = ColorBB(SideToMove ^ 1) & (~PieceBB(PAWN, Color(SideToMove ^ 1)) | hanging) & attacks[from] & ~RANK1and8;
				while (pawnTargets) {
					AddMove(createMove(from, lsb(pawnTargets)));
					pawnTargets &= pawnTargets - 1;
				}
				//Promotion Captures
				pawnTargets = ColorBB(SideToMove ^ 1) & attacks[from] & RANK1and8;
				while (pawnTargets) {
					Square to = lsb(pawnTargets);
					AddMove(createMove<PROMOTION>(from, to, QUEEN));
					canPromote = true;
					pawnTargets &= pawnTargets - 1;
				}
				pawns &= pawns - 1;
			}
		}
		else if (MGT == EQUAL_CAPTURES) {
			Bitboard pawns = PieceBB(PAWN, SideToMove);
			while (pawns) {
				Square from = lsb(pawns);
				Bitboard pawnTargets = PieceBB(PAWN, Color(SideToMove ^ 1)) & attacks[from] & ~hanging;
				while (pawnTargets) {
					AddMove(createMove(from, lsb(pawnTargets)));
					pawnTargets &= pawnTargets - 1;
				}
				pawns &= pawns - 1;
			}
			Bitboard epAttacker;
			if (EPSquare != OUTSIDE && (epAttacker = (GetEPAttackersForToField(EPSquare - PawnStep()) & PieceBB(PAWN, SideToMove)))) {
				while (epAttacker) {
					AddMove(createMove<ENPASSANT>(lsb(epAttacker), EPSquare));
					epAttacker &= epAttacker - 1;
				}
			}
		}
		else if (MGT == NON_LOOSING_CAPTURES) {
			Bitboard pawns = PieceBB(PAWN, SideToMove);
			while (pawns) {
				Square from = lsb(pawns);
				Bitboard pawnTargets = ColorBB(SideToMove ^ 1) & attacks[from] & ~RANK1and8;
				while (pawnTargets) {
					AddMove(createMove(from, lsb(pawnTargets)));
					pawnTargets &= pawnTargets - 1;
				}
				//Promotion captures
				pawnTargets = ColorBB(SideToMove ^ 1) & attacks[from] & RANK1and8;
				while (pawnTargets) {
					Square to = lsb(pawnTargets);
					AddMove(createMove<PROMOTION>(from, to, QUEEN));
					canPromote = true;
					pawnTargets &= pawnTargets - 1;
				}
				pawns &= pawns - 1;
			}
			Bitboard epAttacker;
			if (EPSquare != OUTSIDE && (epAttacker = (GetEPAttackersForToField(EPSquare - PawnStep()) & PieceBB(PAWN, SideToMove)))) {
				while (epAttacker) {
					AddMove(createMove<ENPASSANT>(lsb(epAttacker), EPSquare));
					epAttacker &= epAttacker - 1;
				}
			}
		}
		//Knight Captures
		if (MGT == WINNING_CAPTURES) targets = PieceBB(QUEEN, Color(SideToMove ^ 1)) | PieceBB(ROOK, Color(SideToMove ^ 1)) | hanging;
		else if (MGT == EQUAL_CAPTURES) targets = (PieceBB(BISHOP, Color(SideToMove ^ 1)) | PieceBB(KNIGHT, Color(SideToMove ^ 1))) & ~hanging;
		else if (MGT == LOOSING_CAPTURES) targets = PieceBB(PAWN, Color(SideToMove ^ 1)) & ~hanging;
		else if (MGT == NON_LOOSING_CAPTURES) targets = PieceBB(BISHOP, Color(SideToMove ^ 1)) | PieceBB(KNIGHT, Color(SideToMove ^ 1)) | PieceBB(QUEEN, Color(SideToMove ^ 1)) | PieceBB(ROOK, Color(SideToMove ^ 1)) | hanging;
		else targets = 0;
		Bitboard knights = PieceBB(KNIGHT, SideToMove);
		while (knights) {
			Square from = lsb(knights);
			Bitboard knightTargets = attacks[from] & targets;
			while (knightTargets) {
				AddMove(createMove(from, lsb(knightTargets)));
				knightTargets &= knightTargets - 1;
			}
			knights &= knights - 1;
		}
		//Bishop Captures
		sliders = PieceBB(BISHOP, SideToMove);
		if (MGT == WINNING_CAPTURES) targets = PieceBB(QUEEN, Color(SideToMove ^ 1)) | PieceBB(ROOK, Color(SideToMove ^ 1)) | hanging;
		else if (MGT == EQUAL_CAPTURES) targets = (PieceBB(BISHOP, Color(SideToMove ^ 1)) | PieceBB(KNIGHT, Color(SideToMove ^ 1))) & ~hanging;
		else if (MGT == LOOSING_CAPTURES) targets = PieceBB(PAWN, Color(SideToMove ^ 1)) & ~hanging;
		else if (MGT == NON_LOOSING_CAPTURES) targets = PieceBB(BISHOP, Color(SideToMove ^ 1)) | PieceBB(KNIGHT, Color(SideToMove ^ 1)) | PieceBB(QUEEN, Color(SideToMove ^ 1)) | PieceBB(ROOK, Color(SideToMove ^ 1)) | hanging;
		else targets = 0;
		while (sliders) {
			Square from = lsb(sliders);
			Bitboard sliderTargets = attacks[from] & targets;
			while (sliderTargets) {
				AddMove(createMove(from, lsb(sliderTargets)));
				sliderTargets &= sliderTargets - 1;
			}
			sliders &= sliders - 1;
		}
		//Rook Captures
		sliders = PieceBB(ROOK, SideToMove);
		if (MGT == WINNING_CAPTURES) targets = PieceBB(QUEEN, Color(SideToMove ^ 1)) | hanging;
		else if (MGT == EQUAL_CAPTURES) targets = PieceBB(ROOK, Color(SideToMove ^ 1)) & ~hanging;
		else if (MGT == LOOSING_CAPTURES) targets = (PieceBB(BISHOP, Color(SideToMove ^ 1)) | PieceBB(KNIGHT, Color(SideToMove ^ 1)) | PieceBB(PAWN, Color(SideToMove ^ 1))) & ~hanging;
		else if (MGT == NON_LOOSING_CAPTURES) targets = PieceBB(QUEEN, Color(SideToMove ^ 1)) | PieceBB(ROOK, Color(SideToMove ^ 1)) | hanging;
		else targets = 0;
		while (sliders) {
			Square from = lsb(sliders);
			Bitboard sliderTargets = attacks[from] & targets;
			while (sliderTargets) {
				AddMove(createMove(from, lsb(sliderTargets)));
				sliderTargets &= sliderTargets - 1;
			}
			sliders &= sliders - 1;
		}
		//Queen Captures
		sliders = PieceBB(QUEEN, SideToMove);
		if (MGT == WINNING_CAPTURES) targets = hanging;
		if (MGT == EQUAL_CAPTURES) targets = PieceBB(QUEEN, Color(SideToMove ^ 1)) & ~hanging;
		else if (MGT == NON_LOOSING_CAPTURES) targets = PieceBB(QUEEN, Color(SideToMove ^ 1)) | hanging;
		else if (MGT == LOOSING_CAPTURES) targets = (PieceBB(ROOK, Color(SideToMove ^ 1)) | PieceBB(BISHOP, Color(SideToMove ^ 1)) | PieceBB(KNIGHT, Color(SideToMove ^ 1)) | PieceBB(PAWN, Color(SideToMove ^ 1))) & ~hanging;
		else targets = 0;
		while (sliders) {
			Square from = lsb(sliders);
			Bitboard sliderTargets = attacks[from] & targets;
			while (sliderTargets) {
				AddMove(createMove(from, lsb(sliderTargets)));
				sliderTargets &= sliderTargets - 1;
			}
			sliders &= sliders - 1;
		}
	}
	AddNullMove();
	return firstMove;
}


template<StagedMoveGenerationType SMGT> void Position::InitializeMoveIterator(HistoryManager * historyStats, MoveSequenceHistoryManager * counterHistoryStats, MoveSequenceHistoryManager * followupHistoryStats, killer::Manager * km, Move counter, Move hashmove) {
	processedMoveGenerationPhases = 0;
	if (SMGT == REPETITION) {
		moveIterationPointer = 0;
		generationPhase = generationPhaseOffset[SMGT];
		return;
	}
	if (SMGT == ALL_MOVES) {
		moveIterationPointer = -1;
		generationPhase = generationPhaseOffset[SMGT];
		return;
	}
	if (SMGT == MAIN_SEARCH) killerManager = km; else killerManager = nullptr;
	counterMove = counter;
	moveIterationPointer = -1;
	movepointer = 0;
	phaseStartIndex = 0;
	history = historyStats;
	cmHistory = counterHistoryStats;
	followupHistory = followupHistoryStats;
	hashMove = hashmove;
	if (Checked()) generationPhase = generationPhaseOffset[CHECK] + (hashMove == MOVE_NONE);
	else generationPhase = generationPhaseOffset[SMGT] + (hashMove == MOVE_NONE);
}

inline Bitboard Position::AttacksByPieceType(Color color, PieceType pieceType) const {
	return attacksByPt[GetPiece(pieceType, color)];
}

inline Bitboard Position::AttacksExcludingPieceType(Color color, PieceType excludedPieceType) const
{
	Bitboard bb = EMPTY;
	for (PieceType p = QUEEN; p <= KING; ++p) {
		if (p != excludedPieceType) bb |= AttacksByPieceType(color, p);
	}
	return bb;
}




