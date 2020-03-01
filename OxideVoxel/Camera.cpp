#include "camera.h"


Transform* Camera::Transform()
{
	return &m_transform;
}

/*glm::mat4 Camera::getProjection(Window* window)
{
	return glm::perspective(glm::radians(60.0f), (float)window->getWidth() / (float)window->getHeight(), 0.1f, 100.0f);
}*/

void Camera::UpdateAspect(int width, int height) {
	m_Projection = glm::perspective(glm::radians(60.0f), (float)(width) / (float)(height), 0.1f, 100.0f);
}

Camera::Camera(Window* window) : m_transform(), m_Window(window), m_Projection(glm::perspective(glm::radians(60.0f), ((float)(window)->getWidth()) / (float)window->getHeight(), 0.1f, 100.0f)) {}

glm::mat4 Camera::View() {
	return glm::inverse(m_transform.Get());
}

glm::mat4 Camera::Projection() {
	return m_Projection;
}

void Camera::update(float deltaTime)
{
}




