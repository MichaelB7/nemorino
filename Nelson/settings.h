#pragma once

#include <map>
#include <memory>
#include "types.h"
#include "board.h"
#include "utils.h"

const int PV_MAX_LENGTH = 32; //Maximum Length of displayed Principal Variation
const int MASK_TIME_CHECK = (1 << 14) - 1; //Time is only checked each MASK_TIME_CHECK nodes

const int PAWN_TABLE_SIZE = 1 << 14; //has to be power of 2
const int KILLER_TABLE_SIZE = 1 << 11; //has to be power of 2

////Bonus for Passed Pawns (dynamic evaluations)
//const Eval BONUS_PASSED_PAWN_BACKED = Eval(0, 15); //Passed pawn with own rook behind
//const Eval MALUS_PASSED_PAWN_BLOCKED[6] = { Eval(0, 3), Eval(0,10), Eval(0,10), Eval(0,30), EVAL_ZERO, Eval(0,30) }; //Passed pawn with own rook behind

namespace settings {

	enum CAPTURES {
		QxP, BxP, NxP, RxP, QxN,
		QxR, RxN, QxB, BxN, RxB,
		KxP, EP, PxP, KxN, PxB,
		BxR, PxN, NxR, NxN, NxB,
		RxR, BxB, PxR, KxB, KxR,
		QxQ,
		RxQ, BxQ, NxQ, PxQ, KxQ //Winning Captures of Queen
	};

	struct Parameters {
	public:
		Parameters();
		int LMRReduction(int depth, int moveNumber);

		int HelperThreads = 0;
		Value Contempt = Value(10);
		Color EngineSide = WHITE;
		int EmergencyTime = 0;
		bool extendedOptions = false;
		Eval SCALE_BISHOP_PAIR_WITH_PAWNS = EVAL_ZERO; //Reduce Bonus Bishop Pair by this value for each pawn on the board
		Eval BONUS_BISHOP_PAIR_NO_OPP_MINOR = Eval(10); //Bonus for Bishop pair, if opponent has no minor piece for exchange
		Eval SCALE_EXCHANGE_WITH_PAWNS = EVAL_ZERO; //Decrease Value of Exchange with number of pawns
		Eval SCALE_EXCHANGE_WITH_MAJORS = EVAL_ZERO; //Decrease Value of Exchange with number of majors
		Eval PAWN_STORM[4] = { Eval(10, 0), Eval(25, 0), Eval(15, 0), Eval(5, 0) };
		Value BETA_PRUNING_FACTOR = Value(95);
		inline Value BetaPruningMargin(int depth) { return Value(depth * BETA_PRUNING_FACTOR); }
		Value RAZORING_FACTOR = Value(50);
		Value RAZORING_OFFSET = Value(50);
		inline Value RazoringMargin(int depth) { return Value(depth * RAZORING_FACTOR + RAZORING_OFFSET); }
		int LIMIT_QSEARCH = -3;
		Eval HANGING = Eval(16, 13);
		Eval KING_ON_ONE = Eval(1, 29);
		Eval KING_ON_MANY = Eval(3, 63);
		Eval ROOK_ON_OPENFILE = Eval(10, 0);
		Eval ROOK_ON_SEMIOPENFILE = Eval(10, 0);
		Eval ROOK_ON_7TH = Eval(20, 0);
		Eval MALUS_LOST_CASTLING = Eval(50, -50);

		Eval BONUS_BISHOP_PAIR = Eval(50);
        Value DELTA_PRUNING_SAFETY_MARGIN = Value(VALUE_100CP);
        Eval PAWN_SHELTER_2ND_RANK = Eval(30, -10);
		Eval PAWN_SHELTER_3RD_RANK = Eval(15, -8);
		Eval PAWN_SHELTER_4TH_RANK = Eval(8, -4);
		Eval PAWN_SHELTER[9] = { PAWN_SHELTER_2ND_RANK, PAWN_SHELTER_2ND_RANK, PAWN_SHELTER_2ND_RANK, 
			                     PAWN_SHELTER_3RD_RANK, PAWN_SHELTER_3RD_RANK, PAWN_SHELTER_3RD_RANK, 
			                     PAWN_SHELTER_4TH_RANK, Eval(0), Eval(0) };
        Value PROBCUT_MARGIN = Value(90);
		Eval PieceValues[7]{ Eval(1025), Eval(490, 550), Eval(325), Eval(325), Eval(80, 100), Eval(VALUE_KNOWN_WIN), Eval(0) };
		int FULTILITY_PRUNING_DEPTH = 3;
		Value FUTILITY_PRUNING_LIMIT[4] = { VALUE_ZERO, PieceValues[BISHOP].mgScore, PieceValues[ROOK].mgScore, PieceValues[QUEEN].mgScore };
		Eval PSQT[12][64]{
			{  // White Queens
				Eval(-10,-8), Eval(-7,-5), Eval(-4,-2), Eval(-2,0), Eval(-2,0), Eval(-4,-2), Eval(-7,-5), Eval(-10,-8),  // Rank 1
				Eval(-4,-5), Eval(-2,-2), Eval(0,0), Eval(3,2), Eval(3,2), Eval(0,0), Eval(-2,-2), Eval(-4,-5),  // Rank 2
				Eval(-2,-2), Eval(0,0), Eval(3,2), Eval(5,5), Eval(5,5), Eval(3,2), Eval(0,0), Eval(-2,-2),  // Rank 3
				Eval(0,0), Eval(3,2), Eval(5,5), Eval(8,8), Eval(8,8), Eval(5,5), Eval(3,2), Eval(0,0),  // Rank 4
				Eval(0,0), Eval(3,2), Eval(5,5), Eval(8,8), Eval(8,8), Eval(5,5), Eval(3,2), Eval(0,0),  // Rank 5
				Eval(-2,-2), Eval(0,0), Eval(3,2), Eval(5,5), Eval(5,5), Eval(3,2), Eval(0,0), Eval(-2,-2),  // Rank 6
				Eval(-4,-5), Eval(-2,-2), Eval(0,0), Eval(3,2), Eval(3,2), Eval(0,0), Eval(-2,-2), Eval(-4,-5),  // Rank 7
				Eval(-7,-8), Eval(-4,-5), Eval(-2,-2), Eval(0,0), Eval(0,0), Eval(-2,-2), Eval(-4,-5), Eval(-7,-8)  // Rank 8
			},
			{  // Black Queen
				Eval(7,8), Eval(4,5), Eval(2,2), Eval(0,0), Eval(0,0), Eval(2,2), Eval(4,5), Eval(7,8),  // Rank 1
				Eval(4,5), Eval(2,2), Eval(0,0), Eval(-3,-2), Eval(-3,-2), Eval(0,0), Eval(2,2), Eval(4,5),  // Rank 2
				Eval(2,2), Eval(0,0), Eval(-3,-2), Eval(-5,-5), Eval(-5,-5), Eval(-3,-2), Eval(0,0), Eval(2,2),  // Rank 3
				Eval(0,0), Eval(-3,-2), Eval(-5,-5), Eval(-8,-8), Eval(-8,-8), Eval(-5,-5), Eval(-3,-2), Eval(0,0),  // Rank 4
				Eval(0,0), Eval(-3,-2), Eval(-5,-5), Eval(-8,-8), Eval(-8,-8), Eval(-5,-5), Eval(-3,-2), Eval(0,0),  // Rank 5
				Eval(2,2), Eval(0,0), Eval(-3,-2), Eval(-5,-5), Eval(-5,-5), Eval(-3,-2), Eval(0,0), Eval(2,2),  // Rank 6
				Eval(4,5), Eval(2,2), Eval(0,0), Eval(-3,-2), Eval(-3,-2), Eval(0,0), Eval(2,2), Eval(4,5),  // Rank 7
				Eval(10,8), Eval(7,5), Eval(4,2), Eval(2,0), Eval(2,0), Eval(4,2), Eval(7,5), Eval(10,8)  // Rank 8
			},
			{  // White Rook
				Eval(-1,0), Eval(-1,0), Eval(0,0), Eval(1,0), Eval(1,0), Eval(0,0), Eval(-1,0), Eval(-1,0),  // Rank 1
				Eval(-1,0), Eval(-1,0), Eval(0,0), Eval(1,0), Eval(1,0), Eval(0,0), Eval(-1,0), Eval(-1,0),  // Rank 2
				Eval(-1,0), Eval(-1,0), Eval(0,0), Eval(1,0), Eval(1,0), Eval(0,0), Eval(-1,0), Eval(-1,0),  // Rank 3
				Eval(-1,0), Eval(-1,0), Eval(0,0), Eval(1,0), Eval(1,0), Eval(0,0), Eval(-1,0), Eval(-1,0),  // Rank 4
				Eval(-1,0), Eval(-1,0), Eval(0,0), Eval(1,0), Eval(1,0), Eval(0,0), Eval(-1,0), Eval(-1,0),  // Rank 5
				Eval(-1,0), Eval(-1,0), Eval(0,0), Eval(1,0), Eval(1,0), Eval(0,0), Eval(-1,0), Eval(-1,0),  // Rank 6
				Eval(3,0), Eval(3,0), Eval(4,0), Eval(6,0), Eval(6,0), Eval(4,0), Eval(3,0), Eval(3,0),  // Rank 7
				Eval(-1,0), Eval(-1,0), Eval(0,0), Eval(1,0), Eval(1,0), Eval(0,0), Eval(-1,0), Eval(-1,0)  // Rank 8
			},
			{  // Black Rook
				Eval(1,0), Eval(1,0), Eval(0,0), Eval(-1,0), Eval(-1,0), Eval(0,0), Eval(1,0), Eval(1,0),  // Rank 1
				Eval(-3,0), Eval(-3,0), Eval(-4,0), Eval(-6,0), Eval(-6,0), Eval(-4,0), Eval(-3,0), Eval(-3,0),  // Rank 2
				Eval(1,0), Eval(1,0), Eval(0,0), Eval(-1,0), Eval(-1,0), Eval(0,0), Eval(1,0), Eval(1,0),  // Rank 3
				Eval(1,0), Eval(1,0), Eval(0,0), Eval(-1,0), Eval(-1,0), Eval(0,0), Eval(1,0), Eval(1,0),  // Rank 4
				Eval(1,0), Eval(1,0), Eval(0,0), Eval(-1,0), Eval(-1,0), Eval(0,0), Eval(1,0), Eval(1,0),  // Rank 5
				Eval(1,0), Eval(1,0), Eval(0,0), Eval(-1,0), Eval(-1,0), Eval(0,0), Eval(1,0), Eval(1,0),  // Rank 6
				Eval(1,0), Eval(1,0), Eval(0,0), Eval(-1,0), Eval(-1,0), Eval(0,0), Eval(1,0), Eval(1,0),  // Rank 7
				Eval(1,0), Eval(1,0), Eval(0,0), Eval(-1,0), Eval(-1,0), Eval(0,0), Eval(1,0), Eval(1,0)  // Rank 8
			},
			{  // White Bishop
				Eval(-12,-8), Eval(-10,-5), Eval(-7,-2), Eval(-4,0), Eval(-4,0), Eval(-7,-2), Eval(-10,-5), Eval(-12,-8),  // Rank 1
				Eval(-4,-5), Eval(3,-2), Eval(0,0), Eval(3,2), Eval(3,2), Eval(0,0), Eval(3,-2), Eval(-4,-5),  // Rank 2
				Eval(-2,-2), Eval(0,0), Eval(3,2), Eval(5,5), Eval(5,5), Eval(3,2), Eval(0,0), Eval(-2,-2),  // Rank 3
				Eval(0,0), Eval(3,2), Eval(5,5), Eval(8,8), Eval(8,8), Eval(5,5), Eval(3,2), Eval(0,0),  // Rank 4
				Eval(0,0), Eval(5,2), Eval(5,5), Eval(8,8), Eval(8,8), Eval(5,5), Eval(5,2), Eval(0,0),  // Rank 5
				Eval(-2,-2), Eval(0,0), Eval(3,2), Eval(5,5), Eval(5,5), Eval(3,2), Eval(0,0), Eval(-2,-2),  // Rank 6
				Eval(-4,-5), Eval(-2,-2), Eval(0,0), Eval(3,2), Eval(3,2), Eval(0,0), Eval(-2,-2), Eval(-4,-5),  // Rank 7
				Eval(-7,-8), Eval(-4,-5), Eval(-2,-2), Eval(0,0), Eval(0,0), Eval(-2,-2), Eval(-4,-5), Eval(-7,-8)  // Rank 8
			},
			{  // Black Bishop
				Eval(7,8), Eval(4,5), Eval(2,2), Eval(0,0), Eval(0,0), Eval(2,2), Eval(4,5), Eval(7,8),  // Rank 1
				Eval(4,5), Eval(2,2), Eval(0,0), Eval(-3,-2), Eval(-3,-2), Eval(0,0), Eval(2,2), Eval(4,5),  // Rank 2
				Eval(2,2), Eval(0,0), Eval(-3,-2), Eval(-5,-5), Eval(-5,-5), Eval(-3,-2), Eval(0,0), Eval(2,2),  // Rank 3
				Eval(0,0), Eval(-5,-2), Eval(-5,-5), Eval(-8,-8), Eval(-8,-8), Eval(-5,-5), Eval(-5,-2), Eval(0,0),  // Rank 4
				Eval(0,0), Eval(-3,-2), Eval(-5,-5), Eval(-8,-8), Eval(-8,-8), Eval(-5,-5), Eval(-3,-2), Eval(0,0),  // Rank 5
				Eval(2,2), Eval(0,0), Eval(-3,-2), Eval(-5,-5), Eval(-5,-5), Eval(-3,-2), Eval(0,0), Eval(2,2),  // Rank 6
				Eval(4,5), Eval(-3,2), Eval(0,0), Eval(-3,-2), Eval(-3,-2), Eval(0,0), Eval(-3,2), Eval(4,5),  // Rank 7
				Eval(12,8), Eval(10,5), Eval(7,2), Eval(4,0), Eval(4,0), Eval(7,2), Eval(10,5), Eval(12,8)  // Rank 8
			},
			{  // White Knight
				Eval(-19,-18), Eval(-14,-12), Eval(-8,-7), Eval(-3,-2), Eval(-3,-2), Eval(-8,-7), Eval(-14,-12), Eval(-19,-18),  // Rank 1
				Eval(-14,-12), Eval(-8,-7), Eval(-3,-2), Eval(4,5), Eval(4,5), Eval(-3,-2), Eval(-8,-7), Eval(-14,-12),  // Rank 2
				Eval(-8,-7), Eval(-3,-2), Eval(4,5), Eval(15,16), Eval(15,16), Eval(4,5), Eval(-3,-2), Eval(-8,-7),  // Rank 3
				Eval(-3,-2), Eval(4,5), Eval(15,16), Eval(21,22), Eval(21,22), Eval(15,16), Eval(4,5), Eval(-3,-2),  // Rank 4
				Eval(-3,-2), Eval(4,5), Eval(26,16), Eval(31,22), Eval(31,22), Eval(26,16), Eval(4,5), Eval(-3,-2),  // Rank 5
				Eval(-8,-7), Eval(-3,-2), Eval(15,5), Eval(36,16), Eval(36,16), Eval(15,5), Eval(-3,-2), Eval(-8,-7),  // Rank 6
				Eval(-14,-12), Eval(-8,-7), Eval(-3,-2), Eval(4,5), Eval(4,5), Eval(-3,-2), Eval(-8,-7), Eval(-14,-12),  // Rank 7
				Eval(-19,-18), Eval(-14,-12), Eval(-8,-7), Eval(-3,-2), Eval(-3,-2), Eval(-8,-7), Eval(-14,-12), Eval(-19,-18)  // Rank 8
			},
			{  // Black Knight
				Eval(19,18), Eval(14,12), Eval(8,7), Eval(3,2), Eval(3,2), Eval(8,7), Eval(14,12), Eval(19,18),  // Rank 1
				Eval(14,12), Eval(8,7), Eval(3,2), Eval(-4,-5), Eval(-4,-5), Eval(3,2), Eval(8,7), Eval(14,12),  // Rank 2
				Eval(8,7), Eval(3,2), Eval(-15,-5), Eval(-36,-16), Eval(-36,-16), Eval(-15,-5), Eval(3,2), Eval(8,7),  // Rank 3
				Eval(3,2), Eval(-4,-5), Eval(-26,-16), Eval(-31,-22), Eval(-31,-22), Eval(-26,-16), Eval(-4,-5), Eval(3,2),  // Rank 4
				Eval(3,2), Eval(-4,-5), Eval(-15,-16), Eval(-21,-22), Eval(-21,-22), Eval(-15,-16), Eval(-4,-5), Eval(3,2),  // Rank 5
				Eval(8,7), Eval(3,2), Eval(-4,-5), Eval(-15,-16), Eval(-15,-16), Eval(-4,-5), Eval(3,2), Eval(8,7),  // Rank 6
				Eval(14,12), Eval(8,7), Eval(3,2), Eval(-4,-5), Eval(-4,-5), Eval(3,2), Eval(8,7), Eval(14,12),  // Rank 7
				Eval(19,18), Eval(14,12), Eval(8,7), Eval(3,2), Eval(3,2), Eval(8,7), Eval(14,12), Eval(19,18)  // Rank 8
			},
			{  // White Pawn
				Eval(0,0), Eval(0,0), Eval(0,0), Eval(0,0), Eval(0,0), Eval(0,0), Eval(0,0), Eval(0,0),  // Rank 1
				Eval(-17,-11), Eval(-9,-11), Eval(-7,-11), Eval(-9,-11), Eval(-9,-11), Eval(-7,-11), Eval(-9,-11), Eval(-17,-11),  // Rank 2
				Eval(-15,-9), Eval(-12,-9), Eval(-7,-9), Eval(-1,-9), Eval(-1,-9), Eval(-7,-9), Eval(-12,-9), Eval(-15,-9),  // Rank 3
				Eval(-17,-6), Eval(-9,-6), Eval(-7,-6), Eval(6,-6), Eval(6,-6), Eval(-7,-6), Eval(-9,-6), Eval(-17,-6),  // Rank 4
				Eval(-9,-3), Eval(-7,-3), Eval(-4,-3), Eval(9,-3), Eval(9,-3), Eval(-4,-3), Eval(-7,-3), Eval(-9,-3),  // Rank 5
				Eval(3,9), Eval(6,9), Eval(9,9), Eval(11,9), Eval(11,9), Eval(9,9), Eval(6,9), Eval(3,9),  // Rank 6
				Eval(22,20), Eval(22,20), Eval(22,20), Eval(22,20), Eval(22,20), Eval(22,20), Eval(22,20), Eval(22,20),  // Rank 7
				Eval(0,0), Eval(0,0), Eval(0,0), Eval(0,0), Eval(0,0), Eval(0,0), Eval(0,0), Eval(0,0)  // Rank 8
			},
			{  //  Black Pawn
				Eval(0,0), Eval(0,0), Eval(0,0), Eval(0,0), Eval(0,0), Eval(0,0), Eval(0,0), Eval(0,0),  // Rank 1
				Eval(-22,-20), Eval(-22,-20), Eval(-22,-20), Eval(-22,-20), Eval(-22,-20), Eval(-22,-20), Eval(-22,-20), Eval(-22,-20),  // Rank 2
				Eval(-3,-9), Eval(-6,-9), Eval(-9,-9), Eval(-11,-9), Eval(-11,-9), Eval(-9,-9), Eval(-6,-9), Eval(-3,-9),  // Rank 3
				Eval(9,3), Eval(7,3), Eval(4,3), Eval(-9,3), Eval(-9,3), Eval(4,3), Eval(7,3), Eval(9,3),  // Rank 4
				Eval(17,6), Eval(9,6), Eval(7,6), Eval(-6,6), Eval(-6,6), Eval(7,6), Eval(9,6), Eval(17,6),  // Rank 5
				Eval(15,9), Eval(12,9), Eval(7,9), Eval(1,9), Eval(1,9), Eval(7,9), Eval(12,9), Eval(15,9),  // Rank 6
				Eval(17,11), Eval(9,11), Eval(7,11), Eval(9,11), Eval(9,11), Eval(7,11), Eval(9,11), Eval(17,11),  // Rank 7
				Eval(0,0), Eval(0,0), Eval(0,0), Eval(0,0), Eval(0,0), Eval(0,0), Eval(0,0), Eval(0,0)  // Rank 8
			},
			{  // White King
				Eval(16,-20), Eval(17,-15), Eval(16,-9), Eval(14,-4), Eval(14,-4), Eval(16,-9), Eval(17,-15), Eval(16,-20),  // Rank 1
				Eval(16,-15), Eval(16,-9), Eval(14,-4), Eval(11,7), Eval(11,7), Eval(14,-4), Eval(16,-9), Eval(16,-15),  // Rank 2
				Eval(11,-9), Eval(11,-4), Eval(11,7), Eval(8,23), Eval(8,23), Eval(11,7), Eval(11,-4), Eval(11,-9),  // Rank 3
				Eval(8,-4), Eval(8,7), Eval(3,23), Eval(-2,28), Eval(-2,28), Eval(3,23), Eval(8,7), Eval(8,-4),  // Rank 4
				Eval(3,-4), Eval(0,7), Eval(-2,23), Eval(-7,28), Eval(-7,28), Eval(-2,23), Eval(0,7), Eval(3,-4),  // Rank 5
				Eval(-7,-9), Eval(-7,-4), Eval(-12,7), Eval(-18,23), Eval(-18,23), Eval(-12,7), Eval(-7,-4), Eval(-7,-9),  // Rank 6
				Eval(-12,-15), Eval(-12,-9), Eval(-18,-4), Eval(-18,7), Eval(-18,7), Eval(-18,-4), Eval(-12,-9), Eval(-12,-15),  // Rank 7
				Eval(-18,-20), Eval(-18,-15), Eval(-18,-9), Eval(-18,-4), Eval(-18,-4), Eval(-18,-9), Eval(-18,-15), Eval(-18,-20)  // Rank 8
			},
			{  // Black King
				Eval(18,20), Eval(18,15), Eval(18,9), Eval(18,4), Eval(18,4), Eval(18,9), Eval(18,15), Eval(18,20),  // Rank 1
				Eval(12,15), Eval(12,9), Eval(18,4), Eval(18,-7), Eval(18,-7), Eval(18,4), Eval(12,9), Eval(12,15),  // Rank 2
				Eval(7,9), Eval(7,4), Eval(12,-7), Eval(18,-23), Eval(18,-23), Eval(12,-7), Eval(7,4), Eval(7,9),  // Rank 3
				Eval(-3,4), Eval(0,-7), Eval(2,-23), Eval(7,-28), Eval(7,-28), Eval(2,-23), Eval(0,-7), Eval(-3,4),  // Rank 4
				Eval(-8,4), Eval(-8,-7), Eval(-3,-23), Eval(2,-28), Eval(2,-28), Eval(-3,-23), Eval(-8,-7), Eval(-8,4),  // Rank 5
				Eval(-11,9), Eval(-11,4), Eval(-11,-7), Eval(-8,-23), Eval(-8,-23), Eval(-11,-7), Eval(-11,4), Eval(-11,9),  // Rank 6
				Eval(-16,15), Eval(-16,9), Eval(-14,4), Eval(-11,-7), Eval(-11,-7), Eval(-14,4), Eval(-16,9), Eval(-16,15),  // Rank 7
				Eval(-16,20), Eval(-17,15), Eval(-16,9), Eval(-14,4), Eval(-14,4), Eval(-16,9), Eval(-17,15), Eval(-16,20)  // Rank 8
			}
		};
		Value CAPTURE_SCORES[6][7] = {
			// Captured:  QUEEN, ROOK,   BISHOP,   KNIGHT,    PAWN,   King,   EP-Capture/Promotion
			{ Value(QxQ), Value(QxR),  Value(QxB),  Value(QxN),  Value(QxP),  Value(0),  Value(0) },   // QUEEN
			{ Value(RxQ), Value(RxR), Value(RxB), Value(RxN),  Value(RxP),  Value(0),  Value(0) },   // ROOK
			{ Value(BxQ), Value(BxR), Value(BxB), Value(BxN),  Value(BxP),  Value(0),  Value(0) },   // BISHOP
			{ Value(NxQ), Value(NxR), Value(NxB), Value(NxN), Value(NxP),  Value(0),  Value(0) },   // KNIGHT
			{ Value(PxQ), Value(PxR), Value(PxB), Value(PxN), Value(PxP),  Value(0), Value(EP) },   // PAWN
			{ Value(KxQ), Value(KxR), Value(KxB), Value(KxN), Value(KxP), Value(0),  Value(0) }    // KING
		};
		Eval MOBILITY_BONUS_KNIGHT[9] = { Eval(-29, -23), Eval(-19, -14), Eval(-4, -4), Eval(1, 0), Eval(7, 4), Eval(12, 9), Eval(16, 12), Eval(19, 14), Eval(21, 15) };
		Eval MOBILITY_BONUS_BISHOP[14] = { Eval(-23, -22), Eval(-12, -11), Eval(2, 0), Eval(9, 7), Eval(15, 14), Eval(22, 19), Eval(28, 25), Eval(32, 29), Eval(33, 32),
			Eval(36, 33), Eval(37, 35), Eval(37, 36), Eval(39, 36), Eval(40, 37) };
		Eval MOBILITY_BONUS_ROOK[15] = { Eval(-22, -25), Eval(-14, -12), Eval(-2, 0), Eval(0, 7), Eval(2, 15), Eval(5, 22), Eval(8, 29), Eval(9, 37), Eval(12, 44),
			Eval(14, 50), Eval(14, 53), Eval(15, 56), Eval(16, 57), Eval(16, 57), Eval(16, 58) };
		Eval MOBILITY_BONUS_QUEEN[28] = { Eval(-19, -18), Eval(-12, -11), Eval(-2, -2), Eval(0, 0), Eval(2, 4), Eval(5, 8), Eval(5, 14), Eval(8, 18), Eval(9, 18), Eval(9, 19),
			Eval(9, 19), Eval(9, 19), Eval(9, 19), Eval(11, 19), Eval(11, 19), Eval(11, 19), Eval(11, 19), Eval(11, 19), Eval(11, 19), Eval(11, 19),
			Eval(11, 19), Eval(11, 19), Eval(11, 19), Eval(11, 19), Eval(11, 19), Eval(11, 19), Eval(11, 19), Eval(11, 19) };
		// Threat[defended/weak][minor/major attacking][attacked PieceType] contains
		// bonuses according to which piece type attacks which one.
		// "inspired" by SF
		Eval Threat[2][2][6] = {
			{ { Eval(0, 0), Eval(0, 0), Eval(10, 18), Eval(12, 19), Eval(22, 49), Eval(18, 53) }, // Defended Minor
			{ Eval(0, 0), Eval(0, 0), Eval(5, 7), Eval(5, 7), Eval(4, 7), Eval(12, 24) } }, // Defended Major
			{ { Eval(0, 0), Eval(0, 16), Eval(17, 21), Eval(16, 25), Eval(21, 50), Eval(18, 52) }, // Weak Minor
			{ Eval(0, 0), Eval(0, 14), Eval(13, 29), Eval(13, 29), Eval(0, 22), Eval(12, 26) } } // Weak Major
		};
		Eval PASSED_PAWN_BONUS[6] = { Eval(0), Eval(0), Eval(8), Eval(35), Eval(81), Eval(140) };
		Eval MALUS_BLOCKED[6] = { Eval(30), Eval(20), Eval(10), Eval(5), Eval(0), Eval(0) }; //indexed by distance to conversion
		Eval BONUS_PROTECTED_PASSED_PAWN[6] = { Eval(0), Eval(0), Eval(0), Eval(30), Eval(30), Eval(30) };
        Eval MALUS_ISOLATED_PAWN = Eval(5);
		Eval MALUS_BACKWARD_PAWN = Eval(20);
		Eval MALUS_ISLAND_COUNT = Eval(5);
		Eval BONUS_CANDIDATE = Eval(5);
		Eval BONUS_LEVER = Eval(5);
		Eval MALUS_DOUBLED_PAWN = Eval(0, 20);
		int TBProbeDepth = 1;
		int ATTACK_WEIGHT[4] = { 53, 82, 23, 17 }; //Indexed by Piece Type
		int SAFE_CHECK[4] = { 800, 900, 450, 800 };
		int KING_DANGER_SCALE = 14;
		int KING_RING_ATTACK_FACTOR = 100;
		int WEAK_SQUARES_FACTOR = 200;
		int PINNED_FACTOR = 125;
		int ATTACK_WITH_QUEEN = 900;
		int BONUS_LEVER_ON_KINGSIDE = 10;

		const Eval MOBILITY_CENTER_KNIGHT = Eval(3, 0);
		const Eval MOBILITY_CENTER_EXTENDED_KNIGHT = Eval(3, 0);

		Eval BONUS_TEMPO = Eval(10,5);

		Eval IMBALANCE_Q_vs_RN = Eval(75);
		Eval IMBALANCE_Q_vs_RB = Eval(75);

		void UCIExpose();
		void SetFromUCI(std::string name, std::string value);

		void setParam(std::string key, std::string value);
	private:
		void Initialize();
		int LMR_REDUCTION[64][64];
	};

	extern Parameters parameter;

	void Initialize();

	enum OptionType { SPIN, CHECK, BUTTON, STRING };

	const std::string OPTION_HASH = "Hash";
	const std::string OPTION_CHESS960 = "UCI_Chess960";
	const std::string OPTION_CLEAR_HASH = "Clear Hash";
	const std::string OPTION_PRINT_OPTIONS = "Print Options";
	const std::string OPTION_CONTEMPT = "Contempt";
	const std::string OPTION_MULTIPV = "MultiPV";
	const std::string OPTION_THREADS = "Threads";
	const std::string OPTION_PONDER = "Ponder";
	const std::string OPTION_BOOK_FILE = "BookFile";
	const std::string OPTION_OWN_BOOK = "OwnBook";
	const std::string OPTION_OPPONENT = "UCI_Opponent";
	const std::string OPTION_EMERGENCY_TIME = "MoveOverhead";
	const std::string OPTION_NODES_TIME = "Nodestime"; //Nodes per millisecond
	const std::string OPTION_SYZYGY_PATH = "SyzygyPath";
	const std::string OPTION_SYZYGY_PROBE_DEPTH = "SyzygyProbeDepth";

	class Option {
	public:
		Option() { otype = OptionType::STRING; };
		Option(std::string Name, OptionType Type = OptionType::BUTTON, std::string DefaultValue = "", std::string MinValue = "", std::string MaxValue = "", bool Technical = false);
		virtual ~Option() { };
		void virtual set(std::string value) = 0;
		void virtual read(std::vector<std::string> &tokens) = 0;
		std::string printUCI();
		std::string printInfo() const;
		inline std::string getName() { return name; }
		inline bool isTechnical() { return technical; }
		inline OptionType getType() { return otype; }
	protected:
		std::string name;
		OptionType otype;
		std::string defaultValue;
		std::string minValue;
		std::string maxValue;
		bool technical = false;
	};



	class OptionSpin: public Option {
	public:
		OptionSpin(std::string Name, int Value, int Min, int Max, bool Technical = false) : Option(Name, OptionType::SPIN, std::to_string(Value), std::to_string(Min), std::to_string(Max), Technical) { };
		virtual ~OptionSpin() { };
		inline void set(std::string value) { _value = stoi(value); }
		inline void set(int value) { _value = value; }
		inline void setDefault(std::string value) { defaultValue = value; }
		inline void setDefault(int value) { defaultValue = std::to_string(value); }
		inline int getValue() { 
			if (_value == INT_MIN) set(defaultValue); 
			return _value; 
		}
		inline void read(std::vector<std::string> &tokens) { set(tokens[4]); }
	protected:
		int _value = INT_MIN;
	};

	class OptionCheck : public Option {
	public:
		OptionCheck(std::string Name, bool value, bool Technical = false);
	    virtual ~OptionCheck() { };
		void set(std::string value);
		inline bool getValue() { return _value; }
		inline void read(std::vector<std::string> &tokens) { set(tokens[4]); }
		inline void set(bool value) { _value = value; };
	protected:
		bool _value = true;
	};

	class OptionString : public Option {
	public:
		OptionString(std::string Name, std::string defaultValue = "", bool Technical = false) : Option(Name, OptionType::STRING, defaultValue, "", "", Technical) { _value = defaultValue; };
		virtual ~OptionString() { };
		void set(std::string value) { _value = value; }
		inline std::string getValue() { return _value; }
		void read(std::vector<std::string> &tokens);
	protected:
		std::string _value;
	};

	class OptionButton : public Option {
	public:
		OptionButton(std::string Name, bool Technical = false) : Option(Name, OptionType::BUTTON, "", "", "", Technical) { };
		virtual ~OptionButton() { };
		void set(std::string value) { }
#pragma warning(disable:4100) 
		inline void read(std::vector<std::string> &tokens) { }
#pragma warning(default:4100) 
	};

	class OptionThread : public OptionSpin {
	public:
		OptionThread() : OptionSpin(OPTION_THREADS, parameter.HelperThreads + 1, 1, 128) { };
		virtual ~OptionThread() { };
		void set(std::string value);
	};
	
	class Option960 : public OptionCheck {
	public:
		Option960() : OptionCheck(OPTION_CHESS960, Chess960) { };
		virtual ~Option960() { };
		void set(std::string value);
		inline void set(bool value) { Chess960 = value; };
	};

	class OptionContempt : public OptionSpin {
	public:
		OptionContempt() : OptionSpin(OPTION_CONTEMPT, parameter.Contempt, -1000, 1000) { };
		virtual ~OptionContempt() { };
		void set(std::string value);
		void set(int value);
	};

	class OptionHash : public OptionSpin {
	public:
		OptionHash() : OptionSpin(OPTION_HASH, 32, 1, 16384) { };
		virtual ~OptionHash() { };
		void set(std::string value);
		void set(int value);
	};

	class Options : public std::map<std::string, Option *> {
	public:
		Options();
		virtual ~Options();
		void printUCI();
		void printInfo();
		void read(std::vector<std::string> &tokens);
		int getInt(std::string key);
		bool getBool(std::string key);
		std::string getString(std::string key);
		void set(std::string key, std::string value);
		void set(std::string key, int value);
		void set(std::string key, bool value);
	private:
		void initialize();
	};

	extern Options options;

}