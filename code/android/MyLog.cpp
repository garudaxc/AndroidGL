#include "MyLog.h"
#include "Platfrom.h"
#include <android/log.h>
#include "Thread.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>


#define  LOG_TAG    "androidGL"
#define LOGI(...)	((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGW(...)	((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#define LOGE(...)	((void)__android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__))


Log::Log() :pfLog(NULL), mutex_(NULL){
	const char* path = "/sdcard/mytest";
	int r = access(path, F_OK);
	if (r < 0){
		LOGI("path %s not exist, create", path);
		r = mkdir(path, 0770 /*S_IRWXU | S_IRWXG | S_IROTH | S_IWOTH*/);
		if (r < 0) {
			LOGE("create dir %s failed errno(%d)", path, errno);
		} else {
			LOGI("create dir %s", path);
		}
	}

	char logfile[128];
	sprintf(logfile, "%s/log.txt", path);

	FILE* pf = fopen(logfile, "w+");
	if (pf == NULL) {
		LOGE("create log file failed! %s (%d)", logfile, errno);
		::exit(1);
	}

	LOGI("create log file %s", logfile);
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

	fprintf(pfLog, "info tid(%X): %s\n", Thread::CurrentThreadId(), buff);
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