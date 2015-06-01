#include "fbxsdk.h"
#include <string>
#include <stdio.h>

using namespace std;


int __cdecl MyPrintf(const char *format, ...);

//#define FBXSDK_printf printf



#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif


void DisplayString(const char* pHeader, const char* pValue = "", const char* pSuffix = "");
void DisplayBool(const char* pHeader, bool pValue, const char* pSuffix = "");
void DisplayInt(const char* pHeader, int pValue, const char* pSuffix = "");
void DisplayDouble(const char* pHeader, double pValue, const char* pSuffix = "");
void Display2DVector(const char* pHeader, FbxVector2 pValue, const char* pSuffix = "");
void Display3DVector(const char* pHeader, FbxVector4 pValue, const char* pSuffix = "");
void DisplayColor(const char* pHeader, FbxColor pValue, const char* pSuffix = "");
void Display4DVector(const char* pHeader, FbxVector4 pValue, const char* pSuffix = "");


struct Argument
{
	string		inputFile;
	string		outputPath;
	bool		convertAxis;
	bool		flipTexcoordV;

	
	Argument()
	{
		convertAxis = false;
		flipTexcoordV = false;
	}
};

extern Argument Args;

void ConvertMesh(FbxMesh* mesh);


