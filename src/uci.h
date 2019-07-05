#pragma once
#include <string>
#include <vector>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <atomic>
#include "search.h"

class UCIInterface {
public:
	void loop();

private:
	Search * Engine = new Search;
	Position * _position = nullptr;
	int64_t ponderStartTime = 0;
	bool ponderActive = false;
	bool initialized = false;

	bool dispatch(std::string line);

	std::condition_variable cvStartEngine;
	std::mutex mtxEngineRunning;
	std::thread main_thread;
	std::atomic<bool> engine_active{ false };
	std::atomic<bool> exiting{ false };

	// UCI command handlers
	void uci();
	void isready();
	void setoption(std::vector<std::string> &tokens);
	void ucinewgame();
	void setPosition(std::vector<std::string> &tokens);
	void go(std::vector<std::string> &tokens);
	void perft(std::vector<std::string> &tokens);
	void divide(std::vector<std::string> &tokens);
	void setvalue(std::vector<std::string> &tokens);
	void quit();
	void stop();
	void thinkAsync();
	void ponderhit();
	void deleteThread();
	void see(std::vector<std::string> &tokens);
	void qscore(std::vector<std::string> &tokens);
	void dumpTT(std::vector<std::string> &tokens);
	void updateFromOptions();
	void copySettings(Search * source, Search * destination);
};

