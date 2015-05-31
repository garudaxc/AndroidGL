#include "MeshBuilder.h"
#include "FCollada.h"
#include "FCDocument/FCDGeometryMesh.h"
#include "FCDocument/FCDGeometrySource.h"
#include "FCDocument/FCDGeometryPolygons.h"
#include "FCDocument/FCDGeometryPolygonsInput.h"

#include "FCDocument/FCDController.h"
#include "FCDocument/FCDSkinController.h"

#include <assert.h>
#include <set>




class MeshBuildDataCollada : public MeshBuildData
{
public:

	virtual int				GetNumVertex() const;
	virtual int				GetNumFace() const;
	virtual int				GetNumElement() const;
	virtual int				GetNumElementFace(int nElement) const;

	virtual const int*		GetIndexData() const;

	virtual const float*	GetPositionData() const;
	virtual const float*	GetNormalData() const;
	virtual const float*	GetTexcoordData() const;

	virtual const int* GetBlendIndex() const;
	virtual const float* GetBlendWeight() const;

	virtual void			Release();

	std::vector<int>		FaceCount;
	std::vector<int>		MeshIndex;

	std::vector<float>		PositionData;
	std::vector<float>		NormalData;
	std::vector<float>		TexcoordData;

	std::vector<float>		BlendWeight;
	std::vector<int>		BlendIndex;
};



int MeshBuildDataCollada::GetNumVertex() const
{
	return PositionData.size() / 3;
}

int MeshBuildDataCollada::GetNumFace() const
{
	int nFace = 0;
	for (size_t i = 0; i < FaceCount.size(); i++)
	{
		nFace += FaceCount[i];
	}
	return nFace;
}

int MeshBuildDataCollada::GetNumElement() const
{
	return FaceCount.size();
}

int MeshBuildDataCollada::GetNumElementFace(int nElement) const
{
	return FaceCount[nElement];
}

const int* MeshBuildDataCollada::GetIndexData() const
{
	return &MeshIndex[0];
}

const float* MeshBuildDataCollada::GetPositionData() const
{
	return &PositionData[0];
}

const float* MeshBuildDataCollada::GetNormalData() const
{
	return &NormalData[0];
}

const float* MeshBuildDataCollada::GetTexcoordData() const
{
	return &TexcoordData[0];
}


const int* MeshBuildDataCollada::GetBlendIndex() const
{
	if (BlendIndex.empty())	{
		return NULL;
	}

	return &BlendIndex[0];
}

const float* MeshBuildDataCollada::GetBlendWeight() const
{
	if (BlendWeight.empty()) {
		return NULL;
	}

	return &BlendWeight[0];
}

void MeshBuildDataCollada::Release()
{
	delete this;
}

//////////////////////////////////////////////////////////////////////////


static void SetData(const float* pData, std::vector<float>& buffer, int numFloats)
{
	buffer.resize(numFloats);
	int byteToCopy = sizeof(float) * numFloats;
	memcpy_s(&buffer[0], byteToCopy, pData, byteToCopy);
}



MeshBuilder::MeshBuilder(void):
m_nMeshFaceCount(0),m_bConvertAxis(false),m_bFlipTexcoordV(true)
{
}

MeshBuilder::~MeshBuilder(void)
{
}

 
void MeshBuilder::Release()
{

}

void MeshBuilder::SetColladaMesh(FCDGeometryMesh* pColladaMesh, FCDSkinController* pSkin)
{
	SetVertexData(pColladaMesh);

	SetIndexData(pColladaMesh);

	if (pSkin)
	{
		SetSkinData(pSkin);
	}
}


void MeshBuilder::SetVertexData(FCDGeometryMesh* pColladaMesh)
{
	for (size_t i = 0; i < pColladaMesh->GetSourceCount(); i++)
	{
		FCDGeometrySource* source = pColladaMesh->GetSource(i);

		int dataCount = source->GetDataCount();
		int type = source->GetType();

		if (type == FUDaeGeometryInput::POSITION)
		{
			SetData(source->GetData(), m_PositionData, dataCount);
		}
		else if (type == FUDaeGeometryInput::NORMAL)
		{
			SetData(source->GetData(), m_NormalData, dataCount);
		}
		else if (type == FUDaeGeometryInput::TEXCOORD)
		{
			SetData(source->GetData(), m_TexcoordData, dataCount);
		}
	}
}

void MeshBuilder::SetIndexData(FCDGeometryMesh* pColladaMesh)
{
	size_t polyCount = pColladaMesh->GetPolygonsCount();
	std::vector<int> faceCount;
	faceCount.resize(polyCount);
	for (size_t i = 0; i < polyCount; i++)
	{
		FCDGeometryPolygons* polygons = pColladaMesh->GetPolygons(i);
		faceCount[i] = polygons->GetFaceCount();
	}

	this->SetFaceCount(polyCount, &faceCount[0]);

	for (size_t i = 0; i < polyCount; i++)
	{
		FCDGeometryPolygons* polygons = pColladaMesh->GetPolygons(i);
		for (size_t j = 0; j < polygons->GetInputCount(); j++)
		{
			FCDGeometryPolygonsInput* input = polygons->GetInput(j);

			int indexCount = input->GetIndexCount();
			int type = input->GetSemantic();

			if (type == FUDaeGeometryInput::POSITION)
			{
				this->SetPositionIndex((const int*)input->GetIndices(), i);
			}
			else if (type == FUDaeGeometryInput::NORMAL)
			{
				this->SetNormalIndex((const int*)input->GetIndices(), i);
			}
			else if (type == FUDaeGeometryInput::TEXCOORD)
			{
				this->SetTexcoordIndex((const int*)input->GetIndices(), i);
			}
		}
	}
}


void MeshBuilder::SetSkinData(FCDSkinController* pSkin)
{
	size_t numSkinVert = pSkin->GetInfluenceCount();
	assert(numSkinVert == m_PositionData.size() / 3);

	m_SkinWeight.resize(numSkinVert * 4);
	m_BoneIndex.resize(numSkinVert * 4);

	size_t dataIndex = 0;

	for (size_t i = 0; i < numSkinVert; i++)
	{
		const FCDSkinControllerVertex* pVert = pSkin->GetVertexInfluence(i);

		size_t j = 0;
		for (; j < pVert->GetPairCount(); j++)
		{
			const FCDJointWeightPair* pPair = pVert->GetPair(j);
			m_SkinWeight[dataIndex] = pPair->weight;
			m_BoneIndex[dataIndex] = pPair->jointIndex;

			dataIndex++;
		}

		for (; j < 4; j++)
		{
			m_SkinWeight[dataIndex] = 0.f;
			m_BoneIndex[dataIndex] = 0;

			dataIndex++;
		}
	}
}


int MeshBuilder::GetMeshFaceCount()
{
	return m_nMeshFaceCount;
}

void MeshBuilder::SetFaceCount(int numPolys, const int* eachFaceCount)
{
	m_nMeshFaceCount = 0;
	m_FaceCount.resize(numPolys);
	for (int i = 0; i < numPolys; i++)
	{
		m_FaceCount[i] = eachFaceCount[i];
		m_nMeshFaceCount += eachFaceCount[i];
	}

	m_PositionIndex.resize(m_nMeshFaceCount * 3);
	m_NormalIndex.resize(m_nMeshFaceCount * 3);
	m_TexcoordIndex.resize(m_nMeshFaceCount * 3);
}


int MeshBuilder::GetPolyFaceIndex(int polyIndex)
{
	int index = 0;
	for (int i = 0; i < polyIndex; i++)
	{
		index += m_FaceCount[i];
	}
	return index * 3;
}

void MeshBuilder::SetPositionIndex(const int* index, int polyIndex)
{
	int pos = GetPolyFaceIndex(polyIndex);
	int indexCount = m_FaceCount[polyIndex] * 3;
	int byteToCopy = sizeof(int) * indexCount;
	memcpy_s(&m_PositionIndex[pos], byteToCopy, index, byteToCopy);
}

void MeshBuilder::SetNormalIndex(const int* index, int polyIndex)
{
	int pos = GetPolyFaceIndex(polyIndex);
	int indexCount = m_FaceCount[polyIndex] * 3;
	int byteToCopy = sizeof(int) * indexCount;
	memcpy_s(&m_NormalIndex[pos], byteToCopy, index, byteToCopy);

}

void MeshBuilder::SetTexcoordIndex(const int* index, int polyIndex)
{
	int pos = GetPolyFaceIndex(polyIndex);
	int indexCount = m_FaceCount[polyIndex] * 3;
	int byteToCopy = sizeof(int) * indexCount;
	memcpy_s(&m_TexcoordIndex[pos], byteToCopy, index, byteToCopy);
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
			memcpy(p0, p1, sizeof(float) * stride);
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
	memcpy_s(&tempData[0], sizeof(float) * tempData.size(), pData, sizeof(float) * tempData.size());

	qsort_s(pData, dataCount, sizeof(float) * stride, CompareFloats, &stride);

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

		memcpy((void*)(pBuffer + it->vertIdx * desStride), srcBuffer + srcIndex * srcStride, sizeof(float) * desStride);
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


MeshBuildData* MeshBuilder::BuildMesh()
{
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


	MeshBuildDataCollada* pMeshBuildData = new MeshBuildDataCollada();
	pMeshBuildData->PositionData.resize(nNumMeshVert * 3);
	pMeshBuildData->NormalData.resize(nNumMeshVert * 3);
	pMeshBuildData->TexcoordData.resize(nNumMeshVert * 2);

	CopyVertexBuffer(&pMeshBuildData->PositionData[0], 3, &m_PositionData[0], 3, 0, vertexMap);
	CopyVertexBuffer(&pMeshBuildData->NormalData[0], 3, &m_NormalData[0], 3, 1, vertexMap);
	CopyVertexBuffer(&pMeshBuildData->TexcoordData[0], 2, &m_TexcoordData[0], 3, 2, vertexMap);

	if (m_bConvertAxis)
	{
		ConvertCoordinateSystem(&pMeshBuildData->PositionData[0], nNumMeshVert);
		ConvertCoordinateSystem(&pMeshBuildData->NormalData[0], nNumMeshVert);
	}

	// diff between 3dsmax and dx
	if (m_bFlipTexcoordV)
	{
		FlipTexcoordV(&pMeshBuildData->TexcoordData[0], 2, nNumMeshVert);
	}

	pMeshBuildData->FaceCount = m_FaceCount;
	pMeshBuildData->MeshIndex = m_MeshIndex;

	return pMeshBuildData;
}

//////////////////////////////////////////////////////////////////////////
