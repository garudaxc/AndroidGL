#pragma once
#include "Platfrom.h"
#include "AurMath.h"

namespace FancyTech
{
	struct TrackerSample
	{
		uint32_t	timestamp;
		float		temperature;

		Vector3f	accelerate;
		Vector3f	gyro;
		Vector3f	magnet;
	};

}