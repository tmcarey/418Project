#include "Skybox.h"
#include <iostream>

cl::Image2D* loadSkybox(const char* path, cl::Context context) {
	cl::Image2D* out = new cl::Image2D[6];
	Log("Loading Skybox...");
	std::string bufPath(path);
	out[0] = loadImage((bufPath + std::string("/top.png")).c_str(), context);
	out[1] = loadImage((bufPath + std::string("/bottom.png")).c_str(), context);
	out[3] = loadImage((bufPath + std::string("/right.png")).c_str(), context);
	out[2] = loadImage((bufPath + std::string("/left.png")).c_str(), context);
	out[4] = loadImage((bufPath + std::string("/front.png")).c_str(), context);
	out[5] = loadImage((bufPath + std::string("/back.png")).c_str(), context);
	Log("Done");
	return out;
}


cl::Image2D loadImage(const char* path, cl::Context context) {
	cl::Image2D out;
	int width, height, channels;
	unsigned char* data = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
	if (!data) {
		std::cout << path << std::endl;
		Log("Something went wrong loading an image");
		return out;
	}
	int err;
	out = cl::Image2D(context, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, cl::ImageFormat(CL_RGBA, CL_UNSIGNED_INT8), (size_t)width, (size_t)height, (size_t)(0), data, &err);
	std::cout << width << std::endl;
	if (err) {
		std::cout << "BRUH" << std::endl;
	}

	return out;
}