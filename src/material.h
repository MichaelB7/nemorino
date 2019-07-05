#pragma once

#include "types.h"
//Material table is using the indexing scheme proposed by H.G.Müller in http://www.talkchess.com/forum/viewtopic.php?t=33561
const int materialKeyFactors[] = { 729, 1458, 486, -405, 279, -270, 246, -244, 2916, 26244, 0, 0, 0 };
const int MAX_PIECE_COUNT[] = { 1, 1, 2, 2, 2, 2, 2, 2 }; //Max piece count "normal" positions
const int MATERIAL_KEY_OFFSET = 1839; //this assures that the Material Key is always > 0 => material key = 0 can be used for unusual material due to promotions (2 queens,..)
const int MATERIAL_KEY_KvK = MATERIAL_KEY_OFFSET;
const int MATERIAL_KEY_MAX = 729 + 1458 + 2 * (486 + 279 + 246) + 8 * (2916 + 26244) + MATERIAL_KEY_OFFSET;
const int MATERIAL_KEY_UNUSUAL = MATERIAL_KEY_MAX + 1; //Entry for unusual Material distribution (like 3 Queens, 5 Bishops, ...)

enum MaterialSearchFlags : uint8_t {
	MSF_DEFAULT = 0,
	MSF_SKIP_PRUNING = 1,
	MSF_THEORETICAL_DRAW = 2,
	MSF_TABLEBASE_ENTRY = 4,
	MSF_SCALE = 8,
	MSF_NO_NULLMOVE_WHITE = 16,
	MSF_NO_NULLMOVE_BLACK = 32
};



struct MaterialTableEntry {
	Eval Evaluation;
	Phase_t Phase;
	EvalFunction EvaluationFunction = &evaluateDefault;
	uint8_t Flags;
	uint8_t MostValuedPiece; //high bits for black piece type

	inline bool IsLateEndgame() const { return EvaluationFunction != &evaluateDefault || Phase > 200; }
	inline bool SkipPruning() const { return (Flags & MaterialSearchFlags::MSF_SKIP_PRUNING) != 0; }
	inline bool IsTheoreticalDraw() const { return (Flags & MaterialSearchFlags::MSF_THEORETICAL_DRAW) != 0; }
	inline bool NeedsScaling() const { return (Flags & MaterialSearchFlags::MSF_SCALE) != 0; }
	inline bool DoNullmove(Color col) const { return (Flags & (MaterialSearchFlags::MSF_NO_NULLMOVE_WHITE << col)) == 0; }
	inline Value Score() const { return Evaluation.getScore(Phase); }
	inline bool IsTablebaseEntry() const { return (Flags & MaterialSearchFlags::MSF_TABLEBASE_ENTRY) != 0; }
	inline void SetIsTableBaseEntry(bool isTB) {
		if (isTB) Flags |= MaterialSearchFlags::MSF_TABLEBASE_ENTRY; else Flags &= ~MaterialSearchFlags::MSF_TABLEBASE_ENTRY;
	}
	inline PieceType GetMostExpensivePiece(Color color) const { return PieceType((MostValuedPiece >> (4 * (int)color)) & 15); }
	void setMostValuedPiece(Color color, PieceType pt) { 
		MostValuedPiece &= color == BLACK ? 15 : 240;
		MostValuedPiece |= color == BLACK ? pt << 4 : pt;
	}
};

extern MaterialTableEntry MaterialTable[MATERIAL_KEY_MAX + 2];
extern thread_local MaterialTableEntry UnusualMaterial;

//Phase is 0 in starting position and grows up to 256 when only kings are left
inline Phase_t Phase(int nWQ, int nBQ, int nWR, int nBR, int nWB, int nBB, int nWN, int nBN) {
	int phase = 24 - (nWQ + nBQ) * 4
		- (nWR + nBR) * 2
		- nWB - nBB - nWN - nBN;
	phase = (phase * 256 + 12) / 24;
	return Phase_t(phase);
}

const Phase_t PHASE_LIMIT_ENDGAME = Phase(0, 0, 1, 0, 0, 0, 1, 0);

void InitializeMaterialTable();

inline MaterialTableEntry * probe(MaterialKey_t key) { if (key == MATERIAL_KEY_UNUSUAL) return &UnusualMaterial; else return &MaterialTable[key]; }

MaterialTableEntry * initUnusual(const Position &pos);

Eval calculateMaterialEval(const Position &pos);

Value calculateMaterialScore(const Position &pos);

MaterialKey_t calculateMaterialKey(int * pieceCounts);
MaterialKey_t calculateMaterialKey(int nQW, int nQB, int nRW, int nRB, int nBW, int nBB, int nNW, int nNB, int nPW, int nPB);

inline uint64_t calculateMaterialHash(int * pieceCounts) { return calculateMaterialKey(pieceCounts) * 14695981039346656037ull; }
