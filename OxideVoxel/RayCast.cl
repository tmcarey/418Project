typedef char U8;
const sampler_t skyboxSampler = CLK_NORMALIZED_COORDS_FALSE |
CLK_ADDRESS_REPEAT |
CLK_FILTER_NEAREST;
float4 rand(uint2* state)
{
	const float4 invMaxInt = (float4) (1.0f / 4294967296.0f, 1.0f / 4294967296.0f, 1.0f / 4294967296.0f, 0);
	uint x = (*state).x * 17 + (*state).y * 13123;
	(*state).x = (x << 13) ^ x;
	(*state).y ^= (x << 7);

	uint4 tmp = (uint4)
		((x * (x * x * 15731 + 74323) + 871483),
		(x * (x * x * 13734 + 37828) + 234234),
			(x * (x * x * 11687 + 26461) + 137589), 0);

	return convert_float4(tmp) * invMaxInt;
}
bool trace(float3 origin, float3 d, __global short* data, float3* hitPos, int* hitIdx, float3* norm) {
	int2 stack[23];
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


	float4 green = (float4)(0.0, 1.0, 0.0, 1.0);

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
	float3 pos = (float3)(1.0f, 1.0f, 1.0f);
	//Initialize to size of first-level node
	//Compare entry time to center axis
	if (1.5f * tx_coef - tx_bias > t_enter) index ^= 1, pos.x = 1.5f;
	if (1.5f * ty_coef - ty_bias > t_enter) index ^= 2, pos.y = 1.5f;
	if (1.5f * tz_coef - tz_bias > t_enter) index ^= 4, pos.z = 1.5f;


	short octree = 0;

	if (t_enter > t_exit) {

		return false;
	}
	float3 norm2;
	norm2 = (float3)(0.0, 0.0, 0.0);

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
					if (norm) {
						float tx_close = tx_far + (scale * tx_coef);
						float ty_close = ty_far + (scale * ty_coef);
						float tz_close = tz_far + (scale * tz_coef);
						if (tx_close >= ty_close && tx_close >= tz_close) {
							norm2 = (float3)(1.0, 0.0, 0.0);
						}
						else if (ty_close >= tx_close && ty_close >= tz_close) {
							norm2 = (float3)(0.0, 1.0, 0.0);
						}
						else if (tz_close >= tx_close && tz_close >= ty_close) {
							norm2 = (float3)(0.0, 0.0, 1.0);
						}
					}
					break;
				}
				else {
					float half_scale = scale * 0.5f;
					float tx_center = half_scale * tx_coef + tx_far;
					float ty_center = half_scale * ty_coef + ty_far;
					float tz_center = half_scale * tz_coef + tz_far;
					//PUSH
					stack[depth] = (int2)(currNode, __float_as_int(t_exit));
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

		t_enter = tc_exit;
		index ^= step_mask;

		if ((step_mask & index) != 0) {
			//POP
			unsigned int diff_bits = 0;
			if ((step_mask & 1) != 0) diff_bits |= __float_as_int(pos.x) ^ __float_as_int(pos.x + scale);
			if ((step_mask & 2) != 0) diff_bits |= __float_as_int(pos.y) ^ __float_as_int(pos.y + scale);
			if ((step_mask & 4) != 0) diff_bits |= __float_as_int(pos.z) ^ __float_as_int(pos.z + scale);
			depth = (__float_as_int((float)diff_bits) >> 23) - 127;
			scale = __int_as_float((depth - maxDepth + 127) << 23);
			int2 backToTheFuture = stack[depth];
			currNode = backToTheFuture.x;
			t_exit = __int_as_float(backToTheFuture.y);

			int shx = __float_as_int(pos.x) >> depth;
			int shy = __float_as_int(pos.y) >> depth;
			int shz = __float_as_int(pos.z) >> depth;
			pos.x = __int_as_float(shx << depth);
			pos.y = __int_as_float(shy << depth);
			pos.z = __int_as_float(shz << depth);
			index = (shx & 1) | ((shy & 1) << 1) | ((shz & 1) << 2);

			t_lim = 0.0f;

		}


	}
	if ((correction_mask & 1) == 0) {
		pos.x = 3.0f - scale - pos.x; (norm2).x = -(norm2).x;
	}
	if ((correction_mask & 2) == 0) {
		pos.y = 3.0f - scale - pos.y; (norm2).y = -(norm2).y;
	}
	if ((correction_mask & 4) == 0) {
		pos.z = 3.0f - scale - pos.z; (norm2).z = -(norm2).z;
	}
	if (norm) {
		*norm = norm2;
	}
	if (hitPos) {
		hitPos->x = fmin(fmax(origin.x + t_enter * d.x, pos.x + epsilon), pos.x + scale - epsilon);
		hitPos->y = fmin(fmax(origin.y + t_enter * d.y, pos.y + epsilon), pos.y + scale - epsilon);
		hitPos->z = fmin(fmax(origin.z + t_enter * d.z, pos.z + epsilon), pos.z + scale - epsilon);
	}
	if (hitIdx) {
		*hitIdx = index ^ correction_mask;
	}
	if (depth >= 23) {
		return false;
	}
	else {
		return true;
	}
}

float4 querySkybox(image2d_t skybox_top,
	image2d_t skybox_bottom,
	image2d_t skybox_left,
	image2d_t skybox_right,
	image2d_t skybox_front,
	image2d_t skybox_back,
	float3 vec
) {
	int2 size = get_image_dim(skybox_top);
	int reach = size.x / 2;
	float bound = 0;
	float2 meters = (float2)(0, 0);
	uint4 skyboxQuery;
	if (fabs(vec.x) >= fabs(vec.y) && fabs(vec.x) >= fabs(vec.z)) {
		bound = vec.x;
		meters = (float2)(vec.z, -vec.y);
		int2 coords = (int2)((reach * (meters.x / bound)) + reach, (reach * (meters.y / fabs(bound))) + reach);
		if (vec.x > 0) {
			skyboxQuery = read_imageui(skybox_left, skyboxSampler, coords);
		}
		else {
			skyboxQuery = read_imageui(skybox_right, skyboxSampler, coords);
		}
	}
	else if (fabs(vec.y) >= fabs(vec.x) && fabs(vec.y) >= fabs(vec.z)) {
		bound = vec.y;
		meters = (float2)(-vec.x, vec.z);
		int2 coords = (int2)((reach * (meters.x / fabs(bound))) + reach, (reach * (meters.y / bound)) + reach);
		if (vec.y > 0) {
			skyboxQuery = read_imageui(skybox_top, skyboxSampler, coords);
		}
		else {
			skyboxQuery = read_imageui(skybox_bottom, skyboxSampler, coords);

		}
	}
	else {
		bound = vec.z;
		meters = (float2)(-vec.x, -vec.y);
		int2 coords = (int2)((reach * (meters.x / bound)) + reach, (reach * (meters.y / fabs(bound))) + reach);
		if (vec.z > 0) {
			skyboxQuery = read_imageui(skybox_front, skyboxSampler, coords);
		}
		else {
			skyboxQuery = read_imageui(skybox_back, skyboxSampler, coords);
		}
	}

	float4 skyboxFloated;
	skyboxFloated.x = skyboxQuery.x / 255.0f;
	skyboxFloated.y = skyboxQuery.y / 255.0f;
	skyboxFloated.z = skyboxQuery.z / 255.0f;
	skyboxFloated.w = 1.0f;
	return skyboxFloated;

}

__kernel void RayCast(__write_only image2d_t image,
	float3 eye,
	float3 ray00,
	float3 ray10,
	float3 ray01,
	float3 ray11,
	__global short* data,
	int frame_seed,
	float scale,
	__read_only image2d_t skybox_top,
	__read_only image2d_t skybox_bottom,
	__read_only image2d_t skybox_left,
	__read_only image2d_t skybox_right,
	__read_only image2d_t skybox_front,
	__read_only image2d_t skybox_back
) {
	float bias = 0.000001f;
	eye /= scale;
	eye += 1.0f;
	int N = 4;
	uint2 forRand = (get_global_id(0) * 13123 + get_global_id(1) * 17 + frame_seed * 251, get_global_id(1) * 13123 + get_global_id(0) * 17 + frame_seed * 251);
	int2 size = get_image_dim(image);
	int2 pix = (int2)(get_global_id(0), get_global_id(1));
	float2 pos = (float2)(pix.x, pix.y) / (float2)(size.x - 1, size.y - 1);
	float3 hitPos;
	float3 dir = mix(mix(ray00, ray01, pos.y), mix(ray10, ray11, pos.y), pos.x);
	int hitIdx;
	float3 norm;
	float4 albedo = (float4)(0.498, 0.784, 0.314, 1.0);
	//float4 directLighting = (float4)(0, 0, 0, 1.0);
	//float4 inDirectLighting = (float4)(0, 0, 0, 1.0);
	float4 light = (float4)(1.0, 1.0, 0.78, 1.0);
	float intensity = 0.2;
	float3 sunLoc = (float3)(1.0, 0.5, -0.3);
	bool hit = trace(eye, dir, data, &hitPos, &hitIdx, &norm);
	//write_imagef(image, pix, light);
	//return;
	float4 col = (float4)(0.0, 0.0, 0.0, 1.0);
	if (hitIdx % 2) {
		col.z = 1.0;
	}
	if ((hitIdx >> 1) % 2) {
		col.y = 1.0;
	}
	if ((hitIdx >> 2) % 2) {
		col.x = 1.0;
	}
	int castDepth = 0;
	float4 ambient = (float4)(0.2, 0.2, 0.2, 1.0);
	float4 inDirectLighting = (float4)(0, 0, 0, 1.0);
	if (hit) {
		write_imagef(image, pix, col);
		float4 directLighting = ambient;
		float3 shadow_hitPos;
		float3 shadow_norm;
		bool shadow_hit = trace(hitPos + (norm * bias), sunLoc, data, 0, 0, 0);
		if (!shadow_hit) {
				directLighting += albedo * intensity *  1.25f * dot(sunLoc, norm);
		}
		
		float3 v = (float3)(norm.y, -norm.x, 0.0);
		float3 u = cross(v, norm);
		//Monte Carlo Indirect Lighting
		/*for (int i = 0; i < N; i++) {
			float4 rando = rand(&forRand) * ((float4)M_PI * 2);
			float2 theta = (rando.x, rando.y);
			float3 lightDir = v * sin(theta.x) + u * cos(theta.x) + norm * fabs(sin(theta.y));
			float3 indirectHitPos;
			float3 indirectNorm;
			bool lightHit = trace(hitPos + (norm * 0.000001f), lightDir, data, &indirectHitPos, 0, &indirectNorm);

			if (!lightHit) {
			  inDirectLighting += albedo * fabs(cos(theta.y));
			}
		}
		if (N > 0) {
			inDirectLighting /= (N);
			inDirectLighting.w = 1.0f;
		}*/
		//bool player_light = trace(hitPos + (norm * bias), eye- hitPos, data, 0, 0, 0);
		directLighting += 0.1f * albedo * distance(hitPos, eye) * (float4)(1.0, 1.0, 0.6, 1.0);
		
		inDirectLighting += albedo * 0.1f * intensity * dot(sunLoc, norm);
		write_imagef(image, pix, inDirectLighting + directLighting);
	}
	else {
		float4 skyboxPixel = querySkybox(
			skybox_top,
			skybox_bottom,
			skybox_left,
			skybox_right,
			skybox_front,
			skybox_back, dir);
		skyboxPixel *= (float4)(0.2, 0.2, 0.2, 1.0);
		write_imagef(image, pix, skyboxPixel);
		//write_imagef(image, pix, (float4)(1.0,0.0,1.0,1.0));
	}
}