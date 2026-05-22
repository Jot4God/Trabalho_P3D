#pragma once

#include <vector>
#include <string>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Behaviour.h"
#include "Object.h"
#include "Game.h"
#include "WindowSystem.h"

using namespace game_engine_p3d;

class BallMovement : public Behaviour {
private:
	bool moving_ = false;
	bool space_was_pressed_ = false;

	// Movimento por frame.
	// Como o teu Update() não recebe deltaTime, usamos velocidade fixa por frame.
	float speed_ = 0.03f;

	// Raio aproximado da bola no mundo.
	// Se a bola parar antes de tocar, baixa para 0.16f.
	// Se atravessar a bola, sobe para 0.20f.
	float radius_ = 0.18f;

	// Limites aproximados da zona jogável da mesa.
	// Ajusta se a bola parar antes/depois da borda.
	float min_x_ = -2.6f;
	float max_x_ = 2.6f;
	float min_z_ = -1.35f;
	float max_z_ = 1.35f;

	// Direção do movimento da bola.
	// Ball1 está à esquerda do triângulo, por isso vai andar para +X.
	glm::vec3 direction_ = glm::normalize(glm::vec3(-1.0f, 0.0f, 1.0f));

	std::vector<std::string> other_balls_ = {
		"Ball2", "Ball3", "Ball4", "Ball5", "Ball6",
		"Ball7", "Ball8", "Ball9", "Ball10",
		"Ball11", "Ball12", "Ball13", "Ball14", "Ball15"
	};

public:
	void Update(Object& object) override {
		bool space_pressed = WindowSystem::GetKey(GLFW_KEY_SPACE);

		// Começa a animação apenas no momento em que se carrega no Espaço.
		if (space_pressed && !space_was_pressed_) {
			moving_ = true;
		}

		space_was_pressed_ = space_pressed;

		if (!moving_) {
			return;
		}

		glm::vec3 current_position = object.model().position_;
		glm::vec3 movement = direction_ * speed_;
		glm::vec3 next_position = current_position + movement;

		if (HitsTableLimit(next_position) || HitsOtherBall(object, next_position)) {
			moving_ = false;
			return;
		}

		ApplyTranslationAndRotation(object, next_position, movement);
	}

private:
	bool HitsTableLimit(const glm::vec3& position) const {
		return (
			position.x - radius_ <= min_x_ ||
			position.x + radius_ >= max_x_ ||
			position.z - radius_ <= min_z_ ||
			position.z + radius_ >= max_z_
			);
	}

	bool HitsOtherBall(Object& object, const glm::vec3& next_position) const {
		Game* game = object.game();

		if (game == nullptr) {
			return false;
		}

		for (const std::string& ball_name : other_balls_) {
			Object* other = game->FindObjectByName(ball_name);

			if (other == nullptr) {
				continue;
			}

			glm::vec3 other_position = other->model().position_;

			float dx = next_position.x - other_position.x;
			float dz = next_position.z - other_position.z;

			float distance_squared = dx * dx + dz * dz;
			float minimum_distance = radius_ * 2.0f;

			if (distance_squared <= minimum_distance * minimum_distance) {
				return true;
			}
		}

		return false;
	}

	void ApplyTranslationAndRotation(Object& object, const glm::vec3& next_position, const glm::vec3& movement) {
		Transform& transform = object.model();

		// Atualiza posição lógica.
		transform.position_ = next_position;

		// Distância percorrida neste frame.
		float distance = std::sqrt(
			movement.x * movement.x +
			movement.z * movement.z
		);

		// Rolamento da bola: ângulo = distância / raio.
		float angle_degrees = glm::degrees(distance / radius_);

		// Eixo de rotação aproximado para uma bola a rolar no plano XZ.
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 rotation_axis = glm::normalize(glm::cross(up, direction_));

		transform.orientation_.x += rotation_axis.x * angle_degrees;
		transform.orientation_.z += rotation_axis.z * angle_degrees;

		// Reconstrução da matriz Model.
		// Isto evita problemas por já teres aplicado Scale(0.1f, 0.1f, 0.1f) antes.
		transform.matrix_ = glm::mat4(1.0f);

		transform.matrix_ = glm::translate(transform.matrix_, transform.position_);

		transform.matrix_ = glm::rotate(
			transform.matrix_,
			glm::radians(transform.orientation_.x),
			glm::vec3(1.0f, 0.0f, 0.0f)
		);

		transform.matrix_ = glm::rotate(
			transform.matrix_,
			glm::radians(transform.orientation_.y),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		transform.matrix_ = glm::rotate(
			transform.matrix_,
			glm::radians(transform.orientation_.z),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		transform.matrix_ = glm::scale(transform.matrix_, transform.scale_);
	}
};
