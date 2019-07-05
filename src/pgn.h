#pragma once

#include <vector>
#include <map>
#include "position.h"

namespace pgn {

	std::vector<std::vector<Move>> parsePGNFile(std::string filename);
	std::map<std::string, Move> parsePGNExerciseFile(std::string filename);
	Move parseSANMove(std::string move, Position &pos);
	std::vector<Move> parsePGNGame(std::vector<std::string> lines);
	std::vector<Move> parsePGNGame(std::vector<std::string> lines, std::string &fen);

	enum Result { WHITE_WINS, BLACK_WINS, DRAW, UNKNOWN };

	struct Game {
		Result result;
		std::vector<Move> Moves;
	};

}