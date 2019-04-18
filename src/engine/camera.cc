#include "frametime.h"
#include "camera.h"
#include "logger.h"
#include "shader.h"
#include "type_conversion.h"
#include <cmath>

Camera::Camera(glm::vec3 position, glm::vec3 target_view, float fov, float yaw, float pitch, bool invert_y, ClipSpace space) 
: position_{position}, local_x_{}, local_y_{}, local_z_{}, view_{}, fov_{fov}, yaw_{yaw}, pitch_{pitch}, invert_y_{invert_y}, clip_space_{space} { 
	init(target_view);
}

void Camera::init(glm::vec3 target_view) {
	local_z_ = glm::normalize(position_ - target_view);

	compute_local_xy();
	update_view();
}

void Camera::rotate(double delta_x, double delta_y) {
    LOG("Rotating, x_diff: ", delta_x, "y_diff: ", delta_y);
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

void Camera::set_state(RotationDirection dir, KeyState state) {
    if(state == KeyState::Down)
        rotation_ |= enum_value(dir);
    else
        rotation_ &= ~enum_value(dir);
}

void Camera::set_state(Speed speed) {
    speed_ = speed;
}

void Camera::update() {
    double x_rot{0.0}, y_rot{0.0};
    if((rotation_ & RotationMask::Right) == RotationMask::Right)
        x_rot += ROTATION_SPEED * frametime::delta();
    if((rotation_ & RotationMask::Left) == RotationMask::Left)
        x_rot -= ROTATION_SPEED * frametime::delta();
    if((rotation_ & RotationMask::Up) == RotationMask::Up)
        y_rot -= ROTATION_SPEED * frametime::delta();
    if((rotation_ & RotationMask::Down) == RotationMask::Down)
        y_rot += ROTATION_SPEED * frametime::delta();;
    
    rotate(x_rot, y_rot);
    
    if(direction_ == 0u ||
       direction_ == (MoveMask::Right | MoveMask::Left) ||
       direction_ == (MoveMask::Forward | MoveMask::Backward) ||
       direction_ == (MoveMask::Up | MoveMask::Down))
        return;

    glm::vec3 dir{0.f};

    if((direction_ & MoveMask::Right)    == MoveMask::Right)
        dir += local_x_;
    if((direction_ & MoveMask::Left)     == MoveMask::Left)
        dir -= local_x_;
    if((direction_ & MoveMask::Up)       == MoveMask::Up)
        dir += local_y_;
    if((direction_ & MoveMask::Down)     == MoveMask::Down)
        dir -= local_y_;
    if((direction_ & MoveMask::Backward) == MoveMask::Backward)
        dir += local_z_;
    if((direction_ & MoveMask::Forward)  == MoveMask::Forward)
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

ClipSpace Camera::clip_space() const {
	return clip_space_;
}

glm::vec3 Camera::position() {
    return position_;
}

void Camera::set_position(glm::vec3 pos) {
    LOG("Setting camera position {", pos.x, ", ", pos.y, ", ", pos.z, "}");
    position_ = pos;
    update_view();
    Shader::template upload_to_all<true>(Shader::VIEW_UNIFORM_NAME, view());
}

float Camera::pitch() const {
    return pitch_;
}

void Camera::set_pitch(float pitch) {
    LOG("Setting pitch ", pitch);
    pitch_ = pitch;
	pitch_ = glm::clamp(pitch_, -89.f, 89.f);

	local_z_.x = std::cos(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
	local_z_.y = std::sin(glm::radians(pitch_));
	local_z_.z = std::sin(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
	local_z_ = glm::normalize(local_z_);

	compute_local_xy();
	update_view();
    Shader::template upload_to_all<true>(Shader::VIEW_UNIFORM_NAME, view());
}

void Camera::invert_pitch() {
    LOG("Inverting pitch");
    set_pitch(-pitch_);
}

glm::vec3 Camera::view_direction() const {
    return -local_z_;
}

void Camera::compute_local_xy(){
	local_x_ = glm::normalize(glm::cross(glm::vec3(0.f, 1.f, 0.f), local_z_));
	local_y_ = glm::cross(local_z_, local_x_);
}

void Camera::update_view() {
	view_ = glm::lookAt(position_, position_ - local_z_, glm::vec3{0.f, 1.f, 0.f});
}
