#include "Calibration.h"
#include "MyLog.h"
#include "BitmapFont.h"
#include "SpriteBatch.h"
#include "Platfrom.h"
#include "Timer.h"
#include "Input.h"

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
	}


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
			}
			return true;
		}

	};

	static LogGyroTransform logger;


}