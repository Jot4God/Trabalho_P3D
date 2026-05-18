#pragma once


// ==============================================================
// Ficheiro:    Material.h
// Descrição:   Declaração da classe Material
// Autor:       Duarte Duque
// Data:        21/07/2025
// Versão:      1.0.0
// Dependências: Shader.h, Texture.h, glm (OpenGL Mathematics)
// Observações:
// Esta classe representa um material que pode ser aplicado a objetos 3D.
// Um material é composto por um shader, uma textura e várias propriedades físicas (como cor ambiente, difusa, especular, etc.).
// ==============================================================


#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "Texture.h"


namespace game_engine_p3d {

	// Declarações antecipadas
	class Shader;


	class Material {
	public:
		// ------------------------------------------------------------
		// Construtores e destrutores
		// ------------------------------------------------------------
		Material(const Shader* shader, const std::string obj_filename);
		Material(
			glm::vec3 ka = glm::vec3{ 0.2f, 0.2f, 0.2f },
			glm::vec3 kd = glm::vec3{ 0.8f, 0.8f, 0.8f },
			glm::vec3 ks = glm::vec3{ 1.0f, 1.0f, 1.0f },
			glm::vec3 ke = glm::vec3{ 0.0f, 0.0f, 0.0f },
			float Ns = 32.0f,
			float Ni = 1.0f,
			float d = 1.0f,
			int illum = 2,
			Shader* shader = nullptr, // Sem Shader por defeito
			std::vector<Texture> textures_ = {} // Texturas vazias por defeito
		);
		~Material() = default;
		// ------------------------------------------------------------
		// Accessors e mutators
		// ------------------------------------------------------------
		Shader* shader() const { return shader_; }
		// ------------------------------------------------------------
		// Outras funções-membro
		// ------------------------------------------------------------
		void Use() const; // Ativa o shader e a textura do material

	private:
		// ------------------------------------------------------------
		// Funções-membro privadas
		// ------------------------------------------------------------
		bool findMaterialFile(const std::string& obj_filename); // Encontra o nome do ficheiro MTL no ficheiro OBJ. Guarda-o em 'mtl_filename_'
		void LoadMaterialFromFile(void); // Carrega as propriedades do material a partir de um ficheiro MTL (que está definido em 'mtl_filename_')

		// ------------------------------------------------------------
		// Dados-membro privados
		// ------------------------------------------------------------
		// Atributos do material
		std::string mtl_filename_{};		// Nome do ficheiro MTL associado ao material
		Shader* shader_ = nullptr;			// Shader associado ao material
		std::vector<Texture> texture_{};	// Texturas associadas ao material, pela seguinte ordem (se existirem)
		// Albedo (também conhecida como Diffuse texture map = map_Kd), Normal map (map_bump ou map_Kn), Specular map (map_Ks), Emissive map (map_Ke), etc.
		glm::vec3 ka_{};	// Coeficiente de reflexão ambiente. Cor ambiente (Ambient color) do material. Mesmo que luz ambiente refletida.
		glm::vec3 kd_{};	// Coeficiente de reflexão difusa. Cor difusa (Diffuse color) do material. Define a cor principal do material sob luz direta.
		glm::vec3 ks_{};	// Coeficiente de reflexão especular. Cor especular (Specular color) do material. Define a cor do brilho especular.
		glm::vec3 ke_{};	// Cor emissiva (Emissive color). Define a cor que o material emite independentemente da luz ambiente.
		float Ns_{};		// Exponente de reflexão especular (Shininess). Controla a intensidade do brilho especular. Valores mais altos resultam em reflexos mais concentrados.
		float Ni_{};		// Índice de refração (Index of Refraction). Usado para simular materiais transparentes. Valores típicos: 1.0 (ar), 1.5 (vidro).
		float d_{};			// Opacidade do material (Dissolve). Valores entre 0.0 (totalmente transparente) e 1.0 (totalmente opaco).
		int illum_{};		// Modelo de iluminação (Illumination model). Define o modelo de iluminação usado pelo material. Valores comuns: 0 (apenas ambiente), 1 (ambiente + difusa), 2 (ambiente + difusa + especular)
	};
}