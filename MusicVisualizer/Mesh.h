#pragma once

#include <string>
#include <vector>
#include <fstream>

#include <GL/glew.h>

#include "VertexData.h"

enum Attribute { position = 0, normal, uv };

class Mesh {
public:
	Mesh() {}
	Mesh(std::string filepath);
	void LoadMFile(std::string filepath);
	void setupMesh();
	void reuploadIndexDataToGPU();
	void reuploadVertexDataToGPU();

	std::vector<VertexData> vertexData;
	std::vector<GLuint> indices;

	GLuint vbo, vao, ebo;
};