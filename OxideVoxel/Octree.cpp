#include "Octree.h"
#include <iostream>


void Octree::SaveToFile(const char* path) {
	//FILE* file = fopen(path, "wb");
	//fwrite(data, 1, 10, file);
}

RayCastHit Octree::trace(glm::vec3 origin, glm::vec3 d) {
	RayCastHit out;
	origin /= world_scale;
	origin += 1.0f;
	d = glm::normalize(d);
	std::tuple<int, int> stack[23];
	int depth = 22;
	int currNode = 0;
	const int maxDepth = 23; //floating point precision
	const float epsilon = exp2((float)(-maxDepth));

	if (fabs(d.x) < epsilon) d.x = copysign(epsilon, d.x);
	if (fabs(d.y) < epsilon) d.y = copysign(epsilon, d.y);
	if (fabs(d.z) < epsilon) d.z = copysign(epsilon, d.z);


	float tx_coef = 1.0f / -fabs(d.x);
	float ty_coef = 1.0f / -fabs(d.y);
	float tz_coef = 1.0f / -fabs(d.z);

	float tx_bias = tx_coef * origin.x;
	float ty_bias = ty_coef * origin.y;
	float tz_bias = tz_coef * origin.z;

	int correction_mask = 7;
	if (d.x > 0.0f) {
		correction_mask ^= 1;
		tx_bias = 3.0f * tx_coef - tx_bias;
	}
	if (d.y > 0.0f) {
		correction_mask ^= 2;
		ty_bias = 3.0f * ty_coef - ty_bias;
	}
	if (d.z > 0.0f) {
		correction_mask ^= 4;
		tz_bias = 3.0f * tz_coef - tz_bias;
	}


	float tx_min = (2.0f) * tx_coef - tx_bias;
	float tx_max = (1.0f) * tx_coef - tx_bias;
	//These autofail the trace if we miss the cube

	float ty_min = (2.0f) * ty_coef - ty_bias;
	float ty_max = (1.0f) * ty_coef - ty_bias;


	float tz_min = (2.0f) * tz_coef - tz_bias;
	float tz_max = (1.0f) * tz_coef - tz_bias;

	float t_enter = fmax(fmax(tx_min, ty_min), tz_min);
	float t_exit = fmin(fmin(tx_max, ty_max), tz_max);
	float t_lim = t_exit;
	t_enter = fmax(t_enter, 0.0f);
	t_exit = t_exit;
	int index = 0;
	float scale = 0.5f;
	glm::vec3 pos(1.0f, 1.0f, 1.0f);
	//Initialize to size of first-level node
	//Compare entry time to center axis
	if (1.5f * tx_coef - tx_bias > t_enter) index ^= 1, pos.x = 1.5f;
	if (1.5f * ty_coef - ty_bias > t_enter) index ^= 2, pos.y = 1.5f;
	if (1.5f * tz_coef - tz_bias > t_enter) index ^= 4, pos.z = 1.5f;
	out.lastNode = 0;
	out.lastNodeIndex = 0;
	out.lastNodeDepth = 22;

	U16 octree = 0;

	if (t_enter > t_exit) {
		out.hit = false;
		return out;
	}
	out.norm = glm::vec3(0.0, 0.0, 0.0);

	while (depth < 23) {
		octree = data[(10 * currNode) + 1];
		float tx_far = pos.x * tx_coef - tx_bias;
		float ty_far = pos.y * ty_coef - ty_bias;
		float tz_far = pos.z * tz_coef - tz_bias;
		float tc_exit = fmin(fmin(tx_far, ty_far), tz_far);

		if ((((octree >> 8) >> (index ^ correction_mask)) % 2) && t_enter <= t_exit) {
			float tv_exit = fmin(t_exit, tc_exit);

			if (t_enter <= tv_exit) {
				if (!((octree >> (index ^ correction_mask)) % 2)) {
					//THis is leaf, done
					out.hitNode = currNode;
					float tx_close = tx_far + (scale * tx_coef);
					float ty_close = ty_far + (scale * ty_coef);
					float tz_close = tz_far + (scale * tz_coef);
					if (tx_close >= ty_close && tx_close >= tz_close) {
						out.norm = glm::vec3(1.0, 0.0, 0.0);
					}
					else if (ty_close >= tx_close && ty_close >= tz_close) {
						out.norm = glm::vec3(0.0, 1.0, 0.0);
					}
					else if (tz_close >= tx_close && tz_close >= ty_close) {
						out.norm = glm::vec3(0.0, 0.0, 1.0);
					}
					break;
				}
				else {
					float half_scale = scale * 0.5f;
					float tx_center = half_scale * tx_coef + tx_far;
					float ty_center = half_scale * ty_coef + ty_far;
					float tz_center = half_scale * tz_coef + tz_far;
					//PUSH
					stack[depth] = std::tuple<int, int>(currNode, *reinterpret_cast<int*>(&t_exit));
					if (depth < 1) {
						int f = 1;
					}
					currNode = data[(10 * currNode) + 2 + (index ^ correction_mask)];
					depth -= 1;
					index = 0;
					scale = half_scale;

					if (tx_center > t_enter) index ^= 1, pos.x += scale;
					if (ty_center > t_enter) index ^= 2, pos.y += scale;
					if (tz_center > t_enter) index ^= 4, pos.z += scale;

					t_exit = tv_exit;

				}
				continue;
			}
		}
		//ADVANCE
		int step_mask = 0;
		//norm2 = (float3)(0.0,0.0,0.0);
		if (tx_far <= tc_exit) {
			//	norm2.x = -1.0;
			step_mask ^= 1; pos.x -= scale;
		}
		if (ty_far <= tc_exit) {
			//norm2.y = -1.0;
			step_mask ^= 2; pos.y -= scale;
		}
		if (tz_far <= tc_exit) {
			//norm2.z = -1.0;
			step_mask ^= 4; pos.z -= scale;
		}
		out.lastNodeIndex = index ^ correction_mask;
		out.lastNodeDepth = depth;
		out.lastNode = currNode;

		t_enter = tc_exit;
		index ^= step_mask;

		if ((step_mask & index) != 0) {
			//POP
			unsigned int diff_bits = 0;
			float x_added = pos.x + scale;
			float y_added = pos.y + scale;
			float z_added = pos.z + scale;
			if ((step_mask & 1) != 0) diff_bits |= *reinterpret_cast<int*>(&pos.x) ^ *reinterpret_cast<int*>(&x_added);
			if ((step_mask & 2) != 0) diff_bits |= *reinterpret_cast<int*>(&pos.y) ^ *reinterpret_cast<int*>(&y_added);
			if ((step_mask & 4) != 0) diff_bits |= *reinterpret_cast<int*>(&pos.z) ^ *reinterpret_cast<int*>(&z_added);
			float fdiff = (float)diff_bits;
			depth = (*reinterpret_cast<int*>(&fdiff) >> 23) - 127;
			int idepth = (depth - maxDepth + 127) << 23;
			scale = *reinterpret_cast<float*>(&idepth);
			std::tuple<int, int> backToTheFuture = stack[depth];
			currNode = std::get<0>(backToTheFuture);
			int fback = std::get<1>(backToTheFuture);
			t_exit = *reinterpret_cast<float*>(&fback);

			int shx = (*reinterpret_cast<int*>(&pos.x)) >> depth;
			int shy = (*reinterpret_cast<int*>(&pos.y)) >> depth;
			int shz = (*reinterpret_cast<int*>(&pos.z)) >> depth;
			int tshx = shx << depth;
			int tshy = shy << depth;
			int tshz = shz << depth;
			pos.x = (*reinterpret_cast<float*>(&tshx));
			pos.y = (*reinterpret_cast<float*>(&tshy));
			pos.z = (*reinterpret_cast<float*>(&tshz));
			index = (shx & 1) | ((shy & 1) << 1) | ((shz & 1) << 2);

			t_lim = 0.0f;

		}


	}
	if ((correction_mask & 1) == 0) {
		pos.x = 3.0f - scale - pos.x; out.norm.x = -out.norm.x;
	}
	if ((correction_mask & 2) == 0) {
		pos.y = 3.0f - scale - pos.y; out.norm.y = -out.norm.y;
	}
	if ((correction_mask & 4) == 0) {
		pos.z = 3.0f - scale - pos.z; out.norm.z = -out.norm.z;
	}
	out.pos.x = fmin(fmax(origin.x + t_enter * d.x, pos.x + epsilon), pos.x + scale - epsilon);
	out.pos.y = fmin(fmax(origin.y + t_enter * d.y, pos.y + epsilon), pos.y + scale - epsilon);
	out.pos.z = fmin(fmax(origin.z + t_enter * d.z, pos.z + epsilon), pos.z + scale - epsilon);
	out.pos -= 1.0f;
	out.pos *= world_scale;
	origin -= 1.0f;
	origin *= world_scale;
	out.hitIdx = index ^ correction_mask;
	out.dist = glm::distance(out.pos, origin);
	if (depth >= 23) {
		out.hit = false;
		return out;
	}
	else {
		out.hit = true;
		return out;
	}

}


void HeightmapHelper(unsigned short* out, unsigned char* img, int parentNode, int width, int height2, glm::vec3 pos, float scale, int res, int max_res, int* lim) {
	if (*lim >= ((2048 * 256) / 10)) {
		std::cout << "TOO MUCH MEMORY" << std::endl;
		return;
	}
	unsigned short* current = out + ((long long)parentNode * (long long)10);
	int channels = 4;
	int threshold = (pos.z) * 255;
	int bl = ((width * (pos.x - scale)) + (height2 * (pos.y - scale)));
	int br = ((width * (pos.x + scale)) + (height2 * (pos.y - scale)));
	int fl = ((width * (pos.x - scale)) + (height2 * (pos.y + scale)));
	int fr = ((width * (pos.x + scale)) + (height2 * (pos.y + scale)));
	unsigned char* pixelOffset = img + ((long long)(bl) * (long long)channels);
	if (res < max_res) {
		current[1] |= (1 << 0b010) << 8;
		current[1] |= (1 << 0b000) << 8;
		current[1] |= (1 << 0b000);
		current[2 + (0b000)] = *lim;
		*lim = *lim + 1;
		HeightmapHelper(out, img, *lim - 1, width, height2, pos + glm::vec3(-scale, -scale, scale), scale / 2, res + 1, max_res, lim);
		current[1] |= 1 << 0b010;
		current[2 + (0b010)] = *lim;
		*lim = *lim + 1;
		HeightmapHelper(out, img, *lim - 1, width, height2, pos + glm::vec3(-scale, -scale, -scale), scale / 2, res + 1, max_res, lim);
	}
	else {
		if (pixelOffset[0] > (pos.z - scale) * 255) {
			current[1] |= (1 << 0b010) << 8;
			if (pixelOffset[0] > threshold) {
				current[1] |= (1 << 0b000) << 8;
			}
		}
	}
	pixelOffset = img + ((long long)(br) * (long long)channels);
	if (res < max_res) {
	current[1] |= (1 << 0b011) << 8;
	current[1] |= (1 << 0b001) << 8;
		current[1] |= 1 << 0b001;
		current[1] |= 1 << 0b011;
		current[2 + (0b001)] = *lim;
		*lim = *lim + 1;
		HeightmapHelper(out, img, *lim - 1, width, height2, pos + glm::vec3(-scale, scale, scale), scale / 2, res + 1, max_res, lim);
		current[2 + (0b011)] = *lim;
		*lim = *lim + 1;
		HeightmapHelper(out, img, *lim - 1, width, height2, pos + glm::vec3(-scale, scale, -scale), scale / 2, res + 1, max_res, lim);
	}
	else {
		if (pixelOffset[0] > (pos.z - scale) * 255) {
			current[1] |= (1 << 0b011) << 8;
			if (pixelOffset[0] > threshold) {
				current[1] |= (1 << 0b001) << 8;
			}
		}
	}
	pixelOffset = img + ((long long)(fl) * (long long)channels);
	if (res < max_res) {
	current[1] |= (1 << 0b110) << 8;
	current[1] |= (1 << 0b100) << 8;
		current[1] |= 1 << 0b100;
		current[2 + (0b100)] = *lim;
		*lim = *lim + 1;
		HeightmapHelper(out, img, *lim - 1, width, height2, pos + glm::vec3(scale, -scale, scale), scale / 2, res + 1, max_res, lim);
		current[1] |= 1 << 0b110;
		current[2 + (0b110)] = *lim;
		*lim = *lim + 1;
		HeightmapHelper(out, img, *lim - 1, width, height2, pos + glm::vec3(scale, -scale, -scale), scale / 2, res + 1, max_res, lim);
		pixelOffset = img + ((long long)(fr) * (long long)channels);
	}
	else {
		if (pixelOffset[0] > (pos.z - scale) * 255) {
			current[1] |= (1 << 0b110) << 8;
			if (pixelOffset[0] > threshold) {
				current[1] |= (1 << 0b100) << 8;
			}
		}
	}
	if (res < max_res) {
		current[1] |= (1 << 0b111) << 8;
		current[1] |= (1 << 0b101) << 8;
		current[1] |= 1 << 0b101;
		current[2 + (0b101)] = *lim;
		*lim = *lim + 1;
		HeightmapHelper(out, img, *lim - 1, width, height2, pos + glm::vec3(scale, scale, scale), scale / 2, res + 1, max_res, lim);
		current[1] |= 1 << 0b111;
		current[2 + (0b111)] = *lim;
		*lim = *lim + 1;
		HeightmapHelper(out, img, *lim - 1, width, height2, pos + glm::vec3(scale, scale, -scale), scale / 2, res + 1, max_res, lim);
	}
	else {
		if (pixelOffset[0] > (pos.z - scale) * 255) {
			current[1] |= (1 << 0b111) << 8;
			if (pixelOffset[0] > threshold) {
				current[1] |= (1 << 0b101) << 8;
			}
		}
	}
}

Octree Octree::LoadFromHeightmap(const char* path, float scale, int resolution) {
	int width, height, channels;
	unsigned char* data = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
	int curr_res = 1;
	float res_scale = pow(2, -curr_res);
	unsigned short* out = new unsigned short[2048 * 256]{ 0 };
	int currNode = 0;
	glm::vec3 pos = glm::vec3(1 / (2.0f), 1 / (2.0f), 1 / (2.0f));
	int* lim = new int;
	*lim = 1;
	HeightmapHelper(out, data, 0, width, height * height, pos, 1 / (4.0f), 1, resolution, lim);
	//std::cout << (int)out[1] << std::endl;
	//out[2] = 0;
	std::cout << *lim << std::endl;
	return Octree(out, scale);
}
