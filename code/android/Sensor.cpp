#include <android/sensor.h>
#include <android/looper.h>
//#include <android_native_app_glue.h>
#include <vector>
#include "AurMath.h"
#include "Platfrom.h"
#include "Thread.h"
#include "MyLog.h"
#include "MessageQueue.h"

using namespace std;
using namespace Aurora;


enum {
	SENSOR_TYPE_ACCELEROMETER = 1,
	SENSOR_TYPE_MAGNETIC_FIELD = 2,
	SENSOR_TYPE_ORIENTATION = 3,
	SENSOR_TYPE_GYROSCOPE = 4,
	SENSOR_TYPE_LIGHT = 5,
	SENSOR_TYPE_PROXIMITY = 8,
	SENSOR_TYPE_GRAVITY = 9,
	SENSOR_TYPE_ROTATION_VECTOR = 11, 
	SENSOR_TYPE_GAME_ROTATION_VECTOR = 15,

};


#define EVENT_IDEN 23
#define SENSOR_EVENT_PER_SEC 60

ASensorManager* sensorManager = NULL;
ASensorEventQueue* eventQueue = NULL;
ALooper* looper = NULL;

MessageQueue* messageQueue_ = NULL;


void LogVector(const char* prefix, const float* v)
{
	GLog.LogInfo("%s %f %f %f", prefix, v[0], v[1], v[2]);
}


class SensorFuse
{
public:
	SensorFuse();
	~SensorFuse();

	void UpdateGyroScope(const Vector3f& v, int64_t timestamp);
	void UpdateAccelerometer(const Vector3f& v);
	void UpdateMagneticField(const Vector3f& v);

	void Fuse();
	void Fuse2();

	Matrix4f GetViewMatrix();

private:

	Vector3f gravityVec_;
	Vector3f magneticVec_;
	//Vector3f gyroVec_;

	static float EPSILON;
	Quaternionf gyroRot_;
	Quaternionf accMagRot_;

	Quaternionf fusedRot_;

	bool init_;
};


float SensorFuse::EPSILON = 0.0001f;

SensorFuse::SensorFuse()
{
	init_ = true;
}

SensorFuse::~SensorFuse()
{
}

void IntigrateGyroValue(const ASensorEvent& event)
{
}

void SensorFuse::UpdateGyroScope(const Vector3f& v, int64_t timestamp)
{
	//intigrate GyroValue
	static int64_t lastTime = 0;
	if (init_ || lastTime == 0)
	{
		lastTime = timestamp;
		gyroRot_ = Quaternionf::RotationAxis(Vector3f::UNIT_Z, Mathf::HALF_PI);
		init_ = false;
		return;
	}
	float t = (timestamp - lastTime) * 0.000000001f;
	lastTime = timestamp;

	// 注意这里直接取逆了
	Vector3f eyroSample = -v;

	float omegaMagnitude = eyroSample.Length();
	if (omegaMagnitude < EPSILON) {
		return;
	}

	eyroSample.Normalize();

	float thetaOverTwo = omegaMagnitude * t * 0.5f;
	float sinThetaOverTwo = Mathf::Sin(thetaOverTwo);
	float cosThetaOverTwo = Mathf::Cos(thetaOverTwo);

	eyroSample *= sinThetaOverTwo;
	Quaternionf qRot(cosThetaOverTwo, eyroSample.x, eyroSample.y, eyroSample.z);

	gyroRot_ = qRot * gyroRot_;
}

void SensorFuse::UpdateAccelerometer(const Vector3f& v)
{
	gravityVec_ = v;
}

void SensorFuse::UpdateMagneticField(const Vector3f& v)
{
	magneticVec_ = v;
}

void SensorFuse::Fuse()
{
	// calculate rotation from magnetic and gravity frame
	gravityVec_.Normalize();
	Vector3f x, y, z;
	x = magneticVec_ - gravityVec_ * Vector3f::Dot(gravityVec_, magneticVec_);
	x.Normalize();

	z = gravityVec_;
	y = Vector3f::Cross(z, x);
	y.Normalize();

	// transform matrix from (real) world coordinate to phone coordinate
	Matrix4f view(x.x, x.y, x.z, 0.f,
		y.x, y.y, y.z, 0.f,
		z.x, z.y, z.z, 0.f,
		0.f, 0.f, 0.f, 1.f);

	Quaternionf qRot = view.ToQuaternion();
	if (accMagRot_.LengthSQ() < 0.001f)	{
		accMagRot_ = qRot;
		return;
	}

	float factor = 0.05f;	
	// low pass filter
	accMagRot_ = Quaternionf::Slerp(accMagRot_, qRot, factor);
	//////////////////////////////////////////////////////////////////////////
	
	// fuse
	fusedRot_ = Quaternionf::Slerp(gyroRot_, accMagRot_, factor);

	Vector3f v = Vector3f::UNIT_Z *	gyroRot_;
	v = Vector3f::Cross(v, gravityVec_);
	GLog.LogInfo("diff %f %f %f len %f", v.x, v.y, v.z, v.LengthSQ());

	// compensate gyro rotation
	gyroRot_ = fusedRot_;

}

// only use gravity direction to correct pitch
void SensorFuse::Fuse2()
{
	gravityVec_.Normalize();

	Vector3f v = Vector3f::UNIT_Z *	gyroRot_;
	Quaternionf tiltCorrect = Quaternionf::FromVectorToVector(v, gravityVec_);
	tiltCorrect = Quaternionf::Slerp(Quaternionf::IDENTITY, tiltCorrect, 0.05f);

	fusedRot_ = tiltCorrect * gyroRot_;

	gyroRot_ = fusedRot_;
}


Matrix4f SensorFuse::GetViewMatrix()
{
	Matrix4f view = Matrix4f::Transform(fusedRot_, Vector3f::ZERO);

	// transform from phone coordinate to camera coordinate
	// phone handled in reverseLandscape
	view *= Matrix4f(0.f, -1.f, 0.f, 0.f,
			1.f, 0.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f);

	return view;
}

SensorFuse sensorFuse;

Matrix4f _GetDeviceRotationMatrix()
{
	return sensorFuse.GetViewMatrix();
}


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
			//type == SENSOR_TYPE_ROTATION_VECTOR ||
			//type == SENSOR_TYPE_ORIENTATION ||
			type == SENSOR_TYPE_GRAVITY ||
			type == SENSOR_TYPE_GAME_ROTATION_VECTOR)
		{
			static bool bGyro = false;
			if (type == SENSOR_TYPE_GYROSCOPE){
				if (!bGyro)	{
					bGyro = true;
				} else {
					continue;
				}
			}

			//SensorObj(sensors[i], i);
			sensors_.push_back(SensorObj(sensors[i], i));
		}
	}
}


//Vector3f gravityVec_;
//Vector3f magneticVec_;
//Vector3f gyroVec_;
//Vector3f rotation_;
//Vector3f rotationVec_;

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


void IntigrateGyroValue(const ASensorEvent& event);

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
			//LogVector("MAGNETIC_FIELD", event.data);		

			Vector3f v;
			v.Set(event.data);
			//magneticVec_.Set(event.data);
			sensorFuse.UpdateMagneticField(v);
		}

		if (event.type == SENSOR_TYPE_GRAVITY) {
			Vector3f v;
			v.Set(event.data);
			//gravityVec_.Set(event.data);
			sensorFuse.UpdateAccelerometer(v);
		}

		if (event.type == SENSOR_TYPE_GYROSCOPE) {
			//LogVector("gyroscope", event.data);
			//GLog.LogInfo("time stamp %lld", event.timestamp);
			//IntigrateGyroValue(event);

			Vector3f v;
			v.Set(event.data);
			sensorFuse.UpdateGyroScope(v, event.timestamp);

		}

		if (event.type == SENSOR_TYPE_ORIENTATION) {
			//LogVector("orientation", event.data);
			//rotation_.Set(event.data);
		}

		if (event.type == SENSOR_TYPE_ROTATION_VECTOR ||
			event.type == SENSOR_TYPE_GAME_ROTATION_VECTOR){
			//rotationVec_.Set(event.data);
		}
	}

	sensorFuse.Fuse2();
	//GLog.LogInfo("sensor event count %d time %d", eventCount, GetTicksMS());
}

#if 0
// 使用重力加速计和电子罗盘构造世界坐标系
Matrix4f _GetDeviceRotationMatrix2()
{
	//LogVector("gravityVec_", gravityVec_.Ptr());

	gravityVec_.Normalize();
	Vector3f x, y, z;
	x = magneticVec_ - gravityVec_ * DotProduct(gravityVec_, magneticVec_);
	
	LogVector("north", x.Ptr());

	x.Normalize();

	z = gravityVec_;
	y = CrossProduct(z, x);
	y.Normalize();

	Matrix4f view(x.x, x.y, x.z, 0.f,
		y.x, y.y, y.z, 0.f,
		z.x, z.y, z.z, 0.f,
		0.f, 0.f, 0.f, 1.f);


	Matrix4f frame(0.f, -1.f, 0.f, 0.f,
		1.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f);

	MatrixMultiply(view, view, frame);

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

// 使用传感器ROTATION_VECTOR
Matrix4f _GetDeviceRotationMatrix2()
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


Matrix4f _GetDeviceRotationMatrix3()
{
	// 世界坐标系变换到手机坐标系
	Matrix4f mFrame;
	mFrame.Set(0.f, 1.f, 0.f, 0.f,
		-1.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f);

	Matrix4f mRot;
	MatrixTransform(mRot, GyroRot, Vector3f::ZERO);
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

#endif

//////////////////////////////////////////////////////////////////////////

#define COMMAND_INIT	"init"
#define COMMAND_START	"start"
#define COMMAND_PAUSE	"pause"
#define COMMAND_DESTROY	"destroy"

#define COMMAND_MATCH(cmd, toMatch)		( strcmp(cmd, toMatch) == 0 )


class SensorThread : public Thread
{
public:
	SensorThread();
	~SensorThread()	{}

	virtual void*		Run();
	
private:
	ALooper*	looper_;
	bool		initialized_;
	bool		running_;
};

SensorThread::SensorThread() :looper_(NULL), initialized_(false), running_(false)
{
}

void* SensorThread::Run()
{
	for (;;){
		if (!initialized_){
			messageQueue_->SleepUntilMessage();
			const char* cmd = messageQueue_->GetNextMessage();
			if (COMMAND_MATCH(cmd, COMMAND_INIT)) {
				looper_ = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
				if (looper_ == NULL) {
					GLog.LogError("ALooper_prepare failed!");
					return (void*)0;
				}

				_InitSensor();

				initialized_ = true;
				continue;
			} else {
				GLog.LogError("SensorThread error command %s", cmd);
				continue;
			}
		}

		// sleeping
		if (!running_){
			messageQueue_->SleepUntilMessage();
			const char* cmd = messageQueue_->GetNextMessage();
			if (COMMAND_MATCH(cmd, COMMAND_START)) {
				running_ = true;

				_EnableSensor();

				GLog.LogInfo("SensorThread start running");
				continue;
			} else {
				GLog.LogError("sensor thread expect start command : %s", cmd);
				continue;
			}
		} else {
			// running
			const char* cmd = messageQueue_->GetNextMessage();
			if (cmd == NULL) {
				// Read all pending events.
				int ident;
				int events;
				struct android_poll_source* source;

				if ((ident = ALooper_pollAll(20, NULL, &events, (void**)&source)) >= 0) {
					_ProcessSensorData(ident);
				}
			} else if (COMMAND_MATCH(cmd, COMMAND_PAUSE)) {
				GLog.LogInfo("SensorThread paused");
				
				_DisableSensor();
				running_ = false;
			}			
		}
	}

	return (void*)0;
}

SensorThread sensorThread;

void InitSensor()
{
	messageQueue_ = new MessageQueue(10);

	GLog.LogInfo("Create scesor thread %d", GetTicksMS());
	bool r = sensorThread.Create();
	if (!r) {
		GLog.LogError("sensorThread.Create() failed!");
	}

	GLog.LogInfo("Create scesor thread done ! %d", GetTicksMS());

	messageQueue_->SendString(COMMAND_INIT);
}

void StartSensor()
{
	messageQueue_->PostString(COMMAND_START);
}

void PauseSensor()
{
	messageQueue_->PostString(COMMAND_PAUSE);
}