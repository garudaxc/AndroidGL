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


Vector3f gravityVec_;
Vector3f magneticVec_;
Vector3f gyroVec_;
Vector3f rotation_;
Vector3f rotationVec_;

void _InitSensor()
{	
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

void LogVector(const char* prefix, const float* v)
{
	GLog.LogInfo("%s %f %f %f", prefix, v[0], v[1], v[2]);
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

		//if (event.type == SENSOR_TYPE_ROTATION_VECTOR){
		//	rotationVec.Set(event.data);
		//}

		if (event.type == SENSOR_TYPE_MAGNETIC_FIELD) {
			magneticVec_.Set(event.data);
		}

		if (event.type == SENSOR_TYPE_GRAVITY) {
			gravityVec_.Set(event.data);
		}

		if (event.type == SENSOR_TYPE_GYROSCOPE) {
			LogVector("gyroscope", event.data);
		}

		if (event.type == SENSOR_TYPE_ORIENTATION) {
			LogVector("orientation", event.data);
			rotation_.Set(event.data);
		}

		if (event.type == SENSOR_TYPE_ROTATION_VECTOR){
			rotationVec_.Set(event.data);
		}
	}

	GLog.LogInfo("sensor event count %d", eventCount);
}

// 使用重力加速计和电子罗盘构造世界坐标系
Matrix4f _GetDeviceRotationMatrix2()
{
	magneticVec_.Normalize();
	gravityVec_.Normalize();

	Vector3f z = -magneticVec_;
	Vector3f x = gravityVec_;
	Vector3f y = CrossProduct(z, x);
	y.Normalize();
	x = CrossProduct(y, z);
	
	Matrix4f view(x.y, -x.x, -x.z, 0.f,
		y.y, -y.x, -y.z, 0.f,
		z.y, -z.x, -z.z, 0.f,
		0.f, 0.f, 0.f, 1.f);

	//GLog.LogInfo("magnetic vec %f %f %f", magneticVec_.x, magneticVec_.y, magneticVec_.z);

	return view;
}

// 使用欧拉角
Matrix4f _GetDeviceRotationMatrix1()
{
	Matrix3f frame, roll, pitch, yaw, view;

	frame = Matrix3f::IDENTITY;
	roll = Matrix3f::IDENTITY;
	pitch = Matrix3f::IDENTITY;
	yaw = Matrix3f::IDENTITY;
	view = Matrix3f::IDENTITY;


	MatrixRotationAxis(roll, Vector3f::UNIT_X, -rotation_.z * Mathf::DEG_TO_RAD);
	MatrixRotationAxis(pitch, Vector3f::UNIT_Y, rotation_.y * Mathf::DEG_TO_RAD);
	MatrixRotationAxis(yaw, Vector3f::UNIT_Z, -rotation_.x * Mathf::DEG_TO_RAD);
	
	MatrixMultiply(view, roll, pitch);
	MatrixMultiply(view, view, yaw);

	//frame.Set(1.0f, 0.f, 0.f,
	//	0.f, 0.f, -1.f,
	//	0.f, 1.f, 0.f);
	MatrixMultiply(view, frame, view);

	//view.TransposeSelf();

	Matrix4f view44;
	MatrixTransform(view44, view, Vector3f::ZERO);

	return view44;
}


Matrix4f _GetDeviceRotationMatrix()
{
	// 世界坐标系变换到手机坐标系
	Matrix4f mFrame;
	mFrame.Set(0.f, 1.f, 0.f, 0.f,
		-1.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f);

	// 应用手机的姿态
	float w = Mathf::Sqrt(1.0f - rotationVec_.LengthSQ());
	Quaternionf qRot(w, rotationVec_.x, rotationVec_.y, rotationVec_.z);
	Matrix4f mRot;
	MatrixTransform(mRot, qRot, Vector3f::ZERO);
	MatrixMultiply(mRot, mFrame, mRot);

	// 手机坐标系到世界坐标系
	mFrame.Set(0.f, -1.f, 0.f, 0.f,
		1.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f);
	MatrixMultiply(mRot, mRot, mFrame);

	// 矩阵作为坐标系
	mRot.TransposeSelf();

	return mRot;
}