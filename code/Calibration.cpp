#include "Calibration.h"
#include "MyLog.h"

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


void Calibration::Apply(TrackerSample& sample)
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

Calibration GCalibration;