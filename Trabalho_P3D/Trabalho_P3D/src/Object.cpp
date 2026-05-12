#define GLEW_STATIC // Definição necessária, antes de incluir 'GL\glew.h', sempre que se usa GLEW como uma biblioteca estática
#include <GL/glew.h>

#include "Common.h" // Inclui definições comuns e macros para o motor de jogo
#include "Object.h"
#include "Renderer.h"
#include "Camera.h"
#include "Behaviour.h"


namespace game_engine_p3d {

	// Define o contador de objetos estático, atribuindo um valor inicial de 0.
	int Object::object_count_ = 0;


	// --------------------------------------------------------------------------
	// Definição de funções da classe Object
	// --------------------------------------------------------------------------
	Object::Object(std::string name, Layer layer, Behaviour* behaviour, Renderer* renderer, 
		float x, float y, float z, 
		float pitch, float yaw, float roll, 
		float scale_x, float scale_y, float scale_z) {

		// Verifica se o nome do objeto é vazio
		if (name.empty()) {
			LOG("Error: Object name cannot be empty.");
			exit(EXIT_FAILURE); // Encerra o programa com erro
		}
		// Atribui o nome ao objeto
		name_ = name;

		// Atribui a layer ao objeto
		// Se a layer fornecida for vazia, mantém a layer padrão ("Default")
		if (!layer.empty()) {
			layer_ = layer;
		}

		// Atribui o comportamento (se fornecido) ao objeto. Admite valor nulo.
		behaviour_ = behaviour;

		// Atribui o renderizador (se fornecido) ao objeto. Admite valor nulo.
		renderer_ = renderer;

		// Inicializa a matriz de transformação do objeto com a posição, orientação e escala fornecidas
		model_ = Transform(x, y, z, pitch, yaw, roll, scale_x, scale_y, scale_z);

		// Incrementa o contador de objetos
		object_count_++;

		// Atribui um ID único ao objeto
		id_ = object_count_;

		LOG("Object created with ID: " << id_ << " at position: (" << x << ", " << y << ", " << z << ")");
	}

	void Object::Start() {
		if (behaviour_) {
			behaviour_->Start(*this);
		}
	}

	void Object::Update() {
		if (behaviour_) {
			behaviour_->Update(*this);
		}
	}

	void Object::FixedUpdate() {
		if (behaviour_) {
			behaviour_->FixedUpdate(*this);
		}
	}

	void Object::PhysicsUpdate() {
		if (behaviour_) {
			behaviour_->PhysicsUpdate(*this);
		}
	}

	void Object::LateUpdate() {
		if (behaviour_) {
			behaviour_->LateUpdate(*this);
		}
	}

	void Object::OnCollisionEnter(Object& other) {
		if (behaviour_) {
			behaviour_->OnCollisionEnter(*this, other);
		}
	}

	void Object::OnCollisionExit(Object& other) {
		if (behaviour_) {
			behaviour_->OnCollisionExit(*this, other);
		}
	}

	void Object::OnTriggerEnter(Object& other) {
		if (behaviour_) {
			behaviour_->OnTriggerEnter(*this, other);
		}
	}

	void Object::OnTriggerExit(Object& other) {
		if (behaviour_) {
			behaviour_->OnTriggerExit(*this, other);
		}
	}

	void Object::OnDestroy() {
		if (behaviour_) {
			behaviour_ = nullptr; // Limpa o comportamento após destruir
		}
	}

	void Object::Draw(Camera& camera, std::vector<Light*>& lights) {
		// Se estiver associado um renderizador
		if (renderer_) {
			// Matriz Model do objeto
			glm::mat4 model_matrix = model_.matrix_;

			// Matriz View e Projection da câmara fornecida
			glm::mat4 view_matrix = camera.view().matrix_;
			glm::mat4 projection_matrix = camera.projection().matrix_;

			// Chama o método Draw do renderizador para desenhar o objeto
			renderer_->Draw(model_matrix, view_matrix, projection_matrix, layer_, lights);
		}
	}
}