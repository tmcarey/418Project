#pragma once
#include <stack>
#include <random>


typedef unsigned __int8 U8;
typedef unsigned __int16 U16;
typedef unsigned __int32 U32;

struct Octree {
	U16 data = 0;
	U16 child = 0;
};

Octree* GenerateRandom(int depth);



