#include "MyLog.h"
//#include "Thread.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "Platfrom.h"
#include "unzip.h"

#include <string>
using namespace std;

void Log_ObjectC(const char* msg);

namespace FancyTech {
    
    struct LogImpl
    {
        FILE*	pf;
      //  Mutex	mutex;
        
        LogImpl() :pf(NULL)
        {
        }
    };
    
    Log::Log()
    {
        impl_ = new LogImpl;
        
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
        
//        time_t t;
//        struct tm *tmp;
//        char buff[16];
//        time(&t);
//        tmp = localtime(&t);
//        strftime(buff, sizeof(buff), "%H:%M:%S", tmp);
        
        Log_ObjectC(s);
        
    //    AutoLock lock(&impl->mutex);
        //fprintf(impl->pf, "%s (%s) : %s\n", prefix, buff, s);
        //fflush(impl->pf);
        
        
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
}

extern "C"
{}
    void TestUnzip(const char* path);



void TestUnzip(const char* path)
{
    Log_ObjectC("path");
    Log_ObjectC(path);
    
    string str = path;
    str += "/Shaders.zip";
    
    unzFile file = unzOpen(str.c_str());
    if (file == nullptr)
    {
        Log_ObjectC("unzopen failed!");
        return;
    }
    
    unzGoToFirstFile(file);
    
    char fnBuffer[256];
    unzGetCurrentFileInfo(file, nullptr, fnBuffer, sizeof(fnBuffer), nullptr, 0, nullptr, 0);
    
    Log_ObjectC(fnBuffer);
    
}



