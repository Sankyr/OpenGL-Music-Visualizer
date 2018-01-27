#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "GL/glew.h"


std::string readFile(const char *filePath) {
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if (!fileStream.is_open()) {
		std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

void compileShader(GLuint program, GLuint shader, const char *path) {
	// Read shader
	std::string shaderStr = readFile(path);
	const char *shaderSrc = shaderStr.c_str();

	GLint result = GL_FALSE;
	int logLength;

	// Compile shader
	std::cout << "Compiling " << path << std::endl;
	glShaderSource(shader, 1, &shaderSrc, NULL);
	glCompileShader(shader);

	// Check shader
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<GLchar> shaderError((logLength > 1) ? logLength : 1);
	glGetShaderInfoLog(shader, logLength, NULL, &shaderError[0]);
	std::cout << &shaderError[0] << std::endl;

	glAttachShader(program, shader);
}

void linkProgram(GLuint program) {
	GLint result = GL_FALSE;
	int logLength;

	std::cout << "Linking program" << std::endl;
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> programError((logLength > 1) ? logLength : 1);
	glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
	std::cout << &programError[0] << std::endl;
}

GLuint loadShaders(const char *vertex_path, const char *geometry_path, const char *fragment_path) {
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint geomShader = glCreateShader(GL_GEOMETRY_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint program = glCreateProgram();
	compileShader(program, vertShader, vertex_path);
	compileShader(program, geomShader, geometry_path);
	compileShader(program, fragShader, fragment_path);
	linkProgram(program);
	glDetachShader(program, vertShader);
	glDetachShader(program, geomShader);
	glDetachShader(program, fragShader);
	glDeleteShader(vertShader);
	glDeleteShader(geomShader);
	glDeleteShader(fragShader);

	return program;
}

GLuint loadShaders(const char *vertex_path, const char *fragment_path) {
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint program = glCreateProgram();
	compileShader(program, vertShader, vertex_path);
	compileShader(program, fragShader, fragment_path);
	linkProgram(program);
	glDetachShader(program, vertShader);
	glDetachShader(program, fragShader);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return program;
}

GLuint loadShaders(const char *compute_path) {
	GLuint compShader = glCreateShader(GL_COMPUTE_SHADER);
	GLuint program = glCreateProgram();
	compileShader(program, compShader, compute_path);
	linkProgram(program);
	glDetachShader(program, compShader);
	glDeleteShader(compShader);

	return program;
}