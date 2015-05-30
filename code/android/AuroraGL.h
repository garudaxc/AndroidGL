
//#include "glew.h"
//#include "wglew.h"

#include <android/log.h>


struct glState_t
{
	int		width;
	int		height;
};

extern struct glState_t glState;



//void DrawFrame();
//
//void LoadResource();



int setupGraphics(int w, int h);


void renderFrame();
