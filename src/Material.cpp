#include <iostream>
#include "material.h"
#include "settings.h"
#include "evaluation.h"
#include "bbEndings.h"
#include "position.h"
#include "tbprobe.h"



MaterialTableEntry MaterialTable[MATERIAL_KEY_MAX + 2];
thread_local MaterialTableEntry UnusualMaterial;

MaterialKey_t calculateMaterialKey(int * pieceCounts) {
	MaterialKey_t key = MATERIAL_KEY_OFFSET;
	for (int i = WQUEEN; i <= BPAWN; ++i)
		key += materialKeyFactors[i] * pieceCounts[i];
	return key;
}

MaterialKey_t calculateMaterialKey(int nQW, int nQB, int nRW, int nRB, int nBW, int nBB, int nNW, int nNB, int nPW, int nPB)
{
	MaterialKey_t key = MATERIAL_KEY_OFFSET;
	int pc[10] = { nQW, nQB, nRW, nRB, nBW, nBB, nNW, nNB, nPW, nPB };
	for (int i = WQUEEN; i <= BPAWN; ++i)
		key += materialKeyFactors[i] * pc[i];
	return key;
}

MaterialTableEntry * initUnusual(const Position & pos)
{
	UnusualMaterial.Evaluation = calculateMaterialEval(pos);
	UnusualMaterial.EvaluationFunction = &evaluateDefault;
	UnusualMaterial.Phase = 128;
	UnusualMaterial.MostValuedPiece = 0;
	UnusualMaterial.Flags = MaterialSearchFlags::MSF_DEFAULT;
	if (popcount(pos.ColorBB(WHITE) | pos.ColorBB(BLACK)) <= tablebases::MaxCardinality) UnusualMaterial.Flags |= MaterialSearchFlags::MSF_TABLEBASE_ENTRY;
	return &UnusualMaterial;
}

Eval calculateMaterialEval(const Position &pos) {
	int diffQ = popcount(pos.PieceBB(QUEEN, WHITE)) - popcount(pos.PieceBB(QUEEN, BLACK));
	int diffR = popcount(pos.PieceBB(ROOK, WHITE)) - popcount(pos.PieceBB(ROOK, BLACK));
	int diffB = popcount(pos.PieceBB(BISHOP, WHITE)) - popcount(pos.PieceBB(BISHOP, BLACK));
	int diffN = popcount(pos.PieceBB(KNIGHT, WHITE)) - popcount(pos.PieceBB(KNIGHT, BLACK));
	int diffP = popcount(pos.PieceBB(PAWN, WHITE)) - popcount(pos.PieceBB(PAWN, BLACK));
	return diffQ*settings::parameter.PieceValues[QUEEN] + diffR*settings::parameter.PieceValues[ROOK] + diffB*settings::parameter.PieceValues[BISHOP] 
		+ diffN * settings::parameter.PieceValues[KNIGHT] + diffP * settings::parameter.PieceValues[PAWN];
}

//Calculation (only used for special situations like 3 Queens, ...)
Value calculateMaterialScore(const Position &pos) {
	return calculateMaterialEval(pos).mgScore;

}

const char P[10] = { 'Q', 'q', 'R', 'r', 'B', 'b', 'N', 'n', 'P', 'p' };
void printMaterial(int * pieceCounts) {
	for (int i = WQUEEN; i <= BPAWN; i += 2) {
		for (int j = 0; j < pieceCounts[i]; ++j) {
			std::cout << P[i];
		}
	}
	std::cout << "|";
	for (int i = BQUEEN; i <= BPAWN; i += 2) {
		for (int j = 0; j < pieceCounts[i]; ++j) {
			std::cout << P[i];
		}
	}
}

bool checkImbalance(int deltaQ, int deltaR, int deltaB, int deltaN, int * imb) {
	return imb[0] == deltaQ && imb[1] == deltaR && imb[2] == deltaB && imb[3] == deltaN;
}


void adjust() {
	int pieceCounts[10];
	int pieceCounts2[10];
	int imbalance[5];
	bool adjusted = false;
	while (!adjusted) {
		for (int nWQ = 0; nWQ <= 1; ++nWQ) {
			pieceCounts[0] = nWQ;
			for (int nBQ = 0; nBQ <= 1; ++nBQ) {
				pieceCounts[1] = nBQ;
				for (int nWR = 0; nWR <= 2; ++nWR) {
					pieceCounts[2] = nWR;
					for (int nBR = 0; nBR <= 2; ++nBR) {
						pieceCounts[3] = nBR;
						for (int nWB = 0; nWB <= 2; ++nWB) {
							pieceCounts[4] = nWB;
							for (int nBB = 0; nBB <= 2; ++nBB) {
								pieceCounts[5] = nBB;
								for (int nWN = 0; nWN <= 2; ++nWN) {
									pieceCounts[6] = nWN;
									for (int nBN = 0; nBN <= 2; ++nBN) {
										pieceCounts[7] = nBN;
										//Phase_t phase = Phase(nWQ, nBQ, nWR, nBR, nWB, nBB, nWN, nBN);
										for (int nWP = 0; nWP <= 8; ++nWP) {
											pieceCounts[8] = nWP;
											for (int nBP = 0; nBP <= 8; ++nBP) {											
												pieceCounts[9] = nBP;
												MaterialKey_t key = calculateMaterialKey(&pieceCounts[0]);
												for (int i = 0; i < 5; ++i) {
													imbalance[i] = pieceCounts[2 * i] - pieceCounts[2 * i + 1];
													if (imbalance[i] > 0) {
														for (int j = 0; j < 10; ++j) pieceCounts2[j] = pieceCounts[j];
														for (int count = 0; count < imbalance[i]; ++count) {
															pieceCounts2[2 * i] = count;
															MaterialKey_t key2 = calculateMaterialKey(&pieceCounts2[0]);
															if (MaterialTable[key].Evaluation.egScore < MaterialTable[key2].Evaluation.egScore) {
																MaterialTable[key].Evaluation = MaterialTable[key2].Evaluation;
																//printMaterial(&pieceCounts[0]); std::cout << " => "; printMaterial(&pieceCounts2[0]); std::cout << " " << MaterialTable[key].Evaluation.getScore(MaterialTable[key].Phase) << std::endl;
																adjusted = true;
															}
														}
													}
													else if (imbalance[i] < 0) {
														for (int j = 0; j < 10; ++j) pieceCounts2[j] = pieceCounts[j];
														for (int count = 0; count < -imbalance[i]; ++count) {
															pieceCounts2[2 * i + 1] = count;
															MaterialKey_t key2 = calculateMaterialKey(&pieceCounts2[0]);
															if (MaterialTable[key].Evaluation.egScore > MaterialTable[key2].Evaluation.egScore) {
																MaterialTable[key].Evaluation = MaterialTable[key2].Evaluation;
																//printMaterial(&pieceCounts[0]); std::cout << " => "; printMaterial(&pieceCounts2[0]); std::cout << " " << MaterialTable[key].Evaluation.getScore(MaterialTable[key].Phase) << std::endl;
																adjusted = true;
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void InitializeMaterialTable() {
	MaterialTableEntry undetermined;
	undetermined.Evaluation = Eval(VALUE_NOTYETDETERMINED);
	undetermined.Phase = 128;
	undetermined.EvaluationFunction = nullptr;
	undetermined.Flags = MaterialSearchFlags::MSF_DEFAULT;
	undetermined.MostValuedPiece = 0;
	std::fill_n(MaterialTable, MATERIAL_KEY_MAX + 2, undetermined);
	MaterialTable[MATERIAL_KEY_UNUSUAL].EvaluationFunction = &evaluateDefault;
	int pieceCounts[10];
	int imbalance[5];
	for (int nWQ = 0; nWQ <= 1; ++nWQ) {
		pieceCounts[0] = nWQ;
		for (int nBQ = 0; nBQ <= 1; ++nBQ) {
			pieceCounts[1] = nBQ;
			for (int nWR = 0; nWR <= 2; ++nWR) {
				pieceCounts[2] = nWR;
				for (int nBR = 0; nBR <= 2; ++nBR) {
					pieceCounts[3] = nBR;
					for (int nWB = 0; nWB <= 2; ++nWB) {
						pieceCounts[4] = nWB;
						for (int nBB = 0; nBB <= 2; ++nBB) {
							pieceCounts[5] = nBB;
							for (int nWN = 0; nWN <= 2; ++nWN) {
								pieceCounts[6] = nWN;
								for (int nBN = 0; nBN <= 2; ++nBN) {
									pieceCounts[7] = nBN;
									Phase_t phase = Phase(nWQ, nBQ, nWR, nBR, nWB, nBB, nWN, nBN);
									for (int nWP = 0; nWP <= 8; ++nWP) {
										pieceCounts[8] = nWP;
										for (int nBP = 0; nBP <= 8; ++nBP) {
											pieceCounts[9] = nBP;
											int pawnCount = nWP + nBP - 8;
											MaterialKey_t key = calculateMaterialKey(&pieceCounts[0]);
											assert(key <= MATERIAL_KEY_MAX);
											Eval evaluation(0);
											for (int i = 0; i < 5; ++i) {
												imbalance[i] = pieceCounts[2 * i] - pieceCounts[2 * i + 1];
												evaluation += imbalance[i] * settings::parameter.PieceValues[i];
											}
											evaluation += Eval(2 * imbalance[KNIGHT] * pawnCount);
											if ((nWB == 2 || nBB == 2) && imbalance[BISHOP] != 0) {
												//Bonus for bishop pair
												if (nWB == 2) {//White has Bishop pair
													evaluation += settings::parameter.BONUS_BISHOP_PAIR + (9 - nWP - nBP)*settings::parameter.SCALE_BISHOP_PAIR_WITH_PAWNS;
													if (nBN == 0 && nBB == 0) evaluation += settings::parameter.BONUS_BISHOP_PAIR_NO_OPP_MINOR;
												}
												else {
													evaluation -= settings::parameter.BONUS_BISHOP_PAIR + (9 - nWP - nBP)*settings::parameter.SCALE_BISHOP_PAIR_WITH_PAWNS;
													if (nWN == 0 && nWB == 0) evaluation -= settings::parameter.BONUS_BISHOP_PAIR_NO_OPP_MINOR;
												}
											}

											if (nWB == 1 && nBB == 1 && nWN == 0 && nBN == 0 && nWQ == 0 && nBQ == 0) MaterialTable[key].Flags |= MaterialSearchFlags::MSF_SCALE;
											if (imbalance[ROOK] != 0 && ((imbalance[ROOK] + imbalance[KNIGHT] + imbalance[BISHOP]) == 0)) {
												evaluation += imbalance[ROOK] * ((3 - nWQ - nBQ - nWR - nBR) * settings::parameter.SCALE_EXCHANGE_WITH_MAJORS
													+ (8 - nWP - nBP)*settings::parameter.SCALE_EXCHANGE_WITH_PAWNS);
											}
											//QvsRN
											if (checkImbalance(1, -1, 0, -1, &imbalance[0]))
												evaluation += settings::parameter.IMBALANCE_Q_vs_RN;
											else if (checkImbalance(-1, 1, 0, 1, &imbalance[0]))
												evaluation -= settings::parameter.IMBALANCE_Q_vs_RN;
											//QvsRB
											else if (checkImbalance(1, -1, -1, 0, &imbalance[0]))
												evaluation += settings::parameter.IMBALANCE_Q_vs_RB;
											else if (checkImbalance(-1, 1, 1, 0, &imbalance[0]))
												evaluation -= settings::parameter.IMBALANCE_Q_vs_RB;
											assert(MaterialTable[key].Evaluation.mgScore == VALUE_NOTYETDETERMINED);
											MaterialTable[key].Evaluation = evaluation;
											MaterialTable[key].Phase = phase;
											if (nWQ == 0 && nBQ == 0 && nWR == 0 && nBR == 0 && nWB == 0 && nBB == 0 && nWN == 0 && nBN == 0) MaterialTable[key].EvaluationFunction = &evaluatePawnEnding;
											else if (nWP == 0 && nWN == 0 && nWB == 0 && nWR == 0 && nWQ == 0) MaterialTable[key].EvaluationFunction = &easyMate < BLACK >;
											else if (nBP == 0 && nBN == 0 && nBB == 0 && nBR == 0 && nBQ == 0) MaterialTable[key].EvaluationFunction = &easyMate < WHITE >;
											else MaterialTable[key].EvaluationFunction = &evaluateDefault;
											if (nWQ > 0) MaterialTable[key].setMostValuedPiece(WHITE, QUEEN);
											else if (nWR > 0) MaterialTable[key].setMostValuedPiece(WHITE, ROOK);
											else if (nWB > 0) MaterialTable[key].setMostValuedPiece(WHITE, BISHOP);
											else if (nWN > 0) MaterialTable[key].setMostValuedPiece(WHITE, KNIGHT);
											else if (nWP > 0) MaterialTable[key].setMostValuedPiece(WHITE, PAWN);
											else MaterialTable[key].setMostValuedPiece(WHITE, KING);
											if (nBQ > 0) MaterialTable[key].setMostValuedPiece(BLACK, QUEEN);
											else if (nBR > 0) MaterialTable[key].setMostValuedPiece(BLACK, ROOK);
											else if (nBB > 0) MaterialTable[key].setMostValuedPiece(BLACK, BISHOP);
											else if (nBN > 0) MaterialTable[key].setMostValuedPiece(BLACK, KNIGHT);
											else if (nBP > 0) MaterialTable[key].setMostValuedPiece(BLACK, PAWN);
											else MaterialTable[key].setMostValuedPiece(BLACK, KING);
											if (MaterialTable[key].EvaluationFunction == &evaluateDefault) {
												if (MaterialTable[key].GetMostExpensivePiece(BLACK) != KING && nWQ == 0 && nWR == 0 && nWB == 1 && nWN == 0 && nWP == 1)
													MaterialTable[key].EvaluationFunction = &evaluateKBPKx<WHITE>;
												else if (MaterialTable[key].GetMostExpensivePiece(WHITE) != KING && nBQ == 0 && nBR == 0 && nBB == 1 && nBN == 0 && nBP == 1)
													MaterialTable[key].EvaluationFunction = &evaluateKBPKx<BLACK>;
											}
											if (nWQ == 0 && nBQ == 0 && nWR == 0 && nBR == 0 && (nWB + nWN) == 1 && (nBB + nBN) == 1 && nWP != nBP && (nWP + nBP) < 5 && std::abs(nWP - nBP) < 2) {
												MaterialTable[key].Evaluation.egScore = Value(MaterialTable[key].Evaluation.egScore / (4 - std::max(nWP, nBP)));
											}
											if (tablebases::MaxCardinality > 0) {
												int totalPieceCount = 2;
												for (int i = 0; i < 10; ++i) totalPieceCount += pieceCounts[i];
												if (totalPieceCount <= tablebases::MaxCardinality)
													MaterialTable[key].Flags |= MaterialSearchFlags::MSF_TABLEBASE_ENTRY;
											}
											if (nWQ == 0 && nWR == 0 && nWB == 0 && nWN == 0) MaterialTable[key].Flags |= MaterialSearchFlags::MSF_NO_NULLMOVE_WHITE;
											if (nBQ == 0 && nBR == 0 && nBB == 0 && nBN == 0) MaterialTable[key].Flags |= MaterialSearchFlags::MSF_NO_NULLMOVE_BLACK;
											assert(nWQ == (MaterialTable[key].GetMostExpensivePiece(WHITE) == QUEEN));
											assert(nBQ == (MaterialTable[key].GetMostExpensivePiece(BLACK) == QUEEN));
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	for (int i = 0; i < MATERIAL_KEY_MAX + 1; ++i) {
		if (MaterialTable[i].EvaluationFunction == nullptr) MaterialTable[i].EvaluationFunction = &evaluateFromScratch;
	}
	//Add theoretically drawn endgames
	for (int i = 0; i < 10; ++i) pieceCounts[i] = 0;
	//KBPxKBPx
	//pieceCounts[WBISHOP] = pieceCounts[BBISHOP] = 1;
	//for (int wP = 0; wP <= 8; ++wP) {
	//	for (int bP = 0; bP <= 8; ++bP) {
	//		pieceCounts[WPAWN] = wP;
	//		pieceCounts[BPAWN] = bP;
	//		MaterialKey_t key = calculateMaterialKey(&pieceCounts[0]);
	//		MaterialTable[key].EvaluationFunction = &evaluateKBPxKBPx;
	//	}
	//}
	//for (int i = 0; i < 10; ++i) pieceCounts[i] = 0;
	//KK
	MaterialKey_t key = calculateMaterialKey(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	MaterialTable[key].Evaluation = Eval(VALUE_DRAW);
	MaterialTable[key].EvaluationFunction = &evaluateDraw;
	MaterialTable[key].Flags |= MaterialSearchFlags::MSF_THEORETICAL_DRAW;
	//KBK
	pieceCounts[WBISHOP] = 1;
	key = calculateMaterialKey(0, 0, 0, 0, 1, 0, 0, 0, 0, 0);
	MaterialTable[key].Evaluation = Eval(VALUE_DRAW);
	MaterialTable[key].EvaluationFunction = &evaluateDraw;
	MaterialTable[key].Flags |= MaterialSearchFlags::MSF_THEORETICAL_DRAW;
	key = calculateMaterialKey(0, 0, 0, 0, 0, 1, 0, 0, 0, 0);
	MaterialTable[key].Evaluation = Eval(VALUE_DRAW);
	MaterialTable[key].EvaluationFunction = &evaluateDraw;
	MaterialTable[key].Flags |= MaterialSearchFlags::MSF_THEORETICAL_DRAW;
	//KNK 
	key = calculateMaterialKey(0, 0, 0, 0, 0, 0, 1, 0, 0, 0);
	MaterialTable[key].Evaluation = Eval(VALUE_DRAW);
	MaterialTable[key].EvaluationFunction = &evaluateDraw;
	MaterialTable[key].Flags |= MaterialSearchFlags::MSF_THEORETICAL_DRAW;
	key = calculateMaterialKey(0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
	MaterialTable[key].Evaluation = Eval(VALUE_DRAW);
	MaterialTable[key].EvaluationFunction = &evaluateDraw;
	MaterialTable[key].Flags |= MaterialSearchFlags::MSF_THEORETICAL_DRAW;
	//KNNK 
	key = calculateMaterialKey(0, 0, 0, 0, 0, 0, 2, 0, 0, 0);
	MaterialTable[key].Evaluation = Eval(VALUE_DRAW);
	MaterialTable[key].EvaluationFunction = &evaluateDraw;
	key = calculateMaterialKey(0, 0, 0, 0, 0, 0, 0, 2, 0, 0);
	MaterialTable[key].Evaluation = Eval(VALUE_DRAW);
	MaterialTable[key].EvaluationFunction = &evaluateDraw;
	for (int i = 0; i < 10; ++i) pieceCounts[i] = 0;
	//KPK
	key = calculateMaterialKey(0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
	MaterialTable[key].EvaluationFunction = &kpk::EvaluateKPK < WHITE >;
	key = calculateMaterialKey(0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
	MaterialTable[key].EvaluationFunction = &kpk::EvaluateKPK < BLACK >;
	//KQK, KRK
	key = calculateMaterialKey(1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	MaterialTable[key].EvaluationFunction = &easyMate < WHITE >;
	key = calculateMaterialKey(0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
	MaterialTable[key].EvaluationFunction = &easyMate < BLACK >;
	key = calculateMaterialKey(0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
	MaterialTable[key].EvaluationFunction = &easyMate < WHITE >;
	key = calculateMaterialKey(0, 0, 0, 1, 0, 0, 0, 0, 0, 0);
	MaterialTable[key].EvaluationFunction = &easyMate < BLACK >;
	//KBBK
	key = calculateMaterialKey(0, 0, 0, 0, 2, 0, 0, 0, 0, 0);
	MaterialTable[key].EvaluationFunction = &easyMate < WHITE >;
	key = calculateMaterialKey(0, 0, 0, 0, 0, 2, 0, 0, 0, 0);
	MaterialTable[key].EvaluationFunction = &easyMate < BLACK >;
	//KBNK
	key = calculateMaterialKey(0, 0, 0, 0, 1, 0, 1, 0, 0, 0);
	MaterialTable[key].EvaluationFunction = &evaluateKNBK < WHITE >;
	key = calculateMaterialKey(0, 0, 0, 0, 0, 1, 0, 1, 0, 0);
	MaterialTable[key].EvaluationFunction = &evaluateKNBK < BLACK >;
	//KQKR
	key = calculateMaterialKey(1, 0, 0, 1, 0, 0, 0, 0, 0, 0);
	MaterialTable[key].EvaluationFunction = &easyMate < WHITE >;
	key = calculateMaterialKey(0, 1, 1, 0, 0, 0, 0, 0, 0, 0);
	MaterialTable[key].EvaluationFunction = &easyMate < BLACK >;
	pieceCounts[BQUEEN] = 0;
	pieceCounts[WROOK] = 0;
	//Simple KxKx Endgames should all be draws (x <> PAWN)
	for (PieceType pt = QUEEN; pt < PAWN; ++pt) {
		pieceCounts[GetPiece(pt, WHITE)] = pieceCounts[GetPiece(pt, BLACK)] = 1;
		key = calculateMaterialKey(&pieceCounts[0]);
		MaterialTable[key].EvaluationFunction = &evaluateDraw;
		pieceCounts[GetPiece(pt, WHITE)] = pieceCounts[GetPiece(pt, BLACK)] = 0;
	}
	//KRKN and KRKB is also very drawish
	pieceCounts[WROOK] = pieceCounts[BBISHOP] = 1;
	key = calculateMaterialKey(&pieceCounts[0]); //KRKB
	MaterialTable[key].Evaluation = Eval(20);
	pieceCounts[BPAWN] = 1;
	key = calculateMaterialKey(&pieceCounts[0]); //KRKBP
	MaterialTable[key].Evaluation = Eval(10);
	pieceCounts[BBISHOP] = 0; pieceCounts[BKNIGHT] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);  //KRKNP
	MaterialTable[key].Evaluation = Eval(10);
	pieceCounts[BPAWN] = 0;
	key = calculateMaterialKey(&pieceCounts[0]); //KRKN
	MaterialTable[key].Evaluation = Eval(20);
	pieceCounts[BKNIGHT] = 0; pieceCounts[WROOK] = 0;
	pieceCounts[BROOK] = pieceCounts[WBISHOP] = 1;
	key = calculateMaterialKey(&pieceCounts[0]); //KBKR
	MaterialTable[key].Evaluation = Eval(-20);
	pieceCounts[WPAWN] = 1;
	key = calculateMaterialKey(&pieceCounts[0]); //KBPKR
	MaterialTable[key].Evaluation = Eval(-10);
	pieceCounts[WBISHOP] = 0; pieceCounts[WKNIGHT] = 1;
	key = calculateMaterialKey(&pieceCounts[0]); //KNPKR
	MaterialTable[key].Evaluation = Eval(-10);
	pieceCounts[WPAWN] = 0;
	key = calculateMaterialKey(&pieceCounts[0]); //KNKR
	MaterialTable[key].Evaluation = Eval(-20);
	pieceCounts[WKNIGHT] = 0; pieceCounts[BROOK] = 0;
	//KQNKQ and KQBKQ => draw
	pieceCounts[WQUEEN] = pieceCounts[BQUEEN] = 1;
	for (Piece piece = WBISHOP; piece <= BKNIGHT; ++piece) {
		pieceCounts[piece] = 1;
		key = calculateMaterialKey(&pieceCounts[0]);
		MaterialTable[key].Evaluation = Eval(0);
		MaterialTable[key].EvaluationFunction = &evaluateDraw;
		pieceCounts[piece] = 0;
	}
	pieceCounts[WQUEEN] = pieceCounts[BQUEEN] = 0;
	//KQKRR => draw
	pieceCounts[WQUEEN] = 1; pieceCounts[BROOK] = 2;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].Evaluation = Eval(0);
	MaterialTable[key].EvaluationFunction = &evaluateDraw;
	pieceCounts[WQUEEN] = 0; pieceCounts[BROOK] = 0;
	pieceCounts[BQUEEN] = 1; pieceCounts[WROOK] = 2;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].Evaluation = Eval(0);
	MaterialTable[key].EvaluationFunction = &evaluateDraw;
	pieceCounts[BQUEEN] = 0; pieceCounts[WROOK] = 0;
	//KQKRB => draw
	pieceCounts[WQUEEN] = 1; pieceCounts[BROOK] = 1;  pieceCounts[BBISHOP] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].Evaluation = Eval(0);
	MaterialTable[key].EvaluationFunction = &evaluateDraw;
	pieceCounts[WQUEEN] = 0; pieceCounts[BROOK] = 0;  pieceCounts[BBISHOP] = 0;
	pieceCounts[BQUEEN] = 1; pieceCounts[WROOK] = 1;  pieceCounts[WBISHOP] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].Evaluation = Eval(0);
	MaterialTable[key].EvaluationFunction = &evaluateDraw;
	pieceCounts[BQUEEN] = 0; pieceCounts[WROOK] = 0;  pieceCounts[WBISHOP] = 0;
	//KQKRN => drawisch but slight advantage for Queen
	pieceCounts[WQUEEN] = 1; pieceCounts[BROOK] = 1;  pieceCounts[BKNIGHT] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].Evaluation = Eval(25);
	pieceCounts[WQUEEN] = 0; pieceCounts[BROOK] = 0;  pieceCounts[BKNIGHT] = 0;
	pieceCounts[BQUEEN] = 1; pieceCounts[WROOK] = 1;  pieceCounts[WKNIGHT] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].Evaluation = Eval(-25);
	pieceCounts[BQUEEN] = 0; pieceCounts[WROOK] = 0;  pieceCounts[WKNIGHT] = 0;
	//KRKBN => drawish
	pieceCounts[WROOK] = 1; pieceCounts[BBISHOP] = 1; pieceCounts[BKNIGHT] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].Evaluation = Eval(-25);
	pieceCounts[WROOK] = 0; pieceCounts[BBISHOP] = 0; pieceCounts[BKNIGHT] = 0;
	pieceCounts[BROOK] = 1; pieceCounts[WBISHOP] = 1; pieceCounts[WKNIGHT] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].Evaluation = Eval(25);
	pieceCounts[BROOK] = 0; pieceCounts[WBISHOP] = 0; pieceCounts[WKNIGHT] = 0;
	//KRRKRB => draw
	pieceCounts[WROOK] = 2; pieceCounts[BBISHOP] = 1; pieceCounts[BROOK] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].EvaluationFunction = &evaluateDraw;
	MaterialTable[key].Evaluation = Eval(0);
	pieceCounts[WROOK] = 0; pieceCounts[BBISHOP] = 0; pieceCounts[BROOK] = 0;
	pieceCounts[BROOK] = 2; pieceCounts[WBISHOP] = 1; pieceCounts[WROOK] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].EvaluationFunction = &evaluateDraw;
	MaterialTable[key].Evaluation = Eval(0);
	pieceCounts[BROOK] = 0; pieceCounts[WBISHOP] = 0; pieceCounts[WROOK] = 0;
	//KRRKRN => drawish
	pieceCounts[WROOK] = 2; pieceCounts[BKNIGHT] = 1; pieceCounts[BROOK] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].Evaluation = Eval(20);
	pieceCounts[WROOK] = 0; pieceCounts[BKNIGHT] = 0; pieceCounts[BROOK] = 0;
	pieceCounts[BROOK] = 2; pieceCounts[WKNIGHT] = 1; pieceCounts[WROOK] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].Evaluation = Eval(-20);
	pieceCounts[BROOK] = 0; pieceCounts[WKNIGHT] = 0; pieceCounts[WROOK] = 0;
	//KRNKR and KRBKR are also drawish
	pieceCounts[WROOK] = pieceCounts[BROOK] = pieceCounts[WBISHOP] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].Evaluation = Eval(20);
	pieceCounts[WBISHOP] = 0; pieceCounts[WKNIGHT] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].Evaluation = Eval(10);
	pieceCounts[WKNIGHT] = 0; pieceCounts[BBISHOP] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].Evaluation = Eval(-20);
	pieceCounts[BBISHOP] = 0; pieceCounts[BKNIGHT] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].Evaluation = Eval(-10);
	pieceCounts[WROOK] = pieceCounts[BROOK] = pieceCounts[BKNIGHT] = 0;
	//KBPK
	pieceCounts[WBISHOP] = 1;
	pieceCounts[WPAWN] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].EvaluationFunction = &evaluateKBPK < WHITE >;
	pieceCounts[WBISHOP] = 0;
	pieceCounts[WPAWN] = 0;
	pieceCounts[BBISHOP] = 1;
	pieceCounts[BPAWN] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].EvaluationFunction = &evaluateKBPK < BLACK >;
	pieceCounts[BBISHOP] = 0;
	pieceCounts[BPAWN] = 0;
	//KQKP
	pieceCounts[WQUEEN] = pieceCounts[BPAWN] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].EvaluationFunction = &evaluateKQKP < WHITE >;
	MaterialTable[key].Flags |= MaterialSearchFlags::MSF_SKIP_PRUNING;
	pieceCounts[WQUEEN] = pieceCounts[BPAWN] = 0;
	pieceCounts[BQUEEN] = pieceCounts[WPAWN] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].EvaluationFunction = &evaluateKQKP < BLACK >;
	MaterialTable[key].Flags |= MaterialSearchFlags::MSF_SKIP_PRUNING;
	pieceCounts[BQUEEN] = pieceCounts[WPAWN] = 0;
	//KRKP
	pieceCounts[WROOK] = pieceCounts[BPAWN] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].EvaluationFunction = &evaluateKRKP<WHITE>;
	pieceCounts[WROOK] = pieceCounts[BPAWN] = 0;
	pieceCounts[BROOK] = pieceCounts[WPAWN] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].EvaluationFunction = &evaluateKRKP<BLACK>;
	pieceCounts[BROOK] = pieceCounts[WPAWN] = 0;
	//KNKP
	pieceCounts[WKNIGHT] = pieceCounts[BPAWN] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].EvaluationFunction = &evaluateKNKP<WHITE>;
	pieceCounts[WKNIGHT] = pieceCounts[BPAWN] = 0;
	pieceCounts[BKNIGHT] = pieceCounts[WPAWN] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].EvaluationFunction = &evaluateKNKP<BLACK>;
	pieceCounts[BKNIGHT] = pieceCounts[WPAWN] = 0;
	//KBKP
	pieceCounts[WBISHOP] = pieceCounts[BPAWN] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].EvaluationFunction = &evaluateKBKP<WHITE>;
	pieceCounts[WBISHOP] = pieceCounts[BPAWN] = 0;
	pieceCounts[BBISHOP] = pieceCounts[WPAWN] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].EvaluationFunction = &evaluateKBKP<BLACK>;
	pieceCounts[BBISHOP] = pieceCounts[WPAWN] = 0;
	//KNKPx
	pieceCounts[WKNIGHT] = 1;
	for (int pawnCount = 2; pawnCount < 9; ++pawnCount) {
		pieceCounts[BPAWN] = pawnCount;
		key = calculateMaterialKey(&pieceCounts[0]);
		MaterialTable[key].EvaluationFunction = &evaluateKNKPx<WHITE>;
	}
	pieceCounts[WKNIGHT] = pieceCounts[BPAWN] = 0;
	pieceCounts[BKNIGHT] = 1;
	for (int pawnCount = 2; pawnCount < 9; ++pawnCount) {
		pieceCounts[WPAWN] = pawnCount;
		key = calculateMaterialKey(&pieceCounts[0]);
		MaterialTable[key].EvaluationFunction = &evaluateKNKPx<BLACK>;
	}
	pieceCounts[BKNIGHT] = pieceCounts[WPAWN] = 0;
	//KBKPx
	pieceCounts[WBISHOP] = 1;
	for (int pawnCount = 2; pawnCount < 9; ++pawnCount) {
		pieceCounts[BPAWN] = pawnCount;
		key = calculateMaterialKey(&pieceCounts[0]);
		MaterialTable[key].EvaluationFunction = &evaluateKBKPx<WHITE>;
	}
	pieceCounts[WBISHOP] = pieceCounts[BPAWN] = 0;
	pieceCounts[BBISHOP] = 1;
	for (int pawnCount = 2; pawnCount < 9; ++pawnCount) {
		pieceCounts[WPAWN] = pawnCount;
		key = calculateMaterialKey(&pieceCounts[0]);
		MaterialTable[key].EvaluationFunction = &evaluateKBKPx<BLACK>;
	}
	pieceCounts[BBISHOP] = pieceCounts[WPAWN] = 0;
	//KQKQP
	pieceCounts[WQUEEN] = pieceCounts[WPAWN] = pieceCounts[BQUEEN] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].EvaluationFunction = &evaluateKQPKQ<WHITE>;
	//MaterialTable[key].Flags |= MSF_SKIP_PRUNING;
	pieceCounts[WPAWN] = 0;
	pieceCounts[BPAWN] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].EvaluationFunction = &evaluateKQPKQ<BLACK>;
	//MaterialTable[key].Flags |= MSF_SKIP_PRUNING;
	pieceCounts[WQUEEN] = pieceCounts[BPAWN] = pieceCounts[BQUEEN] = 0;
	//KQKRP
	pieceCounts[WQUEEN] = pieceCounts[BROOK] = pieceCounts[BPAWN] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].EvaluationFunction = &evaluateKQKRP<WHITE>;
	pieceCounts[WQUEEN] = pieceCounts[BROOK] = pieceCounts[BPAWN] = 0;
	pieceCounts[BQUEEN] = pieceCounts[WROOK] = pieceCounts[WPAWN] = 1;
	key = calculateMaterialKey(&pieceCounts[0]);
	MaterialTable[key].EvaluationFunction = &evaluateKQKRP<BLACK>;
	pieceCounts[BQUEEN] = pieceCounts[WROOK] = pieceCounts[WPAWN] = 0;
	adjust();
}