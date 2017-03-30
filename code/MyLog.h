#pragma once

#include <stdio.h>

namespace FancyTech
{

class Log
{
public:
	Log();
	~Log();

	void LogConsole(const char* str, ...);
	void LogInfo(const char* str, ...);
	void LogError(const char* str, ...);

private:
	struct LogImpl* impl_;
};

extern Log GLog;

}
