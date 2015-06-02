#include <string>
#include <vector>
#include "glUtil.h"
#include "MyLog.h"
#include "Platfrom.h"

using namespace std;


const char * GL_ErrorForEnum(const GLenum e){
	switch (e) {
	case GL_NO_ERROR: return "GL_NO_ERROR";
	case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
	case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
	case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
	default: return "Unknown gl error code";
	}
}

void PrintGLString(const char *name, GLenum s) {
	const char *v = (const char *)glGetString(s);
	GLog.LogInfo("GL %s = %s", name, v);
}

void PrintGLInteger(const char* name, GLenum id) {
	GLint value = 0;
	glGetIntegerv(id, &value);
	GLog.LogInfo("%s %d", name, value);
}

void checkGlError(const char* op) {
	for (GLint error = glGetError(); error; error = glGetError()) {
		GLog.LogError("after %s() glError %s (0x%x)", op, GL_ErrorForEnum(error), error);
	}
}

void LogGLInfo() {
	PrintGLString("Version", GL_VERSION);
	PrintGLString("Vendor", GL_VENDOR);
	PrintGLString("Renderer", GL_RENDERER);
	//printGLString("Extensions", GL_EXTENSIONS);

	PrintGLInteger("GL_MAX_VARYING_VECTORS", GL_MAX_VARYING_VECTORS);
	PrintGLInteger("GL_MAX_VERTEX_UNIFORM_VECTORS", GL_MAX_VERTEX_UNIFORM_VECTORS);
	PrintGLInteger("GL_MAX_FRAGMENT_UNIFORM_VECTORS", GL_MAX_FRAGMENT_UNIFORM_VECTORS);

	GLog.LogInfo("GL extensions : ");
	const char *v = (const char *)glGetString(GL_EXTENSIONS);
	if (v != NULL) {
		string ext = v;

		size_t off = 0;
		for (;;) {
			size_t pos = ext.find_first_of(' ', off);
			if (pos == string::npos) {
				break;
			}
			string s = ext.substr(off, pos - off);
			off = pos + 1;
			GLog.LogInfo(s.c_str());
		}
	}
}
