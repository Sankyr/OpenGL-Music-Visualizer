#include "Mesh.h"

Mesh::Mesh(std::string filepath) {
	LoadMFile(filepath);
}

void Mesh::setupMesh() {
	// Initialize VBO, VAO and EBO.
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	// Upload to GPU.
	reuploadIndexDataToGPU();
	reuploadVertexDataToGPU();
}

void Mesh::reuploadIndexDataToGPU() {
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

void Mesh::reuploadVertexDataToGPU() {
	auto dataSize = sizeof(VertexData);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * dataSize, vertexData.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(Attribute::position); // Positions.
	glVertexAttribPointer(Attribute::position, 3, GL_FLOAT, GL_FALSE, dataSize, 0);
	glEnableVertexAttribArray(Attribute::normal); // Normals.
	glVertexAttribPointer(Attribute::normal, 3, GL_FLOAT, GL_FALSE, dataSize, (GLvoid*)offsetof(VertexData, VertexData::normal));
	glEnableVertexAttribArray(Attribute::uv); // Normals.
	glVertexAttribPointer(Attribute::uv, 2, GL_FLOAT, GL_FALSE, dataSize, (GLvoid*)offsetof(VertexData, VertexData::texCoord));
}

void Mesh::LoadMFile(std::string filepath) {
	//std::vector<float> positions;
	//std::vector<float> normals;
	//std::vector<GLuint> indices;

	std::string contents;
	std::ifstream in(filepath, std::ios::in | std::ios::binary);
	if (in.is_open()) {
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
	}
	else
		throw(errno);

	const std::string vertexStr = "Vertex ";
	const std::string normal = "normal=(";
	const std::string uv = "uv=(";
	const std::string face = "Face ";
	size_t line = 0;
	while (true) {
		if (contents[line] == '#') {
			line = contents.find('\n', line + 1);
			if (line != std::string::npos)
				line++;
			else
				break;
			continue;
		}

		if (contents.substr(line, vertexStr.length()).compare(vertexStr) == 0) {
			VertexData vertex;
			size_t current = contents.find(' ', line + vertexStr.length()) + 2;
			size_t next;
			//position
			vertex.position.x = std::stof(contents.substr(current, contents.find(' ', current) - current), &next);
			current += next + 1;
			vertex.position.y = std::stof(contents.substr(current, contents.find(' ', current) - current), &next);
			current += next + 1;
			vertex.position.z = std::stof(contents.substr(current, contents.find(' ', current) - current), &next);
			current = current + next + 2;
			//normal
			if ((current = contents.find(normal, line + vertexStr.length())) != std::string::npos) {
				current += normal.length();
				vertex.normal.x = std::stof(contents.substr(current, contents.find(' ', current) - current), &next);
				current += next + 1;
				vertex.normal.y = std::stof(contents.substr(current, contents.find(' ', current) - current), &next);
				current += next + 1;
				vertex.normal.z = std::stof(contents.substr(current, contents.find(' ', current) - current), &next);
			}
			//uv
			if ((current = contents.find(uv, line + vertexStr.length())) != std::string::npos) {
				current += uv.length();
				vertex.texCoord.x = std::stof(contents.substr(current, contents.find(' ', current) - current), &next);
				current += next + 1;
				vertex.texCoord.y = std::stof(contents.substr(current, contents.find(' ', current) - current), &next);
			}

			vertexData.push_back(vertex);
		}

		if (contents.substr(line, face.length()).compare(face) == 0) {
			size_t current = contents.find(' ', line + face.length()) + 2;
			size_t next;
			indices.push_back((GLuint)std::stoul(contents.substr(current, contents.find(' ', current) - current), &next) - 1);
			current += next + 1;
			indices.push_back((GLuint)std::stoul(contents.substr(current, contents.find(' ', current) - current), &next) - 1);
			current += next + 1;
			indices.push_back((GLuint)std::stoul(contents.substr(current, contents.find(' ', current) - current), &next) - 1);
		}

		line = contents.find('\n', line + 1);
		if (line != std::string::npos)
			line++;
		else
			break;
	}
	//printf("shape[%d] num unique vertices: %d\n", shapeNum, mesh->vertexData.size());
	//shapeNum++;
	setupMesh();
}