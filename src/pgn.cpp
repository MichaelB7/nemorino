#include <regex>
#include <sstream>
#include <fstream>
#include <iostream>
#include "pgn.h"
#include "utils.h"

namespace pgn {

	PieceType getPieceType(char c) {
		switch (c) {
		case 'Q': case 'D': return QUEEN;
		case 'R': case 'T': return ROOK;
		case 'B': case 'L': return BISHOP;
		case 'N': case 'S': return KNIGHT;
		case 'K': return KING;
		default: return PAWN;
		}
	}

	Square getSquare(char file, char rank) {
		int fileI = int(file) - int('a');
		int rankI = int(rank) - int('1');
		return Square(8 * rankI + fileI);
	}
		 
	/* Tokenizing astd::string */
	std::vector<std::string> tokenize(const std::string& p_pcstStr, char delim)  {
		std::vector<std::string> tokens;
		std::stringstream   mySstream(p_pcstStr);
		std::string         temp;
		while (std::getline(mySstream, temp, delim)) {
			tokens.push_back(temp);
		}
		return tokens;
	}
	//Parses a single move (assuming that only move data is part of the std::string - no annotations,
	//no check symbol, ...
	Move parseSANMove(std::string move, position & pos) {
		size_t indx = move.find_first_not_of("abcdefgh12345678QRBNKDLSTO0-x=");
		if (indx != std::string::npos) move = move.substr(0, indx);
		move = utils::Trim(move);
		ValuatedMove * moves = pos.GenerateMoves<LEGAL>();
		Move m;
		if (!move.compare("0-0") || !move.compare("O-O") || !move.compare("0-0-0") || !move.compare("O-O-O")) {
			m = moves->move;
			while (m) {
				if (type(m) == CASTLING) {
					if ((to(m) & 7) == 6 && move.length() < 5) return m;
					else if ((to(m) & 7) == 2 && move.length() > 4) return m;
				}
				++moves;
				m = moves->move;
			}
			return MOVE_NONE;
		}
		PieceType movingPieceType = getPieceType(move[0]);
		bool isPromotion = movingPieceType == PAWN && move.find("=") != std::string::npos;
		size_t len = move.length();
		Square targetSquare;
		if (isPromotion) {
			targetSquare = getSquare(move[len - 4], move[len - 3]);
			m = moves->move;
			while (m) {
				if ((type(m) == PROMOTION) && (to(m) == targetSquare) && (GetPieceType(pos.GetPieceOnSquare(from(m))) == movingPieceType)) return m;
				++moves;
				m = moves->move;
			}
			return MOVE_NONE;
		}
		targetSquare = getSquare(move[len - 2], move[len - 1]);
		Move m1 = MOVE_NONE;
		Move m2 = MOVE_NONE;
		m = moves->move;
		while (m) {
			if ((to(m) == targetSquare) && (GetPieceType(pos.GetPieceOnSquare(from(m))) == movingPieceType)) {
				if (m1) {
					m2 = m;
					break;
				}
				else m1 = m;
			}
			++moves;
			m = moves->move;
		}
		//Now we either have a unique move or we need disambiguation
		if (m2) {
			if (movingPieceType == PAWN) {
				//Only option for ambiguity: pawn capture => file is stored in 1st char
				int fromFile = int(move[0]) - int('a');
				if ((from(m1) & 7) == fromFile) return m1; else return m2;
			}
			else {
				//Non-Pawn move => 2nd char needed 
				int fromFile = int(move[1]) - int('a');
				if (fromFile >= 0 && fromFile <= 7) {
					if ((from(m1) & 7) == fromFile) return m1; else return m2;
				}
				int fromRank = int(move[1]) - int('1');
				if ((from(m1) / 8) == fromRank) return m1; else return m2;
			}
		}
		else return m1;
	}

	const std::string INITIAL_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	std::vector<Move> parsePGNGame(std::vector<std::string> lines, std::string & fen) {
		std::vector<Move> moves;
		position * pos = new position();
		fen = INITIAL_FEN;
		for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
			std::string line = utils::Trim(*it);
			if (line.length() == 0) continue;
			if (line[0] == '[') {
				if (!line.substr(0, 4).compare("[FEN")) {
					size_t indx1 = line.find_first_of("\"");
					size_t indx2 = line.find_last_of("\"");
					fen = line.substr(indx1 + 1, indx2 - indx1 - 1);
					delete(pos);
					pos = new position(fen);
				}
				continue;
			}
			
			std::vector<std::string> tokens = tokenize(line, ' ');
			for (std::vector<std::string>::iterator t = tokens.begin(); t != tokens.end(); ++t) {
				std::string token((*t).append("\0"));
				if (token.find_first_not_of("abcdefgh1234567890KQRBNx+#=.") !=std::string::npos) goto END;
				if (token.find('.') !=std::string::npos) continue; //Move number
				Move move = parseSANMove(token, *pos);
				if (move) {
					moves.push_back(move);
					pos->ApplyMove(move);
				}
			}
		}
		END: delete(pos);
		return moves;
	}

	std::vector<Move> parsePGNGame(std::vector<std::string> lines) {
		std::string fen;
		return parsePGNGame(lines, fen);
	}

	std::vector<std::vector<Move>> parsePGNFile(std::string filename) {
		std::vector<std::vector<Move>> result;
		std::ifstream s;
		s.open(filename);
		std::string l;
		bool inTag = false;
		bool inMoves = false;
		std::vector<std::string> * gameLines = new std::vector<std::string>();
		if (s.is_open())
		{
			while (s)
			{
				std::getline(s, l);
				std::string line = utils::Trim(l);
				gameLines->push_back(line);
				if (line.length() > 0) {
					if (inTag && line[0] != '[') {
						inTag = false;
						inMoves = true;
					}
					else if (!inTag && line[0] == '[') {
						if (inMoves) {
							result.push_back(parsePGNGame(*gameLines));
							gameLines->clear();
						}
						inTag = true;
						inMoves = false;
					}
				}
			}
			if (gameLines->size() > 0) result.push_back(parsePGNGame(*gameLines));
		}
		delete(gameLines);
		return result;
	}

	std::map<std::string, Move> parsePGNExerciseFile(std::string filename) {
		std::map<std::string, Move> result;
		std::ifstream s;
		s.open(filename);
		std::string l;
		bool inTag = false;
		bool inMoves = false;
		uint64_t gameCount = 0;
		std::vector<std::string> * gameLines = new std::vector<std::string>();
		if (s.is_open())
		{
			while (s)
			{
				std::getline(s, l);
				std::string line = utils::Trim(l);
				gameLines->push_back(line);
				if (line.length() > 0) {
					if (inTag && line[0] != '[') {
						inTag = false;
						inMoves = true;
					}
					else if (!inTag && line[0] == '[') {
						if (inMoves) {
							std::string fen;
							std::vector<Move> moves = parsePGNGame(*gameLines, fen);
							if (moves.size() > 0){
								result[fen] = moves[0];
								gameCount++;
								if ((gameCount % 1000) == 0)std::cout << gameCount << std::endl;
							}
							gameLines->clear();
						}
						inTag = true;
						inMoves = false;
					}
				}
			}
			if (gameLines->size() > 0) {
				std::string fen;
				std::vector<Move> moves = parsePGNGame(*gameLines, fen);
				result[fen] = moves[0];
			}
		}
		delete(gameLines);
		return result;
	}

}