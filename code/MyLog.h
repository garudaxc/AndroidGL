#pragma once

#include <stdio.h>

class Mutex;

class Log
{
public:
	Log();
	~Log();

	void LogConsole(const char* str, ...);
	void LogInfo(const char* str, ...);
	void LogError(const char* str, ...);
private:

	FILE* pfLog;
	Mutex* mutex_;
};

extern Log GLog;
