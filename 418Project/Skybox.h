#pragma once
#include <CL/cl.hpp>
#include <glad\glad.h>
#include "Utils.h"
#include "Logger.h"

cl::Image2D* loadSkybox(const char* path, cl::Context context);

cl::Image2D loadImage(const char* path, cl::Context context);
