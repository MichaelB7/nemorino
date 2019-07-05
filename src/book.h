#pragma once
#include <fstream>
#include <string>
#include "types.h"
#include "position.h"
#include "utils.h"

namespace polyglot {

	//A Polyglot book is a series of "entries" of 16 bytes
	//All integers are stored highest byte first(regardless of size)
	//The entries are ordered according to key.Lowest key first.
	struct Entry {
		uint64_t key = 0;
		uint16_t move = MOVE_NONE;
		uint16_t weight = 0;
		uint32_t learn = 0;
	};

	class Book : private std::ifstream
	{
	public:
		Book();
		explicit Book(const std::string& filename);
		~Book();

		Move probe(Position& pos, bool pickBest, ValuatedMove * moves, int moveCount);

	private:
		std::string fileName = "book.bin";
		size_t count;
		Entry read();
	};

}
