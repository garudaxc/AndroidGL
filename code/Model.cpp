#include "Model.h"
//#include "glUtil.h"
#include "MyLog.h"
#include "FileSystem.h"
#include "ShaderManager.h"
#include "assert.h"

#define BUFFER_OFFSET(offset) ((void *)(offset))

struct DrawVertex
{
	float pos[3];
	float normal[3];
	float uv[2];
};



Model::Model() :vao_(0), vbo_(0), ibo_(0)
{
}

Model::~Model()
{
}


void Model::Clear()
{
	if (vbo_) {
		glDeleteBuffers(1, &vbo_);
		vbo_ = 0;
	}

	if (ibo_) {
		glDeleteBuffers(1, &ibo_);
		ibo_ = 0;
	}

#ifdef GL_ARB_vertex_array_object
	if (vao_) {
		glDeleteVertexArrays(1, &vao_);
		vao_ = 0;
	}
#endif
}

void CopyVertexData(void * des, int desStride, void * src, int srcStride, int count)
{
	for (int i = 0; i < count; i++) {
		memcpy(des, src, srcStride);
		des = (char*)des + desStride;
		src = (char*)src + srcStride;
	}
}


bool Model::Load(const char* filename)
{
#ifdef GL_ARB_vertex_array_object
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);
#endif

	File* file = GFileSys->OpenFile(filename);
	if (file ==  NULL){
		GLog.LogError("Model::Load failed! %s", filename);
		return false;
	}
	
	unsigned int magic = 0;
	file->Read(magic);
	if (magic != MAGIC_STR("MESH")){
		GLog.LogError("file %s is not a mesh file!", filename);
		return false;
	}

	int numVertex, numIndex;
	file->Read(numVertex);
	file->Read(numIndex);

	vector<float> pos(numVertex * 3);
	vector<float> normal(numVertex * 3);
	vector<float> uv0(numVertex * 2);

	file->Read(magic);
	assert(magic == MAGIC_STR("POSI"));
	file->ReadArray(&pos[0], pos.size());

	file->Read(magic);
	assert(magic == MAGIC_STR("NORM"));
	file->ReadArray(&normal[0], normal.size());

	file->Read(magic);
	assert(magic == MAGIC_STR("TEX0"));
	file->ReadArray(&uv0[0], uv0.size());

	vector<DrawVertex> vertexes(numVertex);
	CopyVertexData(vertexes[0].pos, sizeof(DrawVertex), &pos[0], sizeof(float)* 3, numVertex);
	CopyVertexData(vertexes[0].normal, sizeof(DrawVertex), &normal[0], sizeof(float)* 3, numVertex);
	CopyVertexData(vertexes[0].uv, sizeof(DrawVertex), &uv0[0], sizeof(float)* 2, numVertex);

	vector<int> indexData(numIndex);
	file->Read(magic);
	assert(magic == MAGIC_STR("INDX"));
	file->ReadArray(&indexData[0], indexData.size());
	vector<unsigned short> index(numIndex);
	for (int i = 0; i < numIndex; i++) {
		index[i] = (unsigned short)indexData[i];
	}

	if (file->Read(magic)) {
		assert(magic == MAGIC_STR("ELEM"));
		int numMeshGroup = 0;
		file->Read(numMeshGroup);
		vector<int> meshGroup(numMeshGroup);
		file->ReadArray(&meshGroup[0], numMeshGroup);

		int indexOffset = 0;
		for (size_t i = 0; i < meshGroup.size(); i++) {
			ModelElement elem;
			elem.indexCount = meshGroup[i] * 3;
			elem.indexOffset = indexOffset;
			elem.vertexOffset = 0;
			elements_.push_back(elem);

			indexOffset += meshGroup[i] * 3;
		}
	}

	file->Close();
	
	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DrawVertex) * numVertex, &vertexes[0], GL_STATIC_DRAW);

#ifdef GL_ARB_vertex_array_object
	glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_POSITION);
	glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(DrawVertex), BUFFER_OFFSET(0));

	glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_NORMAL);
	glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(DrawVertex), BUFFER_OFFSET(12));

	glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_UV0);
	glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION_UV0, 2, GL_FLOAT, GL_FALSE, sizeof(DrawVertex), BUFFER_OFFSET(24));

#endif

	glGenBuffers(1, &ibo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index[0]) * index.size(), &index[0], GL_STATIC_DRAW);

#ifdef GL_ARB_vertex_array_object
	glBindVertexArray(0);
#endif

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (elements_.empty()) {
		ModelElement elem;
		elem.indexCount = numIndex;
		elem.indexOffset = 0;
		elem.vertexOffset = 0;
		elements_.push_back(elem);
	}

	return true;
}


int Model::GetElementCount() const
{
	return elements_.size();
}

const ModelElement* Model::GetElement(int index) const
{
	return &elements_[index];
}

void Model::Bind()
{
#ifdef GL_ARB_vertex_array_object
	glBindVertexArray(vao_);
#else
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_POSITION);
	glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(DrawVertex), BUFFER_OFFSET(0));

	glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_NORMAL);
	glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(DrawVertex), BUFFER_OFFSET(12));

	glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_UV0);
	glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION_UV0, 2, GL_FLOAT, GL_FALSE, sizeof(DrawVertex), BUFFER_OFFSET(24));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
#endif
}