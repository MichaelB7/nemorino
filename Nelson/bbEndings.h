#pragma once
#include "types.h"
#include "board.h"
#include "position.h"


namespace kpk
{
	bool probe(Square WhiteKingSquare, Square WhitePawnSquare, Square BlackKingSquare, Color stm);
	template <Color StrongerSide> Value EvaluateKPK(const Position &pos);

	template <Color StrongerSide> Value EvaluateKPK(const Position &pos) {
		Square pawnSquare;
		Square wKingSquare;
		Square bKingSquare;
		Color stm;
		Value result;
		if (StrongerSide == WHITE) {
			pawnSquare = lsb(pos.PieceBB(PAWN, WHITE));
			wKingSquare = pos.KingSquare(WHITE);
			bKingSquare = pos.KingSquare(BLACK);
			stm = pos.GetSideToMove();
		}
		else {
			//flip colors
			pawnSquare = Square(lsb(pos.PieceBB(PAWN, BLACK)) ^ 56);
			wKingSquare = Square(pos.KingSquare(BLACK) ^ 56);
			bKingSquare = Square(pos.KingSquare(WHITE) ^ 56);
			stm = Color(pos.GetSideToMove() ^ 1);
		}
		if ((pawnSquare & 7) > FileD) {
			pawnSquare = Square(pawnSquare ^ 7);
			wKingSquare = Square(wKingSquare ^ 7);
			bKingSquare = Square(bKingSquare ^ 7);
		}
		if (probe(wKingSquare, pawnSquare, bKingSquare, stm)) {
			result = VALUE_KNOWN_WIN + settings::parameter.PieceValues[PAWN].egScore + Value(pawnSquare >> 3);
		}
		else {
			return VALUE_DRAW;
		}
		return StrongerSide == pos.GetSideToMove() ? result : -result;
	}

}

namespace kqkp
{
	bool probeA2(Square WhiteKingSquare, Square WhiteQueenSquare, Square BlackKingSquare, Color stm);
	bool probeC2(Square WhiteKingSquare, Square WhiteQueenSquare, Square BlackKingSquare, Color stm);
}

