#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


class Graphics {
public:
	bool init();
	GLuint GenComputeProgram(const char* compute_path);
	GLuint GenShaderProgram(const char* vertex_path, const char* fragment_path);
	GLuint LoadShaderSource(std::string shaderName, GLenum shaderType);
};


