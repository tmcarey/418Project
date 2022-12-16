#include "Graphics.h"

#define SHADER_PATH "./"


bool Graphics::init() {
	if (glfwInit() != GLFW_TRUE)
	{
		return false;
	}
	
	return true;
}

GLuint Graphics::GenComputeProgram(const char* compute_path) {
	GLuint ComputeShader = LoadShaderSource(std::string(compute_path), GL_COMPUTE_SHADER);

	if (ComputeShader == 0) {
		printf("Something went wrong during shader loading. Aborting Shaders.");
		return 0;
	}

	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, ComputeShader);
	
	glLinkProgram(ProgramID);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, ComputeShader);

	glDeleteShader(ComputeShader);

	return ProgramID;
}

GLuint Graphics::GenShaderProgram(const char* vertex_path, const char* fragment_path) {
	GLuint VertexShader = LoadShaderSource(std::string(vertex_path), GL_VERTEX_SHADER);
	GLuint FragmentShader = LoadShaderSource(std::string(fragment_path), GL_FRAGMENT_SHADER);

	if (VertexShader == 0 || FragmentShader == 0) {
		printf("Something went wrong during shader loading. Aborting Shaders.");
		return 0;
	}


	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShader);
	glAttachShader(ProgramID, FragmentShader);
	glLinkProgram(ProgramID);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	if (!Result){
		fprintf(stderr, "Error in linking compute shader program\n");
		GLchar log[10240];
		GLsizei length;
		glGetProgramInfoLog(ProgramID, 10239, &length, log);
		fprintf(stderr, "Linker log:\n%s\n", log);
		exit(41);
	}
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShader);
	glDetachShader(ProgramID, FragmentShader);

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);

	return ProgramID;
}

GLuint Graphics::LoadShaderSource(std::string shaderName, GLenum shaderType) {
	std::string shader_path(SHADER_PATH);
	//LOG("Loading shader " << shaderName.c_str() << "...");
	std::string ShaderSource;
	std::ifstream shader_stream(shader_path + shaderName);
	if (shader_stream.is_open()) {
		std::stringstream sstr;
		sstr << shader_stream.rdbuf();
		ShaderSource = sstr.str();
		shader_stream.close();
	}
	else {
		printf("FAILED TO OPEN ");
		return 0;
	}

	//("Compiling shader " << shaderName.c_str() << "...");
	char const* SourcePointer = ShaderSource.c_str();
	GLuint ShaderID = glCreateShader(shaderType);
	glShaderSource(ShaderID, 1, &SourcePointer, NULL);
	glCompileShader(ShaderID);

	GLint Result = GL_FALSE;
	int InfoLogLength;
	glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(ShaderID, InfoLogLength, NULL, &ShaderErrorMessage[0]);
		printf(&ShaderErrorMessage[0]);
		return 0;
	}

	return ShaderID;
}

