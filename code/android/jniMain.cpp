#include <jni.h>
#include "Platfrom.h"
#include "MyLog.h"
#include "AuroraGL.h"
#include <android/native_window_jni.h>	// for native window JNI


using namespace FancyTech;

void _UpdateTimer();
void SuspendSensorThread();
void ResumeSensorThread();
void StopSensorThread();

namespace FancyTech
{
	void DispatchAndroidEvent(int action, float x, float y);
}

#ifdef __cplusplus
extern "C" {
#endif
	static bool inited = false;
	EGLDisplay display_ = NULL;
	EGLSurface surface_ = NULL;
	EGLContext context_ = NULL;



#ifndef ALOGI
#define ALOGI GLog.LogInfo
#endif


	JNIEXPORT EGLSurface JNICALL CreateSurface(ANativeWindow * nativeWindow)
	{
		EGLContext context = eglGetCurrentContext();
		GLog.LogInfo("context %d", context);

		EGLint configId = 0;
		eglQueryContext(display_, context, EGL_CONFIG_ID, &configId);
		GLog.LogInfo("config id from context %d", configId);

		EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

		const EGLint attribs[] = {

			//            EGL_RECORDABLE_ANDROID, EGL_TRUE,
			//            EGL_FRAMEBUFFER_TARGET_ANDROID, EGL_TRUE,
			//            EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,

			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_NONE, 0,
			EGL_NONE
		};
		EGLint w, h, dummy;
		EGLint numConfigs;
		EGLConfig config = 0;

		/* Here, the application chooses the configuration it desires. In this
		* sample, we have a very simplified selection process, where we pick
		* the first EGLConfig that matches our criteria */
		eglChooseConfig(display, attribs, NULL, 0, &numConfigs);
		ALOGI("numconfig %d", numConfigs);

		if (numConfigs == 0) {
			return EGL_NO_SURFACE;
		}

		EGLConfig* const configs = new EGLConfig[numConfigs];
		eglChooseConfig(display, attribs, configs, numConfigs, &numConfigs);

		config = configs[0];

		//for (int i = 0; i < numConfigs; i++) {
		//	EGLint id = 0;
		//	EGLint nativeVisualId = 0;
		//	EGLint nativeVisualType = 0;
		//	eglGetConfigAttrib(display, configs[i], EGL_CONFIG_ID, &id);
		//	eglGetConfigAttrib(display, configs[i], EGL_NATIVE_VISUAL_ID, &nativeVisualId);
		//	eglGetConfigAttrib(display, configs[i], EGL_NATIVE_VISUAL_TYPE, &nativeVisualType);

		//	if (/*nativeVisualId == format*/id == configId){
		//		//ALOGI("config %p id %d visual id %d visual type %d",configs[i], id, nativeVisualId, nativeVisualType);
		//		//LogConfig(display, configs[i]);
		//		config = configs[i];
		//		break;
		//	}
		//}

		if (config == 0) {
			return EGL_NO_SURFACE;
		}

		EGLSurface surface = eglCreateWindowSurface(display, config, nativeWindow, NULL);

		EGLint mWidth, mHeight;

		eglQuerySurface(display, surface, EGL_WIDTH, &mWidth);
		eglQuerySurface(display, surface, EGL_HEIGHT, &mHeight);
		ALOGI("surface %p %d x %d", surface, mWidth, mHeight);


		return surface;
	}




	static EGLSurface engine_init_display(ANativeWindow * nativeWindow) {
		// initialize OpenGL ES and EGL

		/*
		* Here specify the attributes of the desired configuration.
		* Below, we select an EGLConfig with at least 8 bits per color
		* component compatible with on-screen windows
		*/

		EGLint w, h, dummy, format;
		EGLint numConfigs;
		EGLConfig config;
		EGLSurface surface;
		EGLContext context;

		EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

		EGLint  majorVersion = 0;
		EGLint  minorVersion = 0;
		if (!eglInitialize(display, &majorVersion, &minorVersion)) {
			GLog.LogError("eglInitialize failed!");
		}
		else {
			GLog.LogInfo("eglInitialize majorVer %d minorVer %d", majorVersion, minorVersion);
		}

		//EnumGLConfig();

		const EGLint attribs[] = {
			// this specifically requests an Open GL ES 2 renderer
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE, 8,
			EGL_DEPTH_SIZE, 24,
			// (ommiting other configs regarding the color channels etc...
			EGL_NONE
		};
		/* Here, the application chooses the configuration it desires. In this
		* sample, we have a very simplified selection process, where we pick
		* the first EGLConfig that matches our criteria */
		if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs)) {
			GLog.LogError("eglChooseConfig failed!");
		}

		/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
		* guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
		* As soon as we picked a EGLConfig, we can safely reconfigure the
		* ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
		eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

		ANativeWindow_setBuffersGeometry(nativeWindow, 0, 0, format);

		surface = eglCreateWindowSurface(display, config, nativeWindow, NULL);
		if (surface == EGL_NO_SURFACE) {
			GLog.LogError("eglCreateWindowSurface failed!");

			switch (eglGetError()) {
			case EGL_BAD_MATCH:
				// Check window and EGLConfig attributes to determine
				// compatibility, or verify that the EGLConfig 
				// supports rendering to a window, 
				break;
			case EGL_BAD_CONFIG:
				// Verify that provided EGLConfig is valid
				break;
			case EGL_BAD_NATIVE_WINDOW:
				// Verify that provided EGLNativeWindow is valid
				break;
			case EGL_BAD_ALLOC:
				// Not enough resources available. Handle and recover
				break;
			}
		}

		const EGLint context_attrib_list[] = {
			// request a context using Open GL ES 2.0
			EGL_CONTEXT_CLIENT_VERSION, 2,
			EGL_NONE
		};
		context = eglCreateContext(display, config, NULL, context_attrib_list);
		if (context == EGL_NO_CONTEXT)	{
			GLog.LogError("eglCreateContext failed!");
		}

		if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
			GLog.LogError("Unable to eglMakeCurrent");
			return EGL_NO_SURFACE;
		}

		eglQuerySurface(display, surface, EGL_WIDTH, &w);
		eglQuerySurface(display, surface, EGL_HEIGHT, &h);				

		GLog.LogInfo("create device width = %d height = %d", w, h);

		// Initialize GL state.
		//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

		GLog.LogInfo("engine_init_display done!");

		surface_ = surface;
		context_ = context;

		return surface;
	}


	
	extern "C" JNIEXPORT EGLSurface JNICALL init2(EGLContext& cont, jint width, jint height);


	JNIEXPORT void JNICALL Java_com_xvr_aurora_XVRActivity_nativeOnCreate(JNIEnv * env, jobject obj)
	{
		GLog.LogInfo("nativeOnCreate");
	}

	JNIEXPORT void JNICALL Java_com_xvr_aurora_GL2JNILib_init(JNIEnv * env, jobject obj, jint width, jint height, jobject surface)
	{
		GLog.LogInfo("Java_com_xvr_aurora_GL2JNILib_init (onSurfaceChanged)");

		if (!inited) {

			ANativeWindow * nativeWindow = ANativeWindow_fromSurface(env, surface);
			GLog.LogInfo("native Window %p tid %d", nativeWindow, gettid());
			
			display_ = eglGetCurrentDisplay();
			//surface_ = CreateSurface(nativeWindow);

			//surface_ = engine_init_display(nativeWindow);

			surface_ = init2(context_, width, height);
			GLog.LogInfo("surface_ = init2(width, height) %p", surface_);

			if (surface_ != EGL_NO_SURFACE) {
				//EGLContext contex = eglGetCurrentContext();
				//eglMakeCurrent(display_, surface_, surface_, contex);
				setupGraphics(width, height);

				inited = true;
			}
		}	
	}

	JNIEXPORT void JNICALL Java_com_xvr_aurora_GL2JNILib_step(JNIEnv * env, jobject obj)
	{
		if (!inited) {
			return;
		}
		static int framecount = 0;
		//GLog.LogInfo("Java_com_xvr_aurora_GL2JNILib_step %d tid %d", framecount++, gettid());

		eglMakeCurrent(display_, surface_, surface_, context_);

		_UpdateTimer();
		renderFrame();
		glFinish();
		glFlush();
		EGLBoolean result = eglSwapBuffers(display_, surface_);
		if (!result) {
			GLog.LogError("eglSwapBuffers failed");
			int w, h;

			eglQuerySurface(display_, surface_, EGL_WIDTH, &w);
			eglQuerySurface(display_, surface_, EGL_HEIGHT, &h);
			GLog.LogInfo("surface %d width = %d height = %d",surface_, w, h);
		}
	}


	JNIEXPORT void JNICALL Java_com_xvr_aurora_XVRActivity_nativeOnResume(JNIEnv * env, jobject obj)
	{
		GLog.LogInfo("nativeOnResume");
		ResumeSensorThread();
	}

	JNIEXPORT void JNICALL Java_com_xvr_aurora_XVRActivity_nativeOnPause(JNIEnv * env, jobject obj)
	{
		GLog.LogInfo("nativeOnPause");
		SuspendSensorThread();

	}

	JNIEXPORT void JNICALL Java_com_xvr_aurora_XVRActivity_nativeOnDestroy(JNIEnv * env, jobject obj)
	{
		StopSensorThread();
		GLog.LogInfo("nativeOnDestroy");		
	}


	JNIEXPORT void JNICALL Java_com_xvr_aurora_XVRActivity_nativeOnEvent(JNIEnv * env, jobject obj, jint action, jfloat xpos, jfloat ypos)
	{
		DispatchAndroidEvent(action, xpos, ypos);
	}


#ifdef __cplusplus
}
#endif