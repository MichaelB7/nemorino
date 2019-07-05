#pragma once
#include <vector>
#include "types.h"
#include "board.h"
#include "position.h"

namespace test {

	enum PerftType {
		BASIC,  //All moves are generated together
		P1, //tactical and Quiet Moves are generated seperately
		P2, //Winning, Equal, Loosing Captures and Quiets are generated separately
		P3,  //Move iterator is used
		P4   //Legal move generation
	};

	int64_t benchmark(int depth);
	int64_t benchmark(std::string filename, int depth);
	int64_t bench(std::vector<std::string> fens, int depth, int64_t &totalTime);
	int64_t bench(int depth, int64_t &totalTime); //Benchmark positions from SF
	int64_t bench2(int depth, int64_t &totalTime); //100 Random positions from GM games
	int64_t bench3(int depth, int64_t &totalTime); //200 Random positions from own games
	int64_t bench(std::string filename, int depth, int64_t &totalTime);


	uint64_t perft(Position &pos, int depth);
	uint64_t perft3(Position &pos, int depth);
	uint64_t perft4(Position &pos, int depth);
	uint64_t perftcomb(Position &pos, int depth);

	void divide(Position &pos, int depth);
	void divide3(Position &pos, int depth);
	bool testPerft(PerftType perftType = BASIC);
	void testPolyglotKey();
	bool testSEE();
	std::vector<std::string> readTextFile(std::string file);
	void testCheckQuietCheckMoveGeneration();
	void testTacticalMoveGeneration();
	void testSearch(Position &pos, int depth);
	void testFindMate();
	void testResult();
	void testRepetition();
	void testKPK();
	bool testBBOperations();
	bool testPopcount();
	bool testLSB();
	bool testMSB();
	bool testGivesCheck();

	bool testVerticalSymmetry();

	bool testingPlatformSpecifics();

}
