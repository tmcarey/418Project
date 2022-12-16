#pragma once
#include <ext/matrix_clip_space.hpp>
#include "Transform.h"
#include "Window.h"
#include <ext\matrix_transform.hpp>

class Camera {
private:
	Transform m_transform;
	glm::mat4 m_Projection;
	Window* m_Window;
public:
	Camera(Window* window);
	void update(float deltaTime);
	void UpdateAspect(int width, int height);
	Transform* Transform();
	glm::mat4 Projection();
	glm::mat4 View();
};
