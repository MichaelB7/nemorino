#pragma once

#include "types.h"
#include "board.h"
#include "position.h"
#include "bbEndings.h"

int scaleEG(const Position& pos);

extern Eval Contempt;

struct Evaluation
{
public:
	Eval Material = EVAL_ZERO;
	Eval Mobility = EVAL_ZERO;
	Eval Threats = EVAL_ZERO;
	Eval KingSafety = EVAL_ZERO;
	Eval Pieces = EVAL_ZERO;
	Eval PsqEval = EVAL_ZERO;
	//Eval Space = EVAL_ZERO;
	Eval PawnStructure = EVAL_ZERO;

	inline Value GetScore(const Position & pos) {
		Eval total = Material + Mobility + KingSafety + Threats + Pieces + PawnStructure + PsqEval + Contempt;
		total.egScore = Value((scaleEG(pos) * int(total.egScore)) / 128);
		return total.getScore(pos.GetMaterialTableEntry()->Phase) * (1 - 2 * pos.GetSideToMove());
	}
};

Value evaluateDraw(const Position& pos);
Value evaluateFromScratch(const Position& pos);
template<Color WinningSide> Value evaluateKBPK(const Position& pos);
template<Color KBPSide> Value evaluateKBPKx(const Position& pos);
template <Color WinningSide> Value easyMate(const Position& pos);
template <Color WinningSide> Value evaluateKQKP(const Position& pos);
template <Color StrongerSide> Value evaluateKRKP(const Position& pos); 
template <Color StrongerSide> Value evaluateKNKP(const Position& pos);
template <Color StrongerSide> Value evaluateKBKP(const Position& pos);
template <Color SideWithoutPawns> Value evaluateKNKPx(const Position& pos);
template <Color SideWithoutPawns> Value evaluateKBKPx(const Position& pos);
template <Color StrongerSide> Value evaluateKQPKQ(const Position& pos);
template <Color StrongerSide> Value evaluateKQKRP(const Position& pos);
Value evaluateKBPxKBPx(const Position& pos);
Eval evaluateMobility(const Position& pos);
Eval evaluateKingSafety(const Position& pos);
Value evaluatePawnEnding(const Position& pos);
template <Color COL> Eval evaluateThreats(const Position& pos);
template <Color COL> Eval evaluatePieces(const Position& pos);
std::string printDefaultEvaluation(const Position& pos);

const int PSQ_GoForMate[64] = {
	100, 90, 80, 70, 70, 80, 90, 100,
	90, 70, 60, 50, 50, 60, 70, 90,
	80, 60, 40, 30, 30, 40, 60, 80,
	70, 50, 30, 20, 20, 30, 50, 70,
	70, 50, 30, 20, 20, 30, 50, 70,
	80, 60, 40, 30, 30, 40, 60, 80,
	90, 70, 60, 50, 50, 60, 70, 90,
	100, 90, 80, 70, 70, 80, 90, 100
};

const int BonusDistance[8] = { 0, 0, 100, 80, 60, 40, 20, 10 };

template <Color WinningSide> Value easyMate(const Position& pos) {
	Value result = pos.GetMaterialScore();
	if (WinningSide == WHITE) {
		result += VALUE_KNOWN_WIN;
		result += Value(PSQ_GoForMate[pos.KingSquare(BLACK)]);
		result += Value(BonusDistance[ChebishevDistance(pos.KingSquare(BLACK), pos.KingSquare(WHITE))]);
	}
	else {
		result -= VALUE_KNOWN_WIN;
		result -= Value(PSQ_GoForMate[pos.KingSquare(WHITE)]);
		result -= Value(BonusDistance[ChebishevDistance(pos.KingSquare(BLACK), pos.KingSquare(WHITE))]);
	}

	return result * (1 - 2 * pos.GetSideToMove());
}

//If Pawn isn't on 7th Rank it's always a win for stronger side
//on 7th Rank it is a if pawn is no rook or bishop pawn
//for rook or bishop pawns there is a bitbase in bbEndings
template<Color WinningSide> Value evaluateKQKP(const Position& pos) {
	Value result;
	//First check if it's a "clear" win
	Bitboard pawnBB = pos.PieceBB(PAWN, Color(WinningSide ^ 1));
	Square pawnSq = lsb(pawnBB);
	Square winningKingSquare = pos.KingSquare(WinningSide);
	int relativeRank = pawnSq >> 3;
	if (WinningSide == WHITE) relativeRank = 7 - relativeRank;
	if ((pawnBB & (A_FILE | C_FILE | F_FILE | H_FILE)) == 0)
		result = VALUE_KNOWN_WIN + settings::parameter.PieceValues[QUEEN].egScore - settings::parameter.PieceValues[PAWN].egScore - ChebishevDistance(winningKingSquare, pawnSq) - Value(relativeRank);
	else {

		if (relativeRank != Rank7)
			result = VALUE_KNOWN_WIN + settings::parameter.PieceValues[QUEEN].egScore - settings::parameter.PieceValues[PAWN].egScore - ChebishevDistance(winningKingSquare, pawnSq) - Value(relativeRank);
		else {
			//Now we need to probe the bitbase => therefore normalize position (White is winning and pawn is on left half of board)
			Square wKingSquare, bKingSquare, wQueenSquare;
			Color stm;
			if (WinningSide == WHITE) {
				wKingSquare = winningKingSquare;
				bKingSquare = pos.KingSquare(BLACK);
				wQueenSquare = lsb(pos.PieceBB(QUEEN, WHITE));
				stm = pos.GetSideToMove();
			}
			else {
				//flip colors
				pawnSq = Square(pawnSq ^ 56);
				wKingSquare = Square(winningKingSquare ^ 56);
				bKingSquare = Square(pos.KingSquare(WHITE) ^ 56);
				wQueenSquare = Square(lsb(pos.PieceBB(QUEEN, BLACK)) ^ 56);
				stm = Color(pos.GetSideToMove() ^ 1);
			}
			if ((pawnSq & 7) > FileD) {
				pawnSq = Square(pawnSq ^ 7);
				wKingSquare = Square(wKingSquare ^ 7);
				bKingSquare = Square(bKingSquare ^ 7);
				wQueenSquare = Square(wQueenSquare ^ 7);
			}
			bool isWin = pawnSq == A2 ? kqkp::probeA2(wKingSquare, wQueenSquare, bKingSquare, stm) : kqkp::probeC2(wKingSquare, wQueenSquare, bKingSquare, stm);
			if (isWin) result = VALUE_KNOWN_WIN + settings::parameter.PieceValues[QUEEN].egScore - settings::parameter.PieceValues[PAWN].egScore - ChebishevDistance(winningKingSquare, pawnSq) + Value(relativeRank);
			else return evaluateDraw(pos);
		}
	}

	return WinningSide == pos.GetSideToMove() ? result : -result;
}

template<Color WinningSide> Value evaluateKBPK(const Position& pos) {
	//Check for draw
	Value result;
	Bitboard bbPawn = pos.PieceBB(PAWN, WinningSide);
	Square pawnSquare = lsb(bbPawn);
	if ((bbPawn & (A_FILE | H_FILE)) != 0) { //Pawn is on rook file
		Square conversionSquare = WinningSide == WHITE ? Square(56 + (pawnSquare & 7)) : Square(pawnSquare & 7);
		if (oppositeColors(conversionSquare, lsb(pos.PieceBB(BISHOP, WinningSide)))) { //Bishop doesn't match conversion's squares color
			//Now check if weak king control the conversion square
			Bitboard conversionSquareControl = KingAttacks[conversionSquare] | (ToBitboard(conversionSquare));
			if (pos.PieceBB(KING, Color(WinningSide ^ 1)) & conversionSquareControl) return evaluateDraw(pos);
			if ((pos.PieceBB(KING, WinningSide) & conversionSquareControl) == 0) { //Strong king doesn't control conversion square
				Square weakKing = pos.KingSquare(Color(WinningSide ^ 1));
				Square strongKing = pos.KingSquare(WinningSide);
				Bitboard bbRank2 = WinningSide == WHITE ? RANK2 : RANK7;
				if ((ChebishevDistance(weakKing, conversionSquare) + (pos.GetSideToMove() != WinningSide) <= ChebishevDistance(pawnSquare, conversionSquare) - ((bbPawn & bbRank2) != 0)) //King is in Pawnsquare
					&& (ChebishevDistance(weakKing, conversionSquare) - (pos.GetSideToMove() != WinningSide) <= ChebishevDistance(strongKing, conversionSquare))) {
					//Weak King is in pawn square and at least as near to the conversion square as the Strong King => search must solve it
					if (WinningSide == WHITE) {
						//Todo: check if factors 10 and 5 can't be replaced with 2 and 1
						result = pos.GetMaterialScore()
							+ Value(10 * (ChebishevDistance(weakKing, conversionSquare) - ChebishevDistance(strongKing, conversionSquare)))
							+ Value(5 * (pawnSquare >> 3));
					}
					else {
						//Todo: check if factors 10 and 5 can't be replaced with 2 and 1
						result = pos.GetMaterialScore()
							- Value(10 * (ChebishevDistance(weakKing, conversionSquare) - ChebishevDistance(strongKing, conversionSquare)))
							- Value(5 * (7 - (pawnSquare >> 3)));
					}
					return result * (1 - 2 * pos.GetSideToMove());
				}
			}
		}
	}
	result = WinningSide == WHITE ? VALUE_KNOWN_WIN + pos.GetMaterialScore() + Value(pawnSquare >> 3)
		: pos.GetMaterialScore() - VALUE_KNOWN_WIN - Value(7 - (pawnSquare >> 3));
	return result * (1 - 2 * pos.GetSideToMove());
}

template<Color KBPSide> Value evaluateKBPKx(const Position& pos) {
	Value result = evaluateDefault(pos);
	//If Side with KBP is winning according to standard evaluation and pawn is on rook file, check if that's really the case	
	if ((KBPSide == pos.GetSideToMove() && result <= evaluateDraw(pos)) || (KBPSide != pos.GetSideToMove() && result >= evaluateDraw(pos)) 
		||((pos.PieceBB(PAWN, KBPSide) & (A_FILE | H_FILE)) == EMPTY)) return result;
	//Now check if  bishop doesn't control conversion square but opposite king controls it => no win is possible
	Square pawnSquare = lsb(pos.PieceBB(PAWN, KBPSide));
	Square conversionSquare = KBPSide == WHITE ? Square(56 + (pawnSquare & 7)) : Square(pawnSquare & 7);
	if (!oppositeColors(conversionSquare, lsb(pos.PieceBB(BISHOP, KBPSide)))) return result;
	if (((KingAttacks[conversionSquare] | ToBitboard(conversionSquare)) & pos.PieceBB(KING, Color(KBPSide ^ 1))) != EMPTY) {
		//KBPSide can't win
		return evaluateDraw(pos);
	}
	return result;
}

const int PSQ_MateInCorner[64] = {
	200, 190, 180, 170, 160, 150, 140, 130,
	190, 180, 170, 160, 150, 140, 130, 140,
	180, 170, 155, 140, 140, 125, 140, 150,
	170, 160, 140, 120, 110, 140, 150, 160,
	160, 150, 140, 110, 120, 140, 160, 170,
	150, 140, 125, 140, 140, 155, 170, 180,
	140, 130, 140, 150, 160, 170, 180, 190,
	130, 140, 150, 160, 170, 180, 190, 200
};

template <Color WinningSide> Value evaluateKNBK(const Position& pos) {
	Square winnerKingSquare = pos.KingSquare(WinningSide);
	Square loosingKingSquare = pos.KingSquare(Color(WinningSide ^ 1));

	if ((DARKSQUARES & pos.PieceBB(BISHOP, WinningSide)) == 0) {
		//transpose KingSquares
		winnerKingSquare = Square(winnerKingSquare ^ 56);
		loosingKingSquare = Square(loosingKingSquare ^ 56);
	}

	Value result = VALUE_KNOWN_WIN + Value(BonusDistance[ChebishevDistance(winnerKingSquare, loosingKingSquare)]) + Value(PSQ_MateInCorner[loosingKingSquare]);
	return WinningSide == pos.GetSideToMove() ? result : -result;
}

//KQP vs KQ: Try to centralize Queens and weaker side should try to get his king to the "safe" areas
template <Color StrongerSide> Value evaluateKQPKQ(const Position& pos) {
	Value result = pos.GetMaterialScore() + pos.PawnStructureScore().getScore(pos.GetMaterialTableEntry()->Phase) + evaluateMobility(pos).egScore
		+ evaluateThreats<WHITE>(pos).egScore - evaluateThreats<BLACK>(pos).egScore;
	//result.Pieces = evaluatePieces<WHITE>(pos) -evaluatePieces<BLACK>(pos);
	if (StrongerSide == BLACK) result = -result;
	Square pawnSquare = lsb(pos.PieceTypeBB(PAWN));
	File pawnFile = File(pawnSquare & 7);
	Square conversionSquare = ConversionSquare<StrongerSide>(pawnSquare);
	Bitboard safeArea = KingAttacks[conversionSquare] & ToBitboard(conversionSquare);
	if (pawnFile <= FileB) {
		safeArea |= StrongerSide == BLACK ? KingAttacks[H8] | ToBitboard(H8) : KingAttacks[H1] | ToBitboard(H1);
	}
	else if (pawnFile >= FileG) {
		safeArea |= StrongerSide == BLACK ? KingAttacks[A8] | ToBitboard(A8) : KingAttacks[A1] | ToBitboard(A1);
	}
	if ((pos.PieceBB(KING, Color(StrongerSide ^ 1))&safeArea) != 0) result -= 30;
	if ((pos.PieceBB(QUEEN, StrongerSide) & CENTER) != 0) result += 10;
	//if ((pos.PieceBB(QUEEN, StrongerSide) & EXTENDED_CENTER) != 0) result += 10;
	//if ((pos.PieceBB(QUEEN, ~StrongerSide) & CENTER) != 0) result -= 5;
	//if ((pos.PieceBB(QUEEN, ~StrongerSide) & EXTENDED_CENTER) != 0) result -= 5;
	return StrongerSide == pos.GetSideToMove() ? result : -result;
	//Square pawnSquare = lsb(pos.PieceTypeBB(PAWN));
	//File pawnFile = File(pawnSquare & 7);
	//Square conversionSquare = ConversionSquare<StrongerSide>(pawnSquare);
	//Bitboard safeArea = KingAttacks[conversionSquare] & ToBitboard(conversionSquare);
	//if (pawnFile <= FileB) {
	//	safeArea |= StrongerSide == BLACK ? KingAttacks[H8] | ToBitboard(H8) : KingAttacks[H1] | ToBitboard(H1);
	//}
	//else if (pawnFile >= FileG) {
	//	safeArea |= StrongerSide == BLACK ? KingAttacks[A8] | ToBitboard(A8) : KingAttacks[A1] | ToBitboard(A1);
	//}
	//Value result = pos.PawnStructureScore() + pos.GetMaterialTableEntry()->Score;
	//if (StrongerSide == BLACK) result = -result;
	//if ((pos.PieceBB(QUEEN, StrongerSide) & CENTER) != 0) result += 1;
	//if ((pos.PieceBB(QUEEN, StrongerSide) & EXTENDED_CENTER) != 0) result += 1;
	//if ((pos.PieceBB(QUEEN, ~StrongerSide) & CENTER) != 0) result -= 1;
	//if ((pos.PieceBB(QUEEN, ~StrongerSide) & EXTENDED_CENTER) != 0) result -= 1;
	//if ((pos.PieceBB(KING, ~StrongerSide)&safeArea) != 0) result -= 4;
	////Strong King's best place is in front and near of his pawn
	//Bitboard kingArea = StrongerSide == WHITE ? KingAttacks[pawnSquare + 8] : KingAttacks[pawnSquare - 8];
	//if ((pos.PieceBB(KING, StrongerSide) & kingArea) != 0) result += 4;
	//else if ((kingArea & KingAttacks[lsb(pos.PieceBB(KING, StrongerSide))]) != 0) result += 2;
	//return StrongerSide == pos.GetSideToMove() ? result - 4 * pos.IsCheck() : -result;
}

//Stronger Side has no chance to win - this endgame is totally drawish, however if anybody can win, 
//it's the weaker side
template <Color StrongerSide> Value evaluateKNKP(const Position& pos) {
	Square pawnSquare = lsb(pos.PieceBB(PAWN, Color(StrongerSide ^ 1)));
	Square conversionSquare = StrongerSide == WHITE ? Square(pawnSquare & 7) : Square((pawnSquare & 7) + 56);
	int dtc = ChebishevDistance(pawnSquare, conversionSquare);
	//max dtc is 6 =>
	Value result = Value(dtc * dtc - 37); //From stronger side's POV
	return StrongerSide == pos.GetSideToMove() ? result : -result;
}

//Stronger Side has no chance to win - this endgame is totally drawish, however if anybody can win, 
//it's the weaker side
template <Color StrongerSide> Value evaluateKBKP(const Position& pos) {
	Value result;
	Square pawnSquare = lsb(pos.PieceBB(PAWN, Color(StrongerSide ^ 1)));
	Square bishopSquare = lsb(pos.PieceBB(BISHOP, StrongerSide));
	Square conversionSquare = StrongerSide == WHITE ? Square(pawnSquare & 7) : Square((pawnSquare & 7) + 56);
	Bitboard pawnPath = InBetweenFields[pawnSquare][conversionSquare] | ToBitboard(conversionSquare);
	if ((pawnPath & pos.PieceBB(KING, StrongerSide)) != 0 || (pawnPath & pos.GetAttacksFrom(bishopSquare)) != 0) {
		return evaluateDraw(pos);
	}
	else {
		int dtc = ChebishevDistance(pawnSquare, conversionSquare);
		result = Value(dtc * dtc - 38); //From stronger side's POV
	}
	return StrongerSide == pos.GetSideToMove() ? result : -result;
}

//Knight vs 2 or more pawns => Knight can nearly never win
//might convert to KNKQPx-1 Which will be evaluated in the default way 
//=> Therefore evaluation should never exceed default evaluation for this ending
//Evaluation should be positive for SideWithoutPawns (but drawish for 2 pawns)
template <Color SideWithoutPawns> Value evaluateKNKPx(const Position& pos) {
	Bitboard pawns = pos.PieceTypeBB(PAWN);
	int pawnCount = popcount(pawns);
	//Material with scaled down knight value
	//2 pawns => Knight counts half => total Material value = 0.5 Bishop - 2 Pawns = -40
	//3 pawns => Knight counts 2/3  => total Material Value = -80
	//4 pawns => Knight counts 3/4  => total Material Value = -160
	Value result = Value(int(int(settings::parameter.PieceValues[KNIGHT].egScore)*(1.0 - 1.0 / pawnCount))) - pawnCount * settings::parameter.PieceValues[PAWN].egScore; //from Side without Pawns POV
	//King and Knight should try to control or block the pawns frontfill
	Bitboard frontspan = SideWithoutPawns == WHITE ? FrontFillSouth(pawns) : FrontFillNorth(pawns);
	//result += Value(5 * popcount(frontspan & (pos.AttacksByPieceType(SideWithoutPawns, KNIGHT) | pos.PieceTypeBB(KNIGHT)))
	//	          + 5 * popcount(frontspan & (pos.AttacksByPieceType(SideWithoutPawns, KING) | pos.PieceBB(KING, SideWithoutPawns)))
	//			  - 10 * popcount((frontspan | pos.PieceTypeBB(PAWN)) & pos.AttacksByPieceType(~SideWithoutPawns, KING))
	//	);
	result += Value(popcount(frontspan & (pos.AttacksByPieceType(SideWithoutPawns, KNIGHT) | pos.PieceTypeBB(KNIGHT) | pos.AttacksByPieceType(SideWithoutPawns, KING) | pos.PieceBB(KING, SideWithoutPawns))));
	result -= Value(2 * popcount(pos.PieceTypeBB(PAWN) & pos.AttacksByPieceType(Color(SideWithoutPawns ^ 1), KING)));
	if (SideWithoutPawns == BLACK) result = -result; //now White's POV
	result += pos.PawnStructureScore().getScore(pos.GetMaterialTableEntry()->Phase);
	return (1 - 2 * pos.GetSideToMove()) * result;
}

//Bishop vs 2 or more pawns => Bishop can nearly never win
//therefore evaluation should be always positive for Side with Pawns
//To keep evaluation continuity evluation should be less than standard evaluation
//with an additional Knight => we try to use default evaluation but reduce material value
//of Bishop
template <Color SideWithoutPawns> Value evaluateKBKPx(const Position& pos) {
	Bitboard pawns = pos.PieceTypeBB(PAWN);
	//Bitboard frontspan = SideWithoutPawns == WHITE ? FrontFillSouth(pawns) : FrontFillNorth(pawns);
	int pawnCount = popcount(pawns);
	//Material with scaled down bishop value
	//2 pawns => Bishop counts half => total Material value = 0.5 Bishop - 2 Pawns = -40
	//3 pawns => Bishop counts 2/3  => total Material Value = -80
	//4 pawns => Bishop counts 3/4  => total Material Value = -160
	Value result = Value(int(int(settings::parameter.PieceValues[BISHOP].egScore)*(1.0 - 1.0 / pawnCount))) - pawnCount * settings::parameter.PieceValues[PAWN].egScore; //from Side without Pawns POV
	Bitboard frontspan = SideWithoutPawns == WHITE ? FrontFillSouth(pawns) : FrontFillNorth(pawns);
	result += Value(popcount(frontspan & (pos.AttacksByPieceType(SideWithoutPawns, BISHOP) | pos.PieceTypeBB(BISHOP) | pos.AttacksByPieceType(SideWithoutPawns, KING) | pos.PieceBB(KING, SideWithoutPawns))));
	result -= Value(2 * popcount(pos.PieceTypeBB(PAWN) & pos.AttacksByPieceType(Color(SideWithoutPawns ^ 1), KING)));
	if (SideWithoutPawns == BLACK) result = -result; //now White's POV
	//result += evaluateMobility(pos).egScore + pos.PawnStructureScore() + evaluateThreats<WHITE>(pos).egScore - evaluateThreats<BLACK>(pos).egScore;
	result += pos.PawnStructureScore().getScore(pos.GetMaterialTableEntry()->Phase);
	return (1 - 2 * pos.GetSideToMove()) * result;
}

template <Color StrongerSide> Value evaluateKRKP(const Position& pos) {
	Value result;
	//if the stronger King is in the front of the pawn it's a win
	Square pawnSquare = lsb(pos.PieceTypeBB(PAWN));
	Square strongerKingSquare = pos.KingSquare(StrongerSide);
	int dtc = StrongerSide == WHITE ? pawnSquare >> 3 : 7 - (pawnSquare >> 3);
	Bitboard pfront = StrongerSide == WHITE ? FrontFillSouth(pos.PieceTypeBB(PAWN)) : FrontFillNorth(pos.PieceTypeBB(PAWN));
	if (pfront & pos.PieceBB(KING, StrongerSide)) {
		result = VALUE_KNOWN_WIN + settings::parameter.PieceValues[ROOK].egScore - settings::parameter.PieceValues[PAWN].egScore + Value(dtc - ChebishevDistance(strongerKingSquare, pawnSquare));
		return StrongerSide == pos.GetSideToMove() ? result : -result;
	}
	Color WeakerSide = Color(StrongerSide ^ 1);
	int pawnRank = pawnSquare >> 3;
	if (StrongerSide == WHITE && pawnRank == 6) pawnSquare = Square(pawnSquare - 8);
	else if (StrongerSide == BLACK && pawnRank == 1) pawnSquare = Square(pawnSquare + 8);
	//if the weaker king is far away from rook and pawn it's a win
	Square weakKingSquare = pos.KingSquare(WeakerSide);
	Square rookSquare = lsb(pos.PieceTypeBB(ROOK));
	if (ChebishevDistance(weakKingSquare, pawnSquare) > (3 + (pos.GetSideToMove() == WeakerSide))
		&& ChebishevDistance(weakKingSquare, rookSquare) >= 3) {
		result = VALUE_KNOWN_WIN + settings::parameter.PieceValues[ROOK].egScore - settings::parameter.PieceValues[PAWN].egScore + Value(dtc - ChebishevDistance(strongerKingSquare, pawnSquare));
		return StrongerSide == pos.GetSideToMove() ? result : -result;
	}
	// If the pawn is far advanced and supported by the defending king,
	// the position is drawish
	Square conversionSquare = StrongerSide == WHITE ? Square(pawnSquare & 7) : Square((pawnSquare & 7) + 56);
	if (ChebishevDistance(pawnSquare, weakKingSquare) == 1
		&& ChebishevDistance(conversionSquare, weakKingSquare) <= 2
		&& ChebishevDistance(conversionSquare, strongerKingSquare) >= (3 + (pos.GetSideToMove() == StrongerSide))) {
		result = Value(80) - 8 * ChebishevDistance(strongerKingSquare, pawnSquare);
		return StrongerSide == pos.GetSideToMove() ? result : -result;
	}
	else {
		int pawnStep = StrongerSide == WHITE ? -8 : 8;
		result = Value(200) - 8 * (ChebishevDistance(strongerKingSquare, Square(pawnSquare + pawnStep))
			- ChebishevDistance(weakKingSquare, Square(pawnSquare + pawnStep))
			- dtc);
		return StrongerSide == pos.GetSideToMove() ? result : -result;
	}
}

template <Color StrongerSide> Value evaluateKQKRP(const Position& pos) {
	//Check for fortress
	Color weak = StrongerSide == WHITE ? BLACK : WHITE;
	Bitboard pawn = StrongerSide == WHITE ? 0x7e424242424200 : 0x42424242427e00;
	Bitboard rook = pos.PieceTypeBB(ROOK);
	if ((pos.PieceTypeBB(PAWN) & pawn) //Pawn is on fortress location
		&& (rook & 0x3c24243c0000) //Rook is on "6th Rank"
		&& (rook & pos.AttacksByPieceType(weak, PAWN)) //rook is protected by pawn (and by this cutting off the other king
		&& (pos.PieceTypeBB(PAWN) & pos.AttacksByPieceType(weak, KING))) //king is protecting own pawn
	{
		//check if strong king is on other side of rook as weak king
		if ((rook & C_FILE) && (pos.PieceBB(KING, weak) & (A_FILE | B_FILE)) && (pos.PieceBB(KING, StrongerSide) & (bbKINGSIDE | E_FILE | D_FILE))) return evaluateDraw(pos);
		if ((rook & F_FILE) && (pos.PieceBB(KING, weak) & (G_FILE | H_FILE)) && (pos.PieceBB(KING, StrongerSide) & (bbQUEENSIDE | E_FILE | D_FILE))) return evaluateDraw(pos);
		if ((rook & RANK3) && (pos.PieceBB(KING, weak) & (RANK1 | RANK2)) && (pos.PieceBB(KING, StrongerSide)& (HALF_OF_BLACK | RANK4))) return evaluateDraw(pos);
		if ((rook & RANK6) && (pos.PieceBB(KING, weak) & (RANK7 | RANK8)) && (pos.PieceBB(KING, StrongerSide)& (HALF_OF_WHITE | RANK5))) return evaluateDraw(pos);
		//Then we have a fortress
		return evaluateDraw(pos);
	}
	return evaluateDefault(pos);
}

template <Color COL> Eval evaluateThreats(const Position& pos) {
	enum { Defended, Weak };
	enum { Minor, Major };
	Bitboard b, weak, defended;
	Eval result = EVAL_ZERO;
	Color OTHER = Color(COL ^ 1);
	// Non-pawn enemies defended by a pawn
	defended = (pos.ColorBB(OTHER) ^ pos.PieceBB(PAWN, OTHER)) & pos.AttacksByPieceType(OTHER, PAWN);
	// Add a bonus according to the kind of attacking pieces
	if (defended)
	{
		b = defended & (pos.AttacksByPieceType(COL, KNIGHT) | pos.AttacksByPieceType(COL, BISHOP));
		while (b) {
			result += settings::parameter.Threat[Defended][Minor][GetPieceType(pos.GetPieceOnSquare(lsb(b)))];
			b &= b - 1;
		}
		b = defended & pos.AttacksByPieceType(COL, ROOK);
		while (b) {
			result += settings::parameter.Threat[Defended][Major][GetPieceType(pos.GetPieceOnSquare(lsb(b)))];
			b &= b - 1;
		}
	}
	// Enemies not defended by a pawn and under our attack
	weak = pos.ColorBB(OTHER)
		& ~pos.AttacksByPieceType(OTHER, PAWN)
		& pos.AttacksByColor(COL);
	// Add a bonus according to the kind of attacking pieces
	if (weak)
	{
		b = weak & (pos.AttacksByPieceType(COL, KNIGHT) | pos.AttacksByPieceType(COL, BISHOP));
		while (b) {
			result += settings::parameter.Threat[Weak][Minor][GetPieceType(pos.GetPieceOnSquare(lsb(b)))];
			b &= b - 1;
		}
		b = weak & (pos.AttacksByPieceType(COL, ROOK) | pos.AttacksByPieceType(COL, QUEEN));
		while (b) {
			result += settings::parameter.Threat[Weak][Major][GetPieceType(pos.GetPieceOnSquare(lsb(b)))];
			b &= b - 1;
		}
		b = weak & ~pos.AttacksByColor(OTHER);
		if (b) result += settings::parameter.HANGING * popcount(b);
		b = weak & pos.AttacksByPieceType(COL, KING);
		if (b) result += (b & (b - 1)) ? settings::parameter.KING_ON_MANY : settings::parameter.KING_ON_ONE;
	}
	return result;
}

template <Color COL> Eval evaluatePieces(const Position& pos) {
	Color OTHER = Color(COL ^ 1);
		//Rooks
	Bitboard rooks = pos.PieceBB(ROOK, COL);
	Eval bonusRook = EVAL_ZERO;
	if (rooks) {
		//bonusRook = popcount(rooks & seventhRank) * ROOK_ON_7TH;
		Bitboard bbHalfOpen = FileFill(pos.GetPawnEntry()->halfOpenFiles[COL]);
		Bitboard rooksOnSemiOpen = bbHalfOpen & rooks;
		bonusRook += popcount(rooksOnSemiOpen) * settings::parameter.ROOK_ON_SEMIOPENFILE;
		bonusRook += 2 * popcount(FileFill(pos.GetPawnEntry()->openFiles) & rooks) * settings::parameter.ROOK_ON_OPENFILE;
		Bitboard bbRookRays = FileFill(pos.PieceBB(ROOK, COL));
		bonusRook += popcount(bbRookRays & (pos.PieceBB(QUEEN, OTHER) | pos.PieceBB(KING, OTHER))) * Eval(5, 0);
	}
	//Passed Pawns (passed pawn bonus is already assigned statically in pawn::probe. Nevertheless all aspects related to position of other pieces have to be 
	//evaluated here) dynamically 
	Square ownKingSquare = pos.KingSquare(COL);
	Square opponentKingSquare = pos.KingSquare(OTHER);
	Eval bonusPassedPawns = EVAL_ZERO;
	Bitboard passedPawns = pos.GetPawnEntry()->passedPawns & pos.ColorBB(COL);
	while (passedPawns) {
		Square pawnSquare = lsb(passedPawns);
		Square blockSquare = COL == WHITE ? Square(pawnSquare + 8) : Square(pawnSquare - 8);
		uint8_t dtc = MovesToConversion<COL>(pawnSquare);
		int dtcSquare = (6 - dtc) * (6 - dtc);
		bonusPassedPawns.egScore += Value(2 * dtcSquare * ChebishevDistance(opponentKingSquare, blockSquare));
		bonusPassedPawns.egScore -= Value(1 * dtcSquare * ChebishevDistance(ownKingSquare, blockSquare));
		if ((pos.ColorBB(OTHER) & ToBitboard(blockSquare))!= EMPTY) bonusPassedPawns -= settings::parameter.MALUS_BLOCKED[dtc-1];
		passedPawns &= passedPawns - 1;
	}
	Eval malusLostCastles = (pos.GetMaterialTableEntry()->Phase < 128 && pos.HasCastlingLost(COL)) ? settings::parameter.MALUS_LOST_CASTLING : EVAL_ZERO;
	return bonusPassedPawns + bonusRook - malusLostCastles;
}
