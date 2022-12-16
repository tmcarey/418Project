#include "window.h"

Window* Window::m_Instance = nullptr;

void windowResize(GLFWwindow* window, int width, int height);

GLFWwindow *Window::GetWindow(){
	return m_Window;
}

Window* Window::Instance() {
	if (!m_Instance) {
		m_Instance = (Window*)malloc(sizeof(Window));
		m_Instance->m_Width = 1920;
		m_Instance->m_Height = 1080;
	}
	return m_Instance;
}

void Window::Close() {
}

void Window::update()
{
	glfwPollEvents();

	if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(m_Window, true);
	}

	glfwSwapBuffers(m_Window);
}

int Window::getKey(int key)
{
	return glfwGetKey(m_Window, key);
}

void Window::setInputMode(int mode, int value)
{
	glfwSetInputMode(m_Window, mode, value);
}

void Window::getCursorPos(double* x, double* y)
{
	glfwGetCursorPos(m_Window, x, y);
}



bool Window::closed() const {
	return glfwWindowShouldClose(m_Window);
}

bool Window::init() {
	

	m_Name = "418 Project";
	m_Window = 0;
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	Input::Init();
	m_Window = glfwCreateWindow(m_Width, m_Height, m_Name, NULL, NULL);

	glfwSetKeyCallback(m_Window, (GLFWkeyfun)Input::HandleInputEvent);

	glfwMakeContextCurrent(m_Window);

	glfwSetWindowSizeCallback(m_Window, windowResize);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return false;
	}

	return true;
}

//void Window::AddCamera(Camera* cam) {
	
//}

void Window::Resize(int width, int height) {
	m_Width = width;
	m_Height = height;
}

void windowResize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	//Window::Instance()->Resize(width, height);
}

