#include "Calibration.h"
#include "MyLog.h"
#include "BitmapFont.h"
#include "SpriteBatch.h"
#include "Platfrom.h"
#include "Timer.h"
#include "Input.h"
#include <time.h>
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filewritestream.h"   // wrapper of C stream for prettywriter as output
#include "FileSystem.h"
#include <vector>
#include <stdlib.h>

namespace FancyTech
{


	TrackerSample drawSample;
	Vector3f smoothAccel_;

	Vector3f accelBase_;
	float accelBaseThreshold_ = 0.0001f;
	float gyroTransfromThreshold_ = 1.5f;


	struct GyrpTransfrom
	{
		RingBuffer<Vector3f>	samples;
		Vector3f axis;

		GyrpTransfrom()
		{
			samples.reserve(2000);
		}

	} gyroTransfrom_[3];
	


	Calibration::Calibration() :offset_(Vector3f::ZERO), calibrated_(false)
	{
		samples_.reserve(2000);
	}

	Calibration::~Calibration()
	{
	}


	Vector3f MeanValue(const RingBuffer<Vector3f>& buffer)
	{
		Vector3f v(Vector3f::ZERO);
		for (size_t i = 0; i < buffer.size(); i++) {
			v += buffer.at(i);
		}

		if (!buffer.empty()) {
			v /= (float)buffer.size();
		}

		return v;
	}


	void Calibration::CalcGyroOffset(TrackerSample& sample)
	{
		const float alpha = 0.4f;
		// 1.25f is a scaling factor related to conversion from per-axis comparison to length comparison
		const float absLimit = 1.25f * 0.349066f;
		//  const float noiseLimit = 1.25f * 0.03f;		// This was the original threshold based on reported device noise characteristics.
		const float noiseLimit = 0.0175f;			// This is the updated threshold based on analyzing ten GearVR devices and determining a value that best
		// discriminates between stationary on a desk and almost stationary when on the user's head.

		Vector3f gyro = sample.gyro;
		// do a moving average to reject short term noise
		Vector3f avg = (samples_.empty()) ? gyro : gyro * alpha + samples_.back() * (1 - alpha);

		// Make sure the absolute value is below what is likely motion
		// Make sure it is close enough to the current average that it is probably noise and not motion
		if (avg.Length() >= absLimit || (avg - MeanValue(samples_)).Length() >= noiseLimit) {
			samples_.clear();
			calibrated_ = false;
		}

		samples_.push_back(avg);

		if (samples_.full()) {
			offset_ = MeanValue(samples_);
			temperature_ = sample.temperature;
			calibrated_ = true;
			GGyroCalibration.SetValue(temperature_, offset_);
		}

		sample.gyro -= offset_;
	}

	void Calibration::Apply(TrackerSample& sample)
	{
		CalcGyroOffset(sample);

		Vector3f accel = sample.accelerate;
		accel.Normalize();

		float alpha = 0.01f;
		smoothAccel_ = accel * alpha + smoothAccel_ * (1.0f - alpha);

		float l = smoothAccel_.LengthSQ();
		accelBase_ = Vector3f(l, l, l) - Modulate(smoothAccel_, smoothAccel_);

		for (int i = 0; i < 3; i++)	{
			// 加速计的坐标系与世界坐标系对准
			// 陀螺仪的读数足够大
			if (accelBase_[i] < accelBaseThreshold_ && 
				sample.gyro.Length() > gyroTransfromThreshold_)	{
				Vector3f gyro = sample.gyro;
				gyro.Normalize();
				gyroTransfrom_[i].samples.push_back(gyro);
				if (gyroTransfrom_[i].samples.full()) {
					gyroTransfrom_[i].axis = MeanValue(gyroTransfrom_[i].samples);
				}
			}
		}

		drawSample = sample;
	}


	Calibration GCalibration;


	void DrawCalibration(int w, int h, BitmapFont& bitmapFont, SpriteBatch& spriteBatch)
	{

		float lineHeight = -30.0f;
		Vector3f pos(10.f, h - 10.f, 0.f);

		char buff[64];
		sprintf(buff, "fps %.2f", Time.GetFPS());
		bitmapFont.DrawString(&spriteBatch, buff, pos);

		pos.y += lineHeight;
		sprintf(buff, "samples %d", GCalibration.GetNumSamples());
		bitmapFont.DrawString(&spriteBatch, buff, pos);
		spriteBatch.Commit(w, h);

		pos.y += lineHeight;
		sprintf(buff, "accel    %+.4f %+.4f %+.4f", smoothAccel_.x, smoothAccel_.y, smoothAccel_.z);
		bitmapFont.DrawString(&spriteBatch, buff, pos);

		pos.y += lineHeight;
		sprintf(buff, "gyro(c)  %+.4f %+.4f %+.4f", drawSample.gyro.x, drawSample.gyro.y, drawSample.gyro.z);
		bitmapFont.DrawString(&spriteBatch, buff, pos);
		spriteBatch.Commit(w, h);


		pos.y += lineHeight;
		sprintf(buff, "accel base x %+.5f", accelBase_.x);
		bitmapFont.DrawString(&spriteBatch, buff, pos, 1.f, accelBase_.x < accelBaseThreshold_ ? Vector4f::GREEN : Vector4f::RED);
		pos.y += lineHeight;
		sprintf(buff, "accel base y %+.5f", accelBase_.y);
		bitmapFont.DrawString(&spriteBatch, buff, pos, 1.f, accelBase_.y < accelBaseThreshold_ ? Vector4f::GREEN : Vector4f::RED);
		pos.y += lineHeight;
		sprintf(buff, "accel base z %+.5f", accelBase_.z);
		bitmapFont.DrawString(&spriteBatch, buff, pos, 1.f, accelBase_.z < accelBaseThreshold_ ? Vector4f::GREEN : Vector4f::RED);
		spriteBatch.Commit(w, h);

		for (int i = 0; i < 3; i++)	{
			pos.y += lineHeight;
			Vector3f axis = gyroTransfrom_[i].axis;
			sprintf(buff, "gyro transform axis %d samples %d %+.4f %+.4f %+.4f", i, gyroTransfrom_[i].samples.size(), axis.x, axis.y, axis.z);
			bitmapFont.DrawString(&spriteBatch, buff, pos);
			spriteBatch.Commit(w, h);
		}

		if (GCalibration.IsCalibrated()){
			pos.y += lineHeight;
			Vector3f offset = GCalibration.GetOffest();
			sprintf(buff, "Temperature %.2f offset (%.3f %.3f %.3f)", GCalibration.GetTemperature(), offset.x, offset.y, offset.z);
			bitmapFont.DrawString(&spriteBatch, buff, pos);
		}

		spriteBatch.Commit(w, h);


		// 最新陀螺仪的矫正矩阵
		//GyroMatrix = Matrix4f(
		//	0.999407f, 0.024805f, 0.023882f, 0.f,
		//	-0.020954f, 0.988222f, -0.149530f, 0.f,
		//	-0.027318f, 0.148987f, 0.988462f, 0.f,
		//	0.f, 0.f, 0.f, 1.f);
	}

	

	void GyroTempCalibration::Init(const string& serial)
	{
		filename_ = string("GyroCalibration_") + serial + ".json";
		
		for (int i = 0; i < NumBins; i++){
			float temperature = 15.f + i * 5.f;
			for (int j = 0; j < NumSamples; j++) {
				DataEntry entry;
				entry.version = 2;
				entry.time = 0;
				entry.actualTemperature = temperature;
				entry.offset = Vector3f::ZERO;

				data_[i][j] = entry;
			}
		}
	}


	void SplitString(vector<string>& tokens, const string& str)
	{
		char split = ' ';
		size_t i = 0, j = 0;
		while (true) {
			j = str.find_first_of(split, i);
			if (j == string::npos) {
				string token = str.substr(i);
				tokens.push_back(token);
				break;
			} else {
				string token = str.substr(i, j - i);
				tokens.push_back(token);
				i = str.find_first_not_of(split, j);
				if (i == string::npos) {
					break;
				}
			}
		}
	}


	uint32_t StringToUInt32(const string& token)
	{
		char * endptr;
		uint32_t value = strtoul(token.c_str(), &endptr, 10);
		return value;
	}

	float StringToFloat(const string& token)
	{
		char * endptr;
		float value = strtof(token.c_str(), &endptr);
		return value;
	}


	void GyroTempCalibration::SetValue(float temperature, const Vector3f& offset)
	{
		uint32_t t = time(NULL);

		for (int i = 0; i < NumBins; i++){
			float targetTemperature = 15.f + i * 5.f;
			if (Mathf::Abs(temperature - targetTemperature) > 0.1f) {
				continue;
			}

			int oldest = 0;
			for (int j = 0; j < NumSamples; j++) {
				if (data_[i][oldest].time > data_[i][j].time) {
					oldest = j;
				}
			}

			DataEntry& entry = data_[i][oldest];

			entry.version = 2;
			entry.actualTemperature = temperature;
			entry.time = t;
			entry.offset = offset;
		}
	}

	void GyroTempCalibration::Load()
	{
		File* file = GFileSys->OpenFile(filename_);
		if (file == NULL) {
			GLog.LogError("GyroTempCalibration::Load() failed! %s", filename_.c_str());
			return;
		}

		vector<char> buffer(file->Size() + 1);
		file->Read(&buffer[0], file->Size());
		buffer[file->Size()] = '\0';
		file->Close();
		
		typedef rapidjson::Document WDocument;
		typedef rapidjson::Value WValue;

		rapidjson::Document document;
		document.Parse(&buffer[0]);

		if (document.HasParseError()) {
			rapidjson::ParseErrorCode err = document.GetParseError();
			GLog.LogError("pares error! %d %d", err, document.GetErrorOffset());
			return;
		}

		if (!document.IsObject()) {
			printf("GyroTempCalibration::Load() error, not a object");
			return;
		}

		int version = document["Calibration Version"].GetInt();
		string data = document["Data"].GetString();

		vector<string> tokens;
		SplitString(tokens, data);

		for (int i = 0; i < NumBins; i++){
			for (int j = 0; j < NumSamples; j++) {
				int index = i * 5 + j;
				DataEntry& entry = data_[i][j];
				entry.version =				StringToUInt32(tokens[index * 6 + 0].c_str());
				entry.actualTemperature =	StringToFloat (tokens[index * 6 + 1].c_str());
				entry.time =				StringToUInt32(tokens[index * 6 + 2].c_str());
				entry.offset.x =			StringToFloat (tokens[index * 6 + 3].c_str());
				entry.offset.y =			StringToFloat (tokens[index * 6 + 4].c_str());
				entry.offset.z =			StringToFloat (tokens[index * 6 + 5].c_str());
			}
		}
	}

	void GyroTempCalibration::Save()
	{
		string data;
		for (int i = 0; i < NumBins; i++){
			for (int j = 0; j < NumSamples; j++) {				
				const DataEntry& entry = data_[i][j];

				char buffer[512];
				sprintf(buffer, "%u %f %u %f %f %f ", entry.version, entry.actualTemperature, 
					entry.time, entry.offset.x, entry.offset.y, entry.offset.z);
				data += buffer;			
			}
		}

		rapidjson::Document doc;

		rapidjson::Value v(data.c_str(), doc.GetAllocator());
		doc.SetObject()
			.AddMember("Calibration Version", 2, doc.GetAllocator())
			.AddMember("Data", v, doc.GetAllocator());

		char buffer[1024];
		string name = Platfrom::GetTempPath() + "/test.json";
		FILE* pf = fopen(name.c_str(), "wb+");
		if (pf == NULL)	{
			GLog.LogError("GyroTempCalibration::Save() failed");
			return;
		}

		rapidjson::FileWriteStream f(pf, buffer, 1024);
		rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(f);
		doc.Accept(writer);

		fclose(pf);
	}

	GyroTempCalibration  GGyroCalibration;




	class LogGyroTransform : public EventReceiver
	{
	public:

		bool	OnEvent(const Event& event)	{
			if (event.Type == Event::LButtonUp ||
				event.Type == Event::TouchUp)
			{
				GLog.LogInfo("GyroTransform");
				for (int i = 0; i < 3; i++)	{
					Vector3f axis = gyroTransfrom_[i].axis;
					GLog.LogInfo("%ff, %ff, %ff, 1.0f", axis.x, axis.y, axis.z);
				}

				GGyroCalibration.Save();
			}
			return true;
		}

	};

	static LogGyroTransform logger;


}