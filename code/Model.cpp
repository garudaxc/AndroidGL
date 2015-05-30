#include "Model.h"
//#include "glUtil.h"
#include "MyLog.h"

#define BUFFER_OFFSET(offset) ((void *)(offset))

struct MyVertex
{
	float pos[3];
	float normal[3];
	float color[4];
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


bool Model::Load(const char* filename)
{
#ifdef GL_ARB_vertex_array_object
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);
#endif

	MyVertex vertices[] =
	{
		{ { -0.90f, 0.f, -0.90f }, { 0.0f, -1.0f, 0.0f }, { 1.f, 0.f, 0.f, 1.f }, { 0.f, 1.f } },
		{ { 0.90f, 0.f, -0.90f }, { 0.0f, -1.0f, 0.0f }, { 0.f, 1.f, 0.f, 1.f }, { 1.f, 1.f } },
		{ { -0.90f, 0.f, 0.90f }, { 0.0f, -1.0f, 0.0f }, { 0.f, 0.f, 1.f, 1.f }, { 0.f, 0.f } },
		{ { 0.90f, 0.f, 0.90f }, { 0.0f, -1.0f, 0.0f }, { 1.f, 0.f, 1.f, 1.f }, { 1.f, 0.f } },
	};

	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

#ifdef GL_ARB_vertex_array_object
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(0));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(12));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(24));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(40));

#endif

	unsigned short indexdata[] =
	{
		0, 1, 2, 2, 1, 3
	};

	glGenBuffers(1, &ibo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexdata), indexdata, GL_STATIC_DRAW);

#ifdef GL_ARB_vertex_array_object
	glBindVertexArray(0);
#endif

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	ModelElement elem;
	elem.faceCount = 2;
	elem.faceOffest = 0;
	elem.vertexOffset = 0;
	elements_.push_back(elem);

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
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(0));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(12));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(24));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(40));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
#endif
}