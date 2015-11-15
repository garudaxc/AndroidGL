#include <jni.h>
#include "Platfrom.h"
#include "MyLog.h"
#include "AuroraGL.h"


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

	JNIEXPORT void JNICALL Java_com_xvr_aurora_XVRActivity_nativeOnCreate(JNIEnv * env, jobject obj)
	{
		GLog.LogInfo("nativeOnCreate");
	}

	JNIEXPORT void JNICALL Java_com_xvr_aurora_GL2JNILib_init(JNIEnv * env, jobject obj, jint width, jint height)
	{
		GLog.LogInfo("Java_com_xvr_aurora_GL2JNILib_init (onSurfaceChanged)");

		static bool inited = false;
		if (!inited) {
			setupGraphics(width, height);
			inited = true;
		}	
	}

	JNIEXPORT void JNICALL Java_com_xvr_aurora_GL2JNILib_step(JNIEnv * env, jobject obj)
	{
		_UpdateTimer();
		renderFrame();
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