#pragma once
#include <vector>
#include <glad\glad.h>

class Chunk
{
public:
	Chunk(float *_verts);
	~Chunk();

	void BreakBlock(int x, int y, int z);

	void InitializeBlocks();

	void RenderChunk();

	void GenerateMesh();
	static const int CHUNK_SIZE = 256;


	
private:
	float* cubeVerts;
	unsigned int VBO;
	unsigned int VAO;
	unsigned char*** blocks;
	std::vector<float>* vertBuff;
};