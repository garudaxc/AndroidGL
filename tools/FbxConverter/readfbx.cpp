#include "fbxsdk.h"

//extern HANDLE　hOutputHandle;
//int __cdecl MyPrintf (const char *format, ...)
//{	
//	DWORD nRet=0;
//	char buf[200]={0};//用来输出字符的缓冲区
//
//	va_list arglist;
//	va_start(arglist, format);
//	vsprintf(buf, format, arglist);
//	//wsprintf(buf, format, arglist);
//	va_end(arglist);
//
//	WriteConsoleA(hOutputHandle,(void*)buf,strlen(buf),&nRet,NULL);
//
//	return nRet;
//}

int __cdecl MyPrintf (const char *format, ...);

#define FBXSDK_printf							MyPrintf

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif


void DisplayMetaDataConnections(FbxObject* pNode);
void DisplayString(const char* pHeader, const char* pValue  = "", const char* pSuffix  = "");
void DisplayBool(const char* pHeader, bool pValue, const char* pSuffix  = "");
void DisplayInt(const char* pHeader, int pValue, const char* pSuffix  = "");
void DisplayDouble(const char* pHeader, double pValue, const char* pSuffix  = "");
void Display2DVector(const char* pHeader, FbxVector2 pValue, const char* pSuffix  = "");
void Display3DVector(const char* pHeader, FbxVector4 pValue, const char* pSuffix  = "");
void DisplayColor(const char* pHeader, FbxColor pValue, const char* pSuffix  = "");
void Display4DVector(const char* pHeader, FbxVector4 pValue, const char* pSuffix  = "");

void DisplayMesh(FbxNode* pNode);
void DisplayContent(FbxScene* pScene);
void DisplayContent(FbxNode* pNode);
void DisplayTarget(FbxNode* pNode);
void DisplayTransformPropagation(FbxNode* pNode);
void DisplayGeometricTransform(FbxNode* pNode);
void DisplayMetaData(FbxScene* pScene);


void DisplayControlsPoints(FbxMesh* pMesh);
void DisplayPolygons(FbxMesh* pMesh);
void DisplayMaterialMapping(FbxMesh* pMesh);
void DisplayTextureMapping(FbxMesh* pMesh);
void DisplayTextureNames( FbxProperty &pProperty, FbxString& pConnectionString );
void DisplayMaterialConnections(FbxMesh* pMesh);
void DisplayMaterialTextureConnections( FbxSurfaceMaterial* pMaterial, 
	char * header, int pMatId, int l );



void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
{
	//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
	pManager = FbxManager::Create();
	if( !pManager )
	{
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
	if( !pScene )
	{
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		exit(1);
	}
}

void DestroySdkObjects(FbxManager* pManager, bool pExitStatus)
{
	//Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
	if( pManager ) pManager->Destroy();
	if( pExitStatus ) FBXSDK_printf("Program Success!\n");
}



void DisplayContent(FbxScene* pScene)
{
	int i;
	FbxNode* lNode = pScene->GetRootNode();

	if(lNode)
	{
		for(i = 0; i < lNode->GetChildCount(); i++)
		{
			DisplayContent(lNode->GetChild(i));
		}
	}
}

void DisplayContent(FbxNode* pNode)
{
	FbxNodeAttribute::EType lAttributeType;
	int i;

	if(pNode->GetNodeAttribute() == NULL)
	{
		FBXSDK_printf("NULL Node Attribute\n\n");
	}
	else
	{
		lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());

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
			DisplayMesh(pNode);
			break;

	/*	case FbxNodeAttribute::eNurbs:      
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
			break;*/
		}   
	}

	//DisplayUserProperties(pNode);
	//DisplayTarget(pNode);
	//DisplayPivotsAndLimits(pNode);
	//DisplayTransformPropagation(pNode);
	//DisplayGeometricTransform(pNode);

	for(i = 0; i < pNode->GetChildCount(); i++)
	{
		DisplayContent(pNode->GetChild(i));
	}
}


bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename)
{
	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor,  lSDKMinor,  lSDKRevision;
	//int lFileFormat = -1;
	int i, lAnimStackCount;
	bool lStatus;
	char lPassword[1024];

	// Get the file version number generate by the FBX SDK.
	FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create(pManager,"");

	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if( !lImportStatus )
	{
		FbxString error = lImporter->GetStatus().GetErrorString();
		FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
		FBXSDK_printf("Error returned: %s\n\n", error.Buffer());

		if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
		{
			FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
			FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
		}

		return false;
	}

	FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

	if (lImporter->IsFBX())
	{
		FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

		// From this point, it is possible to access animation stack information without
		// the expense of loading the entire file.

		FBXSDK_printf("Animation Stack Information\n");

		lAnimStackCount = lImporter->GetAnimStackCount();

		FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
		FBXSDK_printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
		FBXSDK_printf("\n");

		for(i = 0; i < lAnimStackCount; i++)
		{
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
		IOS_REF.SetBoolProp(IMP_FBX_MATERIAL,        true);
		IOS_REF.SetBoolProp(IMP_FBX_TEXTURE,         true);
		IOS_REF.SetBoolProp(IMP_FBX_LINK,            true);
		IOS_REF.SetBoolProp(IMP_FBX_SHAPE,           true);
		IOS_REF.SetBoolProp(IMP_FBX_GOBO,            true);
		IOS_REF.SetBoolProp(IMP_FBX_ANIMATION,       true);
		IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	}

	// Import the scene.
	lStatus = lImporter->Import(pScene);

	if(lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
	{
		FBXSDK_printf("Please enter password: ");

		lPassword[0] = '\0';

		FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
			scanf("%s", lPassword);
		FBXSDK_CRT_SECURE_NO_WARNING_END

			FbxString lString(lPassword);

		IOS_REF.SetStringProp(IMP_FBX_PASSWORD,      lString);
		IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

		lStatus = lImporter->Import(pScene);

		if(lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
		{
			FBXSDK_printf("\nPassword is wrong, import aborted.\n");
		}
	}

	// Destroy the importer.
	lImporter->Destroy();

	return lStatus;
}


int TestFbx()
{
	FbxManager* lSdkManager = NULL;
	FbxScene* lScene = NULL;
	bool lResult;

	// Prepare the FBX SDK.
	InitializeSdkObjects(lSdkManager, lScene);
	// Load the scene.

	// The example can take a FBX file as an argument.
	FbxString lFilePath("d:\\a.fbx");

	if( lFilePath.IsEmpty() )
	{
		lResult = false;
		FBXSDK_printf("\n\nUsage: ImportScene <FBX file name>\n\n");
	}
	else
	{
		FBXSDK_printf("\n\nFile: %s\n\n", lFilePath.Buffer());
		lResult = LoadScene(lSdkManager, lScene, lFilePath.Buffer());
	}

	if(lResult == false)
	{
		FBXSDK_printf("\n\nAn error occurred while loading the scene...");
	}
	else 
	{	
		DisplayContent(lScene);
	}

	// Destroy all objects created by the FBX SDK.
	DestroySdkObjects(lSdkManager, lResult);

	return 0;
}


void DisplayTarget(FbxNode* pNode)
{
	if(pNode->GetTarget() != NULL)
	{
		DisplayString("    Target Name: ", (char *) pNode->GetTarget()->GetName());
	}
}

void DisplayTransformPropagation(FbxNode* pNode)
{
	FBXSDK_printf("    Transformation Propagation\n");

	// 
	// Rotation Space
	//
	EFbxRotationOrder lRotationOrder;
	pNode->GetRotationOrder(FbxNode::eSourcePivot, lRotationOrder);

	FBXSDK_printf("        Rotation Space: ");

	switch (lRotationOrder)
	{
	case eEulerXYZ: 
		FBXSDK_printf("Euler XYZ\n");
		break;
	case eEulerXZY:
		FBXSDK_printf("Euler XZY\n");
		break;
	case eEulerYZX:
		FBXSDK_printf("Euler YZX\n");
		break;
	case eEulerYXZ:
		FBXSDK_printf("Euler YXZ\n");
		break;
	case eEulerZXY:
		FBXSDK_printf("Euler ZXY\n");
		break;
	case eEulerZYX:
		FBXSDK_printf("Euler ZYX\n");
		break;
	case eSphericXYZ:
		FBXSDK_printf("Spheric XYZ\n");
		break;
	}

	//
	// Use the Rotation space only for the limits
	// (keep using eEulerXYZ for the rest)
	//
	FBXSDK_printf("        Use the Rotation Space for Limit specification only: %s\n",
		pNode->GetUseRotationSpaceForLimitOnly(FbxNode::eSourcePivot) ? "Yes" : "No");


	//
	// Inherit Type
	//
	FbxTransform::EInheritType lInheritType;
	pNode->GetTransformationInheritType(lInheritType);

	FBXSDK_printf("        Transformation Inheritance: ");

	switch (lInheritType)
	{
	case FbxTransform::eInheritRrSs:
		FBXSDK_printf("RrSs\n");
		break;
	case FbxTransform::eInheritRSrs:
		FBXSDK_printf("RSrs\n");
		break;
	case FbxTransform::eInheritRrs:
		FBXSDK_printf("Rrs\n");
		break;
	}
}

void DisplayGeometricTransform(FbxNode* pNode)
{
	FbxVector4 lTmpVector;

	FBXSDK_printf("    Geometric Transformations\n");

	//
	// Translation
	//
	lTmpVector = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	FBXSDK_printf("        Translation: %f %f %f\n", lTmpVector[0], lTmpVector[1], lTmpVector[2]);

	//
	// Rotation
	//
	lTmpVector = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	FBXSDK_printf("        Rotation:    %f %f %f\n", lTmpVector[0], lTmpVector[1], lTmpVector[2]);

	//
	// Scaling
	//
	lTmpVector = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
	FBXSDK_printf("        Scaling:     %f %f %f\n", lTmpVector[0], lTmpVector[1], lTmpVector[2]);
}


void DisplayMetaData(FbxScene* pScene)
{
	FbxDocumentInfo* sceneInfo = pScene->GetSceneInfo();
	if (sceneInfo)
	{
		FBXSDK_printf("\n\n--------------------\nMeta-Data\n--------------------\n\n");
		FBXSDK_printf("    Title: %s\n", sceneInfo->mTitle.Buffer());
		FBXSDK_printf("    Subject: %s\n", sceneInfo->mSubject.Buffer());
		FBXSDK_printf("    Author: %s\n", sceneInfo->mAuthor.Buffer());
		FBXSDK_printf("    Keywords: %s\n", sceneInfo->mKeywords.Buffer());
		FBXSDK_printf("    Revision: %s\n", sceneInfo->mRevision.Buffer());
		FBXSDK_printf("    Comment: %s\n", sceneInfo->mComment.Buffer());

		FbxThumbnail* thumbnail = sceneInfo->GetSceneThumbnail();
		if (thumbnail)
		{
			FBXSDK_printf("    Thumbnail:\n");

			switch (thumbnail->GetDataFormat())
			{
			case FbxThumbnail::eRGB_24:
				FBXSDK_printf("        Format: RGB\n");
				break;
			case FbxThumbnail::eRGBA_32:
				FBXSDK_printf("        Format: RGBA\n");
				break;
			}

			switch (thumbnail->GetSize())
			{
			default:
				break;
			case FbxThumbnail::eNotSet:
				FBXSDK_printf("        Size: no dimensions specified (%ld bytes)\n", thumbnail->GetSizeInBytes());
				break;
			case FbxThumbnail::e64x64:
				FBXSDK_printf("        Size: 64 x 64 pixels (%ld bytes)\n", thumbnail->GetSizeInBytes());
				break;
			case FbxThumbnail::e128x128:
				FBXSDK_printf("        Size: 128 x 128 pixels (%ld bytes)\n", thumbnail->GetSizeInBytes());
			}
		}
	}
}


void DisplayPolygons(FbxMesh* pMesh)
{
	int i, j, lPolygonCount = pMesh->GetPolygonCount();
	FbxVector4* lControlPoints = pMesh->GetControlPoints(); 
	char header[100];

	DisplayString("    Polygons");

	int vertexId = 0;
	for (i = 0; i < lPolygonCount; i++)
	{
		DisplayInt("        Polygon ", i);
		int l;

		for (l = 0; l < pMesh->GetElementPolygonGroupCount(); l++)
		{
			FbxGeometryElementPolygonGroup* lePolgrp = pMesh->GetElementPolygonGroup(l);
			switch (lePolgrp->GetMappingMode())
			{
			case FbxGeometryElement::eByPolygon:
				if (lePolgrp->GetReferenceMode() == FbxGeometryElement::eIndex)
				{
					FBXSDK_sprintf(header, 100, "        Assigned to group: "); 
					int polyGroupId = lePolgrp->GetIndexArray().GetAt(i);
					DisplayInt(header, polyGroupId);
					break;
				}
			default:
				// any other mapping modes don't make sense
				DisplayString("        \"unsupported group assignment\"");
				break;
			}
		}

		int lPolygonSize = pMesh->GetPolygonSize(i);

		for (j = 0; j < lPolygonSize; j++)
		{
			int lControlPointIndex = pMesh->GetPolygonVertex(i, j);

			Display3DVector("            Coordinates: ", lControlPoints[lControlPointIndex]);

			for (l = 0; l < pMesh->GetElementVertexColorCount(); l++)
			{
				FbxGeometryElementVertexColor* leVtxc = pMesh->GetElementVertexColor( l);
				FBXSDK_sprintf(header, 100, "            Color vertex: "); 

				switch (leVtxc->GetMappingMode())
				{
				default:
					break;
				case FbxGeometryElement::eByControlPoint:
					switch (leVtxc->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						DisplayColor(header, leVtxc->GetDirectArray().GetAt(lControlPointIndex));
						break;
					case FbxGeometryElement::eIndexToDirect:
						{
							int id = leVtxc->GetIndexArray().GetAt(lControlPointIndex);
							DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
						}
						break;
					default:
						break; // other reference modes not shown here!
					}
					break;

				case FbxGeometryElement::eByPolygonVertex:
					{
						switch (leVtxc->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
							DisplayColor(header, leVtxc->GetDirectArray().GetAt(vertexId));
							break;
						case FbxGeometryElement::eIndexToDirect:
							{
								int id = leVtxc->GetIndexArray().GetAt(vertexId);
								DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
							}
							break;
						default:
							break; // other reference modes not shown here!
						}
					}
					break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
					break;
				}
			}
			for (l = 0; l < pMesh->GetElementUVCount(); ++l)
			{
				FbxGeometryElementUV* leUV = pMesh->GetElementUV( l);
				FBXSDK_sprintf(header, 100, "            Texture UV: "); 

				switch (leUV->GetMappingMode())
				{
				default:
					break;
				case FbxGeometryElement::eByControlPoint:
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						Display2DVector(header, leUV->GetDirectArray().GetAt(lControlPointIndex));
						break;
					case FbxGeometryElement::eIndexToDirect:
						{
							int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
							Display2DVector(header, leUV->GetDirectArray().GetAt(id));
						}
						break;
					default:
						break; // other reference modes not shown here!
					}
					break;

				case FbxGeometryElement::eByPolygonVertex:
					{
						int lTextureUVIndex = pMesh->GetTextureUVIndex(i, j);
						switch (leUV->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
						case FbxGeometryElement::eIndexToDirect:
							{
								Display2DVector(header, leUV->GetDirectArray().GetAt(lTextureUVIndex));
							}
							break;
						default:
							break; // other reference modes not shown here!
						}
					}
					break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
					break;
				}
			}
			for( l = 0; l < pMesh->GetElementNormalCount(); ++l)
			{
				FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal( l);
				FBXSDK_sprintf(header, 100, "            Normal: "); 

				if(leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leNormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						Display3DVector(header, leNormal->GetDirectArray().GetAt(vertexId));
						break;
					case FbxGeometryElement::eIndexToDirect:
						{
							int id = leNormal->GetIndexArray().GetAt(vertexId);
							Display3DVector(header, leNormal->GetDirectArray().GetAt(id));
						}
						break;
					default:
						break; // other reference modes not shown here!
					}
				}

			}
			for( l = 0; l < pMesh->GetElementTangentCount(); ++l)
			{
				FbxGeometryElementTangent* leTangent = pMesh->GetElementTangent( l);
				FBXSDK_sprintf(header, 100, "            Tangent: "); 

				if(leTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leTangent->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						Display3DVector(header, leTangent->GetDirectArray().GetAt(vertexId));
						break;
					case FbxGeometryElement::eIndexToDirect:
						{
							int id = leTangent->GetIndexArray().GetAt(vertexId);
							Display3DVector(header, leTangent->GetDirectArray().GetAt(id));
						}
						break;
					default:
						break; // other reference modes not shown here!
					}
				}

			}
			for( l = 0; l < pMesh->GetElementBinormalCount(); ++l)
			{

				FbxGeometryElementBinormal* leBinormal = pMesh->GetElementBinormal( l);

				FBXSDK_sprintf(header, 100, "            Binormal: "); 
				if(leBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leBinormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						Display3DVector(header, leBinormal->GetDirectArray().GetAt(vertexId));
						break;
					case FbxGeometryElement::eIndexToDirect:
						{
							int id = leBinormal->GetIndexArray().GetAt(vertexId);
							Display3DVector(header, leBinormal->GetDirectArray().GetAt(id));
						}
						break;
					default:
						break; // other reference modes not shown here!
					}
				}
			}
			vertexId++;
		} // for polygonSize
	} // for polygonCount


	//check visibility for the edges of the mesh
	for(int l = 0; l < pMesh->GetElementVisibilityCount(); ++l)
	{
		FbxGeometryElementVisibility* leVisibility=pMesh->GetElementVisibility(l);
		FBXSDK_sprintf(header, 100, "    Edge Visibility : ");
		DisplayString(header);
		switch(leVisibility->GetMappingMode())
		{
		default:
			break;
			//should be eByEdge
		case FbxGeometryElement::eByEdge:
			//should be eDirect
			for(int j=0; j!=pMesh->GetMeshEdgeCount();++j)
			{
				DisplayInt("        Edge ", j);
				DisplayBool("              Edge visibility: ", leVisibility->GetDirectArray().GetAt(j));
			}

			break;
		}
	}
	DisplayString("");
}

void DisplayMesh(FbxNode* pNode)

{
	FbxMesh* lMesh = (FbxMesh*) pNode->GetNodeAttribute ();

	DisplayString("Mesh Name: ", (char *) pNode->GetName());
	DisplayPolygons(lMesh);
	/*DisplayMetaDataConnections(lMesh);
	DisplayControlsPoints(lMesh);
	DisplayMaterialMapping(lMesh);
	DisplayMaterial(lMesh);
	DisplayTexture(lMesh);
	DisplayMaterialConnections(lMesh);
	DisplayLink(lMesh);
	DisplayShape(lMesh);

	DisplayCache(lMesh);*/
}




//void DisplayMetaDataConnections(FbxObject* pObject)
//{
//	int nbMetaData = pObject->GetSrcObjectCount<FbxObjectMetaData>();
//	if (nbMetaData > 0)
//		DisplayString("    MetaData connections ");
//
//	for (int i = 0; i < nbMetaData; i++)
//	{
//		FbxObjectMetaData* metaData = pObject->GetSrcObject<FbxObjectMetaData>(i);
//		DisplayString("        Name: ", (char*)metaData->GetName());
//	}
//}

void DisplayString(const char* pHeader, const char* pValue /* = "" */, const char* pSuffix /* = "" */)
{
	FbxString lString;

	lString = pHeader;
	lString += pValue;
	lString += pSuffix;
	lString += "\n";
	FBXSDK_printf(lString);
}


void DisplayBool(const char* pHeader, bool pValue, const char* pSuffix /* = "" */)
{
	FbxString lString;

	lString = pHeader;
	lString += pValue ? "true" : "false";
	lString += pSuffix;
	lString += "\n";
	FBXSDK_printf(lString);
}


void DisplayInt(const char* pHeader, int pValue, const char* pSuffix /* = "" */)
{
	FbxString lString;

	lString = pHeader;
	lString += pValue;
	lString += pSuffix;
	lString += "\n";
	FBXSDK_printf(lString);
}


void DisplayDouble(const char* pHeader, double pValue, const char* pSuffix /* = "" */)
{
	FbxString lString;
	FbxString lFloatValue = (float) pValue;

	lFloatValue = pValue <= -HUGE_VAL ? "-INFINITY" : lFloatValue.Buffer();
	lFloatValue = pValue >=  HUGE_VAL ?  "INFINITY" : lFloatValue.Buffer();

	lString = pHeader;
	lString += lFloatValue;
	lString += pSuffix;
	lString += "\n";
	FBXSDK_printf(lString);
}


void Display2DVector(const char* pHeader, FbxVector2 pValue, const char* pSuffix  /* = "" */)
{
	FbxString lString;
	FbxString lFloatValue1 = (float)pValue[0];
	FbxString lFloatValue2 = (float)pValue[1];

	lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
	lFloatValue1 = pValue[0] >=  HUGE_VAL ?  "INFINITY" : lFloatValue1.Buffer();
	lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
	lFloatValue2 = pValue[1] >=  HUGE_VAL ?  "INFINITY" : lFloatValue2.Buffer();

	lString = pHeader;
	lString += lFloatValue1;
	lString += ", ";
	lString += lFloatValue2;
	lString += pSuffix;
	lString += "\n";
	FBXSDK_printf(lString);
}


void Display3DVector(const char* pHeader, FbxVector4 pValue, const char* pSuffix /* = "" */)
{
	FbxString lString;
	FbxString lFloatValue1 = (float)pValue[0];
	FbxString lFloatValue2 = (float)pValue[1];
	FbxString lFloatValue3 = (float)pValue[2];

	lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
	lFloatValue1 = pValue[0] >=  HUGE_VAL ?  "INFINITY" : lFloatValue1.Buffer();
	lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
	lFloatValue2 = pValue[1] >=  HUGE_VAL ?  "INFINITY" : lFloatValue2.Buffer();
	lFloatValue3 = pValue[2] <= -HUGE_VAL ? "-INFINITY" : lFloatValue3.Buffer();
	lFloatValue3 = pValue[2] >=  HUGE_VAL ?  "INFINITY" : lFloatValue3.Buffer();

	lString = pHeader;
	lString += lFloatValue1;
	lString += ", ";
	lString += lFloatValue2;
	lString += ", ";
	lString += lFloatValue3;
	lString += pSuffix;
	lString += "\n";
	FBXSDK_printf(lString);
}

void Display4DVector(const char* pHeader, FbxVector4 pValue, const char* pSuffix /* = "" */)
{
	FbxString lString;
	FbxString lFloatValue1 = (float)pValue[0];
	FbxString lFloatValue2 = (float)pValue[1];
	FbxString lFloatValue3 = (float)pValue[2];
	FbxString lFloatValue4 = (float)pValue[3];

	lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
	lFloatValue1 = pValue[0] >=  HUGE_VAL ?  "INFINITY" : lFloatValue1.Buffer();
	lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
	lFloatValue2 = pValue[1] >=  HUGE_VAL ?  "INFINITY" : lFloatValue2.Buffer();
	lFloatValue3 = pValue[2] <= -HUGE_VAL ? "-INFINITY" : lFloatValue3.Buffer();
	lFloatValue3 = pValue[2] >=  HUGE_VAL ?  "INFINITY" : lFloatValue3.Buffer();
	lFloatValue4 = pValue[3] <= -HUGE_VAL ? "-INFINITY" : lFloatValue4.Buffer();
	lFloatValue4 = pValue[3] >=  HUGE_VAL ?  "INFINITY" : lFloatValue4.Buffer();

	lString = pHeader;
	lString += lFloatValue1;
	lString += ", ";
	lString += lFloatValue2;
	lString += ", ";
	lString += lFloatValue3;
	lString += ", ";
	lString += lFloatValue4;
	lString += pSuffix;
	lString += "\n";
	FBXSDK_printf(lString);
}


void DisplayColor(const char* pHeader, FbxPropertyT<FbxDouble3> pValue, const char* pSuffix /* = "" */)

{
	FbxString lString;

	lString = pHeader;
	//lString += (float) pValue.mRed;
	//lString += (double)pValue.GetArrayItem(0);
	lString += " (red), ";
	//lString += (float) pValue.mGreen;
	//lString += (double)pValue.GetArrayItem(1);
	lString += " (green), ";
	//lString += (float) pValue.mBlue;
	//lString += (double)pValue.GetArrayItem(2);
	lString += " (blue)";
	lString += pSuffix;
	lString += "\n";
	FBXSDK_printf(lString);
}


void DisplayColor(const char* pHeader, FbxColor pValue, const char* pSuffix /* = "" */)
{
	FbxString lString;

	lString = pHeader;
	lString += (float) pValue.mRed;

	lString += " (red), ";
	lString += (float) pValue.mGreen;

	lString += " (green), ";
	lString += (float) pValue.mBlue;

	lString += " (blue)";
	lString += pSuffix;
	lString += "\n";
	FBXSDK_printf(lString);
}
