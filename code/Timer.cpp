#include "Timer.h"
#include "Platfrom.h"


uint32_t LastUpdateTime = 0;
uint32_t DeltaTime = 0;
uint32_t FrameCount = 0;

float fps = 60.f;
uint32_t LastFpsTime = 0;
uint32_t LastFpsFrame = 0;

Time_t Time;

Time_t::Time_t()
{
}

Time_t::~Time_t()
{
}

void Time_t::Reset()
{
	LastUpdateTime = GetTicksMS();
	FrameCount = 0;
	LastFpsTime = LastUpdateTime;
	LastFpsFrame = 0;
}

float Time_t::GetTime() const
{
	return GetTicksMS() / 1000.f;
}

uint32_t Time_t::GetTimeMS() const
{
	return ::GetTicksMS();
}

uint32_t Time_t::GetDeltaTimeMS() const
{
	return DeltaTime;
}

float Time_t::GetFPS() const
{
	return fps;
}

uint32_t Time_t::GetFrameCount() const
{
	return FrameCount;
}

// should only called by framework every frame
void _UpdateTimer()
{
	uint32_t time = GetTicksMS();
	DeltaTime = time - LastUpdateTime;
	LastUpdateTime = time;

	FrameCount++;

	uint32_t fpsTime = time - LastFpsTime;
	if (fpsTime > 1000) {
		fps = ((FrameCount - LastFpsFrame) * 1000) / (float)fpsTime;

		LastFpsTime = time;
		LastFpsFrame = FrameCount;
	}
}

