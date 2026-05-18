#pragma once


// =============================================================
// Ficheiro:    Transform.h
// Descrição:   Classe para representar transformações 3D
// Autor:       Duarte Duque
// Data:        21/07/2025
// Versão:      1.0.0
// Dependências: GLM (OpenGL Mathematics)
// Compilação:	g++ Transform.h -o Transform.o -lglew32s -lglfw3 -lopengl32
// Observações:
// Esta classe encapsula a posição, orientação e escala de um objeto 3D.
// =============================================================


// GLM - OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>


namespace game_engine_p3d {

	class Transform {
	public:
		// ------------------------------------------------------------
		// Construtores e destrutores
		// ------------------------------------------------------------
		Transform() = default; // Construtor padrão (indico ao compilador para gerar o construtor padrão)
		Transform(float x, float y, float z); // Construtor com parâmetros
		Transform(float x, float y, float z, float pitch, float yaw, float roll,
			float scale_x = 1.0f, float scale_y = 1.0f, float scale_z = 1.0f); // Construtor com parâmetros
		~Transform() = default; // Destrutor padrão (indico ao compilador para gerar o destrutor padrão)
		// ------------------------------------------------------------
		// Outras funções-membro
		// ------------------------------------------------------------
		void Translate(float delta_x, float delta_y, float delta_z);
		void Rotate(float delta_pitch, float delta_yaw, float delta_roll);
		void Scale(float scale_x, float scale_y, float scale_z);

		// ------------------------------------------------------------
		// Dados-membro públicos
		// ------------------------------------------------------------
		glm::vec3 position_ = glm::vec3(0.0f);		// x, y, z
		glm::vec3 orientation_ = glm::vec3(0.0f);	// pitch, yaw, roll. Se se imaginar um avião orientado no sentido do eixo do Z negativo, o pitch é o ângulo de subida/descida (rodar em torno de X), yaw é a rotação em torno do eixo vertical (Y) e roll é a rotação em torno do eixo longitudinal (Z).
		// FIXME: Implementar a orientação com recurso a um quaternion para evitar problemas de gimbal lock e facilitar rotações compostas.
		// glm::quat orientation_quat_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Quaternion de orientação, i.e. rotação,  inicializado como identidade.
		glm::vec3 scale_ = glm::vec3(1.0f);			// scale_x, scale_y, scale_z
		glm::mat4 matrix_ = glm::mat4(1.0f);		// Matriz de transformação (inicializada como matriz identidade).
	};
}