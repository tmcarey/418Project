#include "transform.h"
#include <gtc/matrix_transform.hpp>
glm::mat4 Transform::Get() {
	if (isStatic) {
		return trans;
	}
	glm::mat4 m_transform = glm::mat4(1.0);

	m_transform = glm::translate(m_transform, GetPosition());
	m_transform *= glm::mat4_cast(GetRotation());
	m_transform = glm::scale(m_transform, GetScale());
	return m_transform;
}
void Transform::MakeStatic() {
	trans = Get();
	isStatic = true;
}

Transform::Transform() : isStatic(false), trans() {
	position = glm::vec3(0);
	scale = glm::vec3(1);
	rotation = glm::quat(0, 0, 0, 1);
	m_parent = nullptr;
}


void Transform::setParent(Transform* parent) {
	m_parent = parent;
}

void Transform::Translate(glm::vec3 pos) {
	position += pos;
}

void Transform::Rotate(float angle, glm::vec3 vec) {
	/*glm::vec3 pos = glm::vec3(m_transform[3]);
	m_transform = glm::translate(m_transform, -pos);
	m_transform = m_transform * glm::rotate(glm::mat4(1.0f), angle, vec);
	m_transform = glm::translate(m_transform, pos);*/
	rotation = glm::rotate(rotation, angle, vec);
}

glm::vec3 Transform::GetPosition() {
	return m_parent ? (m_parent->GetRotation() * position) + m_parent->GetPosition() : position;
}

glm::quat Transform::GetRotation() {
	return m_parent ? m_parent->GetRotation() * rotation : rotation;
}

glm::vec3 Transform::GetScale() {
	return m_parent ? scale + m_parent->GetScale() : scale;
}



void Transform::SetPosition(glm::vec3 pos) {
	position = pos;
}

void Transform::SetRotation(glm::quat rot) {
	rotation = glm::normalize(rot);
}

void Transform::SetEulerAngles(glm::vec3 ang) {
	rotation = (glm::quat(ang));
}

void Transform::SetScale(glm::vec3 iscale) {
	scale = iscale;
}

glm::vec3 Transform::Forward() {
	return rotation * glm::vec3(0, 0, -1);
}

glm::vec3 Transform::Up() {
	return rotation * glm::vec3(0, 1, 0);
}

glm::vec3 Transform::Right() {
	return rotation * glm::vec3(-1, 0, 0);
}
