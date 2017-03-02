#include "Platfrom.h"
#include <Windows.h>
#include <Mmsystem.h>
#include "ShaderManager.h"
#include "FileSystem.h"
#include "GlobalVar.h"
#include "glUtil.h"

const char* vsInclude = \
"#version 330 core\n"
"#define VERTEX_SHADER\n";

const char* psInclude = \
"#version 330 core\n"
"#define FRAGMENT_SHADER\n";



CRITICAL_SECTION TimeCS;
// timeGetTime() support with wrap.
uint32_t          OldMMTimeMs;
uint32_t          MMTimeWrapCounter;
// Cached performance frequency result.
uint64_t          PrefFrequency;

// Computed as (perfCounterNanos - ticksCounterNanos) initially,
// and used to adjust timing.
uint64_t          PerfMinusTicksDeltaNanos;
// Last returned value in nanoseconds, to ensure we don't back-step in time.
uint64_t          LastResultNanos;


#define  MMTimerResolutionNanos 1000000

#define  MsPerSecond 1000 // Milliseconds in one second.
#define  NanosPerSecond MsPerSecond * 1000 * 1000


uint64_t getFrequency()
{
	if (PrefFrequency == 0)
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		PrefFrequency = freq.QuadPart;
	}
	return PrefFrequency;
}


uint64_t GetTicksNanos()
{
	uint64_t          resultNanos;
	LARGE_INTEGER   li;
	DWORD           mmTimeMs;

	// On Win32 QueryPerformanceFrequency is unreliable due to SMP and
	// performance levels, so use this logic to detect wrapping and track
	// high bits.
	::EnterCriticalSection(&TimeCS);

	// Get raw value and perf counter "At the same time".
	mmTimeMs = timeGetTime();
	QueryPerformanceCounter(&li);

	if (OldMMTimeMs > mmTimeMs)
		MMTimeWrapCounter++;
	OldMMTimeMs = mmTimeMs;

	// Normalize to nanoseconds.
	uint64_t  mmCounterNanos = ((uint64_t(MMTimeWrapCounter) << 32) | mmTimeMs) * 1000000;
	uint64_t  frequency = getFrequency();
	uint64_t  perfCounterSeconds = uint64_t(li.QuadPart) / frequency;
	uint64_t  perfRemainderNanos = ((uint64_t(li.QuadPart) - perfCounterSeconds * frequency) *
		NanosPerSecond) / frequency;
	uint64_t  perfCounterNanos = perfCounterSeconds * NanosPerSecond + perfRemainderNanos;

	if (PerfMinusTicksDeltaNanos == 0)
		PerfMinusTicksDeltaNanos = perfCounterNanos - mmCounterNanos;


	// Compute result before snapping. 
	//
	// On first call, this evaluates to:
	//          resultNanos = mmCounterNanos.    
	// Next call, assuming no wrap:
	//          resultNanos = prev_mmCounterNanos + (perfCounterNanos - prev_perfCounterNanos).        
	// After wrap, this would be:
	//          resultNanos = snapped(prev_mmCounterNanos +/- 1ms) + (perfCounterNanos - prev_perfCounterNanos).
	//
	resultNanos = perfCounterNanos - PerfMinusTicksDeltaNanos;

	// Snap the range so that resultNanos never moves further apart then its target resolution.
	if (resultNanos > (mmCounterNanos + MMTimerResolutionNanos))
	{
		resultNanos = mmCounterNanos + MMTimerResolutionNanos;
		if (resultNanos < LastResultNanos)
			resultNanos = LastResultNanos;
		PerfMinusTicksDeltaNanos = perfCounterNanos - resultNanos;

	}
	else if (resultNanos < (mmCounterNanos - MMTimerResolutionNanos))
	{
		resultNanos = mmCounterNanos - MMTimerResolutionNanos;
		if (resultNanos < LastResultNanos)
			resultNanos = LastResultNanos;
		PerfMinusTicksDeltaNanos = perfCounterNanos - resultNanos;
	}

	LastResultNanos = resultNanos;
	::LeaveCriticalSection(&TimeCS);

	return resultNanos;
}

uint32_t	GetTicksMS()
{
	return (uint32_t)(GetTicksNanos() / 1000000);
}


void Platfrom::Init()
{
	timeBeginPeriod(1);
	InitializeCriticalSection(&TimeCS);
	MMTimeWrapCounter = 0;
	getFrequency();

	GFileSys->SetRootPath("../../resource/", true);
}

void Platfrom::Shutdown()
{
	DeleteCriticalSection(&TimeCS);
	timeEndPeriod(1);
}

const string& Platfrom::GetTempPath()
{
	static string path(".");
	return path;
}
