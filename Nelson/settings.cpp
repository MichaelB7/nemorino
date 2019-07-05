#include <iostream>
#include <fstream>
#include <algorithm>
#include <regex>
#include <thread>
#include "settings.h"
#include "utils.h"
#include "hashtables.h"
#include "material.h"

namespace settings {

	Options options;
	Parameters parameter;

	Parameters::Parameters()
	{
		Initialize();
	}

	void Parameters::Initialize() {
		if (HelperThreads == 0) {
			HelperThreads = std::max(0, (int)std::thread::hardware_concurrency() - 1);
			static_cast<OptionSpin *>(options[OPTION_THREADS])->set(HelperThreads + 1);
			static_cast<OptionSpin *>(options[OPTION_THREADS])->setDefault(HelperThreads + 1);
		}
		for (int depth = 1; depth < 64; depth++) {
			for (int moves = 1; moves < 64; moves++) {
				double reduction = std::log(moves) * std::log(depth) / 2; //F
				if (reduction < 0.8) LMR_REDUCTION[depth][moves] = 0;
				else LMR_REDUCTION[depth][moves] = int(std::round(reduction));
				assert(LMR_REDUCTION[depth][moves] < depth);
			}
		}
		LMR_REDUCTION[0][0] = 0;
		LMR_REDUCTION[1][0] = 0;
		LMR_REDUCTION[0][1] = 0;
	}

	int Parameters::LMRReduction(int depth, int moveNumber)
	{
		return LMR_REDUCTION[std::min(depth, 63)][std::min(moveNumber, 63)];
	}

	void Parameters::UCIExpose()
	{
		sync_cout << "option name BETA_PRUNING_FACTOR type spin default " << BETA_PRUNING_FACTOR << sync_endl;
		sync_cout << "option name PAWN_SHELTER_2ND_RANK_MG type spin default " << PAWN_SHELTER_2ND_RANK.mgScore << sync_endl;
		sync_cout << "option name PAWN_SHELTER_2ND_RANK_EG type spin default " << PAWN_SHELTER_2ND_RANK.egScore << sync_endl;
		sync_cout << "option name PAWN_SHELTER_3RD_RANK_MG type spin default " << PAWN_SHELTER_3RD_RANK.mgScore << sync_endl;
		sync_cout << "option name PAWN_SHELTER_3RD_RANK_EG type spin default " << PAWN_SHELTER_3RD_RANK.egScore << sync_endl;
		sync_cout << "option name BETA_PRUNING_FACTOR type spin default " << BETA_PRUNING_FACTOR << sync_endl;
		for (int i = 0; i < 4; ++i) sync_cout << "option name SAFE_CHECK_" << i << " type spin default " << SAFE_CHECK[i] << sync_endl;
		for (int i = 0; i < 4; ++i) sync_cout << "option name ATTACK_WEIGHT_" << i << " type spin default " << ATTACK_WEIGHT[i] << sync_endl;
		for (int i = 0; i < 6; ++i) sync_cout << "option name MALUS_BLOCKED_" << i << " type spin default " << MALUS_BLOCKED[i].mgScore << sync_endl;
		for (int i = 0; i < 6; ++i) sync_cout << "option name PASSED_PAWN_BONUS_" << i << " type spin default " << PASSED_PAWN_BONUS[i].mgScore << sync_endl;
		for (int i = 0; i < 6; ++i) sync_cout << "option name BONUS_PROTECTED_PASSED_PAWN_" << i << " type spin default " << BONUS_PROTECTED_PASSED_PAWN[i].mgScore << sync_endl;
		sync_cout << "option name KING_RING_ATTACK_FACTOR type spin default " << KING_RING_ATTACK_FACTOR << sync_endl;
		sync_cout << "option name WEAK_SQUARES_FACTOR type spin default " << WEAK_SQUARES_FACTOR << sync_endl;
		sync_cout << "option name PINNED_FACTOR type spin default " << PINNED_FACTOR << sync_endl;
		sync_cout << "option name ATTACK_WITH_QUEEN type spin default " << ATTACK_WITH_QUEEN << sync_endl;
		for (PieceType pt = PieceType::QUEEN; pt <= PieceType::PAWN; ++pt) {
			sync_cout << "option name PIECEVAL_MG_" << static_cast<int>(pt) << " type spin default " << static_cast<int>(PieceValues[pt].mgScore) << sync_endl;
			sync_cout << "option name PIECEVAL_EG_" << static_cast<int>(pt) << " type spin default " << static_cast<int>(PieceValues[pt].egScore) << sync_endl;
		}
		sync_cout << "option name HANGING_MG type spin default " << static_cast<int>(HANGING.mgScore) << sync_endl;
		sync_cout << "option name HANGING_EG type spin default " << static_cast<int>(HANGING.egScore) << sync_endl;
		const int mobilitySize[4] = { 28, 15, 14, 9 };
		for (int mcount = 0; mcount < mobilitySize[static_cast<int>(QUEEN)]; ++mcount) {
			sync_cout << "option name MOBILITY_Q_MG_" << mcount << " type spin default " << static_cast<int>(parameter.MOBILITY_BONUS_QUEEN[mcount].mgScore) << sync_endl;
			sync_cout << "option name MOBILITY_Q_EG_" << mcount << " type spin default " << static_cast<int>(parameter.MOBILITY_BONUS_QUEEN[mcount].egScore) << sync_endl;
		}
		for (int mcount = 0; mcount < mobilitySize[static_cast<int>(ROOK)]; ++mcount) {
			sync_cout << "option name MOBILITY_R_MG_" << mcount << " type spin default " << static_cast<int>(parameter.MOBILITY_BONUS_ROOK[mcount].mgScore) << sync_endl;
			sync_cout << "option name MOBILITY_R_EG_" << mcount << " type spin default " << static_cast<int>(parameter.MOBILITY_BONUS_ROOK[mcount].egScore) << sync_endl;
		}
		for (int mcount = 0; mcount < mobilitySize[static_cast<int>(BISHOP)]; ++mcount) {
			sync_cout << "option name MOBILITY_B_MG_" << mcount << " type spin default " << static_cast<int>(parameter.MOBILITY_BONUS_BISHOP[mcount].mgScore) << sync_endl;
			sync_cout << "option name MOBILITY_B_EG_" << mcount << " type spin default " << static_cast<int>(parameter.MOBILITY_BONUS_BISHOP[mcount].egScore) << sync_endl;
		}
		for (int mcount = 0; mcount < mobilitySize[static_cast<int>(KNIGHT)]; ++mcount) {
			sync_cout << "option name MOBILITY_N_MG_" << mcount << " type spin default " << static_cast<int>(parameter.MOBILITY_BONUS_KNIGHT[mcount].mgScore) << sync_endl;
			sync_cout << "option name MOBILITY_N_EG_" << mcount << " type spin default " << static_cast<int>(parameter.MOBILITY_BONUS_KNIGHT[mcount].egScore) << sync_endl;
		}
		sync_cout << "option name LEVER_ON_KING type spin default " << BONUS_LEVER_ON_KINGSIDE << sync_endl;
		sync_cout << "option name KING_ON_ONE_MG type spin default " << KING_ON_ONE.mgScore << sync_endl;
		sync_cout << "option name KING_ON_ONE_EG type spin default " << KING_ON_ONE.egScore << sync_endl;
		sync_cout << "option name KING_ON_MANY_MG type spin default " << KING_ON_MANY.mgScore << sync_endl;
		sync_cout << "option name KING_ON_MANY_EG type spin default " << KING_ON_MANY.egScore << sync_endl;
		sync_cout << "option name BONUS_BISHOP_PAIR type spin default " << BONUS_BISHOP_PAIR.egScore << sync_endl;
		sync_cout << "option name MALUS_BACKWARD_PAWN type spin default " << MALUS_BACKWARD_PAWN.egScore << sync_endl;

	}

	void Parameters::SetFromUCI(std::string name, std::string value)
	{
		if (!name.compare("KING_DANGER_SCALE")) KING_DANGER_SCALE = stoi(value);	
		else if (!name.compare("PAWN_SHELTER_2ND_RANK_EG")) PAWN_SHELTER_2ND_RANK.egScore = static_cast<Value>(stoi(value));
		else if (!name.compare("PAWN_SHELTER_2ND_RANK_MG")) PAWN_SHELTER_2ND_RANK.mgScore = static_cast<Value>(stoi(value));
		else if (!name.compare("PAWN_SHELTER_3RD_RANK_EG")) PAWN_SHELTER_3RD_RANK.egScore = static_cast<Value>(stoi(value));
		else if (!name.compare("PAWN_SHELTER_3RD_RANK_MG")) PAWN_SHELTER_3RD_RANK.mgScore = static_cast<Value>(stoi(value));
		else if (!name.compare("BETA_PRUNING_FACTOR")) BETA_PRUNING_FACTOR = static_cast<Value>(stoi(value));
		else if (!name.compare("KING_RING_ATTACK_FACTOR")) KING_RING_ATTACK_FACTOR = stoi(value);
		else if (!name.compare("WEAK_SQUARES_FACTOR")) WEAK_SQUARES_FACTOR = stoi(value);
		else if (!name.compare("PINNED_FACTOR")) PINNED_FACTOR = stoi(value);
		else if (!name.compare("ATTACK_WITH_QUEEN")) ATTACK_WITH_QUEEN = stoi(value);
		else if (!name.compare("LEVER_ON_KING")) BONUS_LEVER_ON_KINGSIDE = stoi(value);
		else if (!name.compare("MALUS_BACKWARD_PAWN")) MALUS_BACKWARD_PAWN = Eval(stoi(value));
		else if (name.find("SAFE_CHECK_") == 0) {
			int index = stoi(name.substr(11, std::string::npos));
			SAFE_CHECK[index] = stoi(value);
		}
		else if (name.find("ATTACK_WEIGHT_") == 0) {
			int index = stoi(name.substr(14, std::string::npos));
			ATTACK_WEIGHT[index] = stoi(value);
		}
		else if (name.find("MALUS_BLOCKED_") == 0) {
			int index = stoi(name.substr(14, std::string::npos));
			MALUS_BLOCKED[index] = Eval(stoi(value));
		}
		else if (name.find("PASSED_PAWN_BONUS_") == 0) {
			int index = stoi(name.substr(18, std::string::npos));
			PASSED_PAWN_BONUS[index] = Eval(stoi(value));
		}
		else if (name.find("BONUS_PROTECTED_PASSED_PAWN_") == 0) {
			int index = stoi(name.substr(28, std::string::npos));
			BONUS_PROTECTED_PASSED_PAWN[index] = Eval(stoi(value));
		}
		else if (name.find("PIECEVAL_MG_") == 0) {
			int index = stoi(name.substr(12, std::string::npos));
			PieceValues[index].mgScore = static_cast<Value>(stoi(value));
			InitializeMaterialTable();
		}
		else if (name.find("PIECEVAL_EG_") == 0) {
			int index = stoi(name.substr(12, std::string::npos));
			PieceValues[index].egScore = static_cast<Value>(stoi(value));
			InitializeMaterialTable();
		}
		else if (!name.compare("BONUS_BISHOP_PAIR")) {
			BONUS_BISHOP_PAIR = Eval(static_cast<Value>(stoi(value)));
			InitializeMaterialTable();
		}
		else if (!name.compare("HANGING_EG")) HANGING.egScore = static_cast<Value>(stoi(value));
		else if (!name.compare("HANGING_MG")) HANGING.mgScore = static_cast<Value>(stoi(value));
		else if (!name.compare("KING_ON_ONE_EG")) KING_ON_ONE.egScore = static_cast<Value>(stoi(value));
		else if (!name.compare("KING_ON_ONE_MG")) KING_ON_ONE.mgScore = static_cast<Value>(stoi(value));
		else if (!name.compare("KING_ON_MANY_EG")) KING_ON_MANY.egScore = static_cast<Value>(stoi(value));
		else if (!name.compare("KING_ON_MANY_MG")) KING_ON_MANY.mgScore = static_cast<Value>(stoi(value));
		else if (name.find("MOBILITY_") == 0) {
			char pt = name[9];
			int index = stoi(name.substr(14, std::string::npos));
			bool isMg = name[11] == 'M';
			switch (pt)
			{
			case 'Q':
				if (isMg) parameter.MOBILITY_BONUS_QUEEN[index].mgScore = static_cast<Value>(stoi(value)); else parameter.MOBILITY_BONUS_QUEEN[index].egScore = static_cast<Value>(stoi(value));
				break;
			case 'R':
				if (isMg) parameter.MOBILITY_BONUS_ROOK[index].mgScore = static_cast<Value>(stoi(value)); else parameter.MOBILITY_BONUS_ROOK[index].egScore = static_cast<Value>(stoi(value));
				break;
			case 'B':
				if (isMg) parameter.MOBILITY_BONUS_BISHOP[index].mgScore = static_cast<Value>(stoi(value)); else parameter.MOBILITY_BONUS_BISHOP[index].egScore = static_cast<Value>(stoi(value));
				break;
			case 'N':
				if (isMg) parameter.MOBILITY_BONUS_KNIGHT[index].mgScore = static_cast<Value>(stoi(value)); else parameter.MOBILITY_BONUS_KNIGHT[index].egScore = static_cast<Value>(stoi(value));
				break;
			default:
				break;
			}
		}
	}

	void Parameters::setParam(std::string key, std::string value)
	{
		if (key.find("PIECEVAL_MG_") == 0) {
			int index = std::stoi(key.substr(12, std::string::npos));
			PieceValues[index].mgScore = static_cast<Value>(stoi(value));
		}
		else if (key.find("PIECEVAL_EG_") == 0) {
			int index = std::stoi(key.substr(12, std::string::npos));
			PieceValues[index].egScore = static_cast<Value>(stoi(value));
		}
		else if (key.find("MOB_QUEEN_MG_") == 0) {
			int index = std::stoi(key.substr(13, std::string::npos));
			MOBILITY_BONUS_QUEEN[index].mgScore = static_cast<Value>(stoi(value));
		}
		else if (key.find("MOB_QUEEN_EG_") == 0) {
			int index = std::stoi(key.substr(13, std::string::npos));
			MOBILITY_BONUS_QUEEN[index].egScore = static_cast<Value>(stoi(value));
		}
	}

	Option::Option(std::string Name, OptionType Type, std::string DefaultValue, std::string MinValue, std::string MaxValue, bool Technical)
	{
		name = Name;
		otype = Type;
		defaultValue = DefaultValue;
		maxValue = MaxValue;
		minValue = MinValue;
		technical = Technical;
	}

	std::string Option::printUCI()
	{
		std::stringstream ss;
		ss << "option name " << name << " type ";
		switch (otype)
		{
		case OptionType::BUTTON:
			ss << "button";
			return ss.str();
		case OptionType::CHECK:
			ss << "check";
			if (defaultValue.size() > 0) ss << " default " << defaultValue;
			return ss.str();
		case OptionType::STRING:
			ss << "string";
			if (defaultValue.size() > 0) ss << " default " << defaultValue;
			return ss.str();
		case OptionType::SPIN:
			ss << "spin";
			if (defaultValue.size() > 0) ss << " default " << defaultValue;
			if (minValue.size() > 0) ss << " min " << minValue;
			if (maxValue.size() > 0) ss << " max " << maxValue;
			return ss.str();
		default:
			break;
		}
		return ss.str();
	}

	std::string Option::printInfo() const
	{
		std::stringstream ss;
		ss << "info string " << name << " ";
		switch (otype)
		{
		case OptionType::CHECK:
			ss << ((OptionCheck *)this)->getValue();
			return ss.str();
		case OptionType::STRING:
			ss << ((OptionString *)this)->getValue();
			return ss.str();
		case OptionType::SPIN:
			ss << ((OptionSpin *)this)->getValue();
			return ss.str();
		default:
			break;
		}
		return ss.str();
	}

	void OptionThread::set(std::string value)
	{
		_value = stoi(value);
		parameter.HelperThreads = _value - 1;
	}

	void Option960::set(std::string value)
	{
		Chess960 = !value.compare("true");
	}

	Options::Options()
	{
		initialize();
	}

	Options::~Options()
	{
		for (std::map<std::string, Option *>::iterator itr = this->begin(); itr != this->end(); itr++)
		{
			delete itr->second;
		}
	}




	void Options::printUCI()
	{
		for (auto it = begin(); it != end(); ++it) {
			if (!it->second->isTechnical())
				sync_cout << it->second->printUCI() << sync_endl;
		}
	}

	void Options::printInfo()
	{
		for (auto it = begin(); it != end(); ++it) {
			sync_cout << it->second->printInfo() << sync_endl;
		}
	}

	void Options::read(std::vector<std::string> &tokens)
	{
		if (find(tokens[2]) == end()) return;
		at(tokens[2])->read(tokens);
	}

	int Options::getInt(std::string key)
	{
		return ((OptionSpin *)at(key))->getValue();
	}

	bool Options::getBool(std::string key)
	{
		return ((OptionCheck *)at(key))->getValue();
	}

	std::string Options::getString(std::string key)
	{
		return find(key) == end() ? "" : ((OptionString *)at(key))->getValue();
	}

	void Options::set(std::string key, std::string value)
	{
		if (find(key) == end()) {
			(*this)[key] = (Option *)(new OptionString(key));
		}
		((OptionString *)at(key))->set(value);
	}

	void Options::set(std::string key, int value)
	{
		((OptionSpin *)at(key))->set(value);
	}

	void Options::set(std::string key, bool value)
	{
		((OptionCheck *)at(key))->set(value);
	}

	void Options::initialize()
	{
		(*this)[OPTION_CHESS960] = (Option *)(new Option960());
		(*this)[OPTION_HASH] = (Option *)(new OptionHash());
		(*this)[OPTION_CLEAR_HASH] = (Option *)(new OptionButton(OPTION_CLEAR_HASH));
		(*this)[OPTION_PRINT_OPTIONS] = (Option *)(new OptionButton(OPTION_PRINT_OPTIONS, true));
		(*this)[OPTION_MULTIPV] = (Option *)(new OptionSpin(OPTION_MULTIPV, 1, 1, 216));
		(*this)[OPTION_THREADS] = (Option *)(new OptionThread());
		(*this)[OPTION_PONDER] = (Option *)(new OptionCheck(OPTION_PONDER, false));
		(*this)[OPTION_CONTEMPT] = (Option *)(new OptionContempt());
		(*this)[OPTION_BOOK_FILE] = (Option *)(new OptionString(OPTION_BOOK_FILE, "book.bin"));
		(*this)[OPTION_OWN_BOOK] = (Option *)(new OptionCheck(OPTION_OWN_BOOK, false));
		(*this)[OPTION_OPPONENT] = (Option *)(new OptionString(OPTION_OPPONENT));
		(*this)[OPTION_EMERGENCY_TIME] = (Option *)(new OptionSpin(OPTION_EMERGENCY_TIME, 0, 0, 60000));
		(*this)[OPTION_NODES_TIME] = (Option *)(new OptionSpin(OPTION_NODES_TIME, 0, 0, INT_MAX, true));
		(*this)[OPTION_SYZYGY_PATH] = (Option *)(new OptionString(OPTION_SYZYGY_PATH));
		(*this)[OPTION_SYZYGY_PROBE_DEPTH] = (Option *)(new OptionSpin(OPTION_SYZYGY_PROBE_DEPTH, parameter.TBProbeDepth, 0, MAX_DEPTH + 1));
	}

	OptionCheck::OptionCheck(std::string Name, bool value, bool Technical)
	{
		name = Name;
		otype = OptionType::CHECK;
		defaultValue = utils::bool2String(value);
		_value = value;
		maxValue = "";
		minValue = "";
		technical = Technical;
	}

	void OptionCheck::set(std::string value)
	{
		_value = !value.compare("true");
	}

	void OptionContempt::set(std::string value)
	{
		_value = stoi(value);
		parameter.Contempt = Value(_value);
	}

	void OptionContempt::set(int value)
	{
		_value = value;
		parameter.Contempt = Value(_value);
	}

	void OptionString::read(std::vector<std::string>& tokens)
	{
		if ((int)tokens.size() > 5) {
			std::stringstream ss;
			ss << tokens[4];
			for (int i = 5; i < (int)tokens.size(); ++i) {
				ss << " " << tokens[i];
			}
			_value = ss.str();
		}
		else if ((int)tokens.size() == 5) _value = tokens[4];
		else _value = "";
	}

	void OptionHash::set(std::string value)
	{
		_value = stoi(value);
		tt::InitializeTranspositionTable();
	}

	void OptionHash::set(int value)
	{
		_value = value;
		tt::InitializeTranspositionTable();
	}
}
