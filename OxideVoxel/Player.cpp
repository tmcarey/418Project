#include "player.h"


Transform* Player::GetTransform() {
	return &m_transform;
}

Player::Player(Window* window) : m_transform(), m_window(window) {
	speed = 15.0f;
	horizontalAngle = 0;
	verticalAngle = 0;
	lastMouseX = 0;
	lastMouseY = 0;
	m_window->setInputMode(GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Player::update(double deltaTime) {
	double xpos, ypos;
	m_window->getCursorPos(&xpos, &ypos);

	horizontalAngle -= mouseSensitivity * deltaTime * ((double(m_window->getWidth()) / 2.0) - xpos);
	verticalAngle -= mouseSensitivity * deltaTime * ((double(m_window->getHeight()) / 2.0) - ypos);
	verticalAngle = Clamp(verticalAngle, glm::radians(-90.0f), glm::radians(90.0));
	glfwSetCursorPos(m_window->GetWindow(), m_window->getWidth() / 2, m_window->getHeight() / 2);
	if (horizontalAngle > glm::radians(360.0)) {
		horizontalAngle -= glm::radians(360.0);
	}

	glm::vec3 position(0);
	glm::vec3 globalUp = glm::vec3(0, 1, 0);
	glm::vec3 right = m_transform.Right();
	glm::vec3 direction = m_transform.Forward();

	if (m_window->getKey(GLFW_KEY_Q) == GLFW_PRESS) {
		position += globalUp * (float)deltaTime;
	}
	if (m_window->getKey(GLFW_KEY_E) == GLFW_PRESS) {
		position -= globalUp * (float)deltaTime;
	}
	// Move forward
	if (m_window->getKey(GLFW_KEY_W) == GLFW_PRESS) {
		position += direction * (float)deltaTime;
	}
	// Move backward
	if (m_window->getKey(GLFW_KEY_S) == GLFW_PRESS) {
		position -= direction * (float)deltaTime;
	}
	// Strafe right
	if (m_window->getKey(GLFW_KEY_D) == GLFW_PRESS) {
		position += right * (float)deltaTime;
	}
	// Strafe left
	if (m_window->getKey(GLFW_KEY_A) == GLFW_PRESS) {
		position -= right * (float)deltaTime;
	}

	if (m_window->getKey(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		position *= 2;
	}

	m_transform.Translate(position * speed);
	m_transform.SetEulerAngles(glm::vec3(verticalAngle, horizontalAngle, glm::radians(180.0f)));
}