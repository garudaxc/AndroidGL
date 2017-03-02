#include <string>
#include <vector>
#include <android/sensor.h>
#include <android/looper.h>
#include "Platfrom.h"
#include "MyLog.h"
#include "FileSystem.h"


using namespace std;
using namespace FancyTech;


const char* vsInclude = "#define VERTEX_SHADER\n";
const char* psInclude = "#define FRAGMENT_SHADER\n";


const char * EglErrorString(const EGLint err) {
	//const EGLint err = eglGetError();
	switch (err) {
	case EGL_SUCCESS:			return "EGL_SUCCESS";
	case EGL_NOT_INITIALIZED:	return "EGL_NOT_INITIALIZED";
	case EGL_BAD_ACCESS:		return "EGL_BAD_ACCESS";
	case EGL_BAD_ALLOC:			return "EGL_BAD_ALLOC";
	case EGL_BAD_ATTRIBUTE:		return "EGL_BAD_ATTRIBUTE";
	case EGL_BAD_CONTEXT:		return "EGL_BAD_CONTEXT";
	case EGL_BAD_CONFIG:		return "EGL_BAD_CONFIG";
	case EGL_BAD_CURRENT_SURFACE:return "EGL_BAD_CURRENT_SURFACE";
	case EGL_BAD_DISPLAY:		return "EGL_BAD_DISPLAY";
	case EGL_BAD_SURFACE:		return "EGL_BAD_SURFACE";
	case EGL_BAD_MATCH:			return "EGL_BAD_MATCH";
	case EGL_BAD_PARAMETER:		return "EGL_BAD_PARAMETER";
	case EGL_BAD_NATIVE_PIXMAP:	return "EGL_BAD_NATIVE_PIXMAP";
	case EGL_BAD_NATIVE_WINDOW:	return "EGL_BAD_NATIVE_WINDOW";
	case EGL_CONTEXT_LOST:		return "EGL_CONTEXT_LOST";
	default: return "Unknown egl error code";
	}
}

struct EGLAttri
{
	int		id;
	const char*	name;
};

#define EGLATTRI( v )	{v, #v}


void EnumGLConfig() {

	EGLAttri eglAttri[] = {
		EGLATTRI(EGL_BUFFER_SIZE),
		EGLATTRI(EGL_DEPTH_SIZE),
		EGLATTRI(EGL_STENCIL_SIZE),
		EGLATTRI(EGL_SURFACE_TYPE),
		EGLATTRI(EGL_COLOR_BUFFER_TYPE),
		EGLATTRI(EGL_SAMPLES),
	};

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	EGLint numConfigs = 0;
	EGLBoolean bRes = eglGetConfigs(display, NULL, 0, &numConfigs);
	if (!bRes) {
		GLog.LogError("eglGetConfigs failed!");
	}

	GLog.LogInfo("egl config count : %d", numConfigs);

	vector<EGLConfig> configs(numConfigs);
	bRes = eglGetConfigs(display, &configs[0], numConfigs, &numConfigs);

	for (int i = 0; i < numConfigs; i++) {
		GLog.LogInfo("config %d :", i);
		for (int j = 0; j < sizeof(eglAttri) / sizeof(eglAttri[0]); j++) {
			EGLint value = 0;
			bRes = eglGetConfigAttrib(display, configs[i], eglAttri[j].id, &value);
			if (bRes) {
				GLog.LogInfo("%s\t%d", eglAttri[j].name, value);
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////

uint64_t GetTicksNanos()
{
	// Do NOT change because this should be the same as Java's system.nanoTime(),
	// which is what the Choreographer vsync timestamp is based on.
	struct timespec tp;
	const int       status = clock_gettime(CLOCK_MONOTONIC, &tp);

	if (status != 0)
	{
		//OVR_DEBUG_LOG(("clock_gettime status=%i", status));
	}
	const uint64_t result = (uint64_t)tp.tv_sec * (uint64_t)(1000 * 1000 * 1000) + uint64_t(tp.tv_nsec);
	return result;
}

uint32_t GetTicksMS()
{
	return GetTicksNanos() / 1000000;
}

//////////////////////////////////////////////////////////////////////////


void Platfrom::Init()
{
	GFileSys->SetRootPath("/sdcard/MyTest/", true);
}

void Platfrom::Shutdown()
{

}

const string& Platfrom::GetTempPath()
{
	static string path("/sdcard/mytest");
	return path;
}


///------------------------------------------------
// gles api hook test



extern "C"
{
	struct gl_hooks_t {
		struct gl_t {
			void* foo1;
			void* foo2;
		} gl;
	};

	GL_APICALL void GL_APIENTRY my_glActiveTexture(GLenum texture)
	{
		GLog.LogInfo("my_glActiveTexture %u", texture);
	}

#define TLS_SLOT_OPENGL_API		 3

	void glesApiHookTest()
	{
		void *tls_base = NULL;

		asm volatile(
			"mrc p15, 0, r12, c13, c0, 3"	"\n\t"
			"mov %0, r12"		"\n\t"
			: "=r" (tls_base)
			);

		GLog.LogInfo("tls_base %p", tls_base);

		if (tls_base == NULL) {
			return;
		}

		gl_hooks_t * volatile * tls_hooks =
			reinterpret_cast<gl_hooks_t * volatile *>(tls_base);
		gl_hooks_t * hooks = tls_hooks[TLS_SLOT_OPENGL_API];

		GLog.LogInfo("hooks %p", hooks);

		void* origFunPtr = hooks->gl.foo2;
		GLog.LogInfo("origFunPtr %p", origFunPtr);

		// function list in android code/frameworks/Native/Opengl/Libs/Entries.in
		hooks->gl.foo2 = (void*)my_glActiveTexture;

		// will call to "my_glActiveTexture" function
		glActiveTexture(999);

		hooks->gl.foo2 = origFunPtr;
	}

}