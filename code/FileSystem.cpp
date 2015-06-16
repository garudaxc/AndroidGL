#include "FileSystem.h"
#include "MyLog.h"


File::File() :pf_(NULL)
{
}

File::~File()
{
	this->Close();
}


bool File::Open(const char* fileName)
{
	FILE* pf = fopen(fileName, "rb");
	if (pf == NULL){
		GLog.LogError("File::Open failed! %s", fileName);
		return false;
	}

	pf_ = pf;
	return true;
}


void File::Close()
{
	if (pf_ != NULL){
		fclose(pf_);
		pf_ = NULL;
	}
}

bool File::Read(void* data, int size)
{
	if (pf_ == NULL){
		return false;
	}

	if (fread(data, size, 1, pf_) != 1){
		return false;
	}

	return true;
}

