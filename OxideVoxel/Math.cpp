#include "Math.h"

float Clamp(float val, float min, float max) {
	if (val < min) {
		return min;
	}
	if (val > max) {
		return max;
	}
	return val;
}