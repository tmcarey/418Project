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
#include <CL/cl.hpp>
#include <fstream>
//#include "stb_image.h"

#pragma comment(lib, "Ws2_32.lib")

#define REQUIRE_CONNECT  false

int main()
{
	// INITIALIZE COMPUTE
	std::vector<cl::Platform> platforms;

	cl::Platform::get(&platforms);

	auto platform = platforms.front();
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	auto device = devices.front();
	auto vendor = device.getInfo<CL_DEVICE_VENDOR>();
	auto version = device.getInfo<CL_DEVICE_VERSION>();

	const int WIDTH = 200;
	const int HEIGHT = 200;

	//IMPORT CODE
	std::ifstream rayCastShader("RayCast.cl");
	std::string src(std::istreambuf_iterator<char>(rayCastShader), (std::istreambuf_iterator<char>()));
	cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));

	Connection* conn = Connection::CreateConnection();

	Graphics graphics;

	if (!graphics.init()) {
		printf("Failed to Initialize OpenGL.\n");
		glfwTerminate();
		return -1;
	}

	Window* window = Window::Instance();

	if (!window->init()) {
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
		 -1.0f, 1.0f, 1.0f,
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

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
	/*const int WORLD_SIZE = 1;
	Chunk *chunks[WORLD_SIZE][WORLD_SIZE];
	for(int i = 0; i < WORLD_SIZE;i++){
		for(int j = 0; j < WORLD_SIZE;j++){
			chunks[i][j] = new Chunk(cubeVerts);
			chunks[i][j]->InitializeBlocks();

		}
	}*/

	cl_platform_id plat;
	clGetPlatformIDs(1, &plat, NULL);
	cl_context_properties props[] =
	{
		//OpenCL platform
		CL_CONTEXT_PLATFORM, (cl_context_properties)plat,
		//OpenGL context
		CL_GL_CONTEXT_KHR,   (cl_context_properties)wglGetCurrentContext(),
		//HDC used to create the OpenGL context
		 CL_WGL_HDC_KHR,     (cl_context_properties)wglGetCurrentDC(),
		 0
	};
	cl::Context context(device, props);
	cl::Program program(context, sources);

	auto builderr = program.build("-cl-std=CL1.2");
	if (builderr != CL_SUCCESS) {
		printf("BUILD FAILED\n");
		std::cout << "OpenCL: Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << "\n";
	}
	//int buf[WIDTH * HEIGHT];
	//cl::Buffer memBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(buf));
	cl::Kernel kernel(program, "RayCast");
	cl::ImageGL image(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, renderTexture, 0);
	cl::CommandQueue queue(context, device, NULL, NULL);
	//GO FOR IT
	while (REQUIRE_CONNECT && conn->conn_state != CONNECTION_STATE::CONNECTED) {
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

	std::vector<cl::Memory> datas;
	datas.push_back(image);
	OctreeHashmap* basic = GenerateRandom(1);
	cl::Event ev;
	cl::NDRange r(WIDTH, HEIGHT);

	while (!window->closed()) {
		eyePos = glm::vec4(camera.Transform()->GetPosition(), 1.0);
		//printf("Eye: %f, %f, %f\n", eyePos.x, eyePos.y, eyePos.z);
//		glUniform3fv(eyeLoc, 1, glm::value_ptr(glm::vec3(eyePos)));
		camTransInv = glm::inverse(camera.Projection() * camera.View());
		ray00 = camTransInv * glm::vec4(-1, -1, 0, 1);
		ray00 /= ray00.w;
		ray00 -= eyePos;
		//printf("Ray00: %f, %f, %f\n", ray00.x, ray00.y, ray00.z);
		ray10 = camTransInv * glm::vec4(1, -1, 0, 1);
		ray10 /= ray10.w;
		ray10 -= eyePos;
		ray01 = camTransInv * glm::vec4(-1, 1, 0, 1);
		ray01 /= ray01.w;
		ray01 -= eyePos;
		ray11 = camTransInv * glm::vec4(1, 1, 0, 1);
		ray11 /= ray11.w;
		ray11 -= eyePos;
		queue.enqueueAcquireGLObjects(&datas, NULL, &ev);
		ev.wait();
		queue.finish();
		kernel.setArg(0, image);
		cl_float3 arg_eyePos = { eyePos.x, eyePos.y, eyePos.z };
		kernel.setArg(1, sizeof(cl_float3), &arg_eyePos);
		cl_float3 arg_ray00 = { ray00.x, ray00.y, ray00.z };
		kernel.setArg(2, sizeof(cl_float3), &arg_ray00);
		cl_float3 arg_ray10 = { ray10.x, ray10.y, ray10.z };
		kernel.setArg(3, sizeof(cl_float3), &arg_ray10);
		cl_float3 arg_ray01 = { ray01.x, ray01.y, ray01.z };
		kernel.setArg(4, sizeof(cl_float3), &arg_ray01);
		cl_float3 arg_ray11 = { ray11.x, ray11.y, ray11.z };
		kernel.setArg(5, sizeof(cl_float3), &arg_ray11);

		auto err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(WIDTH, HEIGHT), cl::NDRange());
		printf("%d", err);
		queue.enqueueReleaseGLObjects(&datas, NULL, &ev);
		ev.wait();
		//printf("Eye: %f, %f, %f", eyePos.x, eyePos.y, eyePos.z);
		//glUniform3fv(ray00Loc, 1, glm::value_ptr(ray00));
		//glUniform3fv(ray01Loc, 1, glm::value_ptr(ray01));
		//glUniform3fv(ray10Loc, 1, glm::value_ptr(ray10));
		//glUniform3fv(ray11Loc, 1, glm::value_ptr(ray11));
		//queue.finish();
		//glUseProgram(computeProgram);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		player.update(deltaTime);

		//FOR RAY TRACING
		//glDispatchCompute(window->getWidth(), window->getHeight(), 1);

		//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		//glFinish();

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