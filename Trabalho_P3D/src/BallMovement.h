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
	// Estado do movimento da bola.
	bool moving_ = false;
	bool space_was_pressed_ = false;

	// Velocidade da bola por frame.
	float speed_ = 0.03f;

	// Raio aproximado da bola, usado nas colisões.
	float radius_ = 0.18f;

	// Limites da zona jogável da mesa.
	float min_x_ = -2.6f;
	float max_x_ = 2.6f;
	float min_z_ = -1.35f;
	float max_z_ = 1.35f;

	// Direção do movimento no plano XZ.
	glm::vec3 direction_ = glm::normalize(glm::vec3(-1.0f, 0.0f, 1.0f));

	// Bolas com as quais a bola em movimento pode colidir.
	std::vector<std::string> other_balls_ = {
		"Ball2", "Ball3", "Ball4", "Ball5", "Ball6",
		"Ball7", "Ball8", "Ball9", "Ball10",
		"Ball11", "Ball12", "Ball13", "Ball14", "Ball15"
	};

public:
	void Update(Object& object) override {
		// Lê o estado atual da tecla Espaço.
		bool space_pressed = WindowSystem::GetKey(GLFW_KEY_SPACE);

		// Inicia o movimento apenas no momento em que a tecla é premida.
		// Isto evita que o movimento seja reiniciado várias vezes enquanto
		if (space_pressed && !space_was_pressed_) {
			moving_ = true;
		}

		// Guarda o estado da tecla para comparar no próximo frame.
		space_was_pressed_ = space_pressed;

		// Se a bola ainda não estiver em movimento, não faz nada.
		if (!moving_) {
			return;
		}

		// Obtém a posição atual da bola.
		glm::vec3 current_position = object.model().position_;

		// Calcula o deslocamento deste frame.
		glm::vec3 movement = direction_ * speed_;

		// Calcula a próxima posição da bola antes de a aplicar.
		glm::vec3 next_position = current_position + movement;

		// Antes de mover a bola, verifica se ela iria colidir com a mesa
		if (HitsTableLimit(next_position) || HitsOtherBall(object, next_position)) {
			moving_ = false;
			return;
		}

		// Se não houver colisão, aplica a nova posição e a rotação da bola.
		ApplyTranslationAndRotation(object, next_position, movement);
	}

private:
	bool HitsTableLimit(const glm::vec3& position) const {
		// Verifica se a bola ultrapassa os limites da mesa.
		return (
			position.x - radius_ <= min_x_ ||
			position.x + radius_ >= max_x_ ||
			position.z - radius_ <= min_z_ ||
			position.z + radius_ >= max_z_
			);
	}

	bool HitsOtherBall(Object& object, const glm::vec3& next_position) const {
		// Obtém o Game associado ao objeto.
		Game* game = object.game();

		if (game == nullptr) {
			return false;
		}

		// Percorre todas as bolas paradas registadas na lista.
		for (const std::string& ball_name : other_balls_) {
			Object* other = game->FindObjectByName(ball_name);

			// Se a bola não existir, ignora e passa à próxima.
			if (other == nullptr) {
				continue;
			}

			// Posição da outra bola.
			glm::vec3 other_position = other->model().position_;

			// Calcula a diferença entre os centros das bolas no plano XZ.
			float dx = next_position.x - other_position.x;
			float dz = next_position.z - other_position.z;

			// Calcula a distância ao quadrado entre os centros.
			float distance_squared = dx * dx + dz * dz;

			// A distância mínima entre duas bolas é a soma dos seus raios.
			float minimum_distance = radius_ * 2.0f;

			// Se a distância entre centros for menor ou igual à soma dos raios,
			if (distance_squared <= minimum_distance * minimum_distance) {
				return true;
			}
		}

		return false;
	}

	void ApplyTranslationAndRotation(Object& object, const glm::vec3& next_position, const glm::vec3& movement) {
		// Obtém o Transform da bola.
		Transform& transform = object.model();

		// Atualiza a posição da bola.
		transform.position_ = next_position;

		// Calcula a distância percorrida neste frame no plano da mesa.
		float distance = std::sqrt(
			movement.x * movement.x +
			movement.z * movement.z
		);

		// Calcula a rotação da bola.
		float angle_degrees = glm::degrees(distance / radius_);

		// Calcula um eixo de rotação perpendicular à direção do movimento.
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 rotation_axis = glm::normalize(glm::cross(up, direction_));

		// Atualiza a orientação da bola.
		transform.orientation_.x += rotation_axis.x * angle_degrees;
		transform.orientation_.z += rotation_axis.z * angle_degrees;

		// Reconstrói a matriz Model.
		transform.matrix_ = glm::mat4(1.0f);

		// 1. Aplica a posição.
		transform.matrix_ = glm::translate(transform.matrix_, transform.position_);

		// 2. Aplica a rotação no eixo X.
		transform.matrix_ = glm::rotate(
			transform.matrix_,
			glm::radians(transform.orientation_.x),
			glm::vec3(1.0f, 0.0f, 0.0f)
		);

		// 3. Aplica a rotação no eixo Y.
		transform.matrix_ = glm::rotate(
			transform.matrix_,
			glm::radians(transform.orientation_.y),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		// 4. Aplica a rotação no eixo Z.
		transform.matrix_ = glm::rotate(
			transform.matrix_,
			glm::radians(transform.orientation_.z),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		// 5. Aplica a escala final da bola.
		// Isto mantém a escala definida no main.cpp.
		transform.matrix_ = glm::scale(transform.matrix_, transform.scale_);
	}
};