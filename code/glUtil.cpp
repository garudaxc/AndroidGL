#include <string>
#include <vector>
#include "glUtil.h"
#include "MyLog.h"
#include "Platfrom.h"

using namespace std;
using namespace FancyTech;


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

	//wglGetExtensionsStringARB
	//wglGetExtensionsStringEXT

	GLint numExtensions = 0;
	GLog.LogInfo("GL extensions : ");
	string ext;
	if (glGetString(GL_EXTENSIONS) != NULL){
		ext = (const char *)glGetString(GL_EXTENSIONS);
		size_t off = 0;
		for (;;) {
			size_t pos = ext.find_first_of(' ', off);
			if (pos == string::npos) {
				break;
			}
			string s = ext.substr(off, pos - off);
			off = pos + 1;
			//GLog.LogInfo(s.c_str());
			numExtensions++;
		}
	} else {
#ifdef WIN32
		glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
		for (int i = 0; i < numExtensions; i++)	{
			//GLog.LogInfo((const char*)glGetStringi(GL_EXTENSIONS, i));
		}
#endif
	}

	GLog.LogInfo("number of GL_EXTENSIONS: %d", numExtensions);
}


struct GLEnumPair
{
	uint32_t		id;
	const char*		name;
};

#define GLENUM_PAIR( v )	{v, #v}

const char* GLCompressedTexName(uint32_t type)
{
	/*GLEnumPair GLTypeName[] = {
		GLENUM_PAIR(GL_COMPRESSED_RGB_S3TC_DXT1_EXT),
		GLENUM_PAIR(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT),
		GLENUM_PAIR(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT),
		GLENUM_PAIR(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT),
		GLENUM_PAIR(GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG),
		GLENUM_PAIR(GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG),
		GLENUM_PAIR(GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG),
		GLENUM_PAIR(GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG),
		GLENUM_PAIR(GL_ETC1_RGB8_OES),
		GLENUM_PAIR(GL_COMPRESSED_RGBA_ASTC_4x4_KHR),
		GLENUM_PAIR(GL_COMPRESSED_RGBA_ASTC_5x4_KHR),
		GLENUM_PAIR(GL_COMPRESSED_RGBA_ASTC_5x5_KHR),
		GLENUM_PAIR(GL_COMPRESSED_RGBA_ASTC_6x5_KHR),
		GLENUM_PAIR(GL_COMPRESSED_RGBA_ASTC_6x6_KHR),
		GLENUM_PAIR(GL_COMPRESSED_RGBA_ASTC_8x5_KHR),
		GLENUM_PAIR(GL_COMPRESSED_RGBA_ASTC_8x6_KHR),
		GLENUM_PAIR(GL_COMPRESSED_RGBA_ASTC_8x8_KHR),
		GLENUM_PAIR(GL_COMPRESSED_RGBA_ASTC_10x5_KHR),
		GLENUM_PAIR(GL_COMPRESSED_RGBA_ASTC_10x6_KHR),
		GLENUM_PAIR(GL_COMPRESSED_RGBA_ASTC_10x8_KHR),
		GLENUM_PAIR(GL_COMPRESSED_RGBA_ASTC_10x10_KHR),
		GLENUM_PAIR(GL_COMPRESSED_RGBA_ASTC_12x10_KHR),
		GLENUM_PAIR(GL_COMPRESSED_RGBA_ASTC_12x12_KHR),
		GLENUM_PAIR(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR),
		GLENUM_PAIR(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR),
		GLENUM_PAIR(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR),
		GLENUM_PAIR(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR),
		GLENUM_PAIR(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR),
		GLENUM_PAIR(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR),
		GLENUM_PAIR(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR),
		GLENUM_PAIR(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR),
		GLENUM_PAIR(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR),
		GLENUM_PAIR(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR),
		GLENUM_PAIR(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR),
		GLENUM_PAIR(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR),
		GLENUM_PAIR(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR),
		GLENUM_PAIR(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR),
		GLENUM_PAIR(GL_COMPRESSED_RGB8_ETC2),
		GLENUM_PAIR(GL_COMPRESSED_SRGB8_ETC2),
		GLENUM_PAIR(GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2),
		GLENUM_PAIR(GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2),
		GLENUM_PAIR(GL_COMPRESSED_RGBA8_ETC2_EAC),
		GLENUM_PAIR(GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC),
	};

	for (int i = 0; i < sizeof(GLTypeName) / sizeof(GLTypeName[0]); i++){
		if (GLTypeName[i].id == type){
			return GLTypeName[i].name;
		}
	}*/

	return "Unknow GLCompressedTexture";

}
