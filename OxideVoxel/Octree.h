#pragma once
#include <stack>
#include <random>
#include <unordered_map>
#include "stb_image.h"
#include <glm.hpp>
#include "Logger.h"


typedef unsigned __int8 U8;
typedef unsigned __int16 U16;
typedef unsigned __int32 U32;
typedef unsigned __int32 NODE;



struct RayCastHit {
	bool hit;
	glm::vec3 norm;
	glm::vec3 pos;
	float dist;
	short hitIdx;
	int hitNode;
	int lastNode;
	int lastNodeDepth;
	int lastNodeIndex;
};

class Octree {
public:
	unsigned short* data;
	float world_scale;
	Octree(unsigned short* _data, float _scale) : data(_data), world_scale(_scale) {};
	RayCastHit trace(glm::vec3 origin, glm::vec3 d);
	static Octree LoadFromHeightmap(const char* path, float scale, int resolution);
	void SaveToFile(const char* path);
};
