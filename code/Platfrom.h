#pragma once

#ifdef WIN32
#include "win/glew.h"

typedef unsigned long long uint64_t;
typedef unsigned int uint32_t;

#else
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>


#endif // WIN32

extern const char* vsInclude;
extern const char* psInclude;

void PlatfromInit();
void PlatfromShutDown();


void EnumGLConfig();

void LogSencorInfo();

uint64_t GetTicksNanos();
uint32_t	GetTicksMS();




