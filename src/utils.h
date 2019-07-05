#pragma once
#include <string>
#include <vector>

//copied from SF
enum SyncCout { IO_LOCK, IO_UNLOCK };
std::ostream& operator<<(std::ostream&, SyncCout);

#define sync_cout std::cout << IO_LOCK
#define sync_endl std::endl << IO_UNLOCK

/// Under Windows it is not possible for a process to run on more than one
/// logical processor group. This usually means to be limited to use max 64
/// cores. To overcome this, some special platform specific API should be
/// called to set group affinity for each thread. Original code from Texel by
/// Peter Österlund.
namespace WinProcGroup {
	void bindThisThread(size_t idx);
}

namespace utils {

	void debugInfo(std::string info);
	void debugInfo(std::string info1, std::string info2);
	std::vector<std::string> split(const std::string str, char sep = ' ');
	void replaceExt(std::string& s, const std::string& newExt);
	std::string TrimLeft(const std::string& s);
	std::string TrimRight(const std::string& s);
	std::string Trim(const std::string& s);

	uint64_t MurmurHash2A(uint64_t input, uint64_t seed = 0x4f4863d5038ea3a3);

	inline uint64_t Hash(uint64_t input) { return input * 14695981039346656037ull; }

	inline std::string bool2String(bool val) { if (val) return "true"; else return "false"; }

	template <class T> T clamp(T value, T lowerBound, T upperBound) { return std::max(lowerBound, std::min(value, upperBound)); }

	extern double K;
	inline double sigmoid(Value score) { return 1 / (1 + std::pow(10, K * (int)score)); }

	inline double interpolatedResult(double result, int ply, int totalPlies) {
		return 0.5 + ((result - 0.5) * ply) / totalPlies;
	}
	inline double winExpectation(Value score, double scale = 1.305) { return 1 / (1 + std::pow(10, -scale / 400 * (int)score)); }

	std::string mirrorFenVertical(std::string fen);

	double TexelTuneError(const char* argv[], int argc);

	double TexelTuneError(std::string data, std::string parameter);


#define SCORE(score) score
#define SCORE_MDP(score) score
#define SCORE_TT(score) score
#define SCORE_TB(score) score
#define SCORE_RAZ(score) score
#define SCORE_BP(score) score
#define SCORE_NMP(score) score
#define SCORE_PC(score) score
#define SCORE_BC(score) score
#define SCORE_EXACT(score) score
#define SCORE_FINAL(score) score
#define SCORE_SP(score) score
#define SCORE_DP(score) score

}


