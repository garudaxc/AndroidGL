// FbxConverter.cpp : 定义控制台应用程序的入口点。
//
#include <stdio.h>
#include <tchar.h>
#include <stdarg.h>
#include <vector>

#include "FbxConverter.h"

using namespace std;

Argument Args;


vector<FbxNode*>	Nodes_;
vector<FbxMesh*>	Meshes_;


//int __cdecl MyPrintf (const char *format, ...)
//{	
//	//DWORD nRet=0;
//	char buf[200]={0};//用来输出字符的缓冲区
//
//	va_list arglist;
//	va_start(arglist, format);
//	vsprintf(buf, format, arglist);
//	
//	//wsprintf(buf, format, arglist);
//	va_end(arglist);
//
//	//WriteConsoleA(hOutputHandle,(void*)buf,strlen(buf),&nRet,NULL);
//
//	return nRet;
//}






void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
{
	//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
	pManager = FbxManager::Create();
	if (!pManager){
		FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		exit(1);
	}
	else FBXSDK_printf("Autodesk FBX SDK version %s\n", pManager->GetVersion());

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(ios);

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	pManager->LoadPluginsDirectory(lPath.Buffer());

	//Create an FBX scene. This object holds most objects imported/exported from/to files.
	pScene = FbxScene::Create(pManager, "My Scene");
	if (!pScene){
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		exit(1);
	}
}

void DestroySdkObjects(FbxManager* pManager, bool pExitStatus)
{
	//Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
	if (pManager) pManager->Destroy();
	if (pExitStatus) FBXSDK_printf("Program Success!\n");
}


bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename)
{
	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor, lSDKMinor, lSDKRevision;
	//int lFileFormat = -1;
	int i, lAnimStackCount;
	bool lStatus;
	char lPassword[1024];

	// Get the file version number generate by the FBX SDK.
	FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create(pManager, "");

	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if (!lImportStatus)	{
		FbxString error = lImporter->GetStatus().GetErrorString();
		FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
		FBXSDK_printf("Error returned: %s\n\n", error.Buffer());

		if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)	{
			FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
			FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
		}

		return false;
	}

	FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

	if (lImporter->IsFBX()){
		FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

		// From this point, it is possible to access animation stack information without
		// the expense of loading the entire file.

		FBXSDK_printf("Animation Stack Information\n");

		lAnimStackCount = lImporter->GetAnimStackCount();

		FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
		FBXSDK_printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
		FBXSDK_printf("\n");

		for (i = 0; i < lAnimStackCount; i++){
			FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

			FBXSDK_printf("    Animation Stack %d\n", i);
			FBXSDK_printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
			FBXSDK_printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

			// Change the value of the import name if the animation stack should be imported 
			// under a different name.
			FBXSDK_printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

			// Set the value of the import state to false if the animation stack should be not
			// be imported. 
			FBXSDK_printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
			FBXSDK_printf("\n");
		}

		// Set the import states. By default, the import states are always set to 
		// true. The code below shows how to change these states.
		IOS_REF.SetBoolProp(IMP_FBX_MATERIAL, true);
		IOS_REF.SetBoolProp(IMP_FBX_TEXTURE, true);
		IOS_REF.SetBoolProp(IMP_FBX_LINK, true);
		IOS_REF.SetBoolProp(IMP_FBX_SHAPE, true);
		IOS_REF.SetBoolProp(IMP_FBX_GOBO, true);
		IOS_REF.SetBoolProp(IMP_FBX_ANIMATION, true);
		IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	}

	// Import the scene.
	lStatus = lImporter->Import(pScene);

	if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError){
		FBXSDK_printf("Please enter password: ");

		lPassword[0] = '\0';

		FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
			scanf("%s", lPassword);
		FBXSDK_CRT_SECURE_NO_WARNING_END

		FbxString lString(lPassword);

		IOS_REF.SetStringProp(IMP_FBX_PASSWORD, lString);
		IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

		lStatus = lImporter->Import(pScene);

		if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError){
			FBXSDK_printf("\nPassword is wrong, import aborted.\n");
		}
	}

	// Destroy the importer.
	lImporter->Destroy();

	return lStatus;
}

void ListSceneNode(FbxNode* node)
{
	if (node == NULL) {
		return;
	}
	Nodes_.push_back(node);

	if (node->GetMesh() != NULL) {
		Meshes_.push_back(node->GetMesh());
	}

	/*	FbxNodeAttribute::EType lAttributeType = (node->GetNodeAttribute()->GetAttributeType());

	switch (lAttributeType)
	{
	default:
		break;
	case FbxNodeAttribute::eMarker:
		//DisplayMarker(pNode);
		break;

	case FbxNodeAttribute::eSkeleton:
		//DisplaySkeleton(pNode);
		break;

	case FbxNodeAttribute::eMesh: 
	{
		FbxMesh* mesh = (FbxMesh*)node->GetNodeAttribute();
		Meshes_.push_back(mesh);
		break;
	}

		case FbxNodeAttribute::eNurbs:
		DisplayNurb(pNode);
		break;

		case FbxNodeAttribute::ePatch:
		DisplayPatch(pNode);
		break;

		case FbxNodeAttribute::eCamera:
		DisplayCamera(pNode);
		break;

		case FbxNodeAttribute::eLight:
		DisplayLight(pNode);
		break;

		case FbxNodeAttribute::eLODGroup:
		DisplayLodGroup(pNode);
		break;
	}*/

	//DisplayUserProperties(pNode);
	//DisplayTarget(pNode);
	//DisplayPivotsAndLimits(pNode);
	//DisplayTransformPropagation(pNode);
	//DisplayGeometricTransform(pNode);

	for (int i = 0; i < node->GetChildCount(); i++){
		ListSceneNode(node->GetChild(i));
	}

}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc <= 1) {
		const char* helpInfo = \
			"usage :\n"
			"FbxConverter [-v] [-c] [-o path] file\n"
			"file : input fbx file\n"
			"-v : flip texcoord v\n"
			"-c : change coordinage system\n"
			"-o : custom ouput path\n";

		FBXSDK_printf(helpInfo);
		return 0;
	}

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-c") == 0) {
			Args.convertAxis = true;

		} else if (strcmp(argv[i], "-v") == 0) {
			Args.flipTexcoordV = true;

		} else if (strcmp(argv[i], "-o") == 0) {
			Args.outputPath = argv[i + 1];
			if (Args.outputPath.back() != '\\' &&
				Args.outputPath.back() != '/') {
				Args.outputPath += '\\';
			}

			i++;
		} else {
			Args.inputFile = argv[i];
		}
	}


	FbxManager* lSdkManager = NULL;
	FbxScene* lScene = NULL;
	bool lResult;

	// Prepare the FBX SDK.
	InitializeSdkObjects(lSdkManager, lScene);
	// Load the scene.

	// The example can take a FBX file as an argument.
	FbxString lFilePath("a.fbx");

	if (lFilePath.IsEmpty()){
		lResult = false;
		FBXSDK_printf("\n\nUsage: ImportScene <FBX file name>\n\n");
	}else{
		FBXSDK_printf("\n\nFile: %s\n\n", lFilePath.Buffer());
		lResult = LoadScene(lSdkManager, lScene, lFilePath.Buffer());
	}

	if (lResult == false){
		FBXSDK_printf("\n\nAn error occurred while loading the scene...");
		return 0;
	}


	FbxNode* node = lScene->GetRootNode();
	ListSceneNode(node);

	FBXSDK_printf("number of scene node : %d\n", Nodes_.size());
	for (size_t i = 0; i < Nodes_.size(); i++) {
		FBXSDK_printf("node %d %s\n", i, Nodes_[i]->GetName());
	}

	FBXSDK_printf("number of meshes : %d\n", Meshes_.size());
	for (size_t i = 0; i < Meshes_.size(); i++) {
		FBXSDK_printf("convert mesh %d %s\n", i, Meshes_[i]->GetNode()->GetName());
		ConvertMesh(Meshes_[i]);
	}


	// Destroy all objects created by the FBX SDK.
	DestroySdkObjects(lSdkManager, lResult);
	return 0;
}

