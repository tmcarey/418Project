#pragma once
#include <GLFW/glfw3.h>
#define KEY_SEEN     1
#define KEY_RELEASED 2
#define KEY_NOW 4
class Input {
public:
	static void Init();
	static bool GetKeyDown(int inkey);
	static bool GetKey(int inkey);
	static void HandleInputEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void flushInput();
private:
	static unsigned char* key;
};
