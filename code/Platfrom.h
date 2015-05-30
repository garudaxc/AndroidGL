#pragma once

#ifdef WIN32
#include "win/glew.h"

typedef unsigned long long uint64;

#else
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

typedef uint64_t uint64;

#endif // WIN32


void EnumGLConfig();

void LogSencorInfo();

uint64 GetTicksNanos();


extern const char* vsInclude;
extern const char* psInclude;