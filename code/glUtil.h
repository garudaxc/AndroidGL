#pragma once
#include "Platfrom.h"

#define BUFFER_OFFSET(offset) ((void *)(offset))
#define COLOR(r, g, b, a) (a << 24 | b << 16 | g << 8 | r)

void printGLString(const char *name, GLenum s);
void checkGlError(const char* op);

void LogGLInfo();

const char* GLCompressedTexName(uint32_t type);


