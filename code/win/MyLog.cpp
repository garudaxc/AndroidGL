#include "MyLog.h"
#include "Thread.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

struct LogImpl
{
	FILE*	pf;
	Mutex	mutex;

	LogImpl() :pf(NULL)
	{
	}
};

Log::Log()
{
	impl_ = new LogImpl;

	const char* filename = "log.txt";
	FILE* pf = fopen(filename, "w+");
	if (pf == NULL) {
		::exit(1);
	}

	impl_->pf = pf;
}

Log::~Log() {
	if (impl_->pf) {
		fclose(impl_->pf);
	}

	delete impl_;
}

void WriteLog(LogImpl* impl, const char* prefix, const char* fmt, va_list argptr)
{
	char s[1024];

	vsnprintf(s, sizeof(s)-1, fmt, argptr);

	time_t t;
	struct tm *tmp;
	char buff[16];
	time(&t);
	tmp = localtime(&t);
	strftime(buff, sizeof(buff), "%H:%M:%S", tmp);
	
	AutoLock lock(&impl->mutex);
	fprintf(impl->pf, "%s (%s) : %s\n", prefix, buff, s);
	fflush(impl->pf);
}

void Log::LogInfo(const char* str, ...) {

	int len = strlen(str);
	if (len == 0) {
		return;
	}

	va_list argptr;
	va_start(argptr, str);

	WriteLog(impl_, "info", str, argptr);
	va_end(argptr);
}

void Log::LogError(const char* str, ...) {

	int len = strlen(str);
	if (len == 0) {
		return;
	}

	va_list argptr;
	va_start(argptr, str);

	WriteLog(impl_, "error", str, argptr);
	va_end(argptr);
}

void Log::LogConsole(const char* str, ...){
	// todo

}

Log GLog;