#pragma once
#include "Platfrom.h"


class Time_t
{
public:
	Time_t();
	~Time_t();

	float		GetTime() const;
	uint32_t	GetTimeMS() const;

	uint32_t	GetDeltaTimeMS() const;

	float		GetFPS() const;

	uint32_t	GetFrameCount() const;
	
private:
	Time_t(const Time_t& t)	{}
	
};

extern Time_t Time;

