#pragma once

#ifdef WIN32
#include "win/glew.h"

#else
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif // WIN32


void EnumGLConfig();

void LogSencorInfo();

uint64_t GetTicksNanos();


extern const char* vsInclude;
extern const char* psInclude;