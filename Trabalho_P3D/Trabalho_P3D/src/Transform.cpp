#include "Common.h" // Inclui definições comuns e macros para o motor de jogo
#include "Transform.h"


namespace game_engine_p3d {

	Transform::Transform(float x, float y, float z)
		: position_(x, y, z), orientation_(0.0f), scale_(1.0f) {
		// Inicializa a matriz de modelo como identidade
		matrix_ = glm::mat4(1.0f);

		// Atualiza a matriz de modelo com a transformação inicial
		matrix_ = glm::translate(matrix_, position_);
		LOG("Transform created at position: (" << x << ", " << y << ", " << z << ")");
	}

	Transform::Transform(float x, float y, float z, float pitch, float yaw, float roll, float scale_x, float scale_y, float scale_z)
		: position_(x, y, z), orientation_(pitch, yaw, roll), scale_(scale_x, scale_y, scale_z) {
		// Inicializa a matriz de modelo como identidade
		matrix_ = glm::mat4(1.0f);

		// Atualiza a matriz de modelo com a transformação inicial
		matrix_ = glm::translate(matrix_, position_);
		matrix_ = glm::rotate(matrix_, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
		matrix_ = glm::rotate(matrix_, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		matrix_ = glm::rotate(matrix_, glm::radians(roll), glm::vec3(0.0f, 0.0f, 1.0f));
		matrix_ = glm::scale(matrix_, scale_);
		LOG("Transform created at position: (" << x << ", " << y << ", " << z << ") with orientation: ("
			<< pitch << ", " << yaw << ", " << roll << ") and scale: ("
			<< scale_x << ", " << scale_y << ", " << scale_z << ")");
	}

	void Transform::Translate(float delta_x, float delta_y, float delta_z) {
		position_ += glm::vec3(delta_x, delta_y, delta_z);
		matrix_ = glm::translate(matrix_, glm::vec3(delta_x, delta_y, delta_z));
		LOG("Translated by (" << delta_x << ", " << delta_y << ", " << delta_z << ")");
	}

	void Transform::Rotate(float delta_pitch, float delta_yaw, float delta_roll) {
		orientation_ += glm::vec3(delta_pitch, delta_yaw, delta_roll);
		matrix_ = glm::rotate(matrix_, glm::radians(delta_pitch), glm::vec3(1.0f, 0.0f, 0.0f));
		matrix_ = glm::rotate(matrix_, glm::radians(delta_yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		matrix_ = glm::rotate(matrix_, glm::radians(delta_roll), glm::vec3(0.0f, 0.0f, 1.0f));
		LOG("Rotated by (" << delta_pitch << ", " << delta_yaw << ", " << delta_roll << ")");
	}

	void Transform::Scale(float scale_x, float scale_y, float scale_z) {
		scale_ *= glm::vec3(scale_x, scale_y, scale_z);
		matrix_ = glm::scale(matrix_, glm::vec3(scale_x, scale_y, scale_z));
		LOG("Scaled by (" << scale_x << ", " << scale_y << ", " << scale_z << ")");
	}
}