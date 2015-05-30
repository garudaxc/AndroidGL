#include "MyLog.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

Log::Log() :pfLog(NULL){
	const char* filename = "log.txt";
	FILE* pf = fopen(filename, "w+");
	if (pf == NULL) {
		::exit(1);
	}

	pfLog = pf;
}

Log::~Log() {
	if (pfLog) {
		fclose(pfLog);
		pfLog = NULL;
	}
}

void Log::LogInfo(const char* str, ...) {

	char buff[256];
	int len = strlen(str);
	if (len == 0) {
		return;
	}

	va_list argptr;
	va_start(argptr, str);
	vsnprintf(buff, sizeof(buff)-1, str, argptr);
	va_end(argptr);

	fprintf(pfLog, "info : %s\n", buff);
	fflush(pfLog);
}

void Log::LogError(const char* str, ...) {

	char buff[256];
	int len = strlen(str);
	if (len == 0) {
		return;
	}

	va_list argptr;
	va_start(argptr, str);
	vsnprintf(buff, sizeof(buff)-1, str, argptr);
	va_end(argptr);

	fprintf(pfLog, "error : %s\n", buff);
	fflush(pfLog);
}

void Log::LogConsole(const char* str, ...){
	// todo

}

Log GLog;