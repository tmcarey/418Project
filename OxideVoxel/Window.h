#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <list>
#include "Input.h"


class Window
{
private:
	int m_Width, m_Height;
	const char* m_Name;
	GLFWwindow* m_Window;
	static Window* m_Instance;
	//static void resizeCallback(GLFWwindow *window, int width, int height);
public:
	static void Close();
	static Window* Instance();
	GLFWwindow* GetWindow();
	bool closed() const;
	void update();
	//void AddCamera(Camera* cam);
	int getKey(int key);
	void setInputMode(int mode, int value);
	void getCursorPos(double* x, double* y);
	inline int getWidth() const { return m_Width; };
	inline int getHeight() const { return m_Height; };
	void Resize(int width, int height);
	bool init();
private:
	
};

	