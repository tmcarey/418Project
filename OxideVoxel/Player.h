#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Transform.h"
#include "Window.h"
#include "Math.h"

#define mouseSensitivity 0.05

class Player {
public:
	Player(Window* window);
	Transform* GetTransform();
	void update(double deltaTime);
	float speed;
private:
	Transform m_transform;
	Window *m_window;
	double horizontalAngle;
	double verticalAngle;
	double lastMouseX;
	double lastMouseY;
};
