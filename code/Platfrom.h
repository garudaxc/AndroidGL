#pragma once

#ifdef WIN32
#include <io.h>
#include "win/glew.h"
#include "win/wglew.h"

typedef unsigned char	ubyte_t;
typedef unsigned short	uint16_t;
typedef unsigned int	uint32_t;
typedef unsigned long long uint64_t;

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:6031)
#pragma warning(disable:26495)
#pragma warning(disable:26451)

#endif

#ifdef __ANDROID__
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <unistd.h>

typedef unsigned char ubyte_t;

#endif // WIN32

#include <string>
using namespace std;



#ifdef __APPLE__

#import <OpenGLES/ES2/glext.h>

typedef unsigned char	ubyte_t;
typedef unsigned short	uint16_t;
typedef unsigned int	uint32_t;
typedef unsigned long long uint64_t;

#endif


typedef struct tagRect_t
{
	int    left;
	int    top;
	int    right;
	int    bottom;
} rect_t;

extern const char* vsInclude;
extern const char* psInclude;

void EnumGLConfig();

uint64_t GetTicksNanos();
uint32_t	GetTicksMS();

class Platfrom
{
public:
	static void	Init();
	static void Shutdown();

	static const string&	GetTempPath();
};


