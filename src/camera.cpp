#include "camera.h"

Camera::Camera() :
	position_(glm::vec3(0.0f, 0.0f, 0.0f)),
	worldUp_(glm::vec3(0.0f, 1.0f, 0.0f)),
	yaw_(k_Yaw),
	pitch_(k_Pitch),
	fov_(k_FOV)
{
	updateCameraVectors();
}

Camera::~Camera() {};

Camera::Camera(const glm::vec3 position,
	const glm::vec3 up,
	const float yaw, const float pitch) :
	position_(up),
	worldUp_(up),
	yaw_(yaw),
	pitch_(pitch),
	fov_(k_FOV)
{
	updateCameraVectors();
}

Camera::Camera(const float posX, const float posY, const float posZ,
	const float upX, const float upY, const float upZ,
	const float yaw, const float pitch) :
	position_(glm::vec3(posX, posY, posZ)),
	worldUp_(glm::vec3(upX, upY, upZ)),
	yaw_(yaw),
	pitch_(pitch),
	fov_(k_FOV)
{
	updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
	return lookAt();
}

glm::mat4 Camera::lookAt() const
{
	return glm::lookAt(position_, position_ + front_, up_);
}

float Camera::getFOV() const
{
	return fov_;
}

glm::vec3 Camera::getPosition() const
{
	return position_;
}

void Camera::updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(pitch_)) * cos(glm::radians(yaw_));
	front.y = sin(glm::radians(pitch_));
	front.z = cos(glm::radians(pitch_)) * sin(glm::radians(yaw_));

	front_ = glm::normalize(front);

	right_ = glm::normalize(glm::cross(front_, worldUp_));
	up_ = glm::normalize(glm::cross(right_, front_));
}

void Camera::handleKeyboard(const Movement direction, const float deltaTime)
{
	float velocity = k_Speed * deltaTime;
	float prev_y = position_.y;

	switch (direction)
	{
		case Movement::Forward: position_ += front_ * velocity; break;
		case Movement::Backward: position_ -= front_ * velocity; break;
		case Movement::Left: position_ -= right_ * velocity; break;
		case Movement::Right: position_ += right_ * velocity; break;
	}
	if (!flying_) position_.y = prev_y;
}

void Camera::handleMouseMovement(const float xoffset, const float yoffset, const bool constrainPitch)
{
	float xoff = xoffset * k_Sensitivity;
	float yoff = yoffset * k_Sensitivity;

	yaw_ += xoff;
	pitch_ += yoff;

	if (pitch_ > 89.0f) // constraints for the pitch
		pitch_ = 89.0f;
	if (pitch_ < -89.0f)
		pitch_ = -89.0f;

	updateCameraVectors();
}
void Camera::handleMouseScroll(const float yoffset)
{
	if (fov_ >= 1.0f && fov_ <= 45.0f) fov_ -= yoffset;
	if (fov_ < -1.0f) fov_ = 1.0f;
	if (fov_ > 45.0f) fov_ = 45.0f;
}





