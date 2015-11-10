#include "MyLog.h"
#include "Platfrom.h"
#include <android/log.h>
#include "Thread.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include "time.h"


#define  LOG_TAG    "androidGL"
#define LOGI(...)	((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGW(...)	((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#define LOGE(...)	((void)__android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__))

struct LogImpl
{
	FILE*	pf;
	Mutex	mutex;

	LogImpl() :pf(NULL)
	{
	}
};


Log::Log(){
	impl_ = new LogImpl;

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
	impl_->pf = pf;
}

Log::~Log() {
	if (impl_->pf) {
		fclose(impl_->pf);
	}	

	delete impl_;
}

void GetTimeString(char* buff, int size)
{
	time_t t;
	struct tm *tmp;
	time(&t);
	tmp = localtime(&t);
	strftime(buff, size, "%H:%M:%S", tmp);
}

void Log::LogInfo(const char* str, ...) {

	int len = strlen(str);
	if (len == 0) {
		return;
	}
	char buff[1024];

	va_list argptr;
	va_start(argptr, str);
	vsnprintf(buff, sizeof(buff)-1, str, argptr);
	va_end(argptr);

	char time[16];
	GetTimeString(time, sizeof(time));

	AutoLock lock(&impl_->mutex);
	__android_log_write(ANDROID_LOG_INFO, LOG_TAG, buff);

	fprintf(impl_->pf, "info (%s): %s\n", time, buff);
	fflush(impl_->pf);
}

void Log::LogError(const char* str, ...) {

	int len = strlen(str);
	if (len == 0) {
		return;
	}
	char buff[1024];

	va_list argptr;
	va_start(argptr, str);
	vsnprintf(buff, sizeof(buff)-1, str, argptr);
	va_end(argptr);

	char time[16];
	GetTimeString(time, sizeof(time));

	AutoLock lock(&impl_->mutex);
	__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, buff);

	fprintf(impl_->pf, "error (%s): %s\n", time, buff);
	fflush(impl_->pf);
}

void Log::LogConsole(const char* str, ...){

	va_list argptr;
	va_start(argptr, str);
	__android_log_vprint(ANDROID_LOG_INFO, LOG_TAG, str, argptr);
	va_end(argptr);
}

Log GLog;