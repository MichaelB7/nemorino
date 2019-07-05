#include <stdio.h>
#include <iostream>
#include <string> 
#include <chrono>
#include <iomanip>
#include <fstream>
#include <string>
#include <map>
#include <cstdlib>
#include <vector>
#include "test.h"
#include "search.h"
#include "hashtables.h"
#include "bbEndings.h"
#include "timemanager.h"

namespace test {

	int64_t benchmark(int depth) {
		int64_t runtime = 0;
		int64_t totalTime = 0;
		int64_t totalNodes = bench(depth, runtime);
		totalTime += runtime;
		totalNodes += bench2(depth, runtime);
		totalTime += runtime;
		std::cerr << "\n==========================="
			<< "\nTotal time (ms) : " << totalTime
			<< "\nNodes searched  : " << totalNodes
			<< "\nNodes/second    : " << 1000 * totalNodes / totalTime << std::endl;
		return totalNodes;
	}


	int64_t benchmark(std::string filename, int depth) {
		std::string line;
		int64_t runtime = 0;
		std::ifstream text(filename);
		if (text.is_open())
		{
			std::vector<std::string> fens;
			while (getline(text, line)) fens.push_back(line);
			text.close();
			int64_t totalNodes = bench(fens, depth, runtime);
			std::cerr << "\n==========================="
				<< "\nTotal time (ms) : " << runtime
				<< "\nNodes searched  : " << totalNodes
				<< "\nNodes/second    : " << 1000 * totalNodes / runtime << std::endl;
			return totalNodes;
		}
		else return -1;
	}

	int64_t bench(std::string filename, int depth, int64_t &totalTime) {
		std::string line;
		std::ifstream text(filename);
		if (text.is_open())
		{
			std::vector<std::string> fens;
			while (getline(text, line)) fens.push_back(line);
			text.close();
			return bench(fens, depth, totalTime);
		}
		else return -1;
	}

	std::vector<std::string> benchFens1() {
		std::vector<std::string> fens = {
			"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
			"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 10",
			"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 11",
			"4rrk1/pp1n3p/3q2pQ/2p1pb2/2PP4/2P3N1/P2B2PP/4RRK1 b - - 7 19",
			"rq3rk1/ppp2ppp/1bnpb3/3N2B1/3NP3/7P/PPPQ1PP1/2KR3R w - - 7 14",
			"r1bq1r1k/1pp1n1pp/1p1p4/4p2Q/4Pp2/1BNP4/PPP2PPP/3R1RK1 w - - 2 14",
			"r3r1k1/2p2ppp/p1p1bn2/8/1q2P3/2NPQN2/PPP3PP/R4RK1 b - - 2 15",
			"r1bbk1nr/pp3p1p/2n5/1N4p1/2Np1B2/8/PPP2PPP/2KR1B1R w kq - 0 13",
			"r1bq1rk1/ppp1nppp/4n3/3p3Q/3P4/1BP1B3/PP1N2PP/R4RK1 w - - 1 16",
			"4r1k1/r1q2ppp/ppp2n2/4P3/5Rb1/1N1BQ3/PPP3PP/R5K1 w - - 1 17",
			"2rqkb1r/ppp2p2/2npb1p1/1N1Nn2p/2P1PP2/8/PP2B1PP/R1BQK2R b KQ - 0 11",
			"r1bq1r1k/b1p1npp1/p2p3p/1p6/3PP3/1B2NN2/PP3PPP/R2Q1RK1 w - - 1 16",
			"3r1rk1/p5pp/bpp1pp2/8/q1PP1P2/b3P3/P2NQRPP/1R2B1K1 b - - 6 22",
			"r1q2rk1/2p1bppp/2Pp4/p6b/Q1PNp3/4B3/PP1R1PPP/2K4R w - - 2 18",
			"4k2r/1pb2ppp/1p2p3/1R1p4/3P4/2r1PN2/P4PPP/1R4K1 b - - 3 22",
			"3q2k1/pb3p1p/4pbp1/2r5/PpN2N2/1P2P2P/5PP1/Q2R2K1 b - - 4 26",
			"6k1/6p1/6Pp/ppp5/3pn2P/1P3K2/1PP2P2/3N4 b - - 0 1",
			"3b4/5kp1/1p1p1p1p/pP1PpP1P/P1P1P3/3KN3/8/8 w - - 0 1",
			"2K5/p7/7P/5pR1/8/5k2/r7/8 w - - 0 1",
			"8/6pk/1p6/8/PP3p1p/5P2/4KP1q/3Q4 w - - 0 1",
			"7k/3p2pp/4q3/8/4Q3/5Kp1/P6b/8 w - - 0 1",
			"8/2p5/8/2kPKp1p/2p4P/2P5/3P4/8 w - - 0 1",
			"8/1p3pp1/7p/5P1P/2k3P1/8/2K2P2/8 w - - 0 1",
			"8/pp2r1k1/2p1p3/3pP2p/1P1P1P1P/P5KR/8/8 w - - 0 1",
			"8/3p4/p1bk3p/Pp6/1Kp1PpPp/2P2P1P/2P5/5B2 b - - 0 1",
			"5k2/7R/4P2p/5K2/p1r2P1p/8/8/8 b - - 0 1",
			"6k1/6p1/P6p/r1N5/5p2/7P/1b3PP1/4R1K1 w - - 0 1",
			"1r3k2/4q3/2Pp3b/3Bp3/2Q2p2/1p1P2P1/1P2KP2/3N4 w - - 0 1",
			"6k1/4pp1p/3p2p1/P1pPb3/R7/1r2P1PP/3B1P2/6K1 w - - 0 1",
			"8/3p3B/5p2/5P2/p7/PP5b/k7/6K1 w - - 0 1"
		};
		return fens;
	}

	std::vector<std::string> benchFens2() {
		std::vector<std::string> fens = {
		"5r2/4r1kp/1pnb2p1/p4p2/1P1P4/1NPBpRPP/1P6/5RK1 w - a6",
		"1Q6/8/2N2k2/8/2p4p/6bK/1P6/5n2 b - -",
		"8/1B2k3/8/1p5R/5n1p/PKP2N1r/1P6/8 b - -",
		"r3kb1r/pp5p/q3p1p1/2p1Np2/1n2P2P/3PBQ2/P4PP1/1R3K1R w - f6",
		"8/2R2p2/4pk2/2pr3p/8/5K1P/5PP1/8 w - -",
		"8/5p2/b5p1/3p4/3k4/1R3P1P/6P1/6K1 w - -",
		"rnbqk2r/pp2ppbp/2p2np1/3p4/2PP4/1QN3P1/PP2PPBP/R1B1K1NR b KQkq -",
		"2b2r2/5k1p/1p6/p1pP1pp1/B1P5/1P6/P5PP/4R1K1 w - g6",
		"r1bq1rk1/2p2pp1/p1n2n1p/P1b1p3/1p2P3/1B3N2/1PPN1PPP/R1BQR1K1 w - -",
		"r1bqr1k1/1pp2pp1/1bnp1n1p/p3p3/2P5/P1NP2PP/1PN1PPB1/1RBQ1RK1 b - -",
		"rnbqkbnr/pp2pppp/8/2pp4/4P3/2P5/PP1P1PPP/RNBQKBNR w KQkq d6",
		"4Q1k1/p4p2/1qp1p1p1/2p4p/2P4P/2P1P1P1/P4PK1/8 b - -",
		"4k2r/pR1n1Np1/1n6/1rBp4/3Pp3/4P3/P2K1P1P/6R1 b k -",
		"1k1r3r/p1q1np1P/1p2p3/2n1P3/3p1PP1/P1p4R/2P1N1Q1/1RB1K3 b - -",
		"1rbqr1k1/p2nbppp/2p5/3p4/5B2/1PNB1Q1P/P1P2PP1/3R1RK1 b - -",
		"r1bq1rk1/5pbp/p2ppnp1/1p6/3BPP2/1BN2Q2/PPP3PP/R3K2R w KQ b6",
		"r7/5k1p/6p1/2R1B3/1P2P3/4K3/6bP/8 w - -",
		"rnbq1rk1/p4ppp/1p2pn2/6B1/1bBP4/2N2N2/PP3PPP/R2Q1RK1 b - -",
		"5rk1/p1r1b1pp/bp2Nn2/8/1P6/P7/1B3PPP/R3R1K1 b - -",
		"8/4bk2/p7/1p4P1/1PbB2n1/P5Pp/7P/4R1K1 b - -",
		"q2rr1k1/pb2bppp/1p2p1n1/8/2PP2n1/PQ3NPP/1B1N1P2/3RRBK1 b - -",
		"7k/p1r3p1/1p2P1Qp/5P2/8/P3q3/5RK1/8 b - -",
		"r1bq1rk1/2p1npbp/p2p2p1/np6/3PP3/2N2N1P/PPB2PP1/R1BQ1RK1 b - -",
		"2r3k1/5p2/6p1/R6p/1p3B2/1qb1QP1P/6PK/8 b - -",
		"2rqr1k1/1pB2pp1/p1n3b1/8/1P1pP1P1/P2B1N2/3Q1PK1/3RR3 b - -",
		"r3r1k1/pbqn1pbp/1pp2np1/2P5/3pP3/2N1BN1P/P1Q2PP1/3RRBK1 w - -",
		"2r2rk1/1p3pp1/p6p/q2n4/2n1N3/3Q1PP1/PP1B1R1P/R4K2 b - -",
		"2k4r/2qr1p2/p2p4/2p4p/P1N1P1p1/1Q6/2P2PPb/1R2R2K b - -",
		"5n1k/p6p/5N1q/2Pp4/8/PP1r2b1/1B4P1/RQ5K w - -",
		"r2qkb1r/pp1n1ppp/2p5/2p1p2b/4P3/3P1N1P/PPP2PP1/R1BQKN1R w KQkq e6",
		"r1b2rk1/3nbppp/nqp1p3/p1N5/1p1PN3/1P2Q1P1/1B2PPBP/R4RK1 w - -",
		"6k1/5bp1/8/2K1BP2/5P2/8/1R6/6r1 b - -",
		"6k1/6p1/8/2K1BP1b/5P2/8/1R6/6r1 w - -",
		"rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6",
		"rnbqk2r/1p2bppp/p2p1n2/4p3/4P3/1NN5/PPP1BPPP/R1BQ1RK1 b kq -",
		"r5k1/5ppp/pr2pn2/8/2P5/5NP1/P4PP1/1R3RK1 b - -",
		"rn2k2r/ppp3pp/3n4/4q3/4P3/P2B1P2/1PQ4P/R1B1K2R b KQkq -",
		"6k1/2p5/1p1p2q1/p2P1b1p/2P1p2B/KP3rP1/P7/2Q1R3 w - -",
		"2b2rk1/Q1R2ppp/4p3/8/4q3/1B4P1/P4PKP/8 w - -",
		"r1bq1rk1/pp2ppbp/2n2np1/2pp4/8/P1NP1NP1/1PP1PPBP/R1BQ1RK1 w - d6",
		"3kr3/1p1r4/p1p2p2/P1P3p1/1P2PbP1/7P/2K1R3/4NB2 w - -",
		"8/6k1/6p1/R4p2/Pp5p/5B2/1Pq5/3NK3 b - -",
		"1r4k1/3qnppp/1r6/pp1pP3/2pP1P2/P1P2B2/1R1Q2PP/1R5K w - -",
		"3rr1k1/1p3p2/1pp3p1/8/2q1P1pP/P1R3P1/1PQ2P2/4R1K1 b - -",
		"r1bq1rk1/pp2bppp/2n1p3/2pn4/3P4/2NBPN2/PP3PPP/R1BQ1RK1 w - -",
		"2k5/8/p2pN1p1/3P2P1/1P6/3K3n/8/8 w - -",
		"rnbqkb1r/p2p1ppp/1p2p3/4P3/3P4/2P2N2/P4PPP/R1BQKB1R b KQkq -",
		"r1bqr1k1/1pp2pb1/p1n2npp/3pp3/4P3/2PP1B1P/PPQ2PPN/R1B1RNK1 b - -",
		"r1rq2k1/1b1nbppp/3p1n2/p2Pp3/PpN1P3/3BBN2/1P2QPPP/2R2RK1 w - -",
		"8/5ppk/5n1p/5Q2/2q3P1/7P/5BK1/8 b - -",
		"rnbqnrk1/2p1ppbp/pp1p2p1/4P3/P2P4/2N2N2/1PP1BPPP/R1BQR1K1 b - -",
		"r1bqnrk1/2p1ppbp/ppnp2p1/4P3/P2P4/2N2N2/1PP1BPPP/R1BQR1K1 w - -",
		"8/5k2/8/8/5PK1/6p1/8/8 w - -",
		"8/2q3k1/1ppbb1pp/2p1p3/2P1P3/P5P1/1P3PN1/3Q1BK1 b - -",
		"r3qrk1/p7/2p1b3/5pp1/1nNPp3/4P3/P1Q2BPP/R4RK1 w - -",
		"7r/2R4P/p7/Pp6/8/1k6/3p2K1/8 w - -",
		"8/pp3n1p/2p1kp1P/2P1p1p1/1P2P3/3KNPP1/P7/8 w - -",
		"8/pp3n1p/2p1kp1P/2P1pNp1/1P2P3/3K1PP1/P7/8 b - -",
		"8/6k1/8/1R6/5PKP/8/8/r7 b - -",
		"1k6/6R1/1P6/5K1p/5P1r/8/8/8 b - -",
		"8/6k1/K1PR3p/8/8/8/8/1r6 b - -",
		"rnbqkb1r/pppppppp/5n2/8/8/5N2/PPPPPPPP/RNBQKB1R w KQkq -",
		"2r1k2r/2qb1pb1/1p2p1pn/nP1pP2p/P2B3P/3B1N1Q/3N1PP1/R4RK1 w k -",
		"rn2r1k1/1pp1p1bp/p3Rpp1/8/3q4/5B2/P1PN1PPP/R2Q2K1 w - -",
		"rnbqkbnr/ppp1pppp/8/3p4/5P2/5N2/PPPPP1PP/RNBQKB1R b KQkq -",
		"4qr2/6kp/2r1n1p1/2nRp1P1/4P2P/2p1B1Q1/2P3BK/5R2 b - -",
		"8/6pk/7p/1p1rpq2/1P2R3/Pp3P1P/5QPK/8 w - -",
		"r7/8/1R1pk1p1/4P2p/2p2K1P/5PP1/8/8 b - -",
		"rn1q1rk1/p3ppbp/1p1p2p1/8/2PN4/1PB1P1P1/P4PKP/2RQ1R2 b - -",
		"1rbqk1nr/3nppbp/p2p2p1/1p2P3/3BBP2/2N2N2/PPP3PP/R2QK2R b KQk -",
		"r4r1k/5ppp/8/p1q1p3/1p1nP3/P2B4/1PPQ1RPP/R5K1 w - -",
		"8/3k4/1P4p1/p2P3p/P1K1p2P/8/6P1/8 b - -",
		"r1bqk1nr/pp1pppbp/2n3p1/2p5/2P5/2N2NP1/PP1PPPBP/R1BQK2R b KQkq -",
		"6r1/4pr1k/pQ1pR1Np/1b1P3n/1Pp2P2/6PP/P2qR1BK/8 b - -",
		"1q2r1k1/3r1p1p/1p1pp1p1/pPn1b1B1/2B1P3/1P2QPP1/P6P/1R1R2K1 w - -",
		"rnbqkb1r/ppp2ppp/4pn2/8/2pP4/5NP1/PP2PPBP/RNBQK2R b KQkq -",
		"r1b1k2r/1pqpbppp/p1n1pn2/8/3NP3/P1N1B3/1PP1BPPP/R2QK2R w KQkq -",
		"5bk1/1b1q2p1/p3p3/3pBpp1/Pp1P4/1PrBP2P/5P2/3Q2RK w - -",
		"rn1qkbnr/pp2ppp1/2p3bp/8/3P3P/5NN1/PPP2PP1/R1BQKB1R b KQkq -",
		"4r3/p3kp2/2qb1p1B/1pnR4/8/P1P2P2/1PQ5/2K4R w - -",
		"r2qk2r/pp1b1ppp/4pn2/2b5/8/P2B2N1/1PP2PPP/R1BQ1RK1 w kq -",
		"2kr1bnr/ppp1qp2/3p2p1/3Pn3/2P1PPPp/2N4P/PP2B3/R1BQK2R b KQ f3",
		"8/5pk1/p7/1p1pr1P1/2p4P/P1P5/1P3RP1/7K b - -",
		"4r3/2r3k1/4R1p1/4Kp1p/P1B2P2/bP1R2P1/7P/8 b - -",
		"5rk1/2p4p/2b5/pp6/1P2pP2/P1N3P1/5K1P/3R4 b - -",
		"r7/pp3Q2/2np2pk/2p1pb1q/8/PBB1P3/1P1KN3/8 w - -",
		"3r4/6p1/pN1r1p2/Pbk1p3/7p/1K1P1P2/3R2PP/3R4 w - -",
		"2r5/1p5p/4kp2/1p1p1R2/6P1/1P1KP2P/P7/8 b - -",
		"1Q6/5pk1/5Np1/7p/4PP2/8/2n2KPP/q7 w - -",
		"r1bqk2r/pp1nbppp/2p1pn2/3p2B1/2PP4/2NBPN2/PP3PPP/R2QK2R b KQkq -",
		"2rq1rk1/p3bppp/B1p5/2pp1b2/3Pn3/1P2PN2/PB3PPP/R2Q1RK1 b - -",
		"1r1r2k1/pbpnqppp/2n1pb2/2Pp4/Pp1P3P/1P1N1NP1/1BQ1PPB1/3RR1K1 b - h3",
		"r1bq1rk1/ppp3pp/2n1p3/3pPp2/2PPn3/P1PQ2P1/4NP1P/R1B1KB1R b KQ -",
		"1rr3k1/p3pp2/3p2p1/1qpP2N1/4P1n1/6P1/PPQB1P2/1R4K1 w - -",
		"6n1/bp3pk1/2p3p1/p1q1p2p/P3P3/1PP2BP1/3Q1PKP/2B5 w - -",
		"r1b1kbnr/1pp3pp/5p2/p1n1p3/P1B1P3/2P2N2/1P3PPP/RNB2RK1 w - -",
		"5rk1/4qpp1/1r2pn1p/8/Q2P4/P3P1B1/5PPP/2R2K2 w - -",
		"3R4/2q2pp1/p1k2b2/1n3Q2/1p1p4/1P4P1/PB3P2/1K2R3 b - -",
		"1q1rr1k1/1p3pp1/pbn3bp/3p4/P2N4/1PP1B2P/3QBPP1/3RRK2 w - -",
		"rnbqkbnr/pp2pppp/2p5/3p4/2PP4/2N5/PP2PPPP/R1BQKBNR b KQkq -"
		};
		return fens;
	}

	std::vector<std::string> benchFens3() {
		std::vector<std::string> fens = {
			"1r4k1/2R4p/5Kp1/1PB5/4P1PP/2b5/8/8 w - - 0 1",
			"q6k/5p2/p1N1p1p1/RPP1P1P1/4QP1P/1r6/6K1/8 b - - 0 1",
			"8/8/8/3N4/4p3/4K3/8/1k6 w - - 0 1",
			"8/6k1/1q3p1p/5p1P/2p5/p1Bp1B2/6P1/1K2R3 w - - 0 1",
			"4q2k/p1Q2rp1/1p3p1p/3N1p1n/4n3/PP2BR1P/6P1/6K1 w - - 0 1",
			"2Q5/8/1k1r2p1/1pp5/8/4K3/6PP/8 w - - 0 1",
			"1rbqk1nr/pp3pbp/3pp1p1/2p5/P2nP3/2NPBNP1/1PPQ1PBP/R3K2R b KQk - 0 1",
			"8/2k1n3/p6p/1p3n2/1P3P2/P1P3p1/1KB5/7R b - - 0 1",
			"rnbqk1nr/pp2ppb1/2p3p1/3pP2p/3P1P2/2NB4/PPP3PP/R1BQK1NR b KQkq - 0 1",
			"1b1r2k1/p2pqpp1/2r1pn1p/1Bp5/4NP1Q/P3P3/1P1R2PP/5RK1 b - - 0 1",
			"8/8/3b4/5B2/8/p1k5/K7/8 w - - 0 1",
			"8/8/8/4KQ2/4P3/8/2k5/5q2 b - - 0 1",
			"8/8/8/1k1r4/6PK/8/8/2R5 b - - 0 1",
			"4r3/8/8/8/R1k5/8/1K6/8 b - - 0 1",
			"8/4R3/k7/4p3/2p4p/2P4n/8/5K2 b - - 0 1",
			"8/8/8/8/4N3/7p/7K/k7 w - - 0 1",
			"8/Pb5B/4k3/4n3/4p1pp/2B1P3/5PK1/8 w - - 0 1",
			"rnbqk2r/ppp1ppb1/1n4pp/6B1/3P4/2N2N2/PP2PPPP/R2QKB1R w KQkq - 0 1",
			"2q1n1k1/3n1pbp/1pp3p1/p2p2N1/P2P1B2/2N4P/1PPQ1PP1/6K1 w - - 0 1",
			"r4rk1/pp2n1pp/1bn2p2/q2p4/8/P2QPNB1/1P1N1PPP/R4RK1 b - - 0 1",
			"8/1p4p1/6k1/5q2/5P1p/1P3RP1/3K4/8 w - - 0 1",
			"2rq1rk1/pp2ppbp/3n2p1/2p5/2nP4/1Q2PNP1/PB3PBP/1RR3K1 w - - 0 1",
			"1r3rk1/1bqpbpp1/npp1p2p/p3P3/2PP4/P1N2NP1/1PQ2PBP/R3R1K1 w - - 0 1",
			"8/P7/1KB5/8/4p2k/2n5/1N3p2/8 w - - 0 1",
			"8/8/2p1k2p/7N/5PP1/b6P/2P3K1/8 b - - 0 1",
			"8/8/5k2/5q1p/7P/5BP1/4RK2/8 b - - 0 1",
			"rnbq1rk1/1pp1bppp/pn1p4/3Pp3/1PB1P3/2N1BN1P/P1P2PP1/R2Q1RK1 w - - 0 1",
			"1r1r2k1/4bpp1/p1n1p2p/P1pqP3/1p1P3P/1P3NP1/Q4PK1/B1RR4 b - - 0 1",
			"rnbqkb1r/ppp1pppp/3p1n2/8/3PP3/2N5/PPP2PPP/R1BQKBNR b KQkq - 0 1",
			"8/5k2/r3q3/P4pR1/1P1B1R2/7P/5PPK/8 b - - 0 1",
			"5k2/1R6/1p2N3/1P2n1K1/6P1/8/8/4r3 b - - 0 1",
			"8/8/2K3pk/8/6RP/8/3r4/8 b - - 0 1",
			"1kb4r/1p3qp1/1R3p1p/p7/rN1NP3/8/1Q3PPP/2R4K b - - 0 1",
			"8/4k3/p7/5R2/8/P7/r5r1/1K3R2 b - - 0 1",
			"5bk1/2n1p3/p3Pp1p/1rp2BpP/2N3P1/2PK4/1P3P2/R7 w - - 0 1",
			"3q1r2/1pp2pbk/r1n1b1pp/pN1np3/P7/3P1NP1/1P1BPPBP/R1QR2K1 w - - 0 1",
			"r1bq1rk1/p1pnppbp/1p1p1np1/8/3P3B/2PBP3/PP1NNPPP/R2QK2R b KQ - 0 1",
			"r1bqkb1r/3npp1p/pP1p1np1/2pP4/8/2N2N2/PP2PPPP/R1BQKB1R w KQkq - 0 1",
			"r1bqkb1r/pp2pppp/2n2n2/3p4/2PP4/2N5/PP3PPP/R1BQKBNR w KQkq - 0 1",
			"6r1/2k5/8/1p1Pp2p/p1p1Pp1q/P1R1bP1P/1P2N3/5Q1K w - - 0 1",
			"5rk1/2P5/p7/5R2/1KPBp3/1P3b2/8/8 b - - 0 1",
			"6k1/3n1p1p/2q3p1/4p2P/2B1P3/rp2QP2/5P2/1K2N1R1 b - - 0 1",
			"8/8/3K4/6R1/k7/4N3/8/2r5 w - - 0 1",
			"r1b1k2r/2qpbppp/p1n1pn2/3N4/Pp2P3/1N6/1PPQBPPP/R1BR2K1 b kq - 0 1",
			"rnbq1rk1/4bppp/3p1n2/1pp1p3/p3P3/2PP1N2/PPBN1PPP/R1BQR1K1 w - - 0 1",
			"1q1r4/4rppk/pNNp1b1p/P7/4bP2/2R3QP/1P3KP1/2R5 b - - 0 1",
			"r1bqkb1r/pp1npppp/2n5/2ppP3/5P2/3P1N2/PPP3PP/RNBQKB1R w KQkq - 0 1",
			"8/8/8/6p1/P4rP1/2R4k/8/3K4 b - - 0 1",
			"rnbqkbnr/ppp2ppp/4p3/3p4/3PP3/2N5/PPP2PPP/R1BQKBNR b KQkq - 0 1",
			"r1b4r/ppp1k2p/3p1np1/4ppn1/2PPP3/P1PB1P2/6PP/R1B2RK1 w - - 0 1",
			"4r3/p1qb2kp/2pp1r2/2n2pp1/2P5/4P1P1/PRQ1NPBP/1R4K1 b - - 0 1",
			"8/8/8/7p/4k3/8/8/K3N3 w - h6 0 1",
			"7q/7P/8/6pR/3k2P1/6N1/5K2/8 w - - 0 1",
			"8/r7/PR1bk3/6p1/8/8/5P2/R4K2 w - - 0 1",
			"8/1P3pk1/7p/6p1/3q4/1RR5/8/K7 b - - 0 1",
			"1rb2rk1/p1q2ppp/2pbpn2/8/2B1P3/2N1B2P/PPQ2PP1/R4RK1 b - - 0 1",
			"1k1n4/8/1p1p2p1/1PpPp1P1/2P1P3/3K4/5B2/8 w - - 0 1",
			"1k6/1b5p/p3p1p1/2rpq2n/6R1/R4P2/2PQ3P/3K1B2 w - - 0 1",
			"r1b2rk1/ppp3bp/n2p2q1/3PppBn/2P5/P1N2N2/1P2BPP1/R2QK2R w KQ - 0 1",
			"8/6q1/8/p1p5/1p2N3/1P1P4/1kP1RK2/8 w - - 0 1",
			"8/4k3/7p/p1Rr4/5pp1/P3b1P1/4Qn1P/3q1BK1 b - - 0 1",
			"2N5/8/8/8/7p/7K/8/k7 b - - 0 1",
			"3nr2k/5pp1/4p2p/r1p1PP1P/2Pp1RP1/pP1N4/P2K4/6R1 b - - 0 1",
			"r4rk1/p2n1ppp/b1p1p3/2bpP3/2P4q/1P1B4/PBQN1PPP/R4RK1 w - - 0 1",
			"8/3b4/3k4/1p3p2/1P1K1P2/8/8/5B2 b - - 0 1",
			"r1bqk2r/ppp3pp/2np1n2/1Bb1p3/2Q1P3/5N2/PPP2PPP/RNB2RK1 b kq - 0 1",
			"4r1k1/1p1bq1pn/r2p1p2/p1pP2n1/4P2Q/2N5/PPP3B1/K3NR1R w - - 0 1",
			"8/5pk1/3p1qp1/1ppPp2Q/P1b1P3/2P2P2/2B3R1/6K1 w - - 0 1",
			"8/7R/4k3/1N6/3PP1K1/8/r7/8 b - - 0 1",
			"8/8/3B4/5k2/8/7p/6r1/6K1 w - - 0 1",
			"8/4pk1p/6p1/1R6/1p3P2/1PbpK2P/6P1/8 b - - 0 1",
			"8/8/4r1p1/1R5p/1P4kP/P7/1K6/8 w - - 0 1",
			"r1bq1rk1/pp3pbp/n2p1np1/2pPp3/2P1P1P1/P1NBB2P/1P2NP2/R2QK2R b KQ - 0 1",
			"8/2k1p3/6p1/1p3n2/1P1PQP1p/3P3P/6PK/2r2r2 w - - 0 1",
			"r1b2rk1/pp3ppp/2p5/4n3/3NPP2/8/PP2B1PP/R2R2K1 b - f3 0 1",
			"r4rk1/4ppb1/2q1b1pp/3n4/2p5/N1P3NP/1PQ2PP1/R1BR2K1 b - - 0 1",
			"rnbqkbnr/pppppppp/8/8/8/5N2/PPPPPPPP/RNBQKB1R b KQkq - 0 1",
			"n4rk1/1r3pbn/b2p2p1/p1pPp2P/N1P1P2q/P1QBBP1P/4N1K1/R6R b - - 0 1",
			"2kr1b1r/ppqn1pp1/4p1p1/8/8/6P1/PPP1QPBP/R1B2RK1 b - - 0 1",
			"r1bq1rk1/pp1p1pbp/2n1pnp1/8/1PP5/2N2NP1/3PPPBP/1RBQK2R b K - 0 1",
			"3k4/3p4/8/PQ3q2/2P1pppP/8/7K/7N b - - 0 1",
			"8/3q3k/1p2rppp/p1p1b3/P1N4R/1P1rPQP1/3P3P/5R1K b - - 0 1",
			"8/1p2r3/p2R2kp/4nnp1/8/P4P2/1PB3P1/1K6 b - - 0 1",
			"5rk1/Rp2p3/3p2pq/2nPp1b1/2P1P1Q1/6P1/5NN1/5K2 b - - 0 1",
			"8/8/4p2R/4r2p/6p1/2k3P1/5P2/6K1 b - - 0 1",
			"r1b2rk1/ppp3bp/5q2/2pPpp2/2P1P3/2N3PP/PP2QPK1/R5NR b - - 0 1",
			"8/1pr3p1/p1rpk2p/P3p3/R1Pp1P2/3P4/1P4PP/4R1K1 b - - 0 1",
			"rnb2rk1/ppp2pp1/5q1p/2pQp3/8/2P2NP1/PP2PPBP/R3K2R b KQ - 0 1",
			"8/7p/8/5q2/kpp2P2/4B2P/6QK/8 w - - 0 1",
			"4q3/3b4/3b3k/1ppPp2p/p3Pp2/P2B1NrP/1PR2QP1/7K b - - 0 1",
			"8/4kBb1/2pp1p2/p1p2P1p/P1P1KP2/1P5r/1R5P/8 w - - 0 1",
			"5r2/p7/4n3/1p2Pk2/1P1bR3/P4PBP/5PK1/8 b - - 0 1",
			"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
			"7R/8/6k1/8/8/8/3K4/5r2 b - - 0 1",
			"5k2/8/3R2p1/3K4/3B1PpP/7r/8/8 b - - 0 1",
			"6r1/k5q1/8/1p1Pp3/p1p1Pp1p/P3bP1Q/1PR1N3/7K w - - 0 1",
			"2R5/p2r2p1/1p6/3k1P2/6P1/6KP/P7/8 w - - 0 1",
			"rnq1k2r/pp2ppbp/5np1/2p1N3/8/1P6/PBPP1PPP/RN1Q1RK1 w kq - 0 1",
			"8/7p/p2Ppbp1/2k5/P1N1K3/7P/6P1/8 w - - 0 1",
			"r5k1/2Rr2pp/3n1pq1/pp1Ppb2/7P/PQ6/1P2BPP1/3RB1K1 w - - 0 1",
			"r3qrk1/3bp2p/2p1n1p1/p1Pp1p2/NP2n3/P3PNP1/1RQ2PBP/5RK1 b - - 0 1",
			"8/4n3/p3k2p/1p3n2/1P3P2/P1P3p1/2B5/2K4R w - - 0 1",
			"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
			"r1b1r1k1/pp3pbp/1q1p1np1/2pP4/1nP5/P1N2N2/1P1B1PPP/RB1Q1RK1 b - - 0 1",
			"4nr1k/pppr2pp/1n6/3P1p1q/P1PP4/1Q1BBP2/7P/4RRK1 b - - 0 1",
			"r1bq1rk1/1p2ppbp/1n1p1np1/p1pP4/P3PP2/1BN1BN2/1PP3PP/R3QRK1 w - 0 1",
			"8/8/8/6bp/8/8/1k4K1/8 w - - 0 1",
			"r4b1k/r4pp1/2pp3p/2q2N1P/pp2PP2/4R3/PPP1Q1P1/1K1R4 w - - 0 1",
			"5k2/8/6Q1/1pp4p/2q2N1P/8/5PK1/8 w - - 0 1",
			"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
			"1r3bk1/4pp2/3p3p/3P4/Q1p2Nn1/1r6/1PK3PP/1R6 w - - 0 1",
			"8/4R3/1pBp4/p2P1pp1/5k2/P7/3r3P/7K b - - 0 1",
			"8/2r5/3k4/pB1b1p1p/Prp3pP/2R1P1P1/5P2/3RK3 b - - 0 1",
			"6k1/2R5/7p/p2r4/4npp1/P3b1P1/1B2Q2P/3q1B1K w - - 0 1",
			"6k1/5p2/3p3p/PP4pP/3b4/8/4KR2/7q b - - 0 1",
			"4r3/8/8/8/8/2k5/8/K2R4 w - - 0 1",
			"r5k1/2p2qp1/3p1rnp/p1pb4/P1P1P1R1/1P2Q1RP/3N2P1/6K1 w - - 0 1",
			"r2qk2r/1bppbpp1/np2p2p/p7/2PPP3/2N2NP1/PP3PBP/R2Q1RK1 w kq - 0 1",
			"r1q1rn1k/4bQpn/2pp4/1p2P1P1/p2P3R/P1NBB2b/1P1N4/R5K1 w - - 0 1",
			"2R5/6pk/2P2p1p/p1r2K1P/Pb2PP1B/8/8/8 w - - 0 1",
			"4r1k1/1pnr2pp/pb3p2/3b4/PP1P4/2N5/4NBPP/R2R2K1 w - - 0 1",
			"2rqr1k1/1b2bp1p/pp3np1/2pp4/3P4/PP2P3/1BRNBPPP/3Q1RK1 b - - 0 1",
			"rnbqk2r/ppppp1bp/5np1/5p2/2PP4/5NP1/PP2PP1P/RNBQKB1R w KQkq - 0 1",
			"r3r2k/1pp2R1n/p1bp4/5P2/4P2q/2N1NQ2/PPP4P/1K4R1 w - - 0 1",
			"r1bqkb1r/p2pnppp/2p1p3/2p5/4P2N/3P4/PPP2PPP/RNBQK2R b KQkq - 0 1",
			"1Q6/P6k/6p1/8/3q4/7p/8/1K6 b - - 0 1",
			"4qr1k/5ppn/7p/2R1pP2/5b1r/5PN1/B1Q2PKP/1R6 b - - 0 1",
			"rn3rk1/pp2ppbp/6p1/3B4/3P4/2N3Pq/PP2PP2/R1BQK3 w Q - 0 1",
			"r2qr1k1/pb3pp1/1p3n1p/1PpNb3/2B5/PQ2PN2/5PPP/3R1RK1 b - - 0 1",
			"7K/8/8/8/4Q3/5r2/6k1/6n1 b - - 0 1",
			"3b3r/6k1/1p2qp1r/p1p1p1p1/PnNPB1b1/1PB1P1P1/3R1Q1P/4R1K1 b - - 0 1",
			"5k2/4ppbp/r5p1/Pp1pP3/1p1N1P2/4P2P/1P4P1/R5K1 b - f3 0 1",
			"7k/2rp1Qp1/p3p2p/4Pp2/P1p1bP2/6R1/1r4PP/4K3 b - - 0 1",
			"2qrr1k1/4bppp/p1n1bn2/1p1pN3/8/P1NB3P/1PPQRPPB/4R1K1 b - - 0 1",
			"4n1k1/4r2p/6p1/3Q2PP/6P1/6K1/8/8 b - - 0 1",
			"r1q1rn1k/4bQpB/2pp4/1p2P1P1/p2P3R/P1N1B2b/1P1N4/R5K1 b - - 0 1",
			"6k1/pp2qpp1/3npb1p/1Q6/1P6/1Br1PN1P/P4PP1/3R2K1 w - - 0 1",
			"rn1qkb1r/pbpppppp/1p3n2/8/2PP4/2N2N2/PP2PPPP/R1BQKB1R b KQkq d3 0 1",
			"b7/P5pk/7p/5p1q/5P2/8/3Q2NK/8 w - - 0 1",
			"3r3k/p3bp2/P1p1p3/2p2r1p/2P1Np1P/1P1P1P2/2K2PR1/7R b - - 0 1",
			"8/3Q1kb1/2r2n2/p1q1p1P1/1p2P3/8/1PP2R1K/5R2 b - - 0 1",
			"q7/3k4/1KR5/1R6/8/8/8/8 w - - 0 1",
			"8/3Q1pk1/3bp1p1/3b2P1/1P2n3/3BPN2/5P2/r5K1 w - - 0 1",
			"8/8/P7/8/7k/R3K1p1/5r2/8 b - - 0 1",
			"3R4/1r6/2p4P/4Pp2/1P6/4k1K1/8/8 b - - 0 1",
			"5r2/1p4kp/2p1r3/2P1q3/P2Q1p2/2N3PP/5P2/3R2K1 w - - 0 1",
			"rnbqk2r/ppp1bp1p/4p3/5p2/3PN3/5N2/PPP2PPP/R2QKB1R w KQkq - 0 1",
			"8/p3n3/1p2k3/2pb4/B4B1p/P4P2/r5PP/3R2K1 w - - 0 1",
			"8/8/8/6k1/3B1b2/4K3/2B5/8 w - - 0 1",
			"r2qkbnr/p1p2p1p/2bp2p1/2p5/4P3/2N1NQ2/PPP2PPP/R1B1R1K1 b kq - 0 1",
			"r1b2rk1/p2p1ppp/1p1qp3/1B2N2Q/1P1nP3/7R/P2P1PPP/5RK1 b - - 0 1",
			"R7/P5k1/5p2/8/6p1/4P1P1/r4P2/4K3 w - - 0 1",
			"rn2kb1r/1B1q1p2/7p/1pp3p1/3p4/NPnQ2P1/P1N1PP1P/R1B2RK1 b kq - 0 1",
			"1r6/3qb1k1/1p3p1r/p1p1p1p1/PnN1B3/1PBPP1Pb/5Q1P/R2R2K1 b - - 0 1",
			"8/5p2/2pk4/p7/Pn1P2r1/1NR3p1/6P1/6K1 w - - 0 1",
			"8/8/8/4k3/1B6/8/3r4/K7 b - - 0 1",
			"r2q1rk1/1bp2ppp/n2p4/4Pn2/1p3P2/2N5/BPPQ1BPP/R4RK1 w - - 0 1",
			"4r1k1/1pp2p2/1n1p2pp/r2P4/p3PP1q/PBP2P1P/2QBR3/5RK1 b - - 0 1",
			"rn1q1rk1/1p1bbppp/1np5/p2p4/P7/2NP2PP/1PP1NPB1/R1BQR1K1 w - - 0 1",
			"3Q4/5r1k/7n/8/3B4/5r2/3K4/8 b - - 0 1",
			"1r1qr1k1/pb1p1p1p/1pn1p1p1/1Np2n2/2P2N2/1P2P1P1/PR1P1PBP/3QR1K1 b - - 0 1",
			"8/5pk1/2p5/1p1p1pP1/1P1P1P1p/2P1KR1P/5R2/2r4r w - - 0 1",
			"8/1k6/2pQ4/r7/3P4/2K5/8/8 w - - 0 1",
			"rn4Q1/3pkp1p/p3p3/1p2P3/7R/2N3P1/PPP4P/2q2K2 w - - 0 1",
			"3b4/B4pk1/1p4pp/1Pq5/5PQ1/R3p1PP/2R3K1/8 b - - 0 1",
			"rnbqkbnr/1p1p1ppp/p3p3/8/3NP3/2N5/PPP2PPP/R1BQKB1R b KQkq - 0 1",
			"5r1k/1p5p/2p1r3/2P1q3/P2Q4/6NP/5P2/3R2K1 b - - 0 1",
			"r1bqkb1r/pp2pppp/2np1n2/8/2BNP3/2N5/PPP2PPP/R1BQK2R b KQkq - 0 1",
			"8/2r5/3k4/pB1b1p1p/Prp3pP/2R1P1P1/5P2/2R1K3 w - - 0 1",
			"r1b2rk1/ppp2ppp/7n/8/2BqP3/5N2/PP3PPP/R4RK1 w - - 0 1",
			"B7/8/8/2k5/3b4/3P4/8/3K4 w - - 0 1",
			"3r2k1/pb3pbp/1Pn5/7p/2p5/2N1P1P1/1P3PBP/R1B3K1 b - - 0 1",
			"8/6k1/6p1/1pR3Pp/8/1r5P/6K1/8 w - - 0 1",
			"r3kbnr/pp1q1ppp/2n1p3/1Bp5/4Q3/2P5/PP1P1PPP/RNB1K2R b KQkq - 0 1",
			"1r4k1/1p3r1p/p5p1/8/4Q3/1NP3b1/PP4Pq/R2BRK2 w - - 0 1",
			"8/8/8/5r2/8/2k5/1R6/1K6 b - - 0 1",
			"8/7K/8/5k2/r7/8/8/B7 w - - 0 1",
			"8/8/7k/4B3/4K3/6R1/8/4r3 w - - 0 1",
			"r1r3k1/pbqnpp1p/1p1p1n1b/2p3p1/PP1PP2B/2PB1P2/3NN1PP/R2Q1R1K w - - 0 1",
			"rn1q1rk1/pb1p1ppp/1p2pn2/6B1/1bPp4/PQN1PN2/1P3PPP/R3KB1R w KQ - 0 1",
			"3k3r/1pb1n1p1/4Q3/1p1P2P1/8/4B3/q1B1KP2/2R5 w - - 0 1",
			"4r2k/3n3p/2Q1b2P/p3P3/1p3rP1/2P3N1/P1P4R/1K6 w - - 0 1",
			"8/1r3k2/8/5P2/5K2/8/2R5/8 w - - 0 1",
			"r1bqkb1r/pp1ppppp/2n2n2/2p5/2P5/6P1/PP1PPPBP/RNBQK1NR w KQkq - 0 1",
			"5r1k/1p2r2p/p4n1q/3pBB2/PP3R2/4P3/1Q5P/6RK w - - 0 1",
			"rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq d3 0 1",
			"r1bqk2r/ppppbppp/2n2n2/4p3/2B1P3/3P1N2/PPPN1PPP/R1BQK2R b KQkq - 0 1",
			"r7/8/6p1/1p2kp1p/1P1n3P/P1N3P1/1K6/3R4 w - - 0 1",
			"r1b1k2r/2qnbppp/p2p1n2/4p1B1/Pp2P3/2N2P2/1PPN2PP/R2QKB1R w KQkq - 0 1",
			"1r3k2/2p2n2/3p4/p1pP3p/P1P2N2/1P4rP/2Q3PK/8 b - - 0 1",
			"Q7/8/7p/8/3K4/7P/2q2k2/8 b - - 0 1",
			"rnbq1rk1/pp4bp/2pp1np1/4pp2/1PP5/2NP1NP1/PB2PPBP/R2Q1RK1 b - - 0 1",
			"r4k1r/pp1bnpp1/2n1p1q1/3pP2p/2pP4/P1P2N1Q/3B1PPP/R1RB2K1 w - h6 0 1",
			"Q7/4p1k1/5bpp/P7/1n6/7P/q4PP1/5RK1 w - - 0 1",
			"8/8/5n2/5p1k/7P/8/4K1N1/8 b - - 0 1",
			"8/7p/6p1/2PkNb2/5P2/4P1KP/8/8 w - - 0 1",
			"8/8/8/8/5K2/8/4rk2/8 w - - 0 1",
			"8/8/1k6/1p6/pK1nN3/8/8/8 w - - 0 1",
			"3r4/4k3/6p1/1p3p1p/1P1n3P/P1N3P1/1K1R4/8 b - - 0 1",
			"r6r/1p3kb1/1np1p3/p2pPnQp/P2P1P2/2N1B3/1PP1BP2/2K5 b - - 0 1"
		};
		return fens;
	}


	int64_t bench(int depth, int64_t &totalTime) {
		return bench(benchFens1(), depth, totalTime);
	}

	int64_t bench2(int depth, int64_t &totalTime) {
		return bench(benchFens2(), depth, totalTime);
	}

	int64_t bench3(int depth, int64_t &totalTime) {
		return bench(benchFens3(), depth, totalTime);
	}

	int64_t bench(std::vector<std::string> fens, int depth, int64_t &totalTime) {
		std::cout << "Benchmark" << std::endl;
		std::cout << "------------------------------------------------------------------------" << std::endl;
		int64_t totalNodes = 0;
		int64_t totalQNodes = 0;
		totalTime = 0;
		double avgBF = 0.0;
		std::cout << std::setprecision(3) << std::left << std::setw(4) << "Nr" << std::setw(7) << "Time" << std::setw(10) << "Nodes" << std::setw(6) << "Speed" << std::setw(6) << "BF" << std::setw(6) << "TT[%]"
			<< std::setw(40) << "PV" << std::endl;
		Search * srch = new Search;
		for (int i = 0; i < int(fens.size()); i++) {
			Position* pos = new Position(fens[i]);
			srch->PrintCurrmove = false;
			//srch.uciOutput = false;
			srch->NewGame();
			srch->timeManager.initialize(FIXED_DEPTH, 0, depth);
			srch->Think(*pos);
			int64_t endTime = now();
			totalTime += endTime - srch->timeManager.GetStartTime();
			srch->NodeCount *= settings::parameter.HelperThreads + 1;
			srch->QNodeCount *= settings::parameter.HelperThreads + 1;
			totalNodes += srch->NodeCount;
			totalQNodes += srch->QNodeCount;
			avgBF += srch->timeManager.GetEBF(depth) * (srch->NodeCount - srch->QNodeCount);
			int64_t runtime = endTime - srch->timeManager.GetStartTime();
			int64_t rt = runtime;
			if (rt == 0) rt = 1;
			std::cout << std::left << std::setw(4) << i << std::setw(7) << runtime << std::setw(10) << srch->NodeCount << std::setw(6)
				<< srch->NodeCount / rt << std::setw(6) << srch->timeManager.GetEBF(depth) << std::setw(6) << (settings::parameter.HelperThreads ? 0 : 100.0 * tt::GetHitCounter() / tt::GetProbeCounter())
				<< std::setw(40) << srch->PrincipalVariation(*pos, depth) << std::endl;
			delete(pos);
		}
		delete(srch);
		avgBF = avgBF / (totalNodes - totalQNodes);
		std::cout << "------------------------------------------------------------------------" << std::endl;
		std::cout << std::setprecision(5) << "Total:  Time: " << totalTime / 1000.0 << " s  Nodes: " << totalNodes / 1000000.0 << " - " << totalQNodes / 1000000.0 << " MNodes  Speed: " << totalNodes / totalTime << " kN/s  "
			"BF: " << std::setprecision(3) << avgBF << std::endl;

		return totalNodes;
	}


	uint64_t nodeCount = 0;

	uint64_t perft(Position &pos, int depth) {
		nodeCount++;
		if (depth == 0) return 1;
		ValuatedMove move;
		uint64_t result = 0;
		ValuatedMove * moves = pos.GenerateMoves<ALL>();
		while ((move = *moves).move) {
			Position next(pos);
			if (next.ApplyMove(move.move)) result += perft(next, depth - 1);
			++moves;
		}
		return result;
	}

	uint64_t perft1(Position &pos, int depth) {
		nodeCount++;
		if (depth == 0) return 1;
		ValuatedMove move;
		uint64_t result = 0;
		ValuatedMove * moves = pos.GenerateMoves<TACTICAL>();
		while ((move = *moves).move) {
			Position next(pos);
			if (next.ApplyMove(move.move)) result += perft1(next, depth - 1);
			++moves;
		}
		moves = pos.GenerateMoves<QUIETS>();
		while ((move = *moves).move) {
			Position next(pos);
			if (next.ApplyMove(move.move)) result += perft1(next, depth - 1);
			++moves;
		}
		return result;
	}

	uint64_t perft2(Position &pos, int depth) {
		nodeCount++;
		if (depth == 0) return 1;
		ValuatedMove move;
		uint64_t result = 0;
		ValuatedMove * moves = pos.GenerateMoves<WINNING_CAPTURES>();
		while ((move = *moves).move) {
			Position next(pos);
			if (next.ApplyMove(move.move)) result += perft2(next, depth - 1);
			++moves;
		}
		moves = pos.GenerateMoves<EQUAL_CAPTURES>();
		while ((move = *moves).move) {
			Position next(pos);
			if (next.ApplyMove(move.move)) result += perft2(next, depth - 1);
			++moves;
		}
		moves = pos.GenerateMoves<LOOSING_CAPTURES>();
		while ((move = *moves).move) {
			Position next(pos);
			if (next.ApplyMove(move.move)) result += perft2(next, depth - 1);
			++moves;
		}
		moves = pos.GenerateMoves<QUIETS>();
		while ((move = *moves).move) {
			Position next(pos);
			if (next.ApplyMove(move.move)) result += perft2(next, depth - 1);
			++moves;
		}
		return result;
	}

	uint64_t perft3(Position &pos, int depth) {
		nodeCount++;
		if (depth == 0) return 1;
		uint64_t result = 0;
		pos.InitializeMoveIterator<MAIN_SEARCH>(nullptr, nullptr, nullptr, nullptr, MOVE_NONE);
		Move move;
		while ((move = pos.NextMove())) {
			Position next(pos);
			if (next.ApplyMove(move)) {
				result += perft3(next, depth - 1);
			}
		}
		return result;
	}

	uint64_t perft4(Position &pos, int depth) {
		nodeCount++;
		uint64_t result = 0;
		ValuatedMove * moves = pos.GenerateMoves<LEGAL>();
		int movecount = pos.GeneratedMoveCount();
		if (depth == 1) return movecount;
		for (int i = 0; i < movecount; ++i) {
			Position next(pos);
			next.ApplyMove(moves[i].move);
			result += perft(next, depth - 1);
		}
		return result;
	}

	uint64_t perftcomb(Position &pos, int depth) {
		nodeCount++;
		if (depth == 0) return 1;
		uint64_t result = 0;
		pos.InitializeMoveIterator<MAIN_SEARCH>(nullptr, nullptr, nullptr, nullptr, MOVE_NONE);
		Move move;
		while ((move = pos.NextMove())) {
			Position next(pos);
			if (next.ApplyMove(move)) {
				uint64_t result3 = perftcomb(next, depth - 1);
				uint64_t result1 = perft(next, depth - 1);
				if (result1 != result3) {
					std::cout << "Error at Depth " << depth - 1 << "  " << next.fen() << std::endl;
					std::cout << "  R1: " << result1 << " R3: " << result3 << " Last Move " << toString(move) << std::endl;
					std::cout << "  Pos before: " << pos.fen() << std::endl;
					return result;
				}
				result += result3;
			}
		}
		return result;
	}

	void divide(Position &pos, int depth) {
		ValuatedMove * moves = pos.GenerateMoves<ALL>();
		ValuatedMove move;
		uint64_t total = 0;
		std::vector<ValuatedMove> vm;
		while ((move = *moves).move) {
			vm.push_back(move);
			++moves;
		}
		std::sort(vm.begin(), vm.end(), [](ValuatedMove a, ValuatedMove b) {return toString(a.move) > toString(b.move); });
		for (auto m : vm) {
			Position next(pos);
			if (next.ApplyMove(m.move)) {
				uint64_t p = perft(next, depth - 1);
				std::cout << toString(m.move) << "\t" << p << "\t" << next.fen() << std::endl;
				total += p;
			}
		}
		std::cout << "Total: " << total << std::endl;
	}

	void divide3(Position &pos, int depth) {
		pos.InitializeMoveIterator<MAIN_SEARCH>(nullptr, nullptr, nullptr, nullptr, MOVE_NONE);
		Move move;
		uint64_t total = 0;
		while ((move = pos.NextMove())) {
			Position next(pos);
			if (next.ApplyMove(move)) {
				uint64_t p = perft3(next, depth - 1);
				std::cout << toString(move) << "\t" << p << "\t" << next.fen() << std::endl;
				total += p;
			}
		}
		std::cout << "Total: " << total << std::endl;
	}

	void testSearch(Position &pos, int depth) {
		Search * engine = new Search;
		engine->timeManager.initialize(FIXED_DEPTH, 0, depth);
		ValuatedMove vm = engine->Think(pos);
		std::cout << "Best Move: " << toString(vm.move) << " " << (int)vm.score << std::endl;
		delete engine;
	}

	void testRepetition() {
		Position pos("5r1k/R7/5p2/4p3/1p1pP3/1npP1P2/rqn1b1R1/7K w - - 0 1");
		Search * engine = new Search;
		engine->timeManager.initialize(FIXED_DEPTH, 0, 5);
		ValuatedMove vm = engine->Think(pos);
		std::cout << (((vm.move == createMove(G2, H2)) && (vm.score == VALUE_DRAW)) ? "OK     " : "ERROR ") << toString(vm.move) << "\t" << (int)vm.score << std::endl;
		delete engine;
	}

	void testFindMate() {
		std::map<std::string, Move> puzzles;
		//Mate in 2
		puzzles["2@2bqkbn1/2pppp2/np2N3/r3P1p1/p2N2B1/5Q2/PPPPKPP1/RNB2r2 w KQkq - 0 1"] = createMove(F3, F7);
		puzzles["2@8/6K1/1p1B1RB1/8/2Q5/2n1kP1N/3b4/4n3 w - - 0 1"] = createMove(D6, A3);
		puzzles["2@B7/K1B1p1Q1/5r2/7p/1P1kp1bR/3P3R/1P1NP3/2n5 w - - 0 1"] = createMove(A8, C6);
		puzzles["2@1r6/4b2k/1q1pNrpp/p2Pp3/4P3/1P1R3Q/5PPP/5RK1 w - - 0 1"] = createMove(H3, H6);
		puzzles["2@3r1b1k/5Q1p/p2p1P2/5R2/4q2P/1P2P3/PB5K/8 w - - 0 1"] = createMove(F7, G8);
		puzzles["2@r1bq2r1/b4pk1/p1pp1p2/1p2pP2/1P2P1PB/3P4/1PPQ2P1/R3K2R w - - 0 1"] = createMove(D2, H6);
		puzzles["2@8/8/8/r2p4/kp6/1R1Q4/8/K7 w - - 0 1"] = createMove(B3, B2);
		puzzles["2@7B/2R1KnQ1/1p1PP3/3k4/2N5/r3p1N1/4n3/1q6 w - - 0 1"] = createMove(G7, A1);
		//Mate in 3
		puzzles["3@1r3r1k/5Bpp/8/8/P2qQ3/5R2/1b4PP/5K2 w - - 0 1"] = createMove(E4, H7);
		puzzles["3@r1b1r1k1/1pq1bp1p/p3pBp1/3pR3/7Q/2PB4/PP3PPP/5RK1 w - - 0 1"] = createMove(H4, H7);
		puzzles["3@r5rk/5p1p/5R2/4B3/8/8/7P/7K w - - 0 1"] = createMove(F6, A6);
		puzzles["3@5B2/6P1/1p6/8/1N6/kP6/2K5/8 w - - 0 1"] = createMove<PROMOTION>(G7, G8, KNIGHT);
		puzzles["3@8/R7/4kPP1/3ppp2/3B1P2/1K1P1P2/8/8 w - - 0 1"] = createMove(F6, F7);
		puzzles["3@3r1r1k/1p3p1p/p2p4/4n1NN/6bQ/1BPq4/P3p1PP/1R5K w - - 0 1"] = createMove(G5, F7);
		puzzles["3@rn4k1/3q2r1/p2P3Q/2p1p2P/Pp2P3/1P1B4/2P2P2/2K3R1 w - -"] = createMove(D3, C4);
		puzzles["3@6k1/5pp1/7p/3n4/8/2P5/KQq4P/4R3 b - - 0 1"] = createMove(D5, C3);
		//Mate in 4
		puzzles["4@r5r1/5b1k/2p2p1p/1pP2P2/1P1pRq2/8/1QB3PP/4R1K1 b - -"] = createMove(G8, G2);
		puzzles["4@2q1kr2/6R1/2p1p2Q/ppP2p1P/3P4/8/PP6/1K6 w - - 0 1"] = createMove(H6, G6);
		puzzles["4@r1k4r/4Pp1p/p2Q4/1p2p3/2q1b2P/P7/1PP5/2KR3R w - - 0 1"] = createMove(D6, D8);
		puzzles["4@8/5p1p/4p3/1Q6/P2k4/2p5/7q/2K5 b - - 0 1"] = createMove(H2, D2);
		//Mate in 5
		puzzles["5@6r1/p3p1rk/1p1pPp1p/q3n2R/4P3/3BR2P/PPP2QP1/7K w - -"] = createMove(H5, H6);
		puzzles["5@2q1nk1r/4Rp2/1ppp1P2/6Pp/3p1B2/3P3P/PPP1Q3/6K1 w - - 0 1"] = createMove(E7, E8);
		Search * engine = new Search;
		std::map<std::string, Move>::iterator iter;
		int count = 0;
		for (iter = puzzles.begin(); iter != puzzles.end(); ++iter) {
			engine->Reset();
			std::string mateIn = iter->first.substr(0, 1);
			std::string fen = iter->first.substr(2, std::string::npos);
			engine->timeManager.initialize(FIXED_DEPTH, 0, 2 * atoi(mateIn.c_str()) - 1);
			Position pos(fen);
			ValuatedMove vm = engine->Think(pos);
			std::cout << ((vm.move == iter->second) && (vm.score == VALUE_MATE - engine->timeManager.GetMaxDepth()) ? "OK    " : "ERROR ") << "\t" << toString(vm.move) << "/" << toString(iter->second)
				<< "\t" << (int)vm.score << "/" << (int)VALUE_MATE - engine->timeManager.GetMaxDepth() << "\t" << fen << std::endl;
			count++;
		}
		delete engine;
	}

	void testTacticalMoveGeneration() {
		std::vector<std::string> lines;
		std::ifstream s;
		s.open("C:/Users/chrgu_000/Desktop/Data/cutechess/testpositions/ccrl.epd");
		std::string l;
		if (s.is_open())
		{
			long lineCount = 0;
			while (s)
			{
				std::getline(s, l);
				lines.push_back(l);
				lineCount++;
				if ((lineCount & 8191) == 0) std::cout << ".";
				if (((lineCount & 65535) == 0) || !s) {
					std::cout << lineCount << " Lines read from File - starting to analyze" << std::endl;
					long count = 0;
					for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
						std::string line = *it;
						if (line.length() < 10) continue;
						size_t indx = line.find(" c0");
						std::string fen = line.substr(0, indx);
						Position p1(fen);
						ValuatedMove * tacticalMoves = p1.GenerateMoves<TACTICAL>();
						Position p2(fen);
						ValuatedMove * allMoves = p2.GenerateMoves<ALL>();
						ValuatedMove * tm = tacticalMoves;
						ValuatedMove * am;
						Move move;
						//check move is really tactical
						while ((move = tm->move)) {
							Position p3(p1);
							if (p3.ApplyMove(move)) {
								if (p3.GetMaterialKey() == p1.GetMaterialKey()) {
									std::cout << std::endl << "Move " << toString(move) << " doesn't change Material key: " << fen << std::endl;
									//__debugbreak();
								}
								bool found = false;
								Move amove;
								am = allMoves;
								while ((amove = am->move)) {
									if (amove == move) {
										found = true;
										break;
									}
									am++;
								}
								if (!found) {
									std::cout << std::endl << "Move " << toString(move) << " not part of all moves: " << fen << std::endl;
									//__debugbreak();
								};
							}
							tm++;
						}
						//check for completeness
						am = allMoves;
						while ((move = am->move)) {
							Position p3(p1);
							if (p3.GetMaterialKey() != p1.GetMaterialKey()) {
								tm = tacticalMoves;
								Move tmove;
								bool found = false;
								while ((tmove = tm->move)) {
									if (tmove == move) {
										found = true;
										break;
									}
									tm++;
								}
								if (!found) {
									std::cout << std::endl << "Move " << toString(move) << " isn't detected as tactical: " << fen << std::endl;
									//__debugbreak();
								};
							}
							am++;
						}
						//if ((count & 1023) == 0)std::cout << std::endl << count << "\t";
						++count;
					}
					lines.clear();
				}
			}
			s.close();
			std::cout << std::endl;
			std::cout << lineCount << " lines read!" << std::endl;
		}
		else
		{
			std::cout << "Unable to open file" << std::endl;
		}
	}

	void testResult(std::string filename, Result result) {
		std::vector<std::string> lines;
		std::ifstream s;
		s.open(filename);
		std::string l;
		if (s.is_open())
		{
			unsigned long lineCount = 0;
			while (s)
			{
				std::getline(s, l);

				if ((lineCount & 32767) == 0)std::cout << ".";
				if (l.length() < 10 || !s) {
					unsigned long count = 0;
					for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
						++count;
						std::string line = *it;
						size_t indx = line.find(" c0");
						std::string fen = line.substr(0, indx);
						Position p1(fen);
						if (count < lines.size()) {
							if (p1.GetResult() != Result::OPEN) {
								std::cout << "Expected: Open Actual: " << static_cast<int>(p1.GetResult()) << "\t" << p1.fen() << std::endl;
								//__debugbreak();
								Position p2(fen);
								p2.GetResult();
							}
						}
						else {
							if (p1.GetResult() != result) {
								std::cout << "Expected: " << static_cast<int>(result) << " Actual: " << static_cast<int>(p1.GetResult()) << "\t" << p1.fen() << std::endl;
								//__debugbreak();
								Position p2(fen);
								p2.GetResult();
							}
						}
					}
					lines.clear();
				}
				else {
					lines.push_back(l);
					lineCount++;
				}
			}
			s.close();
			std::cout << std::endl;
			std::cout << lineCount << " lines read!" << std::endl;
		}
		else
		{
			std::cout << "Unable to open file" << std::endl;
		}
	}

	void testKPK() {
		assert(!kpk::probe(A6, A5, A8, WHITE));
		assert(!kpk::probe(A6, A5, A8, BLACK));
		assert(kpk::probe(A6, B6, B8, WHITE));
		assert(!kpk::probe(A6, B6, B8, BLACK));
		Position pos("1k6/8/KP6/8/8/8/8/8 w - -0 1");
		assert(pos.evaluate() > VALUE_DRAW);
		Position pos1("1k6/8/KP6/8/8/8/8/8 b - -0 1");
		assert(pos1.evaluate() == VALUE_DRAW);
		Position pos2("8/8/8/8/8/6pk/8/6K1 w - - 0 1");
		assert(pos2.evaluate() == VALUE_DRAW);
		Position pos3("8/8/8/8/8/6pk/8/6K1 b - - 0 1");
		assert(pos3.evaluate() > VALUE_DRAW);
		std::cout << "KPK Tests succesful!" << std::endl;
	}

	bool testBBOperations()
	{
		return testPopcount() && testLSB() && testMSB();
	}

	bool testPopcount()
	{
		std::cout << "Testing Popcount..." << std::endl;
		for (int i = 0; i < 8; ++i) {
			int pc = popcount(RANKS[i]);
			if (pc != 8) {
				std::cout << "Error: popcount(" << RANKS[i] << ") = " << pc << "   should be 8" << std::endl;
				return false;
			}
		}
		for (int i = 0; i < 8; ++i) {
			int pc = popcount(FILES[i]);
			if (pc != 8) {
				std::cout << "Error: popcount(" << FILES[i] << ") = " << pc << "   should be 8" << std::endl;
				break;
			}
		}
		const Bitboard TESTS[] = { EMPTY, ALL_SQUARES, HALF_OF_BLACK, HALF_OF_WHITE };
		const int RESULTS[] = { 0, 64, 32, 32 };
		for (int i = 0; i < 4; ++i) {
			int pc = popcount(TESTS[i]);
			if (pc != RESULTS[i]) {
				std::cout << "Error: popcount(" << TESTS[i] << ") = " << pc << "   should be " << RESULTS[i] << std::endl;
				return false;
			}
		}
		Bitboard bb = ALL_SQUARES;
		int count = 64;
		do {
			Bitboard isolated = isolateLSB(bb);
			int pc = popcount(isolated);
			if (pc != 1) {
				std::cout << "Error: popcount(" << isolated << ") = " << pc << "   should be 1" << std::endl;
				return false;
			}
			bb &= bb - 1;
			--count;
			pc = popcount(bb);
			if (pc != count) {
				std::cout << "Error: popcount(" << bb << ") = " << pc << "   should be " << count << std::endl;
				return false;
			}
		} while (bb);
		std::cout << "Popcount ok!" << std::endl;
		return true;
	}

	bool testLSB()
	{
		std::cout << "Testing LSB..." << std::endl;
		for (int i = 0; i < 8; ++i) {
			Square pc = lsb(RANKS[i]);
			if (pc != Square(A1 + 8*i)) {
				std::cout << "Error: LSB(" << RANKS[i] << ") = " << pc << "   should be " << Square(A1 + 8 * i) << std::endl;
				return false;
			}
		}
		for (int i = 0; i < 8; ++i) {
			Square pc = lsb(FILES[i]);
			if (pc != Square(A1 + i)) {
				std::cout << "Error: LSB(" << FILES[i] << ") = " << pc << "   should be " << Square(A1 + i) << std::endl;
				return false;
			}
		}
		Bitboard bb = ALL_SQUARES;
		int count = 0;
		while (bb != EMPTY) {
			Square sq = pop_lsb(&bb);
			if (sq != Square(count)) {
				std::cout << "Error: pop_lsb(" << bb << ") = " << sq << "   should be " << Square(count) << std::endl;
				return false;
			}
			++count;
		}
		std::cout << "LSB ok!" << std::endl;
		return true;
	}

	bool testMSB()
	{
		std::cout << "Testing MSB..." << std::endl;
		for (int i = 0; i < 8; ++i) {
			Square pc = msb(RANKS[i]);
			if (pc != Square(H1 + 8 * i)) {
				std::cout << "Error: MSB(" << RANKS[i] << ") = " << pc << "   should be " << Square(A1 + 8 * i) << std::endl;
				return false;
			}
		}
		for (int i = 0; i < 8; ++i) {
			Square pc = msb(FILES[i]);
			if (pc != Square(A8 + i)) {
				std::cout << "Error: MSB(" << FILES[i] << ") = " << pc << "   should be " << Square(A1 + i) << std::endl;
				return false;
			}
		}
		std::cout << "MSB ok!" << std::endl;
		return true;
	}

	bool testGivesCheck()
	{
		bool result = true;
		std::vector<std::string> fens = benchFens3();
		for (auto &fen : fens) {
			Position pos(fen);
			ValuatedMove * moves = pos.GenerateMoves<LEGAL>();
			int moveCount = pos.GeneratedMoveCount();
			for (int i = 0; i < moveCount; ++i) {
				bool givesCheck = pos.givesCheck(moves[i].move);
				Position next(pos);
				next.ApplyMove(moves[i].move);
				result = result && (givesCheck == next.Checked());
				assert(result);
			}
		}
		return result;
	}

	bool testVerticalSymmetry()
	{
		Initialize();
		std::vector<std::string> fens(benchFens1());
		for (int i = 0; i < int(fens.size()); i++) {
			Position * p = new Position(fens[i]);
			bool castles = p->GetCastles() != 0;
			delete p;
			if (castles) continue;
			ValuatedMove m[2];
			for (int j = 0; j < 2; ++j) {
				tt::clear();
				std::string fen = fens[i];
				if (j > 0) fen = utils::mirrorFenVertical(fen);
				Position * pos = new Position(fen);
				Search * srch = new Search();
				srch->PrintCurrmove = false;
				srch->UciOutput = false;
				srch->NewGame();
				srch->timeManager.initialize(FIXED_DEPTH, 0, 8);
				m[j] = srch->Think(*pos);
				std::cout << toString(m[j].move) << " " << (int)m[j].score << "  " << fen << std::endl;
				delete srch;
				delete pos;
			}
			std::cout << std::endl;
		}
		return true;
	}


	bool testingPlatformSpecifics()
	{
		bool resultPopcount = true;
		for (Bitboard bb : RANKS) {
			resultPopcount = resultPopcount && popcount(bb) == 8;
			resultPopcount = resultPopcount && popcount(~bb) == 56;
		}
		for (Bitboard bb : FILES) {
			resultPopcount = resultPopcount && popcount(bb) == 8;
			resultPopcount = resultPopcount && popcount(~bb) == 56;
		}
		resultPopcount = resultPopcount && popcount(ALL_SQUARES) == 64;
		resultPopcount = resultPopcount && popcount(EMPTY) == 0;
		if (resultPopcount) std::cout << "Popcount ok!" << std::endl; else std::cout << "Error with Popcount!" << std::endl;
		bool resultLsb = true;
		for (int i = 0; i < 8; ++i) resultLsb = resultLsb && lsb(RANKS[i]) == 8 * i;
		for (int i = 0; i < 8; ++i) resultLsb = resultLsb && lsb(FILES[i]) == i;
		resultLsb = resultLsb && lsb(ALL_SQUARES) == 0;
		if (resultLsb) std::cout << "LSB ok!" << std::endl; else std::cout << "Error with LSB!" << std::endl;
		bool resultMsb = true;
		for (int i = 0; i < 8; ++i) resultMsb = resultMsb && msb(RANKS[i]) == 8 * i + 7;
		for (int i = 0; i < 8; ++i) resultMsb = resultMsb && msb(FILES[i]) == 56 + i;
		resultMsb = resultMsb && msb(ALL_SQUARES) == 63;
		resultMsb = resultMsb && msbInt(100) == 6;
		if (resultMsb) std::cout << "MSB ok!" << std::endl; else std::cout << "Error with MSB!" << std::endl;
		return resultPopcount && resultLsb && resultMsb;
	}

	void testResult() {
		int64_t begin = now();
		testResult("C:/Users/chrgu_000/Desktop/Data/cutechess/testpositions/stalemate.epd", Result::DRAW);
		testResult("C:/Users/chrgu_000/Desktop/Data/cutechess/testpositions/mate.epd", Result::MATE);
		int64_t end = now();
		int64_t runtime = end - begin;
		std::cout << "Runtime: " << runtime / 1000.0 << " s" << std::endl;
	}

	void testCheckQuietCheckMoveGeneration() {
		std::vector<std::string> lines;
		std::ifstream s;
		s.open("C:/Users/chrgu_000/Desktop/Data/cutechess/testpositions/ccrl.epd");
		std::string l;
		if (s.is_open())
		{
			long lineCount = 0;
			while (s)
			{
				std::getline(s, l);
				lines.push_back(l);
				lineCount++;
				if ((lineCount & 8191) == 0) std::cout << ".";
				if (((lineCount & 65535) == 0) || !s) {
					std::cout << lineCount << " Lines read from File - starting to analyze" << std::endl;
					long count = 0;
					for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
						std::string line = *it;
						if (line.length() < 10) continue;
						size_t indx = line.find(" c0");
						std::string fen = line.substr(0, indx);
						Position p1(fen);
						ValuatedMove * checkGivingMoves = p1.GenerateMoves<QUIET_CHECKS>();
						Position p2(fen);
						ValuatedMove * quietMoves = p2.GenerateMoves<QUIETS>();
						ValuatedMove * cgm = checkGivingMoves;
						ValuatedMove * qm;
						Move move;
						//check quiet check move
						while ((move = cgm->move)) {
							Position p3(p1);
							if (p3.ApplyMove(move)) {
								if (!p3.Checked()) {
									std::cout << std::endl << "Move " << toString(move) << " doesn't give check: " << fen << std::endl;
									//__debugbreak();
								}
								bool found = false;
								Move qmove;
								qm = quietMoves;
								while ((qmove = qm->move)) {
									if (qmove == move) {
										found = true;
										break;
									}
									qm++;
								}
								if (!found) {
									std::cout << std::endl << "Move " << toString(move) << " not part of quiet moves: " << fen << std::endl;
									//__debugbreak();
								};
							}
							cgm++;
						}
						//check for completeness
						qm = quietMoves;
						while ((move = qm->move)) {
							Position p3(p1);
							if (p3.ApplyMove(move) && p3.Checked()) {
								cgm = checkGivingMoves;
								Move tmove;
								bool found = false;
								while ((tmove = cgm->move)) {
									if (tmove == move) {
										found = true;
										break;
									}
									cgm++;
								}
								if (!found) {
									std::cout << std::endl << "Move " << toString(move) << " not part of check giving moves: " << fen << std::endl;
									//__debugbreak();
								};
							}
							qm++;
						}
						//if ((count & 1023) == 0)std::cout << std::endl << count << "\t";
						++count;
					}
					lines.clear();
				}
			}
			s.close();
			std::cout << std::endl;
			std::cout << lineCount << " lines read!" << std::endl;
		}
		else
		{
			std::cout << "Unable to open file" << std::endl;
		}
	}

	void testPolyglotKey() {
		uint64_t key = 0x463b96181691fc9c;
		Position pos;
		std::cout << pos.GetHash() << " - " << key << " " << (pos.GetHash() == key) << " " << pos.fen() << std::endl;
		Move move = createMove(E2, E4);
		pos.ApplyMove(move);
		key = 0x823c9b50fd114196;
		std::cout << pos.GetHash() << " - " << key << " " << (pos.GetHash() == key) << " " << pos.fen() << std::endl;
		move = createMove(D7, D5);
		pos.ApplyMove(move);
		key = 0x0756b94461c50fb0;
		std::cout << pos.GetHash() << " - " << key << " " << (pos.GetHash() == key) << " " << pos.fen() << std::endl;
		move = createMove(E4, E5);
		pos.ApplyMove(move);
		key = 0x662fafb965db29d4;
		std::cout << pos.GetHash() << " - " << key << " " << (pos.GetHash() == key) << " " << pos.fen() << std::endl;
		move = createMove(F7, F5);
		pos.ApplyMove(move);
		key = 0x22a48b5a8e47ff78;
		std::cout << pos.GetHash() << " - " << key << " " << (pos.GetHash() == key) << " " << pos.fen() << std::endl;
		move = createMove(E1, E2);
		pos.ApplyMove(move);
		key = 0x652a607ca3f242c1;
		std::cout << pos.GetHash() << " - " << key << " " << (pos.GetHash() == key) << " " << pos.fen() << std::endl;
		move = createMove(E8, F7);
		pos.ApplyMove(move);
		key = 0x00fdd303c946bdd9;
		std::cout << pos.GetHash() << " - " << key << " " << (pos.GetHash() == key) << " " << pos.fen() << std::endl;
	}

	uint64_t perftNodes = 0;
	uint64_t testCount = 0;
	int64_t perftRuntime;
	bool checkPerft(std::string fen, int depth, uint64_t expectedResult, PerftType perftType = BASIC) {
		testCount++;
		Position pos(fen);
		uint64_t perftResult;
		int64_t begin = now();
		switch (perftType) {
		case BASIC:
			perftResult = perft(pos, depth); break;
		case P1:
			perftResult = perft1(pos, depth); break;
		case P2:
			perftResult = perft2(pos, depth); break;
		case P3:
			perftResult = perft3(pos, depth); break;
		case P4:
			perftResult = perft4(pos, depth); break;
		}
		int64_t end = now();
		int64_t runtime = end - begin;
		perftRuntime += runtime;
		perftNodes += expectedResult;
		if (perftResult == expectedResult) {
			if (runtime > 0) {
				std::cout << testCount << "\t" << "OK\t" << depth << "\t" << perftResult << "\t" << runtime << " ms\t" << expectedResult / runtime << " kNodes/s\t" << std::endl << "\t" << fen << std::endl;
			}
			else {
				std::cout << testCount << "\t" << "OK\t" << depth << "\t" << perftResult << "\t" << runtime << " ms\t" << std::endl << "\t" << fen << std::endl;
			}
			return true;
		}
		else {
			perftcomb(pos, depth);
			std::cout << testCount << "\t" << "Error\t" << depth << "\t" << perftResult << "\texpected\t" << expectedResult << "\t" << fen << std::endl;
			return false;
		}
	}

	bool testPerft(PerftType perftType) {
		std::cout << "Starting Perft Suite in Mode " << perftType << std::endl;
		bool result = true;
		testCount = 0;
		perftNodes = 0;
		perftRuntime = 0;
		//if (!perftType) {
		Chess960 = true;
		std::cout << "Chess 960 Positions" << std::endl;
		result = result && checkPerft("rn2k1r1/ppp1pp1p/3p2p1/5bn1/P7/2N2B2/1PPPPP2/2BNK1RR w Gkq - 4 11", 1, 33, perftType);
		result = result && checkPerft("rn2k1r1/ppp1pp1p/3p2p1/5bn1/P7/2N2B2/1PPPPP2/2BNK1RR w Gkq - 4 11", 2, 1072, perftType);
		result = result && checkPerft("rn2k1r1/ppp1pp1p/3p2p1/5bn1/P7/2N2B2/1PPPPP2/2BNK1RR w Gkq - 4 11", 3, 35141, perftType);
		result = result && checkPerft("rn2k1r1/ppp1pp1p/3p2p1/5bn1/P7/2N2B2/1PPPPP2/2BNK1RR w Gkq - 4 11", 4, 1111449, perftType);
		result = result && checkPerft("rn2k1r1/ppp1pp1p/3p2p1/5bn1/P7/2N2B2/1PPPPP2/2BNK1RR w Gkq - 4 11", 5, 37095094, perftType);
		result = result && checkPerft("nrbkqbnr/pppppppp/8/8/8/8/PPPPPPPP/NRBKQBNR w KQkq - 0 1", 1, 19, perftType);
		result = result && checkPerft("nrbkqbnr/pppppppp/8/8/8/8/PPPPPPPP/NRBKQBNR w KQkq - 0 1", 2, 361, perftType);
		result = result && checkPerft("nrbkqbnr/pppppppp/8/8/8/8/PPPPPPPP/NRBKQBNR w KQkq - 0 1", 3, 7735, perftType);
		result = result && checkPerft("nrbkqbnr/pppppppp/8/8/8/8/PPPPPPPP/NRBKQBNR w KQkq - 0 1", 4, 164966, perftType);
		result = result && checkPerft("nrbkqbnr/pppppppp/8/8/8/8/PPPPPPPP/NRBKQBNR w KQkq - 0 1", 5, 3962549, perftType);
		result = result && checkPerft("nrbkqbnr/pppppppp/8/8/8/8/PPPPPPPP/NRBKQBNR w KQkq - 0 1", 6, 94328606, perftType);
		result = result && checkPerft("2rkr3/5PP1/8/5Q2/5q2/8/5pp1/2RKR3 w KQkq - 0 1", 5, 94370149, perftType);
		result = result && checkPerft("rqkrbnnb/pppppppp/8/8/8/8/PPPPPPPP/RQKRBNNB w KQkq - 0 1", 6, 111825069, perftType);
		//}
		Chess960 = false;
		std::cout << "Initial Position" << std::endl;
		result = result && checkPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 1, 20, perftType);
		result = result && checkPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 2, 400, perftType);
		result = result && checkPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 3, 8902, perftType);
		result = result && checkPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4, 197281, perftType);
		result = result && checkPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5, 4865609, perftType);
		result = result && checkPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 6, 119060324, perftType);
		std::cout << "Kiwi Pete" << std::endl;
		result = result && checkPerft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 1, 48, perftType);
		result = result && checkPerft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 2, 2039, perftType);
		result = result && checkPerft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 3, 97862, perftType);
		result = result && checkPerft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 4, 4085603, perftType);
		result = result && checkPerft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 5, 193690690, perftType);
		std::cout << "Standard Chess positions" << std::endl;
		result = result && checkPerft("4k3/8/8/8/8/8/8/4K2R w K - 0 1", 1, 15, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/4K2R w K - 0 1", 2, 66, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/4K2R w K - 0 1", 3, 1197, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/4K2R w K - 0 1", 4, 7059, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/4K2R w K - 0 1", 5, 133987, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/4K2R w K - 0 1", 6, 764643, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K3 w Q - 0 1", 1, 16, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K3 w Q - 0 1", 2, 71, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K3 w Q - 0 1", 3, 1287, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K3 w Q - 0 1", 4, 7626, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K3 w Q - 0 1", 5, 145232, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K3 w Q - 0 1", 6, 846648, perftType);
		result = result && checkPerft("4k2r/8/8/8/8/8/8/4K3 w k - 0 1", 1, 5, perftType);
		result = result && checkPerft("4k2r/8/8/8/8/8/8/4K3 w k - 0 1", 2, 75, perftType);
		result = result && checkPerft("4k2r/8/8/8/8/8/8/4K3 w k - 0 1", 3, 459, perftType);
		result = result && checkPerft("4k2r/8/8/8/8/8/8/4K3 w k - 0 1", 4, 8290, perftType);
		result = result && checkPerft("4k2r/8/8/8/8/8/8/4K3 w k - 0 1", 5, 47635, perftType);
		result = result && checkPerft("4k2r/8/8/8/8/8/8/4K3 w k - 0 1", 6, 899442, perftType);
		result = result && checkPerft("r3k3/8/8/8/8/8/8/4K3 w q - 0 1", 1, 5, perftType);
		result = result && checkPerft("r3k3/8/8/8/8/8/8/4K3 w q - 0 1", 2, 80, perftType);
		result = result && checkPerft("r3k3/8/8/8/8/8/8/4K3 w q - 0 1", 3, 493, perftType);
		result = result && checkPerft("r3k3/8/8/8/8/8/8/4K3 w q - 0 1", 4, 8897, perftType);
		result = result && checkPerft("r3k3/8/8/8/8/8/8/4K3 w q - 0 1", 5, 52710, perftType);
		result = result && checkPerft("r3k3/8/8/8/8/8/8/4K3 w q - 0 1", 6, 1001523, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1", 1, 26, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1", 2, 112, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1", 3, 3189, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1", 4, 17945, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1", 5, 532933, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1", 6, 2788982, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1", 1, 5, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1", 2, 130, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1", 3, 782, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1", 4, 22180, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1", 5, 118882, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1", 6, 3517770, perftType);
		result = result && checkPerft("8/8/8/8/8/8/6k1/4K2R w K - 0 1", 1, 12, perftType);
		result = result && checkPerft("8/8/8/8/8/8/6k1/4K2R w K - 0 1", 2, 38, perftType);
		result = result && checkPerft("8/8/8/8/8/8/6k1/4K2R w K - 0 1", 3, 564, perftType);
		result = result && checkPerft("8/8/8/8/8/8/6k1/4K2R w K - 0 1", 4, 2219, perftType);
		result = result && checkPerft("8/8/8/8/8/8/6k1/4K2R w K - 0 1", 5, 37735, perftType);
		result = result && checkPerft("8/8/8/8/8/8/6k1/4K2R w K - 0 1", 6, 185867, perftType);
		result = result && checkPerft("8/8/8/8/8/8/1k6/R3K3 w Q - 0 1", 1, 15, perftType);
		result = result && checkPerft("8/8/8/8/8/8/1k6/R3K3 w Q - 0 1", 2, 65, perftType);
		result = result && checkPerft("8/8/8/8/8/8/1k6/R3K3 w Q - 0 1", 3, 1018, perftType);
		result = result && checkPerft("8/8/8/8/8/8/1k6/R3K3 w Q - 0 1", 4, 4573, perftType);
		result = result && checkPerft("8/8/8/8/8/8/1k6/R3K3 w Q - 0 1", 5, 80619, perftType);
		result = result && checkPerft("8/8/8/8/8/8/1k6/R3K3 w Q - 0 1", 6, 413018, perftType);
		result = result && checkPerft("4k2r/6K1/8/8/8/8/8/8 w k - 0 1", 1, 3, perftType);
		result = result && checkPerft("4k2r/6K1/8/8/8/8/8/8 w k - 0 1", 2, 32, perftType);
		result = result && checkPerft("4k2r/6K1/8/8/8/8/8/8 w k - 0 1", 3, 134, perftType);
		result = result && checkPerft("4k2r/6K1/8/8/8/8/8/8 w k - 0 1", 4, 2073, perftType);
		result = result && checkPerft("4k2r/6K1/8/8/8/8/8/8 w k - 0 1", 5, 10485, perftType);
		result = result && checkPerft("4k2r/6K1/8/8/8/8/8/8 w k - 0 1", 6, 179869, perftType);
		result = result && checkPerft("r3k3/1K6/8/8/8/8/8/8 w q - 0 1", 1, 4, perftType);
		result = result && checkPerft("r3k3/1K6/8/8/8/8/8/8 w q - 0 1", 2, 49, perftType);
		result = result && checkPerft("r3k3/1K6/8/8/8/8/8/8 w q - 0 1", 3, 243, perftType);
		result = result && checkPerft("r3k3/1K6/8/8/8/8/8/8 w q - 0 1", 4, 3991, perftType);
		result = result && checkPerft("r3k3/1K6/8/8/8/8/8/8 w q - 0 1", 5, 20780, perftType);
		result = result && checkPerft("r3k3/1K6/8/8/8/8/8/8 w q - 0 1", 6, 367724, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1", 1, 26, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1", 2, 568, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1", 3, 13744, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1", 4, 314346, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1", 5, 7594526, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1", 6, 179862938, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1", 1, 25, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1", 2, 567, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1", 3, 14095, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1", 4, 328965, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1", 5, 8153719, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1", 6, 195629489, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1", 1, 25, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1", 2, 548, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1", 3, 13502, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1", 4, 312835, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1", 5, 7736373, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1", 6, 184411439, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1", 1, 25, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1", 2, 547, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1", 3, 13579, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1", 4, 316214, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1", 5, 7878456, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1", 6, 189224276, perftType);
		result = result && checkPerft("1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1", 1, 26, perftType);
		result = result && checkPerft("1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1", 2, 583, perftType);
		result = result && checkPerft("1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1", 3, 14252, perftType);
		result = result && checkPerft("1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1", 4, 334705, perftType);
		result = result && checkPerft("1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1", 5, 8198901, perftType);
		result = result && checkPerft("1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1", 6, 198328929, perftType);
		result = result && checkPerft("2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1", 1, 25, perftType);
		result = result && checkPerft("2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1", 2, 560, perftType);
		result = result && checkPerft("2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1", 3, 13592, perftType);
		result = result && checkPerft("2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1", 4, 317324, perftType);
		result = result && checkPerft("2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1", 5, 7710115, perftType);
		result = result && checkPerft("2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1", 6, 185959088, perftType);
		result = result && checkPerft("r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1", 1, 25, perftType);
		result = result && checkPerft("r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1", 2, 560, perftType);
		result = result && checkPerft("r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1", 3, 13607, perftType);
		result = result && checkPerft("r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1", 4, 320792, perftType);
		result = result && checkPerft("r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1", 5, 7848606, perftType);
		result = result && checkPerft("r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1", 6, 190755813, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/4K2R b K - 0 1", 1, 5, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/4K2R b K - 0 1", 2, 75, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/4K2R b K - 0 1", 3, 459, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/4K2R b K - 0 1", 4, 8290, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/4K2R b K - 0 1", 5, 47635, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/4K2R b K - 0 1", 6, 899442, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K3 b Q - 0 1", 1, 5, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K3 b Q - 0 1", 2, 80, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K3 b Q - 0 1", 3, 493, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K3 b Q - 0 1", 4, 8897, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K3 b Q - 0 1", 5, 52710, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K3 b Q - 0 1", 6, 1001523, perftType);
		result = result && checkPerft("4k2r/8/8/8/8/8/8/4K3 b k - 0 1", 1, 15, perftType);
		result = result && checkPerft("4k2r/8/8/8/8/8/8/4K3 b k - 0 1", 2, 66, perftType);
		result = result && checkPerft("4k2r/8/8/8/8/8/8/4K3 b k - 0 1", 3, 1197, perftType);
		result = result && checkPerft("4k2r/8/8/8/8/8/8/4K3 b k - 0 1", 4, 7059, perftType);
		result = result && checkPerft("4k2r/8/8/8/8/8/8/4K3 b k - 0 1", 5, 133987, perftType);
		result = result && checkPerft("4k2r/8/8/8/8/8/8/4K3 b k - 0 1", 6, 764643, perftType);
		result = result && checkPerft("r3k3/8/8/8/8/8/8/4K3 b q - 0 1", 1, 16, perftType);
		result = result && checkPerft("r3k3/8/8/8/8/8/8/4K3 b q - 0 1", 2, 71, perftType);
		result = result && checkPerft("r3k3/8/8/8/8/8/8/4K3 b q - 0 1", 3, 1287, perftType);
		result = result && checkPerft("r3k3/8/8/8/8/8/8/4K3 b q - 0 1", 4, 7626, perftType);
		result = result && checkPerft("r3k3/8/8/8/8/8/8/4K3 b q - 0 1", 5, 145232, perftType);
		result = result && checkPerft("r3k3/8/8/8/8/8/8/4K3 b q - 0 1", 6, 846648, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1", 1, 5, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1", 2, 130, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1", 3, 782, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1", 4, 22180, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1", 5, 118882, perftType);
		result = result && checkPerft("4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1", 6, 3517770, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1", 1, 26, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1", 2, 112, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1", 3, 3189, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1", 4, 17945, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1", 5, 532933, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1", 6, 2788982, perftType);
		result = result && checkPerft("8/8/8/8/8/8/6k1/4K2R b K - 0 1", 1, 3, perftType);
		result = result && checkPerft("8/8/8/8/8/8/6k1/4K2R b K - 0 1", 2, 32, perftType);
		result = result && checkPerft("8/8/8/8/8/8/6k1/4K2R b K - 0 1", 3, 134, perftType);
		result = result && checkPerft("8/8/8/8/8/8/6k1/4K2R b K - 0 1", 4, 2073, perftType);
		result = result && checkPerft("8/8/8/8/8/8/6k1/4K2R b K - 0 1", 5, 10485, perftType);
		result = result && checkPerft("8/8/8/8/8/8/6k1/4K2R b K - 0 1", 6, 179869, perftType);
		result = result && checkPerft("8/8/8/8/8/8/1k6/R3K3 b Q - 0 1", 1, 4, perftType);
		result = result && checkPerft("8/8/8/8/8/8/1k6/R3K3 b Q - 0 1", 2, 49, perftType);
		result = result && checkPerft("8/8/8/8/8/8/1k6/R3K3 b Q - 0 1", 3, 243, perftType);
		result = result && checkPerft("8/8/8/8/8/8/1k6/R3K3 b Q - 0 1", 4, 3991, perftType);
		result = result && checkPerft("8/8/8/8/8/8/1k6/R3K3 b Q - 0 1", 5, 20780, perftType);
		result = result && checkPerft("8/8/8/8/8/8/1k6/R3K3 b Q - 0 1", 6, 367724, perftType);
		result = result && checkPerft("4k2r/6K1/8/8/8/8/8/8 b k - 0 1", 1, 12, perftType);
		result = result && checkPerft("4k2r/6K1/8/8/8/8/8/8 b k - 0 1", 2, 38, perftType);
		result = result && checkPerft("4k2r/6K1/8/8/8/8/8/8 b k - 0 1", 3, 564, perftType);
		result = result && checkPerft("4k2r/6K1/8/8/8/8/8/8 b k - 0 1", 4, 2219, perftType);
		result = result && checkPerft("4k2r/6K1/8/8/8/8/8/8 b k - 0 1", 5, 37735, perftType);
		result = result && checkPerft("4k2r/6K1/8/8/8/8/8/8 b k - 0 1", 6, 185867, perftType);
		result = result && checkPerft("r3k3/1K6/8/8/8/8/8/8 b q - 0 1", 1, 15, perftType);
		result = result && checkPerft("r3k3/1K6/8/8/8/8/8/8 b q - 0 1", 2, 65, perftType);
		result = result && checkPerft("r3k3/1K6/8/8/8/8/8/8 b q - 0 1", 3, 1018, perftType);
		result = result && checkPerft("r3k3/1K6/8/8/8/8/8/8 b q - 0 1", 4, 4573, perftType);
		result = result && checkPerft("r3k3/1K6/8/8/8/8/8/8 b q - 0 1", 5, 80619, perftType);
		result = result && checkPerft("r3k3/1K6/8/8/8/8/8/8 b q - 0 1", 6, 413018, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1", 1, 26, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1", 2, 568, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1", 3, 13744, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1", 4, 314346, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1", 5, 7594526, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1", 6, 179862938, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1", 1, 26, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1", 2, 583, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1", 3, 14252, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1", 4, 334705, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1", 5, 8198901, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1", 6, 198328929, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/2R1K2R b Kkq - 0 1", 1, 25, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/2R1K2R b Kkq - 0 1", 2, 560, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/2R1K2R b Kkq - 0 1", 3, 13592, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/2R1K2R b Kkq - 0 1", 4, 317324, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/2R1K2R b Kkq - 0 1", 5, 7710115, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/2R1K2R b Kkq - 0 1", 6, 185959088, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 0 1", 1, 25, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 0 1", 2, 560, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 0 1", 3, 13607, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 0 1", 4, 320792, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 0 1", 5, 7848606, perftType);
		result = result && checkPerft("r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 0 1", 6, 190755813, perftType);
		result = result && checkPerft("1r2k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1", 1, 25, perftType);
		result = result && checkPerft("1r2k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1", 2, 567, perftType);
		result = result && checkPerft("1r2k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1", 3, 14095, perftType);
		result = result && checkPerft("1r2k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1", 4, 328965, perftType);
		result = result && checkPerft("1r2k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1", 5, 8153719, perftType);
		result = result && checkPerft("1r2k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1", 6, 195629489, perftType);
		result = result && checkPerft("2r1k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1", 1, 25, perftType);
		result = result && checkPerft("2r1k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1", 2, 548, perftType);
		result = result && checkPerft("2r1k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1", 3, 13502, perftType);
		result = result && checkPerft("2r1k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1", 4, 312835, perftType);
		result = result && checkPerft("2r1k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1", 5, 7736373, perftType);
		result = result && checkPerft("2r1k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1", 6, 184411439, perftType);
		result = result && checkPerft("r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1", 1, 25, perftType);
		result = result && checkPerft("r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1", 2, 547, perftType);
		result = result && checkPerft("r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1", 3, 13579, perftType);
		result = result && checkPerft("r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1", 4, 316214, perftType);
		result = result && checkPerft("r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1", 5, 7878456, perftType);
		result = result && checkPerft("r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1", 6, 189224276, perftType);
		result = result && checkPerft("8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1", 1, 14, perftType);
		result = result && checkPerft("8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1", 2, 195, perftType);
		result = result && checkPerft("8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1", 3, 2760, perftType);
		result = result && checkPerft("8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1", 4, 38675, perftType);
		result = result && checkPerft("8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1", 5, 570726, perftType);
		result = result && checkPerft("8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1", 6, 8107539, perftType);
		result = result && checkPerft("8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1", 1, 11, perftType);
		result = result && checkPerft("8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1", 2, 156, perftType);
		result = result && checkPerft("8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1", 3, 1636, perftType);
		result = result && checkPerft("8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1", 4, 20534, perftType);
		result = result && checkPerft("8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1", 5, 223507, perftType);
		result = result && checkPerft("8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1", 6, 2594412, perftType);
		result = result && checkPerft("8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1", 1, 19, perftType);
		result = result && checkPerft("8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1", 2, 289, perftType);
		result = result && checkPerft("8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1", 3, 4442, perftType);
		result = result && checkPerft("8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1", 4, 73584, perftType);
		result = result && checkPerft("8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1", 5, 1198299, perftType);
		result = result && checkPerft("8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1", 6, 19870403, perftType);
		result = result && checkPerft("K7/8/2n5/1n6/8/8/8/k6N w - - 0 1", 1, 3, perftType);
		result = result && checkPerft("K7/8/2n5/1n6/8/8/8/k6N w - - 0 1", 2, 51, perftType);
		result = result && checkPerft("K7/8/2n5/1n6/8/8/8/k6N w - - 0 1", 3, 345, perftType);
		result = result && checkPerft("K7/8/2n5/1n6/8/8/8/k6N w - - 0 1", 4, 5301, perftType);
		result = result && checkPerft("K7/8/2n5/1n6/8/8/8/k6N w - - 0 1", 5, 38348, perftType);
		result = result && checkPerft("K7/8/2n5/1n6/8/8/8/k6N w - - 0 1", 6, 588695, perftType);
		result = result && checkPerft("k7/8/2N5/1N6/8/8/8/K6n w - - 0 1", 1, 17, perftType);
		result = result && checkPerft("k7/8/2N5/1N6/8/8/8/K6n w - - 0 1", 2, 54, perftType);
		result = result && checkPerft("k7/8/2N5/1N6/8/8/8/K6n w - - 0 1", 3, 835, perftType);
		result = result && checkPerft("k7/8/2N5/1N6/8/8/8/K6n w - - 0 1", 4, 5910, perftType);
		result = result && checkPerft("k7/8/2N5/1N6/8/8/8/K6n w - - 0 1", 5, 92250, perftType);
		result = result && checkPerft("k7/8/2N5/1N6/8/8/8/K6n w - - 0 1", 6, 688780, perftType);
		result = result && checkPerft("8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1", 1, 15, perftType);
		result = result && checkPerft("8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1", 2, 193, perftType);
		result = result && checkPerft("8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1", 3, 2816, perftType);
		result = result && checkPerft("8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1", 4, 40039, perftType);
		result = result && checkPerft("8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1", 5, 582642, perftType);
		result = result && checkPerft("8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1", 6, 8503277, perftType);
		result = result && checkPerft("8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1", 1, 16, perftType);
		result = result && checkPerft("8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1", 2, 180, perftType);
		result = result && checkPerft("8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1", 3, 2290, perftType);
		result = result && checkPerft("8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1", 4, 24640, perftType);
		result = result && checkPerft("8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1", 5, 288141, perftType);
		result = result && checkPerft("8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1", 6, 3147566, perftType);
		result = result && checkPerft("8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1", 1, 4, perftType);
		result = result && checkPerft("8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1", 2, 68, perftType);
		result = result && checkPerft("8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1", 3, 1118, perftType);
		result = result && checkPerft("8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1", 4, 16199, perftType);
		result = result && checkPerft("8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1", 5, 281190, perftType);
		result = result && checkPerft("8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1", 6, 4405103, perftType);
		result = result && checkPerft("K7/8/2n5/1n6/8/8/8/k6N b - - 0 1", 1, 17, perftType);
		result = result && checkPerft("K7/8/2n5/1n6/8/8/8/k6N b - - 0 1", 2, 54, perftType);
		result = result && checkPerft("K7/8/2n5/1n6/8/8/8/k6N b - - 0 1", 3, 835, perftType);
		result = result && checkPerft("K7/8/2n5/1n6/8/8/8/k6N b - - 0 1", 4, 5910, perftType);
		result = result && checkPerft("K7/8/2n5/1n6/8/8/8/k6N b - - 0 1", 5, 92250, perftType);
		result = result && checkPerft("K7/8/2n5/1n6/8/8/8/k6N b - - 0 1", 6, 688780, perftType);
		result = result && checkPerft("k7/8/2N5/1N6/8/8/8/K6n b - - 0 1", 1, 3, perftType);
		result = result && checkPerft("k7/8/2N5/1N6/8/8/8/K6n b - - 0 1", 2, 51, perftType);
		result = result && checkPerft("k7/8/2N5/1N6/8/8/8/K6n b - - 0 1", 3, 345, perftType);
		result = result && checkPerft("k7/8/2N5/1N6/8/8/8/K6n b - - 0 1", 4, 5301, perftType);
		result = result && checkPerft("k7/8/2N5/1N6/8/8/8/K6n b - - 0 1", 5, 38348, perftType);
		result = result && checkPerft("k7/8/2N5/1N6/8/8/8/K6n b - - 0 1", 6, 588695, perftType);
		result = result && checkPerft("B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1", 1, 17, perftType);
		result = result && checkPerft("B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1", 2, 278, perftType);
		result = result && checkPerft("B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1", 3, 4607, perftType);
		result = result && checkPerft("B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1", 4, 76778, perftType);
		result = result && checkPerft("B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1", 5, 1320507, perftType);
		result = result && checkPerft("B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1", 6, 22823890, perftType);
		result = result && checkPerft("8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1", 1, 21, perftType);
		result = result && checkPerft("8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1", 2, 316, perftType);
		result = result && checkPerft("8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1", 3, 5744, perftType);
		result = result && checkPerft("8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1", 4, 93338, perftType);
		result = result && checkPerft("8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1", 5, 1713368, perftType);
		result = result && checkPerft("8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1", 6, 28861171, perftType);
		result = result && checkPerft("k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1", 1, 21, perftType);
		result = result && checkPerft("k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1", 2, 144, perftType);
		result = result && checkPerft("k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1", 3, 3242, perftType);
		result = result && checkPerft("k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1", 4, 32955, perftType);
		result = result && checkPerft("k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1", 5, 787524, perftType);
		result = result && checkPerft("k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1", 6, 7881673, perftType);
		result = result && checkPerft("K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1", 1, 7, perftType);
		result = result && checkPerft("K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1", 2, 143, perftType);
		result = result && checkPerft("K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1", 3, 1416, perftType);
		result = result && checkPerft("K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1", 4, 31787, perftType);
		result = result && checkPerft("K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1", 5, 310862, perftType);
		result = result && checkPerft("K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1", 6, 7382896, perftType);
		result = result && checkPerft("B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1", 1, 6, perftType);
		result = result && checkPerft("B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1", 2, 106, perftType);
		result = result && checkPerft("B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1", 3, 1829, perftType);
		result = result && checkPerft("B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1", 4, 31151, perftType);
		result = result && checkPerft("B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1", 5, 530585, perftType);
		result = result && checkPerft("B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1", 6, 9250746, perftType);
		result = result && checkPerft("8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1", 1, 17, perftType);
		result = result && checkPerft("8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1", 2, 309, perftType);
		result = result && checkPerft("8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1", 3, 5133, perftType);
		result = result && checkPerft("8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1", 4, 93603, perftType);
		result = result && checkPerft("8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1", 5, 1591064, perftType);
		result = result && checkPerft("8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1", 6, 29027891, perftType);
		result = result && checkPerft("k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1", 1, 7, perftType);
		result = result && checkPerft("k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1", 2, 143, perftType);
		result = result && checkPerft("k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1", 3, 1416, perftType);
		result = result && checkPerft("k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1", 4, 31787, perftType);
		result = result && checkPerft("k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1", 5, 310862, perftType);
		result = result && checkPerft("k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1", 6, 7382896, perftType);
		result = result && checkPerft("K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1", 1, 21, perftType);
		result = result && checkPerft("K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1", 2, 144, perftType);
		result = result && checkPerft("K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1", 3, 3242, perftType);
		result = result && checkPerft("K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1", 4, 32955, perftType);
		result = result && checkPerft("K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1", 5, 787524, perftType);
		result = result && checkPerft("K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1", 6, 7881673, perftType);
		result = result && checkPerft("7k/RR6/8/8/8/8/rr6/7K w - - 0 1", 1, 19, perftType);
		result = result && checkPerft("7k/RR6/8/8/8/8/rr6/7K w - - 0 1", 2, 275, perftType);
		result = result && checkPerft("7k/RR6/8/8/8/8/rr6/7K w - - 0 1", 3, 5300, perftType);
		result = result && checkPerft("7k/RR6/8/8/8/8/rr6/7K w - - 0 1", 4, 104342, perftType);
		result = result && checkPerft("7k/RR6/8/8/8/8/rr6/7K w - - 0 1", 5, 2161211, perftType);
		result = result && checkPerft("7k/RR6/8/8/8/8/rr6/7K w - - 0 1", 6, 44956585, perftType);
		result = result && checkPerft("R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1", 1, 36, perftType);
		result = result && checkPerft("R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1", 2, 1027, perftType);
		result = result && checkPerft("R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1", 3, 29215, perftType);
		result = result && checkPerft("R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1", 4, 771461, perftType);
		result = result && checkPerft("R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1", 5, 20506480, perftType);
		result = result && checkPerft("R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1", 6, 525169084, perftType);
		result = result && checkPerft("7k/RR6/8/8/8/8/rr6/7K b - - 0 1", 1, 19, perftType);
		result = result && checkPerft("7k/RR6/8/8/8/8/rr6/7K b - - 0 1", 2, 275, perftType);
		result = result && checkPerft("7k/RR6/8/8/8/8/rr6/7K b - - 0 1", 3, 5300, perftType);
		result = result && checkPerft("7k/RR6/8/8/8/8/rr6/7K b - - 0 1", 4, 104342, perftType);
		result = result && checkPerft("7k/RR6/8/8/8/8/rr6/7K b - - 0 1", 5, 2161211, perftType);
		result = result && checkPerft("7k/RR6/8/8/8/8/rr6/7K b - - 0 1", 6, 44956585, perftType);
		result = result && checkPerft("R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1", 1, 36, perftType);
		result = result && checkPerft("R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1", 2, 1027, perftType);
		result = result && checkPerft("R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1", 3, 29227, perftType);
		result = result && checkPerft("R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1", 4, 771368, perftType);
		result = result && checkPerft("R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1", 5, 20521342, perftType);
		result = result && checkPerft("R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1", 6, 524966748, perftType);
		result = result && checkPerft("6kq/8/8/8/8/8/8/7K w - - 0 1", 1, 2, perftType);
		result = result && checkPerft("6kq/8/8/8/8/8/8/7K w - - 0 1", 2, 36, perftType);
		result = result && checkPerft("6kq/8/8/8/8/8/8/7K w - - 0 1", 3, 143, perftType);
		result = result && checkPerft("6kq/8/8/8/8/8/8/7K w - - 0 1", 4, 3637, perftType);
		result = result && checkPerft("6kq/8/8/8/8/8/8/7K w - - 0 1", 5, 14893, perftType);
		result = result && checkPerft("6kq/8/8/8/8/8/8/7K w - - 0 1", 6, 391507, perftType);
		result = result && checkPerft("6KQ/8/8/8/8/8/8/7k b - - 0 1", 1, 2, perftType);
		result = result && checkPerft("6KQ/8/8/8/8/8/8/7k b - - 0 1", 2, 36, perftType);
		result = result && checkPerft("6KQ/8/8/8/8/8/8/7k b - - 0 1", 3, 143, perftType);
		result = result && checkPerft("6KQ/8/8/8/8/8/8/7k b - - 0 1", 4, 3637, perftType);
		result = result && checkPerft("6KQ/8/8/8/8/8/8/7k b - - 0 1", 5, 14893, perftType);
		result = result && checkPerft("6KQ/8/8/8/8/8/8/7k b - - 0 1", 6, 391507, perftType);
		result = result && checkPerft("K7/8/8/3Q4/4q3/8/8/7k w - - 0 1", 1, 6, perftType);
		result = result && checkPerft("K7/8/8/3Q4/4q3/8/8/7k w - - 0 1", 2, 35, perftType);
		result = result && checkPerft("K7/8/8/3Q4/4q3/8/8/7k w - - 0 1", 3, 495, perftType);
		result = result && checkPerft("K7/8/8/3Q4/4q3/8/8/7k w - - 0 1", 4, 8349, perftType);
		result = result && checkPerft("K7/8/8/3Q4/4q3/8/8/7k w - - 0 1", 5, 166741, perftType);
		result = result && checkPerft("K7/8/8/3Q4/4q3/8/8/7k w - - 0 1", 6, 3370175, perftType);
		result = result && checkPerft("6qk/8/8/8/8/8/8/7K b - - 0 1", 1, 22, perftType);
		result = result && checkPerft("6qk/8/8/8/8/8/8/7K b - - 0 1", 2, 43, perftType);
		result = result && checkPerft("6qk/8/8/8/8/8/8/7K b - - 0 1", 3, 1015, perftType);
		result = result && checkPerft("6qk/8/8/8/8/8/8/7K b - - 0 1", 4, 4167, perftType);
		result = result && checkPerft("6qk/8/8/8/8/8/8/7K b - - 0 1", 5, 105749, perftType);
		result = result && checkPerft("6qk/8/8/8/8/8/8/7K b - - 0 1", 6, 419369, perftType);
		result = result && checkPerft("6KQ/8/8/8/8/8/8/7k b - - 0 1", 1, 2, perftType);
		result = result && checkPerft("6KQ/8/8/8/8/8/8/7k b - - 0 1", 2, 36, perftType);
		result = result && checkPerft("6KQ/8/8/8/8/8/8/7k b - - 0 1", 3, 143, perftType);
		result = result && checkPerft("6KQ/8/8/8/8/8/8/7k b - - 0 1", 4, 3637, perftType);
		result = result && checkPerft("6KQ/8/8/8/8/8/8/7k b - - 0 1", 5, 14893, perftType);
		result = result && checkPerft("6KQ/8/8/8/8/8/8/7k b - - 0 1", 6, 391507, perftType);
		result = result && checkPerft("K7/8/8/3Q4/4q3/8/8/7k b - - 0 1", 1, 6, perftType);
		result = result && checkPerft("K7/8/8/3Q4/4q3/8/8/7k b - - 0 1", 2, 35, perftType);
		result = result && checkPerft("K7/8/8/3Q4/4q3/8/8/7k b - - 0 1", 3, 495, perftType);
		result = result && checkPerft("K7/8/8/3Q4/4q3/8/8/7k b - - 0 1", 4, 8349, perftType);
		result = result && checkPerft("K7/8/8/3Q4/4q3/8/8/7k b - - 0 1", 5, 166741, perftType);
		result = result && checkPerft("K7/8/8/3Q4/4q3/8/8/7k b - - 0 1", 6, 3370175, perftType);
		result = result && checkPerft("8/8/8/8/8/K7/P7/k7 w - - 0 1", 1, 3, perftType);
		result = result && checkPerft("8/8/8/8/8/K7/P7/k7 w - - 0 1", 2, 7, perftType);
		result = result && checkPerft("8/8/8/8/8/K7/P7/k7 w - - 0 1", 3, 43, perftType);
		result = result && checkPerft("8/8/8/8/8/K7/P7/k7 w - - 0 1", 4, 199, perftType);
		result = result && checkPerft("8/8/8/8/8/K7/P7/k7 w - - 0 1", 5, 1347, perftType);
		result = result && checkPerft("8/8/8/8/8/K7/P7/k7 w - - 0 1", 6, 6249, perftType);
		result = result && checkPerft("8/8/8/8/8/7K/7P/7k w - - 0 1", 1, 3, perftType);
		result = result && checkPerft("8/8/8/8/8/7K/7P/7k w - - 0 1", 2, 7, perftType);
		result = result && checkPerft("8/8/8/8/8/7K/7P/7k w - - 0 1", 3, 43, perftType);
		result = result && checkPerft("8/8/8/8/8/7K/7P/7k w - - 0 1", 4, 199, perftType);
		result = result && checkPerft("8/8/8/8/8/7K/7P/7k w - - 0 1", 5, 1347, perftType);
		result = result && checkPerft("8/8/8/8/8/7K/7P/7k w - - 0 1", 6, 6249, perftType);
		result = result && checkPerft("K7/p7/k7/8/8/8/8/8 w - - 0 1", 1, 1, perftType);
		result = result && checkPerft("K7/p7/k7/8/8/8/8/8 w - - 0 1", 2, 3, perftType);
		result = result && checkPerft("K7/p7/k7/8/8/8/8/8 w - - 0 1", 3, 12, perftType);
		result = result && checkPerft("K7/p7/k7/8/8/8/8/8 w - - 0 1", 4, 80, perftType);
		result = result && checkPerft("K7/p7/k7/8/8/8/8/8 w - - 0 1", 5, 342, perftType);
		result = result && checkPerft("K7/p7/k7/8/8/8/8/8 w - - 0 1", 6, 2343, perftType);
		result = result && checkPerft("7K/7p/7k/8/8/8/8/8 w - - 0 1", 1, 1, perftType);
		result = result && checkPerft("7K/7p/7k/8/8/8/8/8 w - - 0 1", 2, 3, perftType);
		result = result && checkPerft("7K/7p/7k/8/8/8/8/8 w - - 0 1", 3, 12, perftType);
		result = result && checkPerft("7K/7p/7k/8/8/8/8/8 w - - 0 1", 4, 80, perftType);
		result = result && checkPerft("7K/7p/7k/8/8/8/8/8 w - - 0 1", 5, 342, perftType);
		result = result && checkPerft("7K/7p/7k/8/8/8/8/8 w - - 0 1", 6, 2343, perftType);
		result = result && checkPerft("8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1", 1, 7, perftType);
		result = result && checkPerft("8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1", 2, 35, perftType);
		result = result && checkPerft("8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1", 3, 210, perftType);
		result = result && checkPerft("8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1", 4, 1091, perftType);
		result = result && checkPerft("8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1", 5, 7028, perftType);
		result = result && checkPerft("8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1", 6, 34834, perftType);
		result = result && checkPerft("8/8/8/8/8/K7/P7/k7 b - - 0 1", 1, 1, perftType);
		result = result && checkPerft("8/8/8/8/8/K7/P7/k7 b - - 0 1", 2, 3, perftType);
		result = result && checkPerft("8/8/8/8/8/K7/P7/k7 b - - 0 1", 3, 12, perftType);
		result = result && checkPerft("8/8/8/8/8/K7/P7/k7 b - - 0 1", 4, 80, perftType);
		result = result && checkPerft("8/8/8/8/8/K7/P7/k7 b - - 0 1", 5, 342, perftType);
		result = result && checkPerft("8/8/8/8/8/K7/P7/k7 b - - 0 1", 6, 2343, perftType);
		result = result && checkPerft("8/8/8/8/8/7K/7P/7k b - - 0 1", 1, 1, perftType);
		result = result && checkPerft("8/8/8/8/8/7K/7P/7k b - - 0 1", 2, 3, perftType);
		result = result && checkPerft("8/8/8/8/8/7K/7P/7k b - - 0 1", 3, 12, perftType);
		result = result && checkPerft("8/8/8/8/8/7K/7P/7k b - - 0 1", 4, 80, perftType);
		result = result && checkPerft("8/8/8/8/8/7K/7P/7k b - - 0 1", 5, 342, perftType);
		result = result && checkPerft("8/8/8/8/8/7K/7P/7k b - - 0 1", 6, 2343, perftType);
		result = result && checkPerft("K7/p7/k7/8/8/8/8/8 b - - 0 1", 1, 3, perftType);
		result = result && checkPerft("K7/p7/k7/8/8/8/8/8 b - - 0 1", 2, 7, perftType);
		result = result && checkPerft("K7/p7/k7/8/8/8/8/8 b - - 0 1", 3, 43, perftType);
		result = result && checkPerft("K7/p7/k7/8/8/8/8/8 b - - 0 1", 4, 199, perftType);
		result = result && checkPerft("K7/p7/k7/8/8/8/8/8 b - - 0 1", 5, 1347, perftType);
		result = result && checkPerft("K7/p7/k7/8/8/8/8/8 b - - 0 1", 6, 6249, perftType);
		result = result && checkPerft("7K/7p/7k/8/8/8/8/8 b - - 0 1", 1, 3, perftType);
		result = result && checkPerft("7K/7p/7k/8/8/8/8/8 b - - 0 1", 2, 7, perftType);
		result = result && checkPerft("7K/7p/7k/8/8/8/8/8 b - - 0 1", 3, 43, perftType);
		result = result && checkPerft("7K/7p/7k/8/8/8/8/8 b - - 0 1", 4, 199, perftType);
		result = result && checkPerft("7K/7p/7k/8/8/8/8/8 b - - 0 1", 5, 1347, perftType);
		result = result && checkPerft("7K/7p/7k/8/8/8/8/8 b - - 0 1", 6, 6249, perftType);
		result = result && checkPerft("8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1", 1, 5, perftType);
		result = result && checkPerft("8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1", 2, 35, perftType);
		result = result && checkPerft("8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1", 3, 182, perftType);
		result = result && checkPerft("8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1", 4, 1091, perftType);
		result = result && checkPerft("8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1", 5, 5408, perftType);
		result = result && checkPerft("8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1", 6, 34822, perftType);
		result = result && checkPerft("8/8/8/8/8/4k3/4P3/4K3 w - - 0 1", 1, 2, perftType);
		result = result && checkPerft("8/8/8/8/8/4k3/4P3/4K3 w - - 0 1", 2, 8, perftType);
		result = result && checkPerft("8/8/8/8/8/4k3/4P3/4K3 w - - 0 1", 3, 44, perftType);
		result = result && checkPerft("8/8/8/8/8/4k3/4P3/4K3 w - - 0 1", 4, 282, perftType);
		result = result && checkPerft("8/8/8/8/8/4k3/4P3/4K3 w - - 0 1", 5, 1814, perftType);
		result = result && checkPerft("8/8/8/8/8/4k3/4P3/4K3 w - - 0 1", 6, 11848, perftType);
		result = result && checkPerft("4k3/4p3/4K3/8/8/8/8/8 b - - 0 1", 1, 2, perftType);
		result = result && checkPerft("4k3/4p3/4K3/8/8/8/8/8 b - - 0 1", 2, 8, perftType);
		result = result && checkPerft("4k3/4p3/4K3/8/8/8/8/8 b - - 0 1", 3, 44, perftType);
		result = result && checkPerft("4k3/4p3/4K3/8/8/8/8/8 b - - 0 1", 4, 282, perftType);
		result = result && checkPerft("4k3/4p3/4K3/8/8/8/8/8 b - - 0 1", 5, 1814, perftType);
		result = result && checkPerft("4k3/4p3/4K3/8/8/8/8/8 b - - 0 1", 6, 11848, perftType);
		result = result && checkPerft("8/8/7k/7p/7P/7K/8/8 w - - 0 1", 1, 3, perftType);
		result = result && checkPerft("8/8/7k/7p/7P/7K/8/8 w - - 0 1", 2, 9, perftType);
		result = result && checkPerft("8/8/7k/7p/7P/7K/8/8 w - - 0 1", 3, 57, perftType);
		result = result && checkPerft("8/8/7k/7p/7P/7K/8/8 w - - 0 1", 4, 360, perftType);
		result = result && checkPerft("8/8/7k/7p/7P/7K/8/8 w - - 0 1", 5, 1969, perftType);
		result = result && checkPerft("8/8/7k/7p/7P/7K/8/8 w - - 0 1", 6, 10724, perftType);
		result = result && checkPerft("8/8/k7/p7/P7/K7/8/8 w - - 0 1", 1, 3, perftType);
		result = result && checkPerft("8/8/k7/p7/P7/K7/8/8 w - - 0 1", 2, 9, perftType);
		result = result && checkPerft("8/8/k7/p7/P7/K7/8/8 w - - 0 1", 3, 57, perftType);
		result = result && checkPerft("8/8/k7/p7/P7/K7/8/8 w - - 0 1", 4, 360, perftType);
		result = result && checkPerft("8/8/k7/p7/P7/K7/8/8 w - - 0 1", 5, 1969, perftType);
		result = result && checkPerft("8/8/k7/p7/P7/K7/8/8 w - - 0 1", 6, 10724, perftType);
		result = result && checkPerft("8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1", 1, 5, perftType);
		result = result && checkPerft("8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1", 2, 25, perftType);
		result = result && checkPerft("8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1", 3, 180, perftType);
		result = result && checkPerft("8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1", 4, 1294, perftType);
		result = result && checkPerft("8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1", 5, 8296, perftType);
		result = result && checkPerft("8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1", 6, 53138, perftType);
		result = result && checkPerft("8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1", 1, 8, perftType);
		result = result && checkPerft("8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1", 2, 61, perftType);
		result = result && checkPerft("8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1", 3, 483, perftType);
		result = result && checkPerft("8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1", 4, 3213, perftType);
		result = result && checkPerft("8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1", 5, 23599, perftType);
		result = result && checkPerft("8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1", 6, 157093, perftType);
		result = result && checkPerft("8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1", 1, 8, perftType);
		result = result && checkPerft("8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1", 2, 61, perftType);
		result = result && checkPerft("8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1", 3, 411, perftType);
		result = result && checkPerft("8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1", 4, 3213, perftType);
		result = result && checkPerft("8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1", 5, 21637, perftType);
		result = result && checkPerft("8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1", 6, 158065, perftType);
		result = result && checkPerft("k7/8/3p4/8/3P4/8/8/7K w - - 0 1", 1, 4, perftType);
		result = result && checkPerft("k7/8/3p4/8/3P4/8/8/7K w - - 0 1", 2, 15, perftType);
		result = result && checkPerft("k7/8/3p4/8/3P4/8/8/7K w - - 0 1", 3, 90, perftType);
		result = result && checkPerft("k7/8/3p4/8/3P4/8/8/7K w - - 0 1", 4, 534, perftType);
		result = result && checkPerft("k7/8/3p4/8/3P4/8/8/7K w - - 0 1", 5, 3450, perftType);
		result = result && checkPerft("k7/8/3p4/8/3P4/8/8/7K w - - 0 1", 6, 20960, perftType);
		result = result && checkPerft("8/8/7k/7p/7P/7K/8/8 b - - 0 1", 1, 3, perftType);
		result = result && checkPerft("8/8/7k/7p/7P/7K/8/8 b - - 0 1", 2, 9, perftType);
		result = result && checkPerft("8/8/7k/7p/7P/7K/8/8 b - - 0 1", 3, 57, perftType);
		result = result && checkPerft("8/8/7k/7p/7P/7K/8/8 b - - 0 1", 4, 360, perftType);
		result = result && checkPerft("8/8/7k/7p/7P/7K/8/8 b - - 0 1", 5, 1969, perftType);
		result = result && checkPerft("8/8/7k/7p/7P/7K/8/8 b - - 0 1", 6, 10724, perftType);
		result = result && checkPerft("8/8/k7/p7/P7/K7/8/8 b - - 0 1", 1, 3, perftType);
		result = result && checkPerft("8/8/k7/p7/P7/K7/8/8 b - - 0 1", 2, 9, perftType);
		result = result && checkPerft("8/8/k7/p7/P7/K7/8/8 b - - 0 1", 3, 57, perftType);
		result = result && checkPerft("8/8/k7/p7/P7/K7/8/8 b - - 0 1", 4, 360, perftType);
		result = result && checkPerft("8/8/k7/p7/P7/K7/8/8 b - - 0 1", 5, 1969, perftType);
		result = result && checkPerft("8/8/k7/p7/P7/K7/8/8 b - - 0 1", 6, 10724, perftType);
		result = result && checkPerft("8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1", 1, 5, perftType);
		result = result && checkPerft("8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1", 2, 25, perftType);
		result = result && checkPerft("8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1", 3, 180, perftType);
		result = result && checkPerft("8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1", 4, 1294, perftType);
		result = result && checkPerft("8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1", 5, 8296, perftType);
		result = result && checkPerft("8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1", 6, 53138, perftType);
		result = result && checkPerft("8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1", 1, 8, perftType);
		result = result && checkPerft("8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1", 2, 61, perftType);
		result = result && checkPerft("8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1", 3, 411, perftType);
		result = result && checkPerft("8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1", 4, 3213, perftType);
		result = result && checkPerft("8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1", 5, 21637, perftType);
		result = result && checkPerft("8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1", 6, 158065, perftType);
		result = result && checkPerft("8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1", 1, 8, perftType);
		result = result && checkPerft("8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1", 2, 61, perftType);
		result = result && checkPerft("8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1", 3, 483, perftType);
		result = result && checkPerft("8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1", 4, 3213, perftType);
		result = result && checkPerft("8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1", 5, 23599, perftType);
		result = result && checkPerft("8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1", 6, 157093, perftType);
		result = result && checkPerft("k7/8/3p4/8/3P4/8/8/7K b - - 0 1", 1, 4, perftType);
		result = result && checkPerft("k7/8/3p4/8/3P4/8/8/7K b - - 0 1", 2, 15, perftType);
		result = result && checkPerft("k7/8/3p4/8/3P4/8/8/7K b - - 0 1", 3, 89, perftType);
		result = result && checkPerft("k7/8/3p4/8/3P4/8/8/7K b - - 0 1", 4, 537, perftType);
		result = result && checkPerft("k7/8/3p4/8/3P4/8/8/7K b - - 0 1", 5, 3309, perftType);
		result = result && checkPerft("k7/8/3p4/8/3P4/8/8/7K b - - 0 1", 6, 21104, perftType);
		result = result && checkPerft("7k/3p4/8/8/3P4/8/8/K7 w - - 0 1", 1, 4, perftType);
		result = result && checkPerft("7k/3p4/8/8/3P4/8/8/K7 w - - 0 1", 2, 19, perftType);
		result = result && checkPerft("7k/3p4/8/8/3P4/8/8/K7 w - - 0 1", 3, 117, perftType);
		result = result && checkPerft("7k/3p4/8/8/3P4/8/8/K7 w - - 0 1", 4, 720, perftType);
		result = result && checkPerft("7k/3p4/8/8/3P4/8/8/K7 w - - 0 1", 5, 4661, perftType);
		result = result && checkPerft("7k/3p4/8/8/3P4/8/8/K7 w - - 0 1", 6, 32191, perftType);
		result = result && checkPerft("7k/8/8/3p4/8/8/3P4/K7 w - - 0 1", 1, 5, perftType);
		result = result && checkPerft("7k/8/8/3p4/8/8/3P4/K7 w - - 0 1", 2, 19, perftType);
		result = result && checkPerft("7k/8/8/3p4/8/8/3P4/K7 w - - 0 1", 3, 116, perftType);
		result = result && checkPerft("7k/8/8/3p4/8/8/3P4/K7 w - - 0 1", 4, 716, perftType);
		result = result && checkPerft("7k/8/8/3p4/8/8/3P4/K7 w - - 0 1", 5, 4786, perftType);
		result = result && checkPerft("7k/8/8/3p4/8/8/3P4/K7 w - - 0 1", 6, 30980, perftType);
		result = result && checkPerft("k7/8/8/7p/6P1/8/8/K7 w - - 0 1", 1, 5, perftType);
		result = result && checkPerft("k7/8/8/7p/6P1/8/8/K7 w - - 0 1", 2, 22, perftType);
		result = result && checkPerft("k7/8/8/7p/6P1/8/8/K7 w - - 0 1", 3, 139, perftType);
		result = result && checkPerft("k7/8/8/7p/6P1/8/8/K7 w - - 0 1", 4, 877, perftType);
		result = result && checkPerft("k7/8/8/7p/6P1/8/8/K7 w - - 0 1", 5, 6112, perftType);
		result = result && checkPerft("k7/8/8/7p/6P1/8/8/K7 w - - 0 1", 6, 41874, perftType);
		result = result && checkPerft("k7/8/7p/8/8/6P1/8/K7 w - - 0 1", 1, 4, perftType);
		result = result && checkPerft("k7/8/7p/8/8/6P1/8/K7 w - - 0 1", 2, 16, perftType);
		result = result && checkPerft("k7/8/7p/8/8/6P1/8/K7 w - - 0 1", 3, 101, perftType);
		result = result && checkPerft("k7/8/7p/8/8/6P1/8/K7 w - - 0 1", 4, 637, perftType);
		result = result && checkPerft("k7/8/7p/8/8/6P1/8/K7 w - - 0 1", 5, 4354, perftType);
		result = result && checkPerft("k7/8/7p/8/8/6P1/8/K7 w - - 0 1", 6, 29679, perftType);
		result = result && checkPerft("k7/8/8/6p1/7P/8/8/K7 w - - 0 1", 1, 5, perftType);
		result = result && checkPerft("k7/8/8/6p1/7P/8/8/K7 w - - 0 1", 2, 22, perftType);
		result = result && checkPerft("k7/8/8/6p1/7P/8/8/K7 w - - 0 1", 3, 139, perftType);
		result = result && checkPerft("k7/8/8/6p1/7P/8/8/K7 w - - 0 1", 4, 877, perftType);
		result = result && checkPerft("k7/8/8/6p1/7P/8/8/K7 w - - 0 1", 5, 6112, perftType);
		result = result && checkPerft("k7/8/8/6p1/7P/8/8/K7 w - - 0 1", 6, 41874, perftType);
		result = result && checkPerft("k7/8/6p1/8/8/7P/8/K7 w - - 0 1", 1, 4, perftType);
		result = result && checkPerft("k7/8/6p1/8/8/7P/8/K7 w - - 0 1", 2, 16, perftType);
		result = result && checkPerft("k7/8/6p1/8/8/7P/8/K7 w - - 0 1", 3, 101, perftType);
		result = result && checkPerft("k7/8/6p1/8/8/7P/8/K7 w - - 0 1", 4, 637, perftType);
		result = result && checkPerft("k7/8/6p1/8/8/7P/8/K7 w - - 0 1", 5, 4354, perftType);
		result = result && checkPerft("k7/8/6p1/8/8/7P/8/K7 w - - 0 1", 6, 29679, perftType);
		result = result && checkPerft("k7/8/8/3p4/4p3/8/8/7K w - - 0 1", 1, 3, perftType);
		result = result && checkPerft("k7/8/8/3p4/4p3/8/8/7K w - - 0 1", 2, 15, perftType);
		result = result && checkPerft("k7/8/8/3p4/4p3/8/8/7K w - - 0 1", 3, 84, perftType);
		result = result && checkPerft("k7/8/8/3p4/4p3/8/8/7K w - - 0 1", 4, 573, perftType);
		result = result && checkPerft("k7/8/8/3p4/4p3/8/8/7K w - - 0 1", 5, 3013, perftType);
		result = result && checkPerft("k7/8/8/3p4/4p3/8/8/7K w - - 0 1", 6, 22886, perftType);
		result = result && checkPerft("k7/8/3p4/8/8/4P3/8/7K w - - 0 1", 1, 4, perftType);
		result = result && checkPerft("k7/8/3p4/8/8/4P3/8/7K w - - 0 1", 2, 16, perftType);
		result = result && checkPerft("k7/8/3p4/8/8/4P3/8/7K w - - 0 1", 3, 101, perftType);
		result = result && checkPerft("k7/8/3p4/8/8/4P3/8/7K w - - 0 1", 4, 637, perftType);
		result = result && checkPerft("k7/8/3p4/8/8/4P3/8/7K w - - 0 1", 5, 4271, perftType);
		result = result && checkPerft("k7/8/3p4/8/8/4P3/8/7K w - - 0 1", 6, 28662, perftType);
		result = result && checkPerft("7k/3p4/8/8/3P4/8/8/K7 b - - 0 1", 1, 5, perftType);
		result = result && checkPerft("7k/3p4/8/8/3P4/8/8/K7 b - - 0 1", 2, 19, perftType);
		result = result && checkPerft("7k/3p4/8/8/3P4/8/8/K7 b - - 0 1", 3, 117, perftType);
		result = result && checkPerft("7k/3p4/8/8/3P4/8/8/K7 b - - 0 1", 4, 720, perftType);
		result = result && checkPerft("7k/3p4/8/8/3P4/8/8/K7 b - - 0 1", 5, 5014, perftType);
		result = result && checkPerft("7k/3p4/8/8/3P4/8/8/K7 b - - 0 1", 6, 32167, perftType);
		result = result && checkPerft("7k/8/8/3p4/8/8/3P4/K7 b - - 0 1", 1, 4, perftType);
		result = result && checkPerft("7k/8/8/3p4/8/8/3P4/K7 b - - 0 1", 2, 19, perftType);
		result = result && checkPerft("7k/8/8/3p4/8/8/3P4/K7 b - - 0 1", 3, 117, perftType);
		result = result && checkPerft("7k/8/8/3p4/8/8/3P4/K7 b - - 0 1", 4, 712, perftType);
		result = result && checkPerft("7k/8/8/3p4/8/8/3P4/K7 b - - 0 1", 5, 4658, perftType);
		result = result && checkPerft("7k/8/8/3p4/8/8/3P4/K7 b - - 0 1", 6, 30749, perftType);
		result = result && checkPerft("k7/8/8/7p/6P1/8/8/K7 b - - 0 1", 1, 5, perftType);
		result = result && checkPerft("k7/8/8/7p/6P1/8/8/K7 b - - 0 1", 2, 22, perftType);
		result = result && checkPerft("k7/8/8/7p/6P1/8/8/K7 b - - 0 1", 3, 139, perftType);
		result = result && checkPerft("k7/8/8/7p/6P1/8/8/K7 b - - 0 1", 4, 877, perftType);
		result = result && checkPerft("k7/8/8/7p/6P1/8/8/K7 b - - 0 1", 5, 6112, perftType);
		result = result && checkPerft("k7/8/8/7p/6P1/8/8/K7 b - - 0 1", 6, 41874, perftType);
		result = result && checkPerft("k7/8/7p/8/8/6P1/8/K7 b - - 0 1", 1, 4, perftType);
		result = result && checkPerft("k7/8/7p/8/8/6P1/8/K7 b - - 0 1", 2, 16, perftType);
		result = result && checkPerft("k7/8/7p/8/8/6P1/8/K7 b - - 0 1", 3, 101, perftType);
		result = result && checkPerft("k7/8/7p/8/8/6P1/8/K7 b - - 0 1", 4, 637, perftType);
		result = result && checkPerft("k7/8/7p/8/8/6P1/8/K7 b - - 0 1", 5, 4354, perftType);
		result = result && checkPerft("k7/8/7p/8/8/6P1/8/K7 b - - 0 1", 6, 29679, perftType);
		result = result && checkPerft("k7/8/8/6p1/7P/8/8/K7 b - - 0 1", 1, 5, perftType);
		result = result && checkPerft("k7/8/8/6p1/7P/8/8/K7 b - - 0 1", 2, 22, perftType);
		result = result && checkPerft("k7/8/8/6p1/7P/8/8/K7 b - - 0 1", 3, 139, perftType);
		result = result && checkPerft("k7/8/8/6p1/7P/8/8/K7 b - - 0 1", 4, 877, perftType);
		result = result && checkPerft("k7/8/8/6p1/7P/8/8/K7 b - - 0 1", 5, 6112, perftType);
		result = result && checkPerft("k7/8/8/6p1/7P/8/8/K7 b - - 0 1", 6, 41874, perftType);
		result = result && checkPerft("k7/8/6p1/8/8/7P/8/K7 b - - 0 1", 1, 4, perftType);
		result = result && checkPerft("k7/8/6p1/8/8/7P/8/K7 b - - 0 1", 2, 16, perftType);
		result = result && checkPerft("k7/8/6p1/8/8/7P/8/K7 b - - 0 1", 3, 101, perftType);
		result = result && checkPerft("k7/8/6p1/8/8/7P/8/K7 b - - 0 1", 4, 637, perftType);
		result = result && checkPerft("k7/8/6p1/8/8/7P/8/K7 b - - 0 1", 5, 4354, perftType);
		result = result && checkPerft("k7/8/6p1/8/8/7P/8/K7 b - - 0 1", 6, 29679, perftType);
		result = result && checkPerft("k7/8/8/3p4/4p3/8/8/7K b - - 0 1", 1, 5, perftType);
		result = result && checkPerft("k7/8/8/3p4/4p3/8/8/7K b - - 0 1", 2, 15, perftType);
		result = result && checkPerft("k7/8/8/3p4/4p3/8/8/7K b - - 0 1", 3, 102, perftType);
		result = result && checkPerft("k7/8/8/3p4/4p3/8/8/7K b - - 0 1", 4, 569, perftType);
		result = result && checkPerft("k7/8/8/3p4/4p3/8/8/7K b - - 0 1", 5, 4337, perftType);
		result = result && checkPerft("k7/8/8/3p4/4p3/8/8/7K b - - 0 1", 6, 22579, perftType);
		result = result && checkPerft("k7/8/3p4/8/8/4P3/8/7K b - - 0 1", 1, 4, perftType);
		result = result && checkPerft("k7/8/3p4/8/8/4P3/8/7K b - - 0 1", 2, 16, perftType);
		result = result && checkPerft("k7/8/3p4/8/8/4P3/8/7K b - - 0 1", 3, 101, perftType);
		result = result && checkPerft("k7/8/3p4/8/8/4P3/8/7K b - - 0 1", 4, 637, perftType);
		result = result && checkPerft("k7/8/3p4/8/8/4P3/8/7K b - - 0 1", 5, 4271, perftType);
		result = result && checkPerft("k7/8/3p4/8/8/4P3/8/7K b - - 0 1", 6, 28662, perftType);
		result = result && checkPerft("7k/8/8/p7/1P6/8/8/7K w - - 0 1", 1, 5, perftType);
		result = result && checkPerft("7k/8/8/p7/1P6/8/8/7K w - - 0 1", 2, 22, perftType);
		result = result && checkPerft("7k/8/8/p7/1P6/8/8/7K w - - 0 1", 3, 139, perftType);
		result = result && checkPerft("7k/8/8/p7/1P6/8/8/7K w - - 0 1", 4, 877, perftType);
		result = result && checkPerft("7k/8/8/p7/1P6/8/8/7K w - - 0 1", 5, 6112, perftType);
		result = result && checkPerft("7k/8/8/p7/1P6/8/8/7K w - - 0 1", 6, 41874, perftType);
		result = result && checkPerft("7k/8/p7/8/8/1P6/8/7K w - - 0 1", 1, 4, perftType);
		result = result && checkPerft("7k/8/p7/8/8/1P6/8/7K w - - 0 1", 2, 16, perftType);
		result = result && checkPerft("7k/8/p7/8/8/1P6/8/7K w - - 0 1", 3, 101, perftType);
		result = result && checkPerft("7k/8/p7/8/8/1P6/8/7K w - - 0 1", 4, 637, perftType);
		result = result && checkPerft("7k/8/p7/8/8/1P6/8/7K w - - 0 1", 5, 4354, perftType);
		result = result && checkPerft("7k/8/p7/8/8/1P6/8/7K w - - 0 1", 6, 29679, perftType);
		result = result && checkPerft("7k/8/8/1p6/P7/8/8/7K w - - 0 1", 1, 5, perftType);
		result = result && checkPerft("7k/8/8/1p6/P7/8/8/7K w - - 0 1", 2, 22, perftType);
		result = result && checkPerft("7k/8/8/1p6/P7/8/8/7K w - - 0 1", 3, 139, perftType);
		result = result && checkPerft("7k/8/8/1p6/P7/8/8/7K w - - 0 1", 4, 877, perftType);
		result = result && checkPerft("7k/8/8/1p6/P7/8/8/7K w - - 0 1", 5, 6112, perftType);
		result = result && checkPerft("7k/8/8/1p6/P7/8/8/7K w - - 0 1", 6, 41874, perftType);
		result = result && checkPerft("7k/8/1p6/8/8/P7/8/7K w - - 0 1", 1, 4, perftType);
		result = result && checkPerft("7k/8/1p6/8/8/P7/8/7K w - - 0 1", 2, 16, perftType);
		result = result && checkPerft("7k/8/1p6/8/8/P7/8/7K w - - 0 1", 3, 101, perftType);
		result = result && checkPerft("7k/8/1p6/8/8/P7/8/7K w - - 0 1", 4, 637, perftType);
		result = result && checkPerft("7k/8/1p6/8/8/P7/8/7K w - - 0 1", 5, 4354, perftType);
		result = result && checkPerft("7k/8/1p6/8/8/P7/8/7K w - - 0 1", 6, 29679, perftType);
		result = result && checkPerft("k7/7p/8/8/8/8/6P1/K7 w - - 0 1", 1, 5, perftType);
		result = result && checkPerft("k7/7p/8/8/8/8/6P1/K7 w - - 0 1", 2, 25, perftType);
		result = result && checkPerft("k7/7p/8/8/8/8/6P1/K7 w - - 0 1", 3, 161, perftType);
		result = result && checkPerft("k7/7p/8/8/8/8/6P1/K7 w - - 0 1", 4, 1035, perftType);
		result = result && checkPerft("k7/7p/8/8/8/8/6P1/K7 w - - 0 1", 5, 7574, perftType);
		result = result && checkPerft("k7/7p/8/8/8/8/6P1/K7 w - - 0 1", 6, 55338, perftType);
		result = result && checkPerft("k7/6p1/8/8/8/8/7P/K7 w - - 0 1", 1, 5, perftType);
		result = result && checkPerft("k7/6p1/8/8/8/8/7P/K7 w - - 0 1", 2, 25, perftType);
		result = result && checkPerft("k7/6p1/8/8/8/8/7P/K7 w - - 0 1", 3, 161, perftType);
		result = result && checkPerft("k7/6p1/8/8/8/8/7P/K7 w - - 0 1", 4, 1035, perftType);
		result = result && checkPerft("k7/6p1/8/8/8/8/7P/K7 w - - 0 1", 5, 7574, perftType);
		result = result && checkPerft("k7/6p1/8/8/8/8/7P/K7 w - - 0 1", 6, 55338, perftType);
		result = result && checkPerft("3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1", 1, 7, perftType);
		result = result && checkPerft("3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1", 2, 49, perftType);
		result = result && checkPerft("3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1", 3, 378, perftType);
		result = result && checkPerft("3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1", 4, 2902, perftType);
		result = result && checkPerft("3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1", 5, 24122, perftType);
		result = result && checkPerft("3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1", 6, 199002, perftType);
		result = result && checkPerft("7k/8/8/p7/1P6/8/8/7K b - - 0 1", 1, 5, perftType);
		result = result && checkPerft("7k/8/8/p7/1P6/8/8/7K b - - 0 1", 2, 22, perftType);
		result = result && checkPerft("7k/8/8/p7/1P6/8/8/7K b - - 0 1", 3, 139, perftType);
		result = result && checkPerft("7k/8/8/p7/1P6/8/8/7K b - - 0 1", 4, 877, perftType);
		result = result && checkPerft("7k/8/8/p7/1P6/8/8/7K b - - 0 1", 5, 6112, perftType);
		result = result && checkPerft("7k/8/8/p7/1P6/8/8/7K b - - 0 1", 6, 41874, perftType);
		result = result && checkPerft("7k/8/p7/8/8/1P6/8/7K b - - 0 1", 1, 4, perftType);
		result = result && checkPerft("7k/8/p7/8/8/1P6/8/7K b - - 0 1", 2, 16, perftType);
		result = result && checkPerft("7k/8/p7/8/8/1P6/8/7K b - - 0 1", 3, 101, perftType);
		result = result && checkPerft("7k/8/p7/8/8/1P6/8/7K b - - 0 1", 4, 637, perftType);
		result = result && checkPerft("7k/8/p7/8/8/1P6/8/7K b - - 0 1", 5, 4354, perftType);
		result = result && checkPerft("7k/8/p7/8/8/1P6/8/7K b - - 0 1", 6, 29679, perftType);
		result = result && checkPerft("7k/8/8/1p6/P7/8/8/7K b - - 0 1", 1, 5, perftType);
		result = result && checkPerft("7k/8/8/1p6/P7/8/8/7K b - - 0 1", 2, 22, perftType);
		result = result && checkPerft("7k/8/8/1p6/P7/8/8/7K b - - 0 1", 3, 139, perftType);
		result = result && checkPerft("7k/8/8/1p6/P7/8/8/7K b - - 0 1", 4, 877, perftType);
		result = result && checkPerft("7k/8/8/1p6/P7/8/8/7K b - - 0 1", 5, 6112, perftType);
		result = result && checkPerft("7k/8/8/1p6/P7/8/8/7K b - - 0 1", 6, 41874, perftType);
		result = result && checkPerft("7k/8/1p6/8/8/P7/8/7K b - - 0 1", 1, 4, perftType);
		result = result && checkPerft("7k/8/1p6/8/8/P7/8/7K b - - 0 1", 2, 16, perftType);
		result = result && checkPerft("7k/8/1p6/8/8/P7/8/7K b - - 0 1", 3, 101, perftType);
		result = result && checkPerft("7k/8/1p6/8/8/P7/8/7K b - - 0 1", 4, 637, perftType);
		result = result && checkPerft("7k/8/1p6/8/8/P7/8/7K b - - 0 1", 5, 4354, perftType);
		result = result && checkPerft("7k/8/1p6/8/8/P7/8/7K b - - 0 1", 6, 29679, perftType);
		result = result && checkPerft("k7/7p/8/8/8/8/6P1/K7 b - - 0 1", 1, 5, perftType);
		result = result && checkPerft("k7/7p/8/8/8/8/6P1/K7 b - - 0 1", 2, 25, perftType);
		result = result && checkPerft("k7/7p/8/8/8/8/6P1/K7 b - - 0 1", 3, 161, perftType);
		result = result && checkPerft("k7/7p/8/8/8/8/6P1/K7 b - - 0 1", 4, 1035, perftType);
		result = result && checkPerft("k7/7p/8/8/8/8/6P1/K7 b - - 0 1", 5, 7574, perftType);
		result = result && checkPerft("k7/7p/8/8/8/8/6P1/K7 b - - 0 1", 6, 55338, perftType);
		result = result && checkPerft("k7/6p1/8/8/8/8/7P/K7 b - - 0 1", 1, 5, perftType);
		result = result && checkPerft("k7/6p1/8/8/8/8/7P/K7 b - - 0 1", 2, 25, perftType);
		result = result && checkPerft("k7/6p1/8/8/8/8/7P/K7 b - - 0 1", 3, 161, perftType);
		result = result && checkPerft("k7/6p1/8/8/8/8/7P/K7 b - - 0 1", 4, 1035, perftType);
		result = result && checkPerft("k7/6p1/8/8/8/8/7P/K7 b - - 0 1", 5, 7574, perftType);
		result = result && checkPerft("k7/6p1/8/8/8/8/7P/K7 b - - 0 1", 6, 55338, perftType);
		result = result && checkPerft("3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1", 1, 7, perftType);
		result = result && checkPerft("3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1", 2, 49, perftType);
		result = result && checkPerft("3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1", 3, 378, perftType);
		result = result && checkPerft("3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1", 4, 2902, perftType);
		result = result && checkPerft("3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1", 5, 24122, perftType);
		result = result && checkPerft("3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1", 6, 199002, perftType);
		result = result && checkPerft("8/Pk6/8/8/8/8/6Kp/8 w - - 0 1", 1, 11, perftType);
		result = result && checkPerft("8/Pk6/8/8/8/8/6Kp/8 w - - 0 1", 2, 97, perftType);
		result = result && checkPerft("8/Pk6/8/8/8/8/6Kp/8 w - - 0 1", 3, 887, perftType);
		result = result && checkPerft("8/Pk6/8/8/8/8/6Kp/8 w - - 0 1", 4, 8048, perftType);
		result = result && checkPerft("8/Pk6/8/8/8/8/6Kp/8 w - - 0 1", 5, 90606, perftType);
		result = result && checkPerft("8/Pk6/8/8/8/8/6Kp/8 w - - 0 1", 6, 1030499, perftType);
		result = result && checkPerft("n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1", 1, 24, perftType);
		result = result && checkPerft("n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1", 2, 421, perftType);
		result = result && checkPerft("n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1", 3, 7421, perftType);
		result = result && checkPerft("n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1", 4, 124608, perftType);
		result = result && checkPerft("n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1", 5, 2193768, perftType);
		result = result && checkPerft("n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1", 6, 37665329, perftType);
		result = result && checkPerft("8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1", 1, 18, perftType);
		result = result && checkPerft("8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1", 2, 270, perftType);
		result = result && checkPerft("8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1", 3, 4699, perftType);
		result = result && checkPerft("8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1", 4, 79355, perftType);
		result = result && checkPerft("8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1", 5, 1533145, perftType);
		result = result && checkPerft("8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1", 6, 28859283, perftType);
		result = result && checkPerft("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1", 1, 24, perftType);
		result = result && checkPerft("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1", 2, 496, perftType);
		result = result && checkPerft("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1", 3, 9483, perftType);
		result = result && checkPerft("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1", 4, 182838, perftType);
		result = result && checkPerft("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1", 5, 3605103, perftType);
		result = result && checkPerft("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1", 6, 71179139, perftType);
		result = result && checkPerft("8/Pk6/8/8/8/8/6Kp/8 b - - 0 1", 1, 11, perftType);
		result = result && checkPerft("8/Pk6/8/8/8/8/6Kp/8 b - - 0 1", 2, 97, perftType);
		result = result && checkPerft("8/Pk6/8/8/8/8/6Kp/8 b - - 0 1", 3, 887, perftType);
		result = result && checkPerft("8/Pk6/8/8/8/8/6Kp/8 b - - 0 1", 4, 8048, perftType);
		result = result && checkPerft("8/Pk6/8/8/8/8/6Kp/8 b - - 0 1", 5, 90606, perftType);
		result = result && checkPerft("8/Pk6/8/8/8/8/6Kp/8 b - - 0 1", 6, 1030499, perftType);
		result = result && checkPerft("n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1", 1, 24, perftType);
		result = result && checkPerft("n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1", 2, 421, perftType);
		result = result && checkPerft("n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1", 3, 7421, perftType);
		result = result && checkPerft("n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1", 4, 124608, perftType);
		result = result && checkPerft("n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1", 5, 2193768, perftType);
		result = result && checkPerft("n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1", 6, 37665329, perftType);
		result = result && checkPerft("8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1", 1, 18, perftType);
		result = result && checkPerft("8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1", 2, 270, perftType);
		result = result && checkPerft("8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1", 3, 4699, perftType);
		result = result && checkPerft("8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1", 4, 79355, perftType);
		result = result && checkPerft("8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1", 5, 1533145, perftType);
		result = result && checkPerft("8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1", 6, 28859283, perftType);
		result = result && checkPerft("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 1, 24, perftType);
		result = result && checkPerft("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 2, 496, perftType);
		result = result && checkPerft("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 3, 9483, perftType);
		result = result && checkPerft("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 4, 182838, perftType);
		result = result && checkPerft("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 5, 3605103, perftType);
		result = result && checkPerft("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 6, 71179139, perftType);
		if (result)std::cout << "Done OK" << std::endl; else std::cout << "Error!" << std::endl;
		std::cout << "Runtime: " << std::setprecision(3) << perftRuntime / 1000.0 << " s" << std::endl;
		std::cout << "Count:   " << std::setprecision(3) << perftNodes / 1000000.0 << " MNodes" << std::endl;
		std::cout << "Leafs: " << std::setprecision(3) << (1.0 * perftNodes) / perftRuntime / 1000 << " MNodes/s" << std::endl;
		std::cout << "Nodes: " << std::setprecision(3) << (1.0 * nodeCount) / perftRuntime / 1000 << " MNodes/s" << std::endl;
		return result;
	}

	bool testSEE() {
		std::vector<std::string> tests = {
			"r6r/p1p1qpb1/Q4np1/8/4k3/2B4p/PPP1KPPP/R6R w - - 0 1;a6d3;0",
			"8/8/2Kp4/8/3k4/8/8/8 w - - 0 1;c6d6;80",
			"8/8/2Kp4/8/3k4/6b1/8/8 w - - 0 1;c6d6;-9920",
			"3k4/4q3/4r3/4r3/4Q3/4R3/4R3/3K4 w - - 0 1;e4e5;30",
			"3k4/4q3/4r3/4r3/4Q3/4R3/4R3/3K4 w - - 0 1;e5e4;950",
			"3k4/4q3/4R3/4r3/4Q3/4r3/4R3/3K4 w - - 0 1;e6e7;470"
		};
		bool result = true;
		for (int i = 0; i < int(tests.size()); i++) {
			auto tokens = utils::split(tests[i], ';');
			assert(tokens.size() == 3);
			Position pos(tokens[0]);
			Move move = parseMoveInUCINotation(tokens[1], pos);
			std::cout << tokens[0] << std::endl;
			Value vsee = pos.SEE(move);
			bool lresult = vsee == std::stoi(tokens[2]);
			result = result && lresult;
			if (!lresult) std::cout << "ERROR ";
			std::cout << tokens[1] << "\t" << (int)vsee << "\t" << tokens[2] << std::endl;
		}
		return result;
	}

	std::vector<std::string> readTextFile(std::string file) {
		std::vector<std::string> lines;
		std::ifstream s;
		s.open(file);
		std::string line;
		if (s.is_open())
		{
			long lineCount = 0;
			while (s)
			{
				std::getline(s, line);
				lines.push_back(line);
				lineCount++;
				if ((lineCount & 8191) == 0)std::cout << ".";
			}
			s.close();
			std::cout << std::endl;
			std::cout << lineCount << " lines read!" << std::endl;
		}
		else
		{
			std::cout << "Unable to open file" << std::endl;
		}
		return lines;
	}

}