#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

/// <summary> Contains information about vertices such as position, normal, texture coordinate and color. </summary>
class VertexData {
public:
	glm::vec3 position, color, normal;
	glm::vec2 texCoord;
	VertexData(
		glm::vec3 _position = glm::vec3(0, 0, 0), glm::vec3 _color = glm::vec3(1, 1, 1),
		glm::vec3 _normal = glm::vec3(0, 0, 0), glm::vec2 _texCoord = glm::vec2(0, 0)) :
		position(_position), normal(_normal), color(_color), texCoord(_texCoord) {}

	bool operator==(const VertexData& other) const {
		return position == other.position && 
			   color == other.color &&
			   normal == other.normal &&
			   texCoord == other.texCoord;
	}
};


namespace std {
	template<> struct hash<VertexData> {
		size_t operator()(VertexData const& vertex) const {
			size_t seed = 0;
			seed ^= hash<glm::vec3>()(vertex.position) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			seed ^= hash<glm::vec3>()(vertex.color) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			seed ^= hash<glm::vec3>()(vertex.normal) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			seed ^= hash<glm::vec2>()(vertex.texCoord) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			return seed;
		}
	};
}