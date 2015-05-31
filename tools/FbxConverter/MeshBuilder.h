#pragma once

#include <vector>
#include "Mesh.h"

class FCDGeometryMesh;
class FCDSkinController;

/*
build mesh data object from FCollada data object
*/
class MeshBuilder
{
public:
	MeshBuilder(void);
	~MeshBuilder(void);

	void		Release();

	void		SetColladaMesh(FCDGeometryMesh* pColladaMesh, FCDSkinController* pSkin);

	MeshBuildData*		BuildMesh();
private:
	void		SetVertexData(FCDGeometryMesh* pColladaMesh);
	void		SetIndexData(FCDGeometryMesh* pColladaMesh);
	void		SetSkinData(FCDSkinController* pSkin);

	void		SetFaceCount(int numPolys, const int* eachFaceCount);

	void		SetPositionIndex(const int* index, int polyIndex);
	void		SetNormalIndex(const int* index, int polyIndex);
	void		SetTexcoordIndex(const int* index, int polyIndex);

	int			GetMeshFaceCount();

	// 多边形的起始index位置
	int			GetPolyFaceIndex(int polyIndex);

	bool m_bConvertAxis;	

	bool m_bFlipTexcoordV;	// 从max坐标系转换到dx坐标系

	int m_nMeshFaceCount;

	std::vector<float> m_PositionData;	
	std::vector<float> m_NormalData;
	std::vector<float> m_TexcoordData;

	std::vector<float> m_SkinWeight;

	std::vector<int> m_BoneIndex;

	std::vector<int> m_FaceCount;

	std::vector<int> m_PositionIndex;
	std::vector<int> m_NormalIndex;
	std::vector<int> m_TexcoordIndex;

	std::vector<int> m_MeshIndex;

};
