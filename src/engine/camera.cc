#include "frametime.h"
#include "camera.h"
#include "shader.h"
#include "type_conversion.h"
#include <cmath>

Camera::Camera(glm::vec3 position, glm::vec3 target_view, float fov, float yaw, float pitch, bool invert_y, ClippingPlane plane) 
: position_{position}, local_x_{}, local_y_{}, local_z_{}, view_{}, fov_{fov}, yaw_{yaw}, pitch_{pitch}, invert_y_{invert_y}, clipping_plane_{plane} { 
	init(target_view);
}

void Camera::init(glm::vec3 target_view) {
	local_z_ = glm::normalize(position_ - target_view);

	compute_local_xy();
	update_view();
}

void Camera::rotate(double delta_x, double delta_y) {
	yaw_   += delta_x;
	pitch_ += delta_y * (invert_y_ ? -1.f : 1.f);
	pitch_ = glm::clamp(pitch_, -89.f, 89.f);

	local_z_.x = std::cos(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
	local_z_.y = std::sin(glm::radians(pitch_));
	local_z_.z = std::sin(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
	local_z_ = glm::normalize(local_z_);

	compute_local_xy();
	update_view();
}

void Camera::set_state(Direction dir, KeyState state) {
    if(state == KeyState::Down)
        direction_ |= enum_value(dir);
    else
        direction_ &= ~enum_value(dir);
}

void Camera::set_state(Speed speed) {
    speed_ = speed;
}

void Camera::update() {
    if(direction_ == 0u ||
       direction_ == (MoveDir::Right | MoveDir::Left) ||
       direction_ == (MoveDir::Forward | MoveDir::Backward) ||
       direction_ == (MoveDir::Up | MoveDir::Down))
        return;

    glm::vec3 dir{0.f};

    if((direction_ & MoveDir::Right)    == MoveDir::Right)
        dir += local_x_;
    if((direction_ & MoveDir::Left)     == MoveDir::Left)
        dir -= local_x_;
    if((direction_ & MoveDir::Up)       == MoveDir::Up)
        dir += local_y_;
    if((direction_ & MoveDir::Down)     == MoveDir::Down)
        dir -= local_y_;
    if((direction_ & MoveDir::Backward) == MoveDir::Backward)
        dir += local_z_;
    if((direction_ & MoveDir::Forward)  == MoveDir::Forward)
        dir -= local_z_;

    dir = glm::normalize(dir);

    float speed = static_cast<float>(speed_) * frametime::delta();

    position_ = glm::translate(glm::mat4{1.f}, speed * dir) * glm::vec4{position_, 1.f};
    update_view();
}

float Camera::fov() const {
	return fov_;
}

glm::mat4 Camera::view() const {
	return view_;
}

ClippingPlane Camera::clipping_plane() const {
	return clipping_plane_;
}

glm::vec3 Camera::position() {
    return position_;
}

void Camera::set_position(glm::vec3 pos) {
    position_ = pos;
    update_view();
    Shader::template upload_to_all<true>(Shader::VIEW_UNIFORM_NAME, view());
}

void Camera::invert_pitch() {
    pitch_ *= -1.f;
	pitch_ = glm::clamp(pitch_, -89.f, 89.f);

	local_z_.x = std::cos(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
	local_z_.y = std::sin(glm::radians(pitch_));
	local_z_.z = std::sin(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
	local_z_ = glm::normalize(local_z_);

	compute_local_xy();
	update_view();
    Shader::template upload_to_all<true>(Shader::VIEW_UNIFORM_NAME, view());
}

void Camera::compute_local_xy(){
	local_x_ = glm::normalize(glm::cross(glm::vec3(0.f, 1.f, 0.f), local_z_));
	local_y_ = glm::cross(local_z_, local_x_);
}

void Camera::update_view() {
	view_ = glm::lookAt(position_, position_ - local_z_, glm::vec3{0.f, 1.f, 0.f});
}
