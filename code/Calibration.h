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




class GyroTempCalibration
{
public:
	void	Load();
	void	Save();

	struct DataEntry
	{
		uint32_t	version;
		float		actualTemperature;
		uint32_t	time;
		Vector3f	offset;

		DataEntry()
		{
			version = 2;
		}
	};

	enum { GyroCalibrationNumBins = 7 };
	enum { GyroCalibrationNumSamples = 5 };

	DataEntry data_[GyroCalibrationNumBins][GyroCalibrationNumSamples];

};




extern Calibration GCalibration;

}