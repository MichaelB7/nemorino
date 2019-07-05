#pragma once
#include <assert.h>
#include <string>
#include <algorithm>
#ifdef _MSC_VER
#include <intrin.h>
#define NOMINMAX
#include <Windows.h>
#endif
#include <inttypes.h>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstring>
#include <sstream>
#include <iomanip>
#include "version.h"

#ifdef __arm__
#define NO_POPCOUNT 1
#endif

#ifdef _MSC_VER
#ifdef _WIN64
#pragma intrinsic(_BitScanForward64)
#pragma intrinsic(_BitScanReverse64)
#else
#pragma intrinsic(_BitScanForward)
#pragma intrinsic(_BitScanReverse)
#endif
#endif

const std::string VERSION_INFO = "Nemorino 5.15";

#define CHECK(x)

typedef uint64_t Bitboard;
typedef int64_t Time_t;

/* Move Encoding as unsigned short (copied from Stockfish)
* to                  = bits 0-5
* from                    = bits 6-11
* conversion piece type = bits 12-13
* type                  = bits 14-15
*/
typedef uint16_t Move;

const Move MOVE_NONE = 0;

struct Position;

enum MoveType {
	NORMAL, PROMOTION = 1 << 14, ENPASSANT = 2 << 14, CASTLING = 3 << 14
};

enum Color : unsigned char {
	WHITE, BLACK
};

enum Piece : unsigned char {
	WQUEEN, BQUEEN, WROOK, BROOK, WBISHOP, BBISHOP, WKNIGHT, BKNIGHT, WPAWN, BPAWN, WKING, BKING, BLANK
};

enum PieceType : unsigned char {
	QUEEN, ROOK, BISHOP, KNIGHT, PAWN, KING, NO_TYPE
};

inline PieceType GetPieceType(Piece piece) { return PieceType(piece >> 1); }
inline Piece GetPiece(PieceType pieceType, Color color) { return Piece(2 * pieceType + color); }
inline Color GetColor(Piece piece) { return Color(piece & 1); }

inline Color& operator++(Color& d) { return d = Color(int(d) + 1); }

enum Square : unsigned char {
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8,
	OUTSIDE
};

enum Rank {
	Rank1, Rank2, Rank3, Rank4, Rank5, Rank6, Rank7, Rank8
};

inline Rank relativeRank(Color color, Rank rank) { return Rank(rank ^ (color * 7)); }
inline Rank relativeRank(Color color, int rank) { return Rank(rank ^ (color * 7)); }

enum File {
	FileA, FileB, FileC, FileD, FileE, FileF, FileG, FileH
};

enum CastleFlag {
	NoCastles, W0_0, W0_0_0, B0_0 = 4, B0_0_0 = 8, W_LOST_CASTLING = 16, B_LOST_CASTLING = 32
};

const int CastlesbyColor[] = { W0_0 | W0_0_0, B0_0 | B0_0_0 };

enum MoveGenerationType {
	WINNING_CAPTURES, EQUAL_CAPTURES, LOOSING_CAPTURES, NON_LOOSING_CAPTURES, TACTICAL, QUIETS, CHECK_EVASION, QUIET_CHECKS, ALL, LEGAL, FIND_ANY, FIND_ANY_CHECKED, HASHMOVE, KILLER, REPEAT_ALL, UNDERPROMOTION, QUIETS_POSITIVE, QUIETS_NEGATIVE, FORKS, FORKS_NO_CHECKS, NONE
};

enum StagedMoveGenerationType {
	MAIN_SEARCH, QSEARCH, CHECK, QSEARCH_WITH_CHECKS, REPETITION, ALL_MOVES
};

//Result as used in Search
enum struct Result { RESULT_UNKNOWN, OPEN, DRAW, MATE };

//Result according to rules 
enum struct DetailedResult { NO_RESULT, WHITE_MATES, BLACK_MATES, DRAW_50_MOVES, DRAW_STALEMATE, DRAW_REPETITION, DRAW_MATERIAL };


enum Value : int16_t {
	VALUE_NOTYETDETERMINED = 0 - 32767,
	VALUE_MIN = SHRT_MIN + 2,
	VALUE_DRAW = 0,
	VALUE_ZERO = 0,
	VALUE_100CP = 100,
	VALUE_KNOWN_WIN = 10000,
	VALUE_MATE = 32000,
	VALUE_INFINITE = 32001,
	VALUE_MATE_THRESHOLD = 31000,
	VALUE_MAX = SHRT_MAX - 2
};

inline Value operator*(const float f, const Value v) { return Value(int(round(int(v) * f))); }

typedef uint32_t MaterialKey_t;
typedef uint64_t PawnKey_t;
typedef uint16_t Phase_t;

//Operator overloads are copied from SF
#define ENABLE_BASE_OPERATORS_ON(T)                                             \
	constexpr T operator+(const T d1, const T d2) { return T(int(d1) + int(d2)); } \
	constexpr T operator-(const T d1, const T d2) { return T(int(d1) - int(d2)); } \
	inline T operator*(int i, const T d) { return T(i * int(d)); }              \
	inline T operator*(const T d, int i) { return T(int(d) * i); }              \
	constexpr T operator-(const T d) { return T(-int(d)); }                        \
	inline T& operator+=(T& d1, const T d2) { return d1 = d1 + d2; }            \
	inline T& operator-=(T& d1, const T d2) { return d1 = d1 - d2; }            \
	inline T& operator*=(T& d, int i) { return d = T(int(d) * i); }

#define ENABLE_FULL_OPERATORS_ON(T)                                             \
	ENABLE_BASE_OPERATORS_ON(T)                                                 \
	inline T& operator++(T& d) { return d = T(int(d) + 1); }                    \
	inline T& operator--(T& d) { return d = T(int(d) - 1); }                    \
	inline T operator/(const T d, int i) { return T(int(d) / i); }              \
	inline T& operator/=(T& d, int i) { return d = T(int(d) / i); }

ENABLE_FULL_OPERATORS_ON(Rank)
ENABLE_FULL_OPERATORS_ON(File)
ENABLE_FULL_OPERATORS_ON(Square);
ENABLE_FULL_OPERATORS_ON(Value);
ENABLE_FULL_OPERATORS_ON(PieceType);
ENABLE_FULL_OPERATORS_ON(Piece);

constexpr Value operator-(Value v, int i) { return Value(int(v) - i); }
inline Value operator+=(Value v, int i) { return Value(int(v) + i); }
inline Value operator-=(Value v, int i) { return Value(int(v) - i); }
constexpr Value operator+(Value v, int i) { return Value(int(v) + i); }

inline Square createSquare(Rank rank, File file) { return Square((rank << 3) + file); }
inline char toChar(File f) { return char(f - FileA + 'a'); }
inline char toChar(Rank r) { return char(r - Rank1 + '1'); }

inline std::string toString(Square square) {
	char ch[] = { toChar(File(square & 7)), toChar(Rank(square >> 3)), 0 };
	return ch;
}

template<MoveType T>
inline Move createMove(Square from, Square to,
	PieceType pieceType = PieceType::QUEEN) {
	return Move(to | (from << 6) | T | ((pieceType - PieceType::QUEEN) << 12));
}

inline Move createMove(Square from, Square to) { return Move(to | (from << 6)); }
inline Move createMove(int from, int to) { return Move(to | (from << 6)); }

inline Square from(Move move) { return Square((move >> 6) & 0x3F); }

inline Square to(Move move) { return Square(move & 0x3F); }
inline Move invert(Move m) { return Move((m & 0xF000) | from(m) | (to(m) << 6)); }

inline Square flip(Square s) { return Square(s ^ 56); }

inline MoveType type(Move move) { return MoveType(move & (3 << 14)); }


inline PieceType promotionType(Move move) { return PieceType((move >> 12) & 3); }

const uint64_t EPAttackersForToField[] = { 0x2000000ull,
0x5000000ull, 0xa000000ull, 0x14000000ull, 0x28000000ull,
0x50000000ull, 0xa0000000ull, 0x40000000ull, 0x200000000ull,
0x500000000ull, 0xa00000000ull, 0x1400000000ull, 0x2800000000ull,
0x5000000000ull, 0xa000000000ull, 0x4000000000ull };

inline uint64_t GetEPAttackersForToField(Square to) { return EPAttackersForToField[to - A4]; }
inline uint64_t GetEPAttackersForToField(int to) { return EPAttackersForToField[to - A4]; }

#ifdef _MSC_VER
#ifdef _WIN64
#ifdef NO_POPCOUNT
inline int popcount(Bitboard bb) {
	bb -= (bb >> 1) & 0x5555555555555555ULL;
	bb = ((bb >> 2) & 0x3333333333333333ULL) + (bb & 0x3333333333333333ULL);
	bb = ((bb >> 4) + bb) & 0x0F0F0F0F0F0F0F0FULL;
	return (bb * 0x0101010101010101ULL) >> 56;
}
#else
#ifdef __clang__
inline int popcount(Bitboard bb) { return __builtin_popcountll(bb); }
#else
inline int popcount(Bitboard bb) { return (int)_mm_popcnt_u64(bb); }
#endif
#endif
#else
inline int popcount(Bitboard bb) {
	bb -= (bb >> 1) & 0x5555555555555555ULL;
	bb = ((bb >> 2) & 0x3333333333333333ULL) + (bb & 0x3333333333333333ULL);
	bb = ((bb >> 4) + bb) & 0x0F0F0F0F0F0F0F0FULL;
	return (bb * 0x0101010101010101ULL) >> 56;
}
#endif

inline Square lsb(Bitboard bb) {
	assert(bb != 0);
	unsigned long  index;
#ifdef _WIN64
	_BitScanForward64(&index, bb);
#else
	if (unsigned long(bb) != 0) _BitScanForward(&index, unsigned long(bb));
	else {
		_BitScanForward(&index, unsigned long(bb >> 32));
		index += 32;
	}
#endif
#pragma warning(suppress: 6102)
	return Square(index);
}

inline unsigned long msbInt(int n) {
	assert(n != 0);
	unsigned long result;
#ifdef _WIN64
	_BitScanReverse64(&result, n);
#else
	BitScanReverse(&result, n);
#endif
#pragma warning(suppress: 6102)
	return result;
}

inline Square msb(Bitboard b) {
	assert(b != 0);
	unsigned long result;
#ifdef _WIN64
	_BitScanReverse64(&result, b);
#else
	BitScanReverse(&result, b >> 32);
	if (result) result += 32;
		else BitScanReverse(&result, unsigned int(b));
#endif
#pragma warning(suppress: 6102)
	return Square(result);
}


#endif

#ifdef __GNUC__
#ifdef NO_POPCOUNT
inline int popcount(Bitboard bb) {
	bb -= (bb >> 1) & 0x5555555555555555ULL;
	bb = ((bb >> 2) & 0x3333333333333333ULL) + (bb & 0x3333333333333333ULL);
	bb = ((bb >> 4) + bb) & 0x0F0F0F0F0F0F0F0FULL;
	return (bb * 0x0101010101010101ULL) >> 56;
}
#else
inline int popcount(Bitboard bb) { return __builtin_popcountll(bb); }
#endif

inline Square lsb(Bitboard b) {  return Square(__builtin_ctzll(b)); }

inline Square msbInt(int n) { return Square(31 - __builtin_clz(n)); }

inline Square msb(Bitboard b) { return Square(63 - __builtin_clzll(b)); }
//#define offsetof(type, member)  __builtin_offsetof (type, member)
#endif // __GNUC__

inline Bitboard isolateLSB(Bitboard bb) { return bb & (0 - bb); }

inline Square pop_lsb(Bitboard* bb) {
	const Square s = lsb(*bb);
	*bb &= *bb - 1;
	return s;
}

//inline Square lsb(Bitboard bb) { return Square(popcount((bb & (0 - bb)) - 1)); }

inline Square frontmostSquare(Color c, Bitboard b) { return c == WHITE ? msb(b) : lsb(b); }
inline Square backmostSquare(Color c, Bitboard b) { return c == WHITE ? lsb(b) : msb(b); }

struct Eval {

	Value mgScore = Value(0);
	Value egScore = Value(0);

	Eval() {

	}

	Eval(Value mgValue, Value egValue) {
		mgScore = mgValue;
		egScore = egValue;
	}

	Eval(int mgValue, int egValue) {
		mgScore = Value(mgValue);
		egScore = Value(egValue);
	}

	explicit Eval(int value) {
		mgScore = Value(value);
		egScore = Value(value);
	}

	explicit Eval(Value value) {
		mgScore = value;
		egScore = value;
	}

	inline Value getScore(Phase_t phase) const {
		return Value(((((int)mgScore) * (256 - phase)) + (phase * (int)egScore)) / 256);
	}

	inline Value getAverage() const { return Value((mgScore + egScore) / 2); }

	std::string print() {
		std::stringstream ss;
		ss << std::setw(6) << (int)mgScore << " : " << std::setw(6) << (int)egScore;
		return ss.str();
	}
};

const Eval EVAL_ZERO;

inline Eval operator-(const Eval &e) { return Eval(-e.mgScore, -e.egScore); }
inline Eval operator+(const Eval &e1, const Eval &e2) { return Eval(e1.mgScore + e2.mgScore, e1.egScore + e2.egScore); }
inline Eval operator+=(Eval& e1, const Eval &e2) {
	e1.mgScore += e2.mgScore;
	e1.egScore += e2.egScore;
	return e1;
}

inline Eval operator+=(Eval& e1, const Value v) {
	e1.mgScore += v;
	e1.egScore += v;
	return e1;
}

inline Eval operator-=(Eval& e1, const Eval& e2) {
	e1.mgScore -= e2.mgScore;
	e1.egScore -= e2.egScore;
	return e1;
}

inline Eval operator-=(Eval& e1, const Value v) {
	e1.mgScore -= v;
	e1.egScore -= v;
	return e1;
}

inline Eval operator-(const Eval& e1, const Eval& e2) {
	return Eval(e1.mgScore - e2.mgScore, e1.egScore - e2.egScore);
}
inline Eval operator*(const Eval& e, const int i) { return Eval(e.mgScore * i, e.egScore * i); }
inline Eval operator*(const int i, const Eval& e) { return Eval(e.mgScore * i, e.egScore * i); }
inline Eval operator/(const Eval& e, const int i) { return Eval(e.mgScore / i, e.egScore / i); }
inline Eval operator*(const float f, const Eval& e) { return Eval(f * e.mgScore, f * e.egScore); }

struct ValuatedMove {
	Move move;
	Value score;

	ValuatedMove() { }
	ValuatedMove(Move m, Value s) { move = m; score = s; }
};

const ValuatedMove VALUATED_MOVE_NONE(MOVE_NONE, VALUE_ZERO);

struct ExtendedMove {
	Piece piece = BLANK;
	Move move = MOVE_NONE;

	ExtendedMove() { }
	ExtendedMove(Piece p, Move m) { piece = p; move = m; }
};

const ExtendedMove EXTENDED_MOVE_NONE(BLANK, MOVE_NONE);

inline bool operator==(const ExtendedMove& em1, const ExtendedMove& em2) { return em1.move == em2.move && em1.piece == em2.piece; }
inline bool operator!=(const ExtendedMove& em1, const ExtendedMove& em2) { return em1.move != em2.move || em1.piece != em2.piece; }

inline bool positiveScore(const ValuatedMove& vm) { return vm.score > 0; }

inline bool operator<(const ValuatedMove& f, const ValuatedMove& s) {
	return f.score < s.score;
}

inline bool sortByScore(const ValuatedMove& m1, const ValuatedMove& m2) { return m1.score > m2.score; }

const int MAX_DEPTH = 128;

struct Position;

typedef Value(*EvalFunction)(const Position&);

Value evaluateDefault(const Position& pos);
Value evaluatePawnEnding(const Position& pos);

#ifdef _MSC_VER
inline Time_t now() {
	LARGE_INTEGER s_frequency;
	QueryPerformanceFrequency(&s_frequency);
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	return (1000LL * now.QuadPart) / s_frequency.QuadPart;
}
#else
inline Time_t now() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	//return GetTickCount64();
}
#endif

#ifdef USE_PEXT 
#include <immintrin.h> // Header for _pext_u64() intrinsic
inline Bitboard pext(Bitboard val, Bitboard mask) {
	return _pext_u64(val, mask);
}
#else
inline Bitboard pext(Bitboard val, Bitboard mask) {
	Bitboard res = 0;
	for (Bitboard bb = 1; mask; bb += bb) {
		if (val & mask & (0 - mask))
			res |= bb;
		mask &= mask - 1;
	}
	return res;
}
#endif

const Value MAX_HISTORY_VALUE = Value(2000);
//Inspired by (=more or less copied from) Stockfish 
struct HistoryManager {
public:
	HistoryManager() {
		initialize();
	}

	inline void update(Value v, Piece p, Move m) {
		int fs = from(m);
		int ts = to(m);
		if (abs(int(Table[p][fs][ts])) < MAX_HISTORY_VALUE) {
			Table[p][fs][ts] += v;
		}
	}
	inline Value getValue(const Piece p, const Move m) { return Table[p][from(m)][to(m)]; }
	inline void initialize() { std::memset(Table, 0, sizeof(Table)); }
	inline void age() {
		for (int i = 0; i < 12; ++i) {
			for (int j = 0; j < 64; ++j) {
				for (int k = 0; k < 64; ++k) {
					Table[i][j][k] = Table[i][j][k] / 2;
				}
			}
		}
	}
private:
	Value Table[12][64][64];
};

//Inspired by (=more or less copied from) Stockfish 
//Is used to keep track of countermove and followup move history
struct MoveSequenceHistoryManager {
public:
	MoveSequenceHistoryManager() { initialize(); }

	inline void update(Value v, Piece p1, Square s1, Piece p2, Square s2) {
		if (abs(int(Table[p1][s1][p2][s2])) < MAX_HISTORY_VALUE) {
			Table[p1][s1][p2][s2] += v;
		}
	}
	inline Value getValue(const Piece p1, const Square s1, const Piece p2, const Square s2) { return Table[p1][s1][p2][s2]; }
	inline void initialize() { std::memset(Table, 0, sizeof(Table)); }
	inline void age() {
		for (int i = 0; i < 12; ++i) {
			for (int j = 0; j < 64; ++j) {
				for (int i1 = 0; i1 < 12; ++i1) {
					for (int j1 = 0; j1 < 64; ++j1) {
						Table[i][j][i1][j1] = Table[i][j][i1][j1] / 2;
					}
				}
			}
		}
	}
private:
	Value Table[12][64][12][64];
};
