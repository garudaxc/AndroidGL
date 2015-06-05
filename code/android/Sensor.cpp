#include <android/sensor.h>
#include <android/looper.h>
#include <android_native_app_glue.h>
#include <vector>
#include "AurMath.h"
#include "Matrix.h"
#include "Vector.h"
#include "MathFunction.h"
#include "Quaternion.h"
#include "Platfrom.h"
#include "MyLog.h"

using namespace std;
using namespace Aurora;


enum {
	SENSOR_TYPE_ACCELEROMETER = 1,
	SENSOR_TYPE_MAGNETIC_FIELD = 2,
	SENSOR_TYPE_GYROSCOPE = 4,
	SENSOR_TYPE_LIGHT = 5,
	SENSOR_TYPE_PROXIMITY = 8,
	SENSOR_TYPE_GRAVITY = 9,
	SENSOR_TYPE_ROTATION_VECTOR = 11,
	SENSOR_TYPE_ORIENTATION = 3,
};


#define EVENT_IDEN 23
#define SENSOR_EVENT_PER_SEC 60

ASensorManager* sensorManager = NULL;
ASensorEventQueue* eventQueue = NULL;
ASensorRef accelerometer = NULL;
ASensorRef gyroscope = NULL;
ASensorRef magnetic = NULL;
ALooper* looper = NULL;


struct SensorObj
{
	ASensorRef	sensor;
	int			id;
	const char*	name;
	int			type;
	float		resolution;
	int			minDelay;

	SensorObj(ASensorRef ref, int _id)
	{
		sensor = ref;
		id = _id;
		name = ASensor_getName(sensor);
		type = ASensor_getType(sensor);
		resolution = ASensor_getResolution(sensor);
		minDelay = ASensor_getMinDelay(sensor);
	}
};

vector<SensorObj>	sensors_;


void ListSensors(){
	ASensorManager* mgr = ASensorManager_getInstance();
	ASensorList sensors = NULL;

	int numSensor = ASensorManager_getSensorList(mgr, &sensors);
	GLog.LogInfo("num sensor : %d", numSensor);

	for (int i = 0; i < numSensor; i++) {

		GLog.LogInfo("0x%X name %s\tvendor %s\ttype %d\tresolution %f\tmin delay %d",
			sensors[i],
			ASensor_getName(sensors[i]),
			ASensor_getVendor(sensors[i]),
			ASensor_getType(sensors[i]),
			ASensor_getResolution(sensors[i]),
			ASensor_getMinDelay(sensors[i]));

		int type = ASensor_getType(sensors[i]);
		if (type == SENSOR_TYPE_MAGNETIC_FIELD ||
			type == SENSOR_TYPE_GYROSCOPE ||
			type == SENSOR_TYPE_ROTATION_VECTOR ||
			type == SENSOR_TYPE_ORIENTATION ||
			type == SENSOR_TYPE_GRAVITY)
		{
			//SensorObj(sensors[i], i);
			sensors_.push_back(SensorObj(sensors[i], i));
		}
	}
}

Vector3f rotationVec;
Vector3f gravityVec_;

void _InitSensor()
{	
	rotationVec = Vector3f::ZERO;

	sensorManager	= ASensorManager_getInstance();

	ListSensors();
	GLog.LogInfo("sensor added %d", sensors_.size());
	
	ALooper* looper = ALooper_forThread();
	if (looper == NULL) {
		GLog.LogError("ALooper_forThread failed!");
		return;
	}

	eventQueue = ASensorManager_createEventQueue(sensorManager, looper, EVENT_IDEN, NULL, NULL);

	//{
	//	ASensorRef gryo = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_GYROSCOPE);
	//	if (gryo == NULL){
	//		GLog.LogError("can not find gryoscope sensor!");
	//	}
	//	else {
	//		GLog.LogInfo("default gyroscope 0x%X", gryo);
	//		SensorObj s(gryo);
	//		sensors.push_back(s);
	//	}
	//}

	//{
	//	ASensorRef magnetic = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_MAGNETIC_FIELD);
	//	if (magnetic == NULL){
	//		GLog.LogError("can not find magnetic sensor!");
	//	}
	//	else {
	//		GLog.LogInfo("default magnetic 0x%X", magnetic);
	//		SensorObj s(magnetic);
	//		sensors.push_back(s);
	//	}
	//}

	//ASensorRef acceler = ASensorManager_getDefaultSensor(mgr, ASENSOR_TYPE_ACCELEROMETER);
	//GLog.LogInfo("default accelerometer 0x%X Resolution %f MinDelay %f", acceler,
	//	ASensor_getResolution(acceler), ASensor_getMinDelay(acceler));
	
	//succ = ASensorEventQueue_setEventRate(eventQueue, acceler, (1000 / 60) * 1000);
	//if (succ < 0) {
	//	GLog.LogError("ASensorEventQueue_setEventRate failed! code : %d", succ);
	//	return;
	//}
}


void _EnableSensor()
{
	for (vector<SensorObj>::iterator it = sensors_.begin(); it != sensors_.end(); ++it){
		int succ = ASensorEventQueue_enableSensor(eventQueue, it->sensor);
		if (succ < 0) {
			GLog.LogError("ASensorEventQueue_enableSensor failed! code : %d", succ);
		}
		if (it->minDelay > 0){
			ASensorEventQueue_setEventRate(eventQueue, it->sensor, it->minDelay);
		}
	}
}

void _DisableSensor()
{
	for (vector<SensorObj>::iterator it = sensors_.begin(); it != sensors_.end(); ++it){
		int succ = ASensorEventQueue_disableSensor(eventQueue, it->sensor);
		if (succ < 0) {
			GLog.LogError("ASensorEventQueue_disableSensor failed! code : %d", succ);
		}
	}
}



void MagnaticData(const ASensorEvent& e)
{
	GLog.LogInfo("MagnaticData\t%f\t%f\t%f", e.data[0], e.data[1], e.data[2]);
}

void GyroscopeData(const ASensorEvent& e)
{
	GLog.LogInfo("GyroscopeData\t%f\t%f\t%f", e.data[0], e.data[1], e.data[2]);
}


void _ProcessSensorData(int identifier)
{
	if (identifier != EVENT_IDEN){
		return;
	}
	
	int eventCount = 0;
	ASensorEvent event;
	while (ASensorEventQueue_getEvents(eventQueue, &event, 1) > 0) {

		//GLog.LogInfo("%f %f %f, sensor event sensor type %d", event.data[0], event.data[1], event.data[2], event.type);
		eventCount++;

		if (event.type == SENSOR_TYPE_ROTATION_VECTOR){
			rotationVec.Set(event.data);
		}

		if (event.type == SENSOR_TYPE_GRAVITY) {
			gravityVec_.Set(event.data);
		}
	}

	GLog.LogInfo("sensor event count %d", eventCount);
}


Matrix4f _GetDeviceRotationMatrix()
{
	Matrix3f phoneFrame;
	phoneFrame.Set(0.f, 1.f, 0.f,
		0.f, 0.f, -1.f,
		-1.f, 0.f, 0.f);

	gravityVec_.Normalize();

	Vector3f vecInVr;
	Vector3Transform(vecInVr, gravityVec_, phoneFrame);

	Quaternionf qRot;
	QuaternionVectorToVector(qRot, vecInVr, -Vector3f::UNIT_Z);
	
	Matrix4f matRot;
	MatrixFromQuaternion(matRot, qRot);

	return matRot;
}