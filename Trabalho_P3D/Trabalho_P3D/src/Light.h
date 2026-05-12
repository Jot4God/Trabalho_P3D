#pragma once

// ==============================================================
// Ficheiro:    Light.h
// Descrição:   Declaração da classe Light
// Autor:       Duarte Duque
// Data:        21/07/2025
// Versão:      1.0.0
// Dependências: glm (OpenGL Mathematics)
// Observações:
// Esta classe representa uma fonte de luz no motor de jogo.
// ==============================================================
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>


namespace game_engine_p3d {

	// Tipos de luzes suportadas
	enum class LightType {
		kAmbient,		// Luz ambiente (luz global)
		kDirectional,	// Luz direcional (ex: sol)
		kPoint,			// Luz pontual (ex: lâmpada)
		kSpotlight		// Luz spot (ex: farol)
	};

	// Classe que representa uma fonte de luz
	class Light {
	public:
		// ------------------------------------------------------------
		// Construtores e destrutores
		// ------------------------------------------------------------
		Light() = default; // Construtor padrão (indico ao compilador para gerar o construtor padrão)
		Light(const glm::vec3& ambient); // Construtor para luz ambiente
		Light(const glm::vec3& direction,
			const glm::vec3& ambient,
			const glm::vec3& diffuse,
			const glm::vec3& specular
		); // Construtor para luz direcional
		Light(const glm::vec3& position,
			const glm::vec3& ambient,
			const glm::vec3& diffuse,
			const glm::vec3& specular,
			float constant,
			float linear,
			float quadratic
		); // Construtor para luz pontual
		Light(const glm::vec3& position,
			const glm::vec3& direction,
			const glm::vec3& ambient,
			const glm::vec3& diffuse,
			const glm::vec3& specular,
			float constant,
			float linear,
			float quadratic,
			float cutOff,
			float outerCutOff
		); // Construtor para luz cónica (spotlight)
		~Light() = default; // Destrutor padrão (indico ao compilador para gerar o destrutor padrão)
		// ------------------------------------------------------------
		// Accessors e mutators
		// ------------------------------------------------------------
		LightType type() const { return type_; }
		std::string type_string() const; // Retorna o tipo da luz como string (ex: "Ambient", "Directional", etc.)
		void set_type(LightType type) { type_ = type; }
		glm::vec3 position() const { return position_; }
		void set_position(const glm::vec3& position) { position_ = position; }
		glm::vec3 direction() const { return direction_; }
		void set_direction(const glm::vec3& direction) { direction_ = direction; }
		glm::vec3 ambient() const { return ambient_; }
		void set_ambient(const glm::vec3& ambient) { ambient_ = ambient; }
		glm::vec3 diffuse() const { return diffuse_; }
		void set_diffuse(const glm::vec3& diffuse) { diffuse_ = diffuse; }
		glm::vec3 specular() const { return specular_; }
		void set_specular(const glm::vec3& specular) { specular_ = specular; }
		float constant() const { return constant_; }
		void set_constant(float constant) { constant_ = constant; }
		float linear() const { return linear_; }
		void set_linear(float linear) { linear_ = linear; }
		float quadratic() const { return quadratic_; }
		void set_quadratic(float quadratic) { quadratic_ = quadratic; }
		float cutOff() const { return cutOff_; }
		void set_cutOff(float cutOff) { cutOff_ = cutOff; }
		float outerCutOff() const { return outerCutOff_; }
		void set_outerCutOff(float outerCutOff) { outerCutOff_ = outerCutOff; }
		std::vector<Layer>& light_layers() { return light_layers_; } // Retorna as layers que a luz afeta
		bool IsInLayer(Layer layer) const; // Verifica se a luz afeta uma layer específica
		// ------------------------------------------------------------
		// Outras funções-membro
		// ------------------------------------------------------------
		void AddLayerToLightLayers(Layer layer); // Adiciona uma layer às layers que a luz afeta

	private:
		// ------------------------------------------------------------
		// Dados-membro privados
		// ------------------------------------------------------------
		LightType type_ = LightType::kPoint; // Tipo de luz (padrão: luz pontual)
		glm::vec3 position_ = glm::vec3(0.0f); // Posição da luz (relevante para luzes pontuais e spot)
		glm::vec3 direction_ = glm::vec3(0.0f, -1.0f, 0.0f); // Direção da luz (relevante para luzes direcionais e spot)
		glm::vec3 ambient_ = glm::vec3(0.2f); // Componente ambiente da luz
		glm::vec3 diffuse_ = glm::vec3(0.5f); // Componente difusa da luz
		glm::vec3 specular_ = glm::vec3(1.0f); // Componente especular da luz
		float constant_ = 1.0f; // Fator de atenuação constante (relevante para luzes pontuais e spot)
		float linear_ = 0.09f; // Fator de atenuação linear (relevante para luzes pontuais e spot)
		float quadratic_ = 0.032f; // Fator de atenuação quadrática (relevante para luzes pontuais e spot)
		float cutOff_ = 20.0f; // Ângulo de corte interno para luz spot (em graus, relevante para luzes spot)
		float outerCutOff_ = 60.0f; // Ângulo de corte externo para luz spot (em graus, relevante para luzes spot)
		// Layers que a luz afeta (mesmos nomes que os utilizados nas 'culling masks' das câmaras)
		std::vector<Layer> light_layers_{ "Default" }; // Por defeito afeta apenas a layer "Default"
	};
}