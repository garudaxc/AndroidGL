#include "audio/WavFile.h"
#include "FileSystem.h"
#include "MyLog.h"

using namespace FancyTech;

WavFile::WavFile() :pcmData_(NULL), pcmDataLenth_(0)
{
}

WavFile::~WavFile()
{
}

bool WavFile::Load(const char* pathName)
{
	File* file = GFileSys->OpenFile(pathName);
	if (file == NULL) { 
		return false;
	}

	FOURCC_ cc;
	file->Read(cc);
	if (cc != "RIFF"){
		GLog.LogError("wav file format error! not start with \"RIFF\" %s", pathName);
		file->Close();
		return false;
	}

	uint32_t chunkLen = 0;
	file->Read(chunkLen);

	file->Read(cc);
	if (cc != "WAVE"){
		GLog.LogError("wav file format error! not find id \"WAVE\" %s", pathName);
		file->Close();
		return false;
	}

	file->Read(cc);		// fmt
	file->Read(chunkLen);	// 16 or 18

	file->Read(tag_);

	if (tag_.wFormatTag != 1){
		GLog.LogError("wav file format error! not a PCM format! %s", pathName);
		file->Close();
		return false;
	}

	if (chunkLen > sizeof(tag_))	{
		uint16_t extSize;
		file->Read(extSize);
		if (extSize > 0){
			//assert(extSize < 128);
			char buffer[128];
			file->Read(buffer, extSize);
		}
	}
	
	file->Read(cc);
	if (cc != "data"){
		GLog.LogError("wav file format error! invlid chunk id \"data\" %s", pathName);
		file->Close();
		return false;
	}

	file->Read(chunkLen);

	pcmDataLenth_ = chunkLen;
	pcmData_ = new char[pcmDataLenth_];
	if (pcmData_ == NULL) {
		GLog.LogError("wav file alloc pam data buffer failed size=%u %s", pcmDataLenth_, pathName);
		file->Close();
		return false;
	}

	file->Read(pcmData_, pcmDataLenth_);
	file->Close();
	
	return true;
}

const void* WavFile::GetPCMData() const
{
	return pcmData_;
}

uint32_t WavFile::GetPCMDataLength() const
{
	return pcmDataLenth_;
}

void WavFile::GetPCMInfo(PCMInfo& info) const
{
	info.channels		= tag_.nChannels;
	info.samplesPerSec	= tag_.nSamplesPerSec;
	info.bitsPerSample	= tag_.wBitsPerSample;
	info.containerSize = tag_.wBitsPerSample;
}

