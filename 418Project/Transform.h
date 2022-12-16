#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <quaternion.hpp>
#include <glm.hpp>

class Transform {
public:
	void MakeStatic();
	Transform();
	void Translate(glm::vec3 position);
	void Rotate(float angle, glm::vec3 dir);
	glm::vec3 GetPosition();
	glm::quat GetRotation();
	glm::vec3 GetScale();

	glm::mat4 Get();
	void setParent(Transform* parent);
	glm::vec3 Forward();
	glm::vec3 Up();
	glm::vec3 Right();
	void SetPosition(glm::vec3 pos);
	void SetRotation(glm::quat rot);
	void SetScale(glm::vec3 scale);
	void SetEulerAngles(glm::vec3 ang);
private:
	glm::mat4 trans;
	bool isStatic;
	Transform* m_parent;
	glm::vec3 position;
	glm::vec3 scale;
	glm::quat rotation;
};
