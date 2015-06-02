#include <string>
#include <vector>
#include <stdio.h>
#include "Platfrom.h"
#include <stdio.h>
#include <stdlib.h>
//#include "glew.h"
#include "AuroraGL.h"
#include "MyLog.h"
#include "glUtil.h"
#include "ShaderManager.h"
#include "Model.h"
#include "AurMath.h"
#include "Matrix.h"
#include "Vector.h"
#include "MathFunction.h"
#include "Texture.h"

using namespace std;
using namespace Aurora;

struct glState_t glState;

Model model;

Texture texDiffuse;

GLint unifromScale;
GLint unifromTransfrom;


int setupGraphics(int w, int h) {

	LogGLInfo();

	model.Load("/sdcard/MyTest/OilTank001.mesh");
	checkGlError("model.load");
	
	GShaderManager.LoadFromFile("/sdcard/MyTest/shader.glsl");

	texDiffuse.Load("/sdcard/MyTest/1.png");
	checkGlError("texDiffuse.Load");

	glViewport(0, 0, w, h);
	checkGlError("glViewport");
	return 1;
}

void renderFrame() {

	glClearColor(0, 0, 0, 0);
	glClearDepthf(1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glDisable(GL_CULL_FACE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	model.Bind();
	checkGlError("model.Bind");

	static float angle = 0.f;
	angle += 0.1f;

	Matrix4f mWorld;
	MatrixRotationAxis(mWorld, Vector3f::UNIT_Z, angle);

	Matrix4f mView, mProj;
	MatrixLookAtRH(mView, Vector3f(0.f, -3.0f, 0.0f), Vector3f::ZERO, Vector3f::UNIT_Z);
	MatrixPerspectiveFovRH(mProj, Mathf::PI / 4.0f, glState.width / (float)glState.height, 0.1f);
	mProj._33 = mProj._33 * 2.f + mProj._34 * -1.f;
	mProj._43 = mProj._43 * 2.f;


	GShaderManager.Bind();
	checkGlError("GShaderManager.Bind()");
	GShaderManager.SetUnifrom(SU_WORLD, mWorld.Ptr());
	GShaderManager.SetUnifrom(SU_VIEW, mView.Ptr());
	GShaderManager.SetUnifrom(SU_PROJECTION, mProj.Ptr());
	checkGlError("GShaderManager.SetUnifrom");

	GShaderManager.SetUnifrom(SU_TEX_DIFFUSE, 0);
	glActiveTexture(GL_TEXTURE0);
	texDiffuse.Bind();
	checkGlError("texDiffuse.Bind()");

	for (int i = 0; i < model.GetElementCount(); i++) {
		const ModelElement* e = model.GetElement(i);
			
		void* index = (void*)(e->indexOffset * sizeof(unsigned short));
		glDrawElements(GL_TRIANGLES, e->indexCount, GL_UNSIGNED_SHORT, index);
		checkGlError("glDrawElements");
	}

}


