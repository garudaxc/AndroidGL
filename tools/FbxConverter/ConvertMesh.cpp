#include "fbxsdk.h"
#include "FbxConverter.h"
#include <vector>
#include <assert.h>
#include <map>


using namespace std;


struct MeshData
{
	vector<float>		position_;
	vector<float>		normal_;
	vector<float>		uv0_;
	vector<float>		blendWeight_;
	vector<unsigned char>		blendIndex_;

	vector<int>			index_;

	int		vertexCount_;

	void	Alloc(int vertexCount, int indexCount);
	bool	SaveToFile(const char* fileName);
};


void MeshData::Alloc(int vertexCount, int indexCount)
{
	vertexCount_	= vertexCount;

	position_.resize(vertexCount * 3);
	normal_.resize(vertexCount * 3);
	uv0_.resize(vertexCount * 2);

	index_.resize(indexCount);
}


#define MAGIC(a, b, c, d) (a << 24 | b << 16 | c << 8 | d)
#define MAGIC_STR(str) (str[0] << 24 | str[1] << 16 | str[2] << 8 | str[3])

FILE* OutFile = NULL;
inline void WriteMagic(unsigned int magic)
{
	fwrite(&magic, sizeof(unsigned int), 1, OutFile);
}

inline void WriteInt(int n)
{
	fwrite(&n, sizeof(int), 1, OutFile);
}

inline void WriteFloatArray(const float* array, int numFloats)
{
	fwrite(array, sizeof(float), numFloats, OutFile);
}

bool MeshData::SaveToFile(const char* fileName)
{
	FILE* pf = fopen(fileName, "wb");
	if (pf == NULL) {
		printf("write to file %s failed\n", fileName);
		return false;
	}
	OutFile = pf;

	WriteMagic(MAGIC_STR("MESH"));
	WriteInt(vertexCount_);
	WriteInt(index_.size());

	WriteMagic(MAGIC_STR("POSI"));
	WriteFloatArray(&position_[0], position_.size());
	WriteMagic(MAGIC_STR("NORM"));
	WriteFloatArray(&normal_[0], normal_.size());
	WriteMagic(MAGIC_STR("TEX0"));
	WriteFloatArray(&uv0_[0], uv0_.size());

	WriteMagic(MAGIC_STR("INDX"));
	fwrite(&index_[0], sizeof(int), index_.size(), OutFile);

	fclose(OutFile);
	OutFile = NULL;

	return true;
}




void DisplayPolygons(FbxMesh* pMesh)
{
	int i, j, lPolygonCount = pMesh->GetPolygonCount();
	FbxVector4* lControlPoints = pMesh->GetControlPoints();
	char header[100];

	DisplayString("    Polygons");
	DisplayInt("control point count : ", pMesh->GetControlPointsCount());
	DisplayInt("Polygon count : ", lPolygonCount);

	FbxArray<FbxVector4> normals;
	bool b = pMesh->GetPolygonVertexNormals(normals);
	DisplayBool("GetPolygonVertexNormals", b);
	DisplayInt("normals ", normals.Size());

	int ElementPolygonGroupCount = pMesh->GetElementPolygonGroupCount();
	DisplayInt("ElementPolygonGroupCount ", ElementPolygonGroupCount);

	int textureUVCount = pMesh->GetTextureUVCount();
	int uvLayerCount = pMesh->GetUVLayerCount();
	DisplayInt("textureUVCount ", textureUVCount);
	DisplayInt("uvLayerCount ", uvLayerCount);

	FbxLayerElementArrayTemplate< FbxVector2 >* 	pLockableArray = NULL;
	b = pMesh->GetTextureUV(&pLockableArray);
	DisplayBool("GetTextureUV ", b);
	DisplayInt("pLockableArray ", (int)pLockableArray);
	DisplayInt("size of pLockableArray ", pLockableArray->GetCount());

	for (int i = 0; i < pLockableArray->GetCount(); i++)
	{
		FbxVector2 uv = pLockableArray->GetAt(i);
		printf("%.2f %.2f\n", uv[0], uv[1]);
	}

	for (int i = 0; i < lPolygonCount; i++)
	{
		int uv[3] = { 0 };

		int lPolygonSize = pMesh->GetPolygonSize(i);
		for (int j = 0; j < lPolygonSize; j++)
		{
			uv[j] = pMesh->GetTextureUVIndex(i, j);
		}
		printf("uv index %d : %d %d %d\n", i, uv[0], uv[1], uv[2]);
	}


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
				FbxGeometryElementVertexColor* leVtxc = pMesh->GetElementVertexColor(l);
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
				FbxGeometryElementUV* leUV = pMesh->GetElementUV(l);
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
			for (l = 0; l < pMesh->GetElementNormalCount(); ++l)
			{
				FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal(l);
				FBXSDK_sprintf(header, 100, "            Normal: ");

				if (leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leNormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						FBXSDK_printf("map mode eDirect\n");
						Display3DVector(header, leNormal->GetDirectArray().GetAt(vertexId));
						break;
					case FbxGeometryElement::eIndexToDirect:
					{

															   FBXSDK_printf("map mode eIndexToDirect\n");
															   int id = leNormal->GetIndexArray().GetAt(vertexId);
															   Display3DVector(header, leNormal->GetDirectArray().GetAt(id));
					}
						break;
					default:
						break; // other reference modes not shown here!
					}
				}

			}
			for (l = 0; l < pMesh->GetElementTangentCount(); ++l)
			{
				FbxGeometryElementTangent* leTangent = pMesh->GetElementTangent(l);
				FBXSDK_sprintf(header, 100, "            Tangent: ");

				if (leTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
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
			for (l = 0; l < pMesh->GetElementBinormalCount(); ++l)
			{

				FbxGeometryElementBinormal* leBinormal = pMesh->GetElementBinormal(l);

				FBXSDK_sprintf(header, 100, "            Binormal: ");
				if (leBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
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


	return;
	//check visibility for the edges of the mesh
	for (int l = 0; l < pMesh->GetElementVisibilityCount(); ++l)
	{
		FbxGeometryElementVisibility* leVisibility = pMesh->GetElementVisibility(l);
		FBXSDK_sprintf(header, 100, "    Edge Visibility : ");
		DisplayString(header);
		switch (leVisibility->GetMappingMode())
		{
		default:
			break;
			//should be eByEdge
		case FbxGeometryElement::eByEdge:
			//should be eDirect
			for (int j = 0; j != pMesh->GetMeshEdgeCount(); ++j)
			{
				DisplayInt("        Edge ", j);
				DisplayBool("              Edge visibility: ", leVisibility->GetDirectArray().GetAt(j));
			}

			break;
		}
	}
	DisplayString("");
}



static void SetData(const float* pData, std::vector<float>& buffer, int numFloats)
{
	buffer.resize(numFloats);
	int byteToCopy = sizeof(float)* numFloats;
	memcpy_s(&buffer[0], byteToCopy, pData, byteToCopy);
}



int CompareFloats2(const float* p0, const float* p1)
{
	for (int i = 0; i < 2; i++){
		if (p0[i] < p1[i]){
			return -1;
		}

		if (p0[i] > p1[i]){
			return 1;
		}
	}

	return 0;
}


int CompareFloats(void* context, const void* elem0, const void* elem1)
{
	int numFloat = *((int*)context);
	const float* p0 = (const float*)elem0;
	const float* p1 = (const float*)elem1;

	for (int i = 0; i < numFloat; i++){
		if (p0[i] < p1[i]){
			return -1;
		}

		if (p0[i] > p1[i]){
			return 1;
		}
	}

	return 0;
}

int ShrinkData(float* pData, int stride, int dataCount)
{
	float* p0 = pData;
	float* p1 = pData + stride;
	float* pEnd = pData + stride * dataCount;

	while (p1 < pEnd){
		while (CompareFloats(&stride, p0, p1) == 0 && p1 < pEnd){
			p1 += stride;
		}

		if (p1 != pEnd)	{
			p0 += stride;
			memcpy(p0, p1, sizeof(float)* stride);
		}
	}
	return (p0 - pData) / stride + 1;
}

// stride: float2 or float3
// dataCount: number of float2 or float3
// dataMapper: old to new data mapper
// return :new data count
int EliminateData(float* pData, int stride, int dataCount, std::vector<int>& dataMapper)
{
	std::vector<float> tempData;
	tempData.resize(stride * dataCount);
	memcpy_s(&tempData[0], sizeof(float)* tempData.size(), pData, sizeof(float)* tempData.size());

	qsort_s(pData, dataCount, sizeof(float)* stride, CompareFloats, &stride);

	int uniqueCount = ShrinkData(pData, stride, dataCount);

	dataMapper.resize(dataCount);
	for (int i = 0; i < dataCount; i++)	{
		for (int j = 0; j < uniqueCount;){
			const float* p0 = &tempData[i * stride];
			const float* p1 = pData + j * stride;
			if (CompareFloats(&stride, p0, p1) == 0){
				dataMapper[i] = j;
				break;
			}

			j++;
			assert(j != uniqueCount);
		}
	}

	return uniqueCount;
}


void RemapIndex(int* index, int indexCount, int* mapper)
{
	for (int i = 0; i < indexCount; i++){
		int indexMapTo = mapper[index[i]];
		index[i] = indexMapTo;
	}
}


void ProcessBufferIndex(std::vector<float>& buffer, std::vector<int>& index, int stride)
{
	std::vector<int> dataMapper;

	int uniqueCount = EliminateData(&buffer[0], stride, buffer.size() / stride, dataMapper);
	buffer.resize(uniqueCount * stride);
	RemapIndex(&index[0], index.size(), &dataMapper[0]);
}



struct VertInfo
{
	std::vector<int>	indeics;
};

struct VertInfoComp : public std::binary_function<VertInfo, VertInfo, bool>
{	// functor for operator<
	bool operator()(const VertInfo& _Left, const VertInfo& _Right) const
	{	// apply operator< to operands
		return (_Left.indeics < _Right.indeics);
	}
};

typedef std::map<VertInfo, int, VertInfoComp> VertexInfoMap;

void CopyVertexBuffer(float* pBuffer, int desStride, const float* srcBuffer,
						int srcStride, int elemIdx, const VertexInfoMap& vertexMap)
{
	for (auto it = vertexMap.begin(); it != vertexMap.end(); ++it){
		int srcIndex = it->first.indeics[elemIdx];
		memcpy((void*)(pBuffer + it->second * desStride), 
			srcBuffer + srcIndex * srcStride, sizeof(float)* desStride);
	}
}


void ConvertCoordinateSystem(float* pVectors, int numVectors)
{
	float* p = pVectors;
	for (int i = 0; i < numVectors; i++){
		float origY = p[1];
		float origZ = p[2];
		p[1] = origZ;
		p[2] = -origY;

		p += 3;
	}
}

void FlipTexcoordV(float* pUV, int stride, int numVert)
{
	float* p = pUV;
	for (int i = 0; i < numVert; i++){
		p[1] = 1.f - p[1];
		p += stride;
	}
}


void SetFloatArray(std::vector<float>& des, const FbxVector4* src, int numVec, int numPerVec)
{
	des.resize(numVec * numPerVec);
	int index = 0;
	for (int i = 0; i < numVec; i++) {
		for (int j = 0; j < numPerVec; j++)	{
			des[index++] = (float)src[i][j];
		}
	}
}


void ConvertMesh(FbxMesh* mesh)
{
	//DisplayPolygons(mesh);

	if (!mesh->IsTriangleMesh()) {
		FBXSDK_printf("Error ! mesh %s is not triangle mesh\n", mesh->GetNode()->GetName());
		return;
	}
	
	int polygonCount = mesh->GetPolygonCount();
	
	std::vector<float> positionData;
	std::vector<float> normalData;
	std::vector<float> texcoordData;
	std::vector<int> positionIndex;
	std::vector<int> normalIndex;
	std::vector<int> texcoordIndex;
	std::vector<int> meshIndex;

	positionIndex.resize(polygonCount * 3);
	normalIndex.resize(polygonCount * 3);
	texcoordIndex.resize(polygonCount * 3);

	// position
	int controlPointCount = mesh->GetControlPointsCount();
	FbxVector4* controlPoints = mesh->GetControlPoints();
	SetFloatArray(positionData, controlPoints, controlPointCount, 3);
	for (int i = 0; i < polygonCount; i++){
		assert(mesh->GetPolygonSize(i) == 3);
		for (int j = 0; j < 3; j++){
			positionIndex[i * 3 + j] = mesh->GetPolygonVertex(i, j);
		}
	}

	// normal
	FbxArray<FbxVector4> normals;
	bool bRes = mesh->GetPolygonVertexNormals(normals);
	if (!bRes){
		FBXSDK_printf("Error! GetPolygonVertexNormals failed\n");
		return;
	}
	SetFloatArray(normalData, normals.GetArray(), normals.Size(), 3);
	assert(normals.Size() == polygonCount * 3);
	for (int i = 0; i < polygonCount * 3; i++){
		normalIndex[i] = i;
	}

	// UV
	int textureUVCount = mesh->GetTextureUVCount();
	//int uvLayerCount = mesh->GetUVLayerCount();
	//DisplayInt("textureUVCount ", textureUVCount);
	//DisplayInt("uvLayerCount ", uvLayerCount);
	FbxLayerElementArrayTemplate< FbxVector2 >* pLockableArray = NULL;
	bRes = mesh->GetTextureUV(&pLockableArray);
	if (!bRes){
		FBXSDK_printf("Error! GetTextureUV failed\n");
		return;
	}
	texcoordData.resize(textureUVCount * 2);
	for (int i = 0; i < pLockableArray->GetCount(); i++){
		FbxVector2 uv = pLockableArray->GetAt(i);
		texcoordData[i * 2 + 0] = (float)uv[0];
		texcoordData[i * 2 + 1] = (float)uv[1];
	}
	for (int i = 0; i < polygonCount; i++){
		for (int j = 0; j < 3; j++){
			texcoordIndex[i * 3 + j] = mesh->GetTextureUVIndex(i, j);
		}
	}

	ProcessBufferIndex(positionData, positionIndex, 3);
	ProcessBufferIndex(normalData, normalIndex, 3);
	ProcessBufferIndex(texcoordData, texcoordIndex, 2);

	meshIndex.resize(polygonCount * 3);
	
	std::map<VertInfo, int, VertInfoComp> vertexMap;
	int expVertIdx = 0;
	for (size_t i = 0; i < meshIndex.size(); i++){
		VertInfo info;
		info.indeics.push_back(positionIndex[i]);
		info.indeics.push_back(normalIndex[i]);
		info.indeics.push_back(texcoordIndex[i]);

		auto iter = vertexMap.find(info);
		if (iter == vertexMap.end()){
			vertexMap[info] = expVertIdx;
			meshIndex[i] = expVertIdx;
			expVertIdx++;
		} else {
			meshIndex[i] = iter->second;
		}
	}

	int nNumMeshVert = expVertIdx;
	printf("final vertex count %d\n", nNumMeshVert);

	MeshData meshData;
	meshData.Alloc(nNumMeshVert, polygonCount * 3);
	
	CopyVertexBuffer(&meshData.position_[0], 3, &positionData[0], 3, 0, vertexMap);
	CopyVertexBuffer(&meshData.normal_[0], 3, &normalData[0], 3, 1, vertexMap);
	CopyVertexBuffer(&meshData.uv0_[0], 2, &texcoordData[0], 2, 2, vertexMap);

	meshData.index_ = meshIndex;

	if (Args.convertAxis){
		ConvertCoordinateSystem(&meshData.position_[0], nNumMeshVert);
		ConvertCoordinateSystem(&meshData.normal_[0], nNumMeshVert);
	}

	// diff between 3dsmax and dx
	if (Args.flipTexcoordV){
		FlipTexcoordV(&meshData.uv0_[0], 2, nNumMeshVert);
	}

	char outFile[256];
	sprintf_s(outFile, sizeof(outFile), "%s%s.mesh", Args.outputPath.c_str(), mesh->GetNode()->GetName());
	bRes = meshData.SaveToFile(outFile);
	if (!bRes) {
		printf("save file %s failed\n", outFile);
		return;
	}
	
	printf("save %s succeeded!\n", outFile);

}