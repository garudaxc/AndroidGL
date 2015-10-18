#include <jni.h>
#include "Platfrom.h"
#include "MyLog.h"
#include "AuroraGL.h"



void StartSensor();
void PauseSensor();
void InitSensor();

void _UpdateTimer();



#ifdef __cplusplus
extern "C" {
#endif

	JNIEXPORT void JNICALL Java_com_xvr_aurora_XVRActivity_nativeOnCreate(JNIEnv * env, jobject obj)
	{
		GLog.LogInfo("nativeOnCreate");

		InitSensor();
	}

	JNIEXPORT void JNICALL Java_com_xvr_aurora_GL2JNILib_init(JNIEnv * env, jobject obj, jint width, jint height)
	{
		setupGraphics(width, height);
	}

	JNIEXPORT void JNICALL Java_com_xvr_aurora_GL2JNILib_step(JNIEnv * env, jobject obj)
	{
		_UpdateTimer();
		renderFrame();
	}


	JNIEXPORT void JNICALL Java_com_xvr_aurora_XVRActivity_nativeOnResume(JNIEnv * env, jobject obj)
	{
		GLog.LogInfo("nativeOnResume");

		StartSensor();
	}

	JNIEXPORT void JNICALL Java_com_xvr_aurora_XVRActivity_nativeOnPause(JNIEnv * env, jobject obj)
	{
		GLog.LogInfo("nativeOnPause");

		PauseSensor();
	}

	JNIEXPORT void JNICALL Java_com_xvr_aurora_XVRActivity_nativeOnDestroy(JNIEnv * env, jobject obj)
	{
		GLog.LogInfo("nativeOnDestroy");
	}


#ifdef __cplusplus
}
#endif