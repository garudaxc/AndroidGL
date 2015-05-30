#include <string>
#include <vector>
#include <android/sensor.h>
#include <android/looper.h>
#include "Platfrom.h"
#include "MyLog.h"


using namespace std;


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

#define EVENT_IDEN 234
#define SENSOR_EVENT_PER_SEC 60

ASensorEventQueue* eventQueue;

void LogSencorInfo(){
	ASensorManager* mgr = ASensorManager_getInstance();
	ASensorList sensors = NULL;

	int numSensor = ASensorManager_getSensorList(mgr, &sensors);
	GLog.LogInfo("num sensor : %d", numSensor);

	for (int i = 0; i < numSensor; i++) {
		GLog.LogInfo("0x%X name %s\tvendor %s\ttype %d\tresolution %f",
			sensors[i],
			ASensor_getName(sensors[i]),
			ASensor_getVendor(sensors[i]),
			ASensor_getType(sensors[i]),
			ASensor_getResolution(sensors[i]));
	}

	ASensorRef gryo = ASensorManager_getDefaultSensor(mgr, ASENSOR_TYPE_GYROSCOPE);
	GLog.LogInfo("default gyroscope 0x%X", gryo);

	ASensorRef acceler = ASensorManager_getDefaultSensor(mgr, ASENSOR_TYPE_ACCELEROMETER);
	GLog.LogInfo("default accelerometer 0x%X Resolution %f MinDelay %f", acceler,
		ASensor_getResolution(acceler), ASensor_getMinDelay(acceler));


	//ALooper* looper = ALooper_forThread();
	//if (looper == NULL) {
	//	GLog.LogError("ALooper_forThread failed!");
	//	return;
	//}

	//ASensorEventQueue* eventQueue = ASensorManager_createEventQueue(mgr, looper, EVENT_IDEN, NULL, NULL);

	//int succ = ASensorEventQueue_enableSensor(eventQueue, acceler);
	//if (succ < 0) {
	//	GLog.LogError("ASensorEventQueue_enableSensor failed! code : %d", succ);
	//	return;
	//}

	//succ = ASensorEventQueue_setEventRate(eventQueue, acceler, (1000 / 60) * 1000);
	//if (succ < 0) {
	//	GLog.LogError("ASensorEventQueue_setEventRate failed! code : %d", succ);
	//	return;
	//}


}

ASensorRef Accelerometer = NULL;


void LogAccData(){

}


uint64 GetTicksNanos()
{
	// Do NOT change because this should be the same as Java's system.nanoTime(),
	// which is what the Choreographer vsync timestamp is based on.
	struct timespec tp;
	const int       status = clock_gettime(CLOCK_MONOTONIC, &tp);

	if (status != 0)
	{
		//OVR_DEBUG_LOG(("clock_gettime status=%i", status));
	}
	const uint64 result = (uint64)tp.tv_sec * (uint64_t)(1000 * 1000 * 1000) + uint64(tp.tv_nsec);
	return result;
}
