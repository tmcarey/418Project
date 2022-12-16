#include "Chunk.h"
#define BUFFER_OFFSET(i) ((char*)NULL + (i))


Chunk::Chunk(float *_verts) : cubeVerts(_verts) {
	glGenBuffers(1, &VBO);

	glGenVertexArrays(1, &VAO);

	//Construct empty blocks
	blocks = new unsigned char** [CHUNK_SIZE];
	
	for(int i = 0; i < CHUNK_SIZE; i++){
		blocks[i] = new unsigned char* [CHUNK_SIZE];

		for(int j = 0; j < CHUNK_SIZE; j++){
			blocks[i][j] = new unsigned char[CHUNK_SIZE];
		}
	}

}

void Chunk::BreakBlock(int x, int y, int z){
	if(blocks[x][y][z] == 0){
		return;
	}else{
		blocks[x][y][z] = 0;
	}
	GenerateMesh();
}

void Chunk::RenderChunk(){
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertBuff->size());
}

Chunk::~Chunk()
{
	// Delete the blocks
	for (int i = 0; i < CHUNK_SIZE; ++i)
	{
		for (int j = 0; j < CHUNK_SIZE; ++j)
		{
			delete[] blocks[i][j];
		}

		delete[] blocks[i];
	}
	delete[] blocks;
}

void Chunk::InitializeBlocks(){
	for(int x = 0; x < CHUNK_SIZE; x++){
		for(int y = 0; y < CHUNK_SIZE; y++){
			for(int z = 0; z < CHUNK_SIZE; z++){
				blocks[x][y][z] = 1;
				/*if(abs(x - 32) + abs(z - 32) < ((y - 32) * (y - 32) / 15)){
					blocks[x][y][z] = 1;
				}else{
					blocks[x][y][z] = 0;
				}*/
			}
		}
	}
}

void Chunk::GenerateMesh(){
	if(vertBuff){
		free(vertBuff);
	}
	vertBuff = new std::vector<float>();
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				if (blocks[x][y][z] == 0)
				{
					// Don't create triangle data for inactive blocks
					continue;
				}

				if(x == CHUNK_SIZE - 1 ||  blocks[x + 1][y][z] == 0){
					for (int i = 0; i < 36; i += 6) {
						vertBuff->push_back(cubeVerts[i] + x);
						vertBuff->push_back(cubeVerts[i + 1] + y);
						vertBuff->push_back(cubeVerts[i + 2] + z);
						vertBuff->push_back(cubeVerts[i + 3]);
						vertBuff->push_back(cubeVerts[i + 4]);
						vertBuff->push_back(cubeVerts[i + 5]);
					}
				}
				if (x == 0 || blocks[x - 1][y][z] == 0) {
					for (int i = 36; i < 36 + 36; i += 6) {
						vertBuff->push_back(cubeVerts[i] + x);
						vertBuff->push_back(cubeVerts[i + 1] + y);
						vertBuff->push_back(cubeVerts[i + 2] + z);
						vertBuff->push_back(cubeVerts[i + 3]);
						vertBuff->push_back(cubeVerts[i + 4]);
						vertBuff->push_back(cubeVerts[i + 5]);
					}
				}
				if (y == CHUNK_SIZE - 1 || blocks[x][y + 1][z] == 0) {
					for (int i = 72; i < 72 + 36; i += 6) {
						vertBuff->push_back(cubeVerts[i] + x);
						vertBuff->push_back(cubeVerts[i + 1] + y);
						vertBuff->push_back(cubeVerts[i + 2] + z);
						vertBuff->push_back(cubeVerts[i + 3]);
						vertBuff->push_back(cubeVerts[i + 4]);
						vertBuff->push_back(cubeVerts[i + 5]);
					}
				}
				if (y == 0 || blocks[x][y - 1][z] == 0) {
					for (int i = 108; i < 108 + 6 * 3 * 2; i += 6) {
						vertBuff->push_back(cubeVerts[i] + x);
						vertBuff->push_back(cubeVerts[i + 1] + y);
						vertBuff->push_back(cubeVerts[i + 2] + z);
						vertBuff->push_back(cubeVerts[i + 3]);
						vertBuff->push_back(cubeVerts[i + 4]);
						vertBuff->push_back(cubeVerts[i + 5]);
					}
				}
				if (z == CHUNK_SIZE - 1 || blocks[x][y][z + 1] == 0) {
					for (int i = 144; i < 144 + 6 * 3 * 2; i += 6) {
						vertBuff->push_back(cubeVerts[i] + x);
						vertBuff->push_back(cubeVerts[i + 1] + y);
						vertBuff->push_back(cubeVerts[i + 2] + z);
						vertBuff->push_back(cubeVerts[i + 3]);
						vertBuff->push_back(cubeVerts[i + 4]);
						vertBuff->push_back(cubeVerts[i + 5]);
					}
				}
				if (z == 0 || !blocks[x][y][z - 1]) {
					for (int i = 180; i < 180 + 6 * 3 * 2; i += 6) {
						vertBuff->push_back(cubeVerts[i] + x);
						vertBuff->push_back(cubeVerts[i + 1] + y);
						vertBuff->push_back(cubeVerts[i + 2] + z);
						vertBuff->push_back(cubeVerts[i + 3]);
						vertBuff->push_back(cubeVerts[i + 4]);
						vertBuff->push_back(cubeVerts[i + 5]);
					}
				}
				
				
				/*for(int i = 0; i < 36 * 3; i+= 3){
					vertBuff->push_back(cubeVerts[i] + x);
					vertBuff->push_back(cubeVerts[i + 1] + y);
					vertBuff->push_back(cubeVerts[i + 2] + z);
				}*/
				//vertBuff->insert(vertBuff->end(), Chunk::cubeVerts, Chunk::cubeVerts + (36*3) + glm::vector(x, y, z));

			}
		}
	}
	
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertBuff->size() * sizeof(float), &vertBuff->front(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(3 * sizeof(float)));
}