#pragma once
#include <vector>
#include "Platfrom.h"

using namespace std;

struct ModelElement
{
	int		vertexOffset;
	int		indexCount;
	int		indexOffset;
};


class Model
{
public:
	Model();
	~Model();

	bool					Load(const char* filename);
	int						GetElementCount() const;
	const ModelElement*		GetElement(int index) const;

	void					Bind();

	void					Clear();

private:
	vector<ModelElement>	elements_;

	GLuint		vao_;
	GLuint		vbo_;
	GLuint		ibo_;

};

