#pragma once
#include "Platfrom.h"
#include "SensorDevice.h"
#include "RingBuffer.h"

namespace FancyTech
{

class Calibration
{
public:
	Calibration();
	~Calibration();

	void Apply(TrackerSample& sample);

	const Vector3f& GetOffest() const {
		return offset_;
	}

	float GetTemperature() const {
		return temperature_;
	}

	bool	IsCalibrated() const {
		return calibrated_;
	}

	size_t	GetNumSamples() const {
		return samples_.size();
	}

private:
	void	CalcGyroOffset(TrackerSample& sample);


	RingBuffer<Vector3f>	samples_;
	Vector3f	offset_;
	float		temperature_;
	bool		calibrated_;

};

extern Calibration GCalibration;

}