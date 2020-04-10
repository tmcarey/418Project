#include <glad/glad.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <iphlpapi.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include <gtc/type_ptr.hpp>
#include "Connection.h"
#include "Graphics.h"
#include "Camera.h"
#include "Transform.h"
#include "Chunk.h"
#include "Player.h"
#include "Octree.h"
//#include "stb_image.h"

#pragma comment(lib, "Ws2_32.lib")

#define REQUIRE_CONNECT  false

int main()
{

    Connection *conn = Connection::CreateConnection();

	Graphics graphics;

	if(!graphics.init()){
		printf("Failed to Initialize OpenGL.\n");
		glfwTerminate();
		return -1;
	}

	Window* window = Window::Instance();
	
	if(!window->init()){
		printf("Failed to Initialize OpenGL.\n");
		glfwTerminate();
		return -1;
	}

	printf("%s\n", glGetString(GL_VERSION));

	glClearColor(0.529f, 0.808f, 0.922f, 0.0f);


	Player player(window);

	Camera camera(window);
	camera.Transform()->setParent(player.GetTransform());

	unsigned int shaderProgram = graphics.GenShaderProgram("VertexShader.glsl", "FragmentShader.glsl");
	unsigned int computeProgram = graphics.GenComputeProgram("RaytraceShader.glsl");
	glUseProgram(shaderProgram);
	//float colors[9] = { 1.0, 0.0,0.0, 0.0, 1.0, 0.0, 0.0,0.0,1.0 };
//	glUniform3fv(glGetUniformLocation(shaderProgram, "colors"), 9, colors);
	//GLuint transformInID = glGetUniformLocation(shaderProgram, "transform");


	float cubeVerts[36 * 3 * 2] = {
		//Positive X Face
		1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f,-1.0f,-1.0f, 
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f,-1.0f, 
		1.0f, 0.0f, 0.0f,
		1.0f,-1.0f,-1.0f, 
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 
		1.0f, 0.0f, 0.0f,
		1.0f,-1.0f, 1.0f, 
		1.0f, 0.0f, 0.0f,

		//Negative X Face
		-1.0f,-1.0f,-1.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 0.0f, 0.0f,

		//Positive Y Face
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f,-1.0f,
		0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f,-1.0f,
		0.0f, 1.0f, 0.0f,
		 1.0f, 1.0f, 1.0f,
		 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f,-1.0f,
		0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f,

		//Negative Y Face
		1.0f,-1.0f, 1.0f,
		0.0f, -1.0f, 0.0f,
		-1.0f,-1.0f, 1.0f,
		0.0f, -1.0f, 0.0f,
		-1.0f,-1.0f,-1.0f,
		0.0f, -1.0f, 0.0f,
		1.0f,-1.0f, 1.0f,
		0.0f, -1.0f, 0.0f,
		-1.0f,-1.0f,-1.0f,
		0.0f, -1.0f, 0.0f,
		1.0f,-1.0f,-1.0f,
		0.0f, -1.0f, 0.0f,

		//Positive Z Face
		 1.0f, 1.0f, 1.0f,
		 0.0f, 0.0f, 1.0f,
		 -1.0f, 1.0f, 1.0f,
		 0.0f, 0.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 0.0f, 0.0f, 1.0f,
		 - 1.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		 -1.0f,-1.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		//Negative Z Face
		1.0f, 1.0f,-1.0f,
		0.0f, 0.0f, -1.0f,
		-1.0f,-1.0f,-1.0f,
		0.0f, 0.0f, -1.0f,
		-1.0f, 1.0f,-1.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 1.0f,-1.0f,
		0.0f, 0.0f, -1.0f,
		1.0f,-1.0f,-1.0f,
		0.0f, 0.0f, -1.0f,
		-1.0f,-1.0f,-1.0f,
		0.0f, 0.0f, -1.0f,
	};
   


	//Create Render Texture
	GLuint renderTexture;
	glGenTextures(1, &renderTexture);
	glActiveTexture(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, window->getWidth(), window->getHeight(), 0, GL_RGBA, GL_FLOAT, NULL);

	// Because we're also using this tex as an image (in order to write to it),
	// we bind it to an image unit as well
	glBindImageTexture(0, renderTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glUseProgram(shaderProgram);
	
	

	GLuint vertArray;
	glGenVertexArrays(1, &vertArray);
	glBindVertexArray(vertArray);

	GLuint posBuf;
	glGenBuffers(1, &posBuf);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf);
	float data[] = {
		-1.0f, -1.0f,
		-1.0f, 1.0f,
		1.0f, -1.0f,
		1.0f, 1.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, data, GL_STREAM_DRAW);
	GLint posPtr = glGetAttribLocation(shaderProgram, "aPos");
	glVertexAttribPointer(posPtr, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posPtr);


	glUseProgram(computeProgram);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	GLuint eyeLoc = glGetUniformLocation(computeProgram, "eye");
	GLuint ray00Loc = glGetUniformLocation(computeProgram, "ray00");
	GLuint ray01Loc = glGetUniformLocation(computeProgram, "ray01");
	GLuint ray10Loc = glGetUniformLocation(computeProgram, "ray10");
	GLuint ray11Loc = glGetUniformLocation(computeProgram, "ray11");

	glUniform1i(glGetUniformLocation(computeProgram, "frame"), 0);

	glUseProgram(shaderProgram);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glUniform1i(glGetUniformLocation(shaderProgram, "render"), 0);

		
	
	glEnable(GL_DEPTH_TEST);

	glUseProgram(shaderProgram);
	const int WORLD_SIZE = 1;
	Chunk *chunks[WORLD_SIZE][WORLD_SIZE];
	for(int i = 0; i < WORLD_SIZE;i++){
		for(int j = 0; j < WORLD_SIZE;j++){
			chunks[i][j] = new Chunk(cubeVerts);
			chunks[i][j]->InitializeBlocks();
				
		}
    }

	printf("done building mesh");

    while(REQUIRE_CONNECT && conn->conn_state != CONNECTION_STATE::CONNECTED){
        conn->AttemptConnection();
    }


	double time = glfwGetTime();
	double deltaTime = 0;
	float speed = 1.0f;
	Transform voxelMan;

	int x = 0;
	int y = 0;
	int z = 0;
	glm::mat4 camTransInv;
	glm::vec4 eyePos;
	glm::vec4 ray00;
	glm::vec4 ray01;
	glm::vec4 ray10;
	glm::vec4 ray11;

	Octree* octreeTrue = GenerateRandom(1);

    while(!window->closed()){
		
		glUseProgram(computeProgram);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		eyePos = glm::vec4(camera.Transform()->GetPosition(), 1.0);
		//printf("Eye: %f, %f, %f\n", eyePos.x, eyePos.y, eyePos.z);
		glUniform3fv(eyeLoc, 1, glm::value_ptr(glm::vec3(eyePos)));
		camTransInv =  glm::inverse(camera.Projection() * camera.View());
		ray00 =  camTransInv * glm::vec4(-1, -1, 0, 1);
		ray00 /= ray00.w;
		ray00 -= eyePos;
		//printf("Ray00: %f, %f, %f\n", ray00.x, ray00.y, ray00.z);
		ray10 =  camTransInv * glm::vec4(1, -1, 0, 1);
		ray10 /= ray10.w;
		ray10 -= eyePos;
		ray01 = camTransInv * glm::vec4(-1, 1, 0, 1);
		ray01 /= ray01.w;
		ray01 -= eyePos;
		ray11 = camTransInv * glm::vec4(1, 1, 0, 1);
		ray11 /= ray11.w;
		ray11 -= eyePos;
		//printf("Eye: %f, %f, %f", eyePos.x, eyePos.y, eyePos.z);
		glUniform3fv(ray00Loc, 1, glm::value_ptr(ray00));
		glUniform3fv(ray01Loc, 1, glm::value_ptr(ray01));
		glUniform3fv(ray10Loc, 1, glm::value_ptr(ray10));
		glUniform3fv(ray11Loc, 1, glm::value_ptr(ray11));
		
		for(int i = 0; i < WORLD_SIZE;i++){
			for(int j = 0; j < WORLD_SIZE; j++){
				//chunks[i][j]->RenderChunk();
			}
		}
		
		player.update(deltaTime);

		//FOR RAY TRACING
		glDispatchCompute(window->getWidth(), window->getHeight(), 1);
		
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glFinish();

		glUseProgram(shaderProgram);
		glBindVertexArray(vertArray);
		glActiveTexture(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderTexture);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		window->update();
		
		deltaTime = glfwGetTime() - time;
		time = glfwGetTime();
        
    }
}