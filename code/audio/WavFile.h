#pragma once
#include "Platfrom.h"

struct FOURCC_
{
	union
	{
		char cc[4];
		uint32_t	ci;
	};

	bool operator == (const char* o)
	{
		return
			(cc[0] == o[0]) &&
			(cc[1] == o[1]) &&
			(cc[2] == o[2]) &&
			(cc[3] == o[3]);
	}

	bool operator == (const FOURCC_& o)
	{
		return (ci == o.ci);
	}

	bool operator != (const char* o)
	{
		return !(*this == o);
	}

	bool operator != (const FOURCC_& o)
	{
		return !(*this == o);
	}
};


struct PCMInfo
{
	uint16_t	channels;
	uint32_t	samplesPerSec;
	uint16_t	bitsPerSample;
	uint16_t	containerSize;
};



class WavFile
{
public:
	WavFile();
	~WavFile();

	const void*		GetPCMData() const;
	uint32_t		GetPCMDataLength() const;
	void			GetPCMInfo(PCMInfo& info) const;	

	bool		Load(const char* pathName);
private:

	struct waveformat_extended_tag {
		uint16_t	wFormatTag;
		uint16_t	nChannels;
		uint32_t	nSamplesPerSec;
		uint32_t	nAvgBytesPerSec;
		uint16_t	nBlockAlign;
		uint16_t	wBitsPerSample;
		//uint16_t	cbSize;
	};

	char*		pcmData_;
	uint32_t	pcmDataLenth_;

	waveformat_extended_tag tag_;
};
