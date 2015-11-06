#pragma once
#include "Platfrom.h"
#include "AurMath.h"

using namespace Aurora;



struct TrackerSample
{
	uint32_t	timestamp;
	int			temperature;
	
	Vector3f	accelerate;
	Vector3f	gyro;
	Vector3f	magnet;
};
