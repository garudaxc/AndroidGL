#include "MyLog.h"
#include <android/log.h>
#include "Thread.h"
#include <string.h>
#include <stdlib.h>


#define  LOG_TAG    "androidGL"
#define LOGI(...)	((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGW(...)	((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#define LOGE(...)	((void)__android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__))


Log::Log() :pfLog(NULL), mutex_(NULL){
	const char* filename = "/sdcard/MyTest/log.txt";
	FILE* pf = fopen(filename, "w+");
	if (pf == NULL) {
		LOGE("create log file failed! %s (%d)", filename, errno);
		::exit(1);
	}

	LOGI("create log file %s", filename);
	pfLog = pf;
	
	mutex_ = new Mutex();
}

Log::~Log() {
	if (pfLog) {
		fclose(pfLog);
		pfLog = NULL;
	}	

	delete mutex_;
	mutex_ = NULL;
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

	AutoLock lock(mutex_);
	__android_log_write(ANDROID_LOG_INFO, LOG_TAG, buff);

	fprintf(pfLog, "info tid(%X): %s\n", (uint32_t)pthread_self(), buff);
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

	AutoLock lock(mutex_);
	__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, buff);
	fprintf(pfLog, "error : %s\n", buff);
	fflush(pfLog);
}

void Log::LogConsole(const char* str, ...){

	va_list argptr;
	va_start(argptr, str);
	__android_log_vprint(ANDROID_LOG_INFO, LOG_TAG, str, argptr);
	va_end(argptr);
}

Log GLog;