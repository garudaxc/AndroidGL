#include <android/sensor.h>
#include "MyLog.h"
#include "AurMath.h"


using namespace FancyTech;

enum State
{
	Ready = 0,
	Dropping,
	Colliding,
	Idle,
};

int state			= Ready;
int64_t timeStamp	= 0;
float dropHeight	= 0.f;

float averageImpact = 0.f;
int nImpact = 0;


// use accelerometer track the dropping process
// measuring time and estimate dropping height
void TrackDropping(const ASensorEvent& event){

	Vector3f v(event.acceleration.x, event.acceleration.y, event.acceleration.z);
	float lenth = v.Length();

	if (state == Ready) {
		// detect weightlessness
		if (lenth < 1.0f) {
			timeStamp = event.timestamp;
			state = Dropping;
		}
	}

	if (state == Dropping) {
		// detect collide
		if (lenth > ASENSOR_STANDARD_GRAVITY) {
			// in milliseconds, timestamp is in nanoseconds
			int64_t during = (event.timestamp - timeStamp) / 1000000;
			float sec = during / 1000.0f;

			dropHeight = 0.5f * ASENSOR_STANDARD_GRAVITY * sec * sec;

			state = Colliding;
			return;
		}
	}

	if (state == Colliding) {
		averageImpact += lenth;
		nImpact++;

		if (lenth < ASENSOR_STANDARD_GRAVITY) {
			averageImpact /= nImpact;
			GLog.LogInfo("Drop height %.2f impact %.2f", dropHeight, averageImpact);

			averageImpact = 0.f;
			nImpact = 0;
			timeStamp = event.timestamp;
			state = Idle;
		}
		return;
	}

	if (state == Idle) {
		if ((event.timestamp - timeStamp) / 1000000 > 3000) {
			state = Ready;
		}
	}
}

