#include <glad/glad.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <iphlpapi.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include <gtc/type_ptr.hpp>
#include "Graphics.h"
#include "Camera.h"
#include "Transform.h"
#include "Chunk.h"
#include "Player.h"
#include "Input.h"
#include "Octree.h"
#include <CL/cl.hpp>
#include <fstream>
#include "Skybox.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#define _USE_MATH_DEFINES
#include <math.h>
//#include "stb_image.h"
#include <chrono>

#pragma comment(lib, "Ws2_32.lib")

#define REQUIRE_CONNECT  false

using namespace std::chrono;

int main()
{


	bool takeSinglePicture = true;


	// INITIALIZE COMPUTE
	std::vector<cl::Platform> platforms;

	cl::Platform::get(&platforms);

	auto platform = platforms.front();
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	auto device = devices.front();
	auto vendor = device.getInfo<CL_DEVICE_VENDOR>();
	auto version = device.getInfo<CL_DEVICE_VERSION>();
	std::cout << device.getInfo<CL_DEVICE_MAX_PARAMETER_SIZE>() << std::endl;

	Input::Init();

	//IMPORT CODE
	std::ifstream rayCastShader("RayCast.cl");
	std::string src(std::istreambuf_iterator<char>(rayCastShader), (std::istreambuf_iterator<char>()));
	cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));

	//INITIALIZE GRAPHICS
	Graphics graphics;

	if (!graphics.init()) {
		printf("Failed to Initialize OpenGL.\n");
		glfwTerminate();
		return -1;
	}

	Window*	window = Window::Instance();
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
	player.GetTransform()->SetPosition(glm::vec3(250, 170, 250));
	player.GetTransform()->SetEulerAngles(glm::vec3(90.0f, 90.0f, 90.0f));

	unsigned int shaderProgram = graphics.GenShaderProgram("VertexShader.glsl", "FragmentShader.glsl");
	glUseProgram(shaderProgram);
	//unsigned int computeProgram = graphics.GenComputeProgram("RaytraceShader.glsl");


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
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glUniform1i(glGetUniformLocation(shaderProgram, "render"), 0);


	glEnable(GL_DEPTH_TEST);

	glUseProgram(shaderProgram);

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
	int err;
	cl::Context context(device, props, NULL, NULL, &err);
	cl::Program program(context, sources, &err);

	if (err) {
		std::cout << err << std::endl;
	}
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
	cl::Event ev;
	kernel.setArg(0, image);
	cl::Image2D* skybox = loadSkybox("H:/Resources/Skyboxes/space1", context);
	kernel.setArg(9, skybox[0]);
	kernel.setArg(10, skybox[1]);
	kernel.setArg(11, skybox[2]);
	kernel.setArg(12, skybox[3]);
	kernel.setArg(13, skybox[4]);
	kernel.setArg(14, skybox[5]);

	float scale = 256.0f;

	auto startHeightmap = high_resolution_clock::now();
	Octree mainOctree = Octree::LoadFromHeightmap("H:/Resources/Heightmaps/test-heightmap-3.png", scale, 11);
	auto endHeightMap = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(endHeightMap - startHeightmap);

	printf("Heightmap Generated in %lld\n", duration);
	queue.enqueueAcquireGLObjects(&datas, NULL, &ev);
	queue.finish();

	float gravity = 0.0f;
	cl::Buffer octreeData(context, CL_MEM_COPY_HOST_PTR, sizeof(unsigned int) * 100000 * 256, mainOctree.data, NULL);
	kernel.setArg(8, scale);
	while (!window->closed()) {
		/*RayCastHit below = mainOctree.trace(eyePos, glm::vec3(0, -1.0, 0));
		if (below.hit && below.dist - 2.0f < .0001f) {
			player.GetTransform()->SetPosition(glm::vec3(
				player.GetTransform()->GetPosition().x,
				below.pos.y + 2.0f,
				player.GetTransform()->GetPosition().z));
		}
		else {
			player.GetTransform()->SetPosition(player.GetTransform()->GetPosition() - (glm::vec3(0, 1.0f, 0) * (float)deltaTime * gravity));
		}*/
		//below = mainOctree.trace(eyePos, glm::vec3(0, -1.0, 0));
		///std::cout << below.dist << std::endl;

		player.update(deltaTime);
		eyePos = glm::vec4(camera.Transform()->GetPosition(), 1.0);
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
		kernel.setArg(6, octreeData);
		kernel.setArg(7, rand());

		auto startRender = high_resolution_clock::now();
		auto err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(window->getWidth(), window->getHeight()), cl::NDRange(4, 4, 1));
		queue.finish();
		
		auto endRender = high_resolution_clock::now();
		auto renderDuration = duration_cast<microseconds>(endRender - startRender);

		printf("Rendered in %lld us\n", renderDuration);


		if (player.paused) {
			int f = 0;
		}

		glm::vec3 yo = glm::mix(mix(ray00, ray01, 0.5f), mix(ray10, ray11, 0.5f), 0.5f);

		if (Input::GetKeyDown(GLFW_KEY_R)) {
			RayCastHit targeted = mainOctree.trace(eyePos, yo);
			if (targeted.hit) {
				//std::cout << targeted.lastNodeDepth << std::endl;
				if (targeted.dist <= 10.0f) {
					mainOctree.data[(targeted.lastNode * 10) + 1] |= ((1 << 8) << targeted.lastNodeIndex);
					octreeData = cl::Buffer(context, CL_MEM_COPY_HOST_PTR, sizeof(unsigned int) * 100000 * 128, mainOctree.data, NULL);
					kernel.setArg(6, octreeData);
				}
			}
		}
		else if (Input::GetKeyDown(GLFW_KEY_T)) {
			RayCastHit targeted = mainOctree.trace(eyePos, yo);
			if (targeted.hit) {
				//std::cout << targeted.lastNodeDepth << std::endl;
				if (targeted.dist <= 10.0f) {
					mainOctree.data[(targeted.hitNode * 10) + 1] ^= ((1 << 8) << targeted.hitIdx);
					octreeData = cl::Buffer(context, CL_MEM_COPY_HOST_PTR, sizeof(unsigned int) * 100000 * 128, mainOctree.data, NULL);
					kernel.setArg(6, octreeData);
				}
			}

		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glUseProgram(shaderProgram);
		glBindVertexArray(vertArray);
		glActiveTexture(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderTexture);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		window->update();

		if (takeSinglePicture) {
			int dataSize = 1920 * 1080 * 3;
			unsigned short* data = new unsigned short[dataSize];
			glReadPixels(0, 0, 1920, 1080, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_flip_vertically_on_write(1);
			stbi_write_png("../result.jpg", 1920, 1080, 3, data, 1920 * 3);
			takeSinglePicture = false;
			break;
		}

		deltaTime = glfwGetTime() - time;
		time = glfwGetTime();
		Input::flushInput();
	}

	queue.enqueueReleaseGLObjects(&datas, NULL, &ev);
	queue.finish();
}