#include "fbxsdk.h"
#include "FbxConverter.h"
#include <vector>
#include <assert.h>
#include <set>

using namespace std;


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
	for (int i = 0; i < 2; i++)
	{
		if (p0[i] < p1[i])
		{
			return -1;
		}

		if (p0[i] > p1[i])
		{
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

	for (int i = 0; i < numFloat; i++)
	{
		if (p0[i] < p1[i])
		{
			return -1;
		}

		if (p0[i] > p1[i])
		{
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

	while (p1 < pEnd)
	{
		while (CompareFloats(&stride, p0, p1) == 0 && p1 < pEnd)
		{
			p1 += stride;
		}

		if (p1 != pEnd)
		{
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
	for (int i = 0; i < dataCount; i++)
	{
		for (int j = 0; j < uniqueCount;)
		{
			const float* p0 = &tempData[i * stride];
			const float* p1 = pData + j * stride;
			if (CompareFloats(&stride, p0, p1) == 0)
			{
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
	for (int i = 0; i < indexCount; i++)
	{
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
	int			vertIdx;
};


struct VertInfoComp : public std::binary_function<VertInfo, VertInfo, bool>
{	// functor for operator<
	bool operator()(const VertInfo& _Left, const VertInfo& _Right) const
	{	// apply operator< to operands
		return (_Left.indeics < _Right.indeics);
	}
};

void CopyVertexBuffer(float* pBuffer, int desStride, const float* srcBuffer,
	int srcStride, int elemIdx, std::set<VertInfo, VertInfoComp>& vertexMap)
{
	for (std::set<VertInfo, VertInfoComp>::iterator it = vertexMap.begin(); it != vertexMap.end(); ++it)
	{
		int srcIndex = it->indeics[elemIdx];

		memcpy((void*)(pBuffer + it->vertIdx * desStride), srcBuffer + srcIndex * srcStride, sizeof(float)* desStride);
	}
}


void ConvertCoordinateSystem(float* pVectors, int numVectors)
{
	float* p = pVectors;
	for (int i = 0; i < numVectors; i++)
	{
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
	for (int i = 0; i < numVert; i++)
	{
		p[1] = 1.f - p[1];
		p += stride;
	}
}









void ConvertMesh(FbxMesh* mesh)
{
	DisplayPolygons(mesh);

	if (!mesh->IsTriangleMesh()) {
		FBXSDK_printf("Error ! mesh %s is not triangle mesh\n", mesh->GetNode()->GetName());
		return;
	}

	int polygonCount = mesh->GetPolygonCount();
	int controlPointCount = mesh->GetControlPointsCount();
	FbxVector4* lControlPoints = mesh->GetControlPoints();



	std::vector<float> m_PositionData;
	std::vector<float> m_NormalData;
	std::vector<float> m_TexcoordData;
	std::vector<int> m_PositionIndex;
	std::vector<int> m_NormalIndex;
	std::vector<int> m_TexcoordIndex;
	std::vector<int> m_MeshIndex;
	int m_nMeshFaceCount;


	ProcessBufferIndex(m_PositionData, m_PositionIndex, 3);
	ProcessBufferIndex(m_NormalData, m_NormalIndex, 3);
	ProcessBufferIndex(m_TexcoordData, m_TexcoordIndex, 2);

	m_MeshIndex.resize(m_nMeshFaceCount * 3);

	std::set<VertInfo, VertInfoComp> vertexMap;

	int expVertIdx = 0;
	for (size_t i = 0; i < m_MeshIndex.size(); i++)
	{
		VertInfo info;
		info.indeics.push_back(m_PositionIndex[i]);
		info.indeics.push_back(m_NormalIndex[i]);
		info.indeics.push_back(m_TexcoordIndex[i]);

		std::pair<std::set<VertInfo, VertInfoComp>::iterator, bool> res = vertexMap.insert(info);

		if (res.second)
		{
			res.first->vertIdx = expVertIdx++;
		}

		m_MeshIndex[i] = res.first->vertIdx;
	}
	int nNumMeshVert = expVertIdx;

	//////////////////////////////////////////////////////////////////////////



	for (int p = 0; p < polygonCount; p++) {
		int polySize = mesh->GetPolygonSize(p);
		if (polySize != 3) {
			FBXSDK_printf("Error ! polygon %d has %d vertexes, only support mesh\n", p, polySize);
			return;
		}

	}


}