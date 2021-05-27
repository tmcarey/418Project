#include "Input.h"
#include <iostream>
unsigned char* Input::key = nullptr;
void Input::Init()
{
	key = new unsigned char[349];
	memset(key, 0, 349 * sizeof(char));
}

bool Input::GetKey(int keycode)
{
	return ((int)key[keycode]) > 0;
}

bool Input::GetKeyDown(int keycode){
	std::cout << (int)key[keycode] << std::endl;
	return ((key[keycode]) & KEY_NOW) > 0;
}

void Input::HandleInputEvent(GLFWwindow * window, int keycode, int scancode, int action, int mods)
{
	switch (action) {
	case GLFW_PRESS:
		key[keycode] = KEY_SEEN | KEY_RELEASED | KEY_NOW;
		break;
	case GLFW_RELEASE:
		key[keycode] &= KEY_RELEASED;
		break;
	}
}

void Input::flushInput()
{
	for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
		if((int)key[i] == 7){
			key[i] = KEY_NOW | KEY_SEEN;
		}else{
			key[i] &= KEY_SEEN;
		}
	}
}




