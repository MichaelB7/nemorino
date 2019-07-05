#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "evaluation.h"
#include "position.h"
#include "settings.h"

Eval Contempt(settings::parameter.Contempt, settings::parameter.Contempt / 2);

Value evaluateDefault(const Position& pos) {
	Evaluation result;
	result.Material = pos.GetMaterialTableEntry()->Evaluation;
	result.Mobility = evaluateMobility(pos);
	result.KingSafety = evaluateKingSafety(pos);
	result.PawnStructure = pos.PawnStructureScore();
	result.Threats = 7 * (evaluateThreats<WHITE>(pos) - evaluateThreats<BLACK>(pos)) / 8;
	result.Pieces = evaluatePieces<WHITE>(pos) - evaluatePieces<BLACK>(pos);
	result.PsqEval = pos.GetPsqEval();
	return result.GetScore(pos);
}

std::string printDefaultEvaluation(const Position& pos) {
	std::stringstream ss;
	Evaluation result;
	result.Material = pos.GetMaterialTableEntry()->Evaluation;
	result.Mobility = evaluateMobility(pos);
	result.KingSafety = evaluateKingSafety(pos);
	result.PawnStructure = pos.PawnStructureScore();
	result.Threats = evaluateThreats<WHITE>(pos) - evaluateThreats<BLACK>(pos);
	result.Pieces = evaluatePieces<WHITE>(pos) - evaluatePieces<BLACK>(pos);
	result.PsqEval = pos.GetPsqEval();
	ss << "       Material: " << result.Material.print() << std::endl;
	ss << "       Mobility: " << result.Mobility.print() << std::endl;
	ss << "    King Safety: " << result.KingSafety.print() << std::endl;
	ss << " Pawn Structure: " << result.PawnStructure.print() << std::endl;
	ss << "Threats (White): " << evaluateThreats<WHITE>(pos).print() << std::endl;
	ss << "Threats (Black): " << evaluateThreats<BLACK>(pos).print() << std::endl;
	ss << " Pieces (White): " << evaluatePieces<WHITE>(pos).print() << std::endl;
	ss << " Pieces (Black): " << evaluatePieces<BLACK>(pos).print() << std::endl;
	ss << "           PSQT: " << pos.GetPsqEval().print() << std::endl;
	Value score = result.GetScore(pos);
	if (pos.GetSideToMove() == BLACK) score = -score;
	ss << "Total evaluation: " << static_cast<int>(score) << " (white side)" << std::endl;
	return ss.str();
}

Value evaluateDraw(const Position& pos) {
	return Contempt.getScore(pos.GetMaterialTableEntry()->Phase) * (1 - 2 * pos.GetSideToMove());;
}

Eval evaluateKingSafety(const Position& pos) {
	if (pos.GetMaterialTableEntry()->Phase > PHASE_LIMIT_ENDGAME) return EVAL_ZERO;
	Eval result;
	int attackerCount[2] = { 0, 0 };
	int attackWeight[2] = { 0, 0 };
	int kingZoneAttacks[2] = { 0, 0 };
	Bitboard kingRing[2] = { pos.PieceBB(KING, WHITE) | KingAttacks[pos.KingSquare(WHITE)] , pos.PieceBB(KING, BLACK) | KingAttacks[pos.KingSquare(BLACK)] };
	Bitboard kingZone[2] = {
		(pos.PieceBB(KING, WHITE) & RANK1) != EMPTY ? kingRing[0] | (kingRing[0] << 8) : kingRing[0],
		(pos.PieceBB(KING, BLACK) & RANK8) != EMPTY ? kingRing[1] | (kingRing[1] >> 8) : kingRing[1]
	};
	if ((pos.KingSquare(WHITE) & 7) == 7) kingZone[0] |= kingRing[0] >> 1;  else if ((pos.KingSquare(WHITE) & 7) == 0)  kingZone[0] |= kingRing[0] << 1;
	if ((pos.KingSquare(BLACK) & 7) == 7)  kingZone[1] |= kingRing[1] >> 1; else if ((pos.KingSquare(BLACK) & 7) == 0) kingZone[1] |= kingRing[1] << 1;
	Bitboard bbExclQueen = pos.OccupiedBB() & ~pos.PieceTypeBB(QUEEN);
	for (int c = static_cast<int>(Color::WHITE); c <= static_cast<int>(Color::BLACK); ++c) {
		const Color color_attacker = static_cast<Color>(c);
		for (PieceType pt = PieceType::QUEEN; pt <= PieceType::KNIGHT; ++pt) {
			Bitboard pieceBB = pos.PieceBB(pt, color_attacker);
			while (pieceBB) {
				const Square s = lsb(pieceBB);
				Bitboard bbKingZoneAttacks = pos.GetAttacksFrom(s);
				Bitboard bbPinned;
				if (pt < KNIGHT &&
					(bbPinned = pos.GetAttacksFrom(s) & pos.PinnedPieces(static_cast<Color>(c ^ 1))) != EMPTY &&
					(InBetweenFields[s][pos.KingSquare(static_cast<Color>(c ^ 1))] & bbPinned) != EMPTY) {
					bbKingZoneAttacks |= InBetweenFields[s][pos.KingSquare(static_cast<Color>(c ^ 1))];
				}
				bbKingZoneAttacks &= kingZone[c ^ 1];
				if (bbKingZoneAttacks) {
					++attackerCount[c];
					attackWeight[c] += settings::parameter.ATTACK_WEIGHT[pt];
					kingZoneAttacks[c] += popcount(bbKingZoneAttacks);
				}
				pieceBB &= pieceBB - 1;
			}
		}
		attackerCount[c] += popcount(kingZone[c ^ 1] & pos.AttacksByPieceType(color_attacker, PAWN));
	}
	int attackScore[2] = { 0, 0 };
	Value attackVals[2] = { VALUE_ZERO, VALUE_ZERO };
	for (int c = static_cast<int>(Color::WHITE); c <= static_cast<int>(Color::BLACK); ++c) {
		const Color color_defender = static_cast<Color>(c);
		const Color color_attacker = static_cast<Color>(c ^ 1);
		//if (attackerCount[c ^ 1] + static_cast<int>(pos.PieceBB(QUEEN, color_attacker) != EMPTY) < 2) continue;
		Bitboard bbWeak = pos.AttacksByColor(color_attacker) & KingAttacks[pos.KingSquare(color_defender)] & ~pos.dblAttacks(color_defender);
		Bitboard bbUndefended = pos.AttacksByColor(color_attacker) & ~pos.AttacksByColor(color_defender) & kingZone[color_defender] & ~pos.ColorBB(color_attacker);
		attackScore[c] = attackerCount[c ^ 1] * attackWeight[c ^ 1];
		attackScore[c] += settings::parameter.KING_RING_ATTACK_FACTOR * kingZoneAttacks[c ^ 1];
		attackScore[c] += settings::parameter.WEAK_SQUARES_FACTOR * popcount(bbWeak | bbUndefended);
		attackScore[c] += settings::parameter.PINNED_FACTOR * popcount(pos.PinnedPieces(color_defender) & pos.ColorBB(color_defender) & ~pos.PieceTypeBB(PAWN));
		//attackScore[c] += settings::parameter.PINNED_FACTOR * popcount(pos.PinnedPieces(color_defender));
		attackScore[c] -= settings::parameter.ATTACK_WITH_QUEEN * (pos.GetMaterialTableEntry()->GetMostExpensivePiece(color_attacker) != QUEEN);
		//Safe checks
		Bitboard bbSafe = (~pos.AttacksByColor(color_defender) | (bbWeak & pos.dblAttacks(color_attacker))) & ~pos.ColorBB(color_attacker);
		const Bitboard bbRookAttacks = RookTargets(pos.KingSquare(color_defender), bbExclQueen);
		const Bitboard bbBishopAttacks = BishopTargets(pos.KingSquare(color_defender), bbExclQueen);
		if ((bbRookAttacks | bbBishopAttacks) & pos.AttacksByPieceType(color_attacker, QUEEN) & bbSafe)
			attackScore[c] += settings::parameter.SAFE_CHECK[QUEEN];
		bbSafe |= pos.dblAttacks(color_attacker) & ~(pos.dblAttacks(color_defender) | pos.ColorBB(color_attacker)) & pos.AttacksByPieceType(color_defender, QUEEN);
		if (bbRookAttacks & pos.AttacksByPieceType(color_attacker, ROOK) & bbSafe)
			attackScore[c] += settings::parameter.SAFE_CHECK[ROOK];
		if (bbBishopAttacks & pos.AttacksByPieceType(color_attacker, BISHOP) & bbSafe)
			attackScore[c] += settings::parameter.SAFE_CHECK[BISHOP];
		const Bitboard bbKnightAttacks = KnightAttacks[pos.KingSquare(color_defender)] & pos.AttacksByPieceType(color_attacker, KNIGHT);
		if (bbKnightAttacks & bbSafe)
			attackScore[c] += settings::parameter.SAFE_CHECK[KNIGHT];
		if (attackScore[c] > 0) {
			attackVals[c] = static_cast<Value>((attackScore[c] * attackScore[c]) >> settings::parameter.KING_DANGER_SCALE);
		}
	}
	result.mgScore += attackVals[BLACK] - attackVals[WHITE];
	Bitboard bbWhite = pos.PieceBB(PAWN, WHITE);
	Bitboard bbBlack = pos.PieceBB(PAWN, BLACK);
	//Pawn shelter/storm
	Eval pawnStorm;
	if (pos.PieceBB(KING, WHITE) & SaveSquaresForKing & HALF_OF_WHITE) { //Bonus only for castled king
		Bitboard bbShelter = bbWhite & ((kingRing[0] & ShelterPawns2ndRank) | (kingZone[0] & ShelterPawns3rdRank) | ((kingZone[0] << 8) & ShelterPawns4thRank));
		Eval shelter;
		while (bbShelter) {
			Square s = lsb(bbShelter);
			shelter += settings::parameter.PAWN_SHELTER[PawnShieldIndex[s]];
			bbShelter &= bbShelter - 1;
		}
		pawnStorm += shelter;
#ifndef NDEBUG
		Eval shelterOld = settings::parameter.PAWN_SHELTER_2ND_RANK * popcount(bbWhite & kingRing[0] & ShelterPawns2ndRank);
		shelterOld += settings::parameter.PAWN_SHELTER_3RD_RANK * popcount(bbWhite & kingZone[0] & ShelterPawns3rdRank);
		shelterOld += settings::parameter.PAWN_SHELTER_4TH_RANK * popcount(bbWhite & (kingZone[0] << 8) & ShelterPawns4thRank);
		assert(shelterOld.mgScore == shelter.mgScore && shelterOld.egScore == shelter.egScore);
#endif
		const bool kingSide = (pos.KingSquare(WHITE) & 7) > 3;
		Bitboard pawnStormArea = kingSide ? bbKINGSIDE : bbQUEENSIDE;
		Bitboard stormPawns = pos.PieceBB(PAWN, BLACK) & pawnStormArea & (HALF_OF_WHITE | RANK5);
		while (stormPawns) {
			const Square sq = lsb(stormPawns);
			stormPawns &= stormPawns - 1;
			if ((pos.GetAttacksFrom(sq) & pos.PieceBB(PAWN, WHITE)) != EMPTY) {
				int rank = sq >> 3;
				if (rank > 1 && rank < 4) pawnStorm -= Eval(settings::parameter.BONUS_LEVER_ON_KINGSIDE,0); //lever
			}
			else {
				const Piece blocker = pos.GetPieceOnSquare(static_cast<Square>(sq - 8));
				if (blocker == WKING || GetPieceType(blocker) == PAWN) continue;
			}
			pawnStorm -= settings::parameter.PAWN_STORM[(sq >> 3) - 1];
		}
	}
	if (pos.PieceBB(KING, BLACK) & SaveSquaresForKing & HALF_OF_BLACK) {
		Bitboard bbShelter = bbBlack & ((kingRing[1] & ShelterPawns2ndRank) | (kingZone[1] & ShelterPawns3rdRank) | ((kingZone[1] >> 8) & ShelterPawns4thRank));
		Eval shelter;
		while (bbShelter) {
			Square s = lsb(bbShelter);
			shelter += settings::parameter.PAWN_SHELTER[PawnShieldIndex[s]];
			bbShelter &= bbShelter - 1;
		}
		pawnStorm -= shelter;
#ifndef NDEBUG
		Eval shelterOld = settings::parameter.PAWN_SHELTER_2ND_RANK * popcount(bbBlack & kingRing[1] & ShelterPawns2ndRank);
		shelterOld += settings::parameter.PAWN_SHELTER_3RD_RANK * popcount(bbBlack & kingZone[1] & ShelterPawns3rdRank);
		shelterOld += settings::parameter.PAWN_SHELTER_4TH_RANK * popcount(bbBlack & (kingZone[1] >> 8) & ShelterPawns4thRank);
		assert(shelterOld.mgScore == shelter.mgScore && shelterOld.egScore == shelter.egScore);
#endif
		const bool kingSide = (pos.KingSquare(BLACK) & 7) > 3;
		Bitboard pawnStormArea = kingSide ? bbKINGSIDE : bbQUEENSIDE;
		Bitboard stormPawns = pos.PieceBB(PAWN, WHITE) & pawnStormArea & (HALF_OF_BLACK | RANK4);
		while (stormPawns) {
			const Square sq = lsb(stormPawns);
			stormPawns &= stormPawns - 1;
			if ((pos.GetAttacksFrom(sq) & pos.PieceBB(PAWN, BLACK)) != EMPTY) {
				int rank = sq >> 3;
				if (rank > 3 && rank < 6) pawnStorm += Eval(settings::parameter.BONUS_LEVER_ON_KINGSIDE, 0); //lever
			}
			else {
				const Piece blocker = pos.GetPieceOnSquare(static_cast<Square>(sq + 8));
				if (blocker == BKING || GetPieceType(blocker) == PAWN) continue;
			}
			pawnStorm += settings::parameter.PAWN_STORM[6 - (sq >> 3)];
		}
	}
	result += pawnStorm;
	return result;
}

Eval evaluateMobility(const Position& pos) {
	Eval result;
	//Create attack bitboards
	Bitboard abbWPawn = pos.AttacksByPieceType(WHITE, PAWN);
	Bitboard abbBPawn = pos.AttacksByPieceType(BLACK, PAWN);

	//Leichtfiguren (N+B)
	Bitboard abbWMinor = abbWPawn | pos.AttacksByPieceType(WHITE, KNIGHT) | pos.AttacksByPieceType(WHITE, BISHOP);
	Bitboard abbBMinor = abbBPawn | pos.AttacksByPieceType(BLACK, KNIGHT) | pos.AttacksByPieceType(BLACK, BISHOP);
	//Rooks
	Bitboard abbWRook = abbWMinor | pos.AttacksByPieceType(WHITE, ROOK);
	Bitboard abbBRook = abbBMinor | pos.AttacksByPieceType(BLACK, ROOK);

	Bitboard bbBlockedPawns[2] = { (pos.PieceBB(PAWN, WHITE) << 8) & pos.PieceBB(PAWN, BLACK), (pos.PieceBB(PAWN, BLACK) >> 8) & pos.PieceBB(PAWN, WHITE) };
	Bitboard allowedWhite = ~((pos.PieceBB(PAWN, WHITE) & (RANK2 | RANK3)) | bbBlockedPawns[WHITE] | pos.PieceBB(KING, WHITE));
	Bitboard allowedBlack = ~((pos.PieceBB(PAWN, BLACK) & (RANK6 | RANK7)) | bbBlockedPawns[BLACK] | pos.PieceBB(KING, BLACK));

	//Bitboard bbBattery[2] = { pos.BatteryAttacks(WHITE), pos.BatteryAttacks(BLACK) };
	//Now calculate Mobility
	//Queens can move to all unattacked squares and if protected to all squares attacked by queens or kings
	Bitboard pieceBB = pos.PieceBB(QUEEN, WHITE);
	while (pieceBB) {
		const Square square = lsb(pieceBB);
		Bitboard targets = pos.GetAttacksFrom(square) & allowedWhite;
		targets &= ~abbBRook;
		result += settings::parameter.MOBILITY_BONUS_QUEEN[popcount(targets)];
		pieceBB &= pieceBB - 1;
	}
	pieceBB = pos.PieceBB(QUEEN, BLACK);
	while (pieceBB) {
		const Square square = lsb(pieceBB);
		Bitboard targets = pos.GetAttacksFrom(square) & allowedBlack;
		targets &= ~abbWRook;
		result -= settings::parameter.MOBILITY_BONUS_QUEEN[popcount(targets)];
		pieceBB &= pieceBB - 1;
	}
	//Rooks can move to all unattacked squares and if protected to all squares attacked by rooks or less important pieces
	pieceBB = pos.PieceBB(ROOK, WHITE);
	while (pieceBB) {
		const Square square = lsb(pieceBB);
		Bitboard targets = pos.GetAttacksFrom(square) & allowedWhite;
		targets &= ~abbBMinor;
		result += settings::parameter.MOBILITY_BONUS_ROOK[popcount(targets)];
		pieceBB &= pieceBB - 1;
	}
	pieceBB = pos.PieceBB(ROOK, BLACK);
	while (pieceBB) {
		const Square square = lsb(pieceBB);
		Bitboard targets = pos.GetAttacksFrom(square) & allowedBlack;
		targets &= ~abbWMinor;
		result -= settings::parameter.MOBILITY_BONUS_ROOK[popcount(targets)];
		pieceBB &= pieceBB - 1;
	}
	//Leichtfiguren
	pieceBB = pos.PieceBB(BISHOP, WHITE);
	while (pieceBB) {
		const Square square = lsb(pieceBB);
		Bitboard targets = pos.GetAttacksFrom(square) & allowedWhite;
		targets &= ~abbBPawn;
		result += settings::parameter.MOBILITY_BONUS_BISHOP[popcount(targets)];
		pieceBB &= pieceBB - 1;
	}
	pieceBB = pos.PieceBB(BISHOP, BLACK);
	while (pieceBB) {
		const Square square = lsb(pieceBB);
		Bitboard targets = pos.GetAttacksFrom(square) & allowedBlack;
		targets &= ~abbWPawn;
		result -= settings::parameter.MOBILITY_BONUS_BISHOP[popcount(targets)];
		pieceBB &= pieceBB - 1;
	}
	pieceBB = pos.PieceBB(KNIGHT, WHITE);
	while (pieceBB) {
		const Square square = lsb(pieceBB);
		Bitboard targets = pos.GetAttacksFrom(square) & allowedWhite;
		targets &= ~abbBPawn;
		result += settings::parameter.MOBILITY_BONUS_KNIGHT[popcount(targets)];
		result += popcount(targets & EXTENDED_CENTER) * settings::parameter.MOBILITY_CENTER_EXTENDED_KNIGHT;
		result += popcount(targets & CENTER) * settings::parameter.MOBILITY_CENTER_KNIGHT;
		pieceBB &= pieceBB - 1;
	}
	pieceBB = pos.PieceBB(KNIGHT, BLACK);
	while (pieceBB) {
		const Square square = lsb(pieceBB);
		Bitboard targets = pos.GetAttacksFrom(square) & allowedBlack;
		targets &= ~abbWPawn;
		result -= settings::parameter.MOBILITY_BONUS_KNIGHT[popcount(targets)];
		result -= popcount(targets & EXTENDED_CENTER) * settings::parameter.MOBILITY_CENTER_EXTENDED_KNIGHT;
		result -= popcount(targets & CENTER) * settings::parameter.MOBILITY_CENTER_KNIGHT;
		pieceBB &= pieceBB - 1;
	}
	//Pawn mobility
	Bitboard pawnTargets = abbWPawn & pos.ColorBB(BLACK);
	pawnTargets |= (pos.PieceBB(PAWN, WHITE) << 8) & ~pos.OccupiedBB();
	result += Eval(10, 10) * popcount(pawnTargets);
	pawnTargets = abbBPawn & pos.ColorBB(WHITE);
	pawnTargets |= (pos.PieceBB(PAWN, BLACK) >> 8) & ~pos.OccupiedBB();
	result -= Eval(10, 10) * popcount(pawnTargets);
	return result;
}

Value evaluateFromScratch(const Position& pos) {
	Evaluation result;
	MaterialTableEntry * material = pos.GetMaterialTableEntry();
	for (PieceType pt = QUEEN; pt <= PAWN; ++pt) {
		int diff = popcount(pos.PieceBB(pt, WHITE)) - popcount(pos.PieceBB(pt, BLACK));
		material->Evaluation += diff * settings::parameter.PieceValues[pt];
	}
	const Phase_t phase = Phase(popcount(pos.PieceBB(QUEEN, WHITE)), popcount(pos.PieceBB(QUEEN, BLACK)),
		popcount(pos.PieceBB(ROOK, WHITE)), popcount(pos.PieceBB(ROOK, BLACK)),
		popcount(pos.PieceBB(BISHOP, WHITE)), popcount(pos.PieceBB(BISHOP, BLACK)),
		popcount(pos.PieceBB(KNIGHT, WHITE)), popcount(pos.PieceBB(KNIGHT, BLACK)));
	material->Phase = phase;
	result.Material = material->Evaluation;
	material->EvaluationFunction = &evaluateDefault;
	return result.GetScore(pos);
}

int scaleEG(const Position & pos)
{

	if (pos.GetMaterialTableEntry()->NeedsScaling() && pos.oppositeColoredBishops()) {
		if (pos.PieceTypeBB(ROOK) == EMPTY) {
			if (popcount(pos.PieceTypeBB(PAWN)) <= 1) return 18; return 62;
		}
		else return 92;
	}
	return 128;
}

Value evaluatePawnEnding(const Position& pos) {
	//try to find unstoppable pawns
	Value ppEval = VALUE_ZERO;
	if (pos.GetPawnEntry()->passedPawns) {
		int minUnstoppable[2] = { 999, 999 };
		int distant[2] = { 0, 0 };
		int distcount[2] = { 0, 0 };
		Bitboard wpassed = pos.GetPawnEntry()->passedPawns & pos.PieceBB(PAWN, WHITE);
		while (wpassed) {
			Square passedPawnSquare = lsb(wpassed);
			Square convSquare = ConversionSquare<WHITE>(passedPawnSquare);
			int distToConv = std::min(7 - (passedPawnSquare >> 3), 5);
			if (distToConv < (ChebishevDistance(pos.KingSquare(BLACK), convSquare) - (pos.GetSideToMove() == BLACK))) {
				minUnstoppable[0] = std::min(minUnstoppable[0], distToConv);
			}
			else {
				distcount[0]++;
				Square blockSquare = Square(passedPawnSquare + 8);
				int dtcSquare = (6 - distToConv) * (6 - distToConv);
				int distPoints = 2 * dtcSquare * ChebishevDistance(pos.KingSquare(BLACK), blockSquare) + dtcSquare * ChebishevDistance(pos.KingSquare(WHITE), blockSquare);
				distant[0] += distPoints;
			}
			wpassed &= wpassed - 1;
		}
		Bitboard bpassed = pos.GetPawnEntry()->passedPawns & pos.PieceBB(PAWN, BLACK);
		while (bpassed) {
			Square passedPawnSquare = lsb(bpassed);
			Square convSquare = ConversionSquare<BLACK>(passedPawnSquare);
			int distToConv = std::min((passedPawnSquare >> 3), 5);
			if (distToConv < (ChebishevDistance(pos.KingSquare(WHITE), convSquare) - (pos.GetSideToMove() == WHITE))) {
				minUnstoppable[1] = std::min(minUnstoppable[1], distToConv);
			}
			else {
				distcount[1]++;
				Square blockSquare = Square(passedPawnSquare - 8);
				int dtcSquare = (6 - distToConv) * (6 - distToConv);
				int distPoints = 2 * dtcSquare * ChebishevDistance(pos.KingSquare(WHITE), blockSquare) + dtcSquare * ChebishevDistance(pos.KingSquare(BLACK), blockSquare);
				distant[1] += distPoints;
			}
			bpassed &= bpassed - 1;
		}
		if (minUnstoppable[0] < (minUnstoppable[1] - (pos.GetSideToMove() == BLACK))) {
			ppEval += Value(settings::parameter.PieceValues[QUEEN].egScore - 100 * minUnstoppable[0]);
		}
		else if (minUnstoppable[1] < (minUnstoppable[0] - (pos.GetSideToMove() == WHITE))) {
			ppEval -= Value(settings::parameter.PieceValues[QUEEN].egScore - 100 * minUnstoppable[1]);
		}
		else {
			ppEval += Value(11 * (distant[0] * distcount[0] - distant[1] * distcount[1]) / 5);
		}
	}
	return Value((pos.GetMaterialScore() + pos.GetPawnEntry()->Score.egScore + pos.GetPsqEval().egScore + ppEval + Contempt.egScore) * (1 - 2 * pos.GetSideToMove()));
}

Value evaluateKBPxKBPx(const Position& pos) {
	Evaluation result;
	result.Material = pos.GetMaterialTableEntry()->Evaluation;
	result.Mobility = evaluateMobility(pos);
	result.PawnStructure = pos.PawnStructureScore();
	//result.Space = evaluateSpace<WHITE>(pos) -evaluateSpace<BLACK>(pos);
	Bitboard darkSquareBishops = pos.PieceTypeBB(BISHOP) & DARKSQUARES;
	if (darkSquareBishops != 0 && (darkSquareBishops & (darkSquareBishops - 1)) == 0) {
		//oposite colored bishops
		result.Material.egScore -= result.Material.egScore > 0 ? settings::parameter.PieceValues[PAWN].egScore : -settings::parameter.PieceValues[PAWN].egScore;
	}
	return result.GetScore(pos);
}



