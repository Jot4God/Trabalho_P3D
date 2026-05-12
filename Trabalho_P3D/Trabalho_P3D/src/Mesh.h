#pragma once


// ==============================================================
// Ficheiro:    Mesh.h
// Descrição:   Declaração da classe Mesh
// Autor:       Duarte Duque
// Data:        21/07/2025
// Versão:      1.0.0
// Dependências: GLEW, GLFW, glm (OpenGL Mathematics)
// Observações:
// Esta classe representa uma malha 3D, contendo vértices, normais e coordenadas de textura.
// Irá carregar os atributos de um ficheiro OBJ para os respetivos buffers OpenGL (VBOs) e configurar um VAO para renderização.
// Nota que a configuração do VAO associa os atributos (VBOs) da malha às 'locations' dos atributos no shader.
// ==============================================================


#include <vector>
#include <glm/glm.hpp>


namespace game_engine_p3d {

	// Declarações antecipadas
	class Shader;

	class Mesh {
	public:
		// ------------------------------------------------------------
		// Construtores e destrutores
		// ------------------------------------------------------------
		Mesh(const Shader* shader, const std::string& obj_filename);
		~Mesh();
		// ------------------------------------------------------------
		// Accessors e mutators
		// ------------------------------------------------------------
		int vertex_count() { return static_cast<int>(v_.size()); } // Retorna o número de vértices na malha
		// ------------------------------------------------------------
		// Outras funções-membro
		// ------------------------------------------------------------
		void Use(void) const; // Ativa o VAO para renderização

	private:
		// ------------------------------------------------------------
		// Funções-membro privadas
		// ------------------------------------------------------------
		void ObjLoader(const std::string& obj_filename);		// Carrega a malha a partir de um ficheiro OBJ
		bool isObjFile(const std::string file);					// Verifica se o ficheiro é um ficheiro OBJ
		const char* loadDataFromObjFile(const std::string file);// Carrega os dados do ficheiro OBJ
		std::vector<std::string> extractObjAttribute(const char* data, std::string AttributeDelimiter); // Extrai os atributos do ficheiro OBJ
		void FindMaxMinFromVec(std::vector<glm::vec3> vec, glm::vec3& max, glm::vec3& min); // Encontra os valores máximo e mínimo de um vetor de glm::vec3

		// ------------------------------------------------------------
		// Dados-membro privados
		// ------------------------------------------------------------
		GLuint vao_ = 0;			// Vertex Array Object
		GLuint vbo_vertices_ = 0;	// Vertex Buffer Object para vértices
		GLuint vbo_normals_ = 0;	// Vertex Buffer Object para normais
		GLuint vbo_texcoords_ = 0;	// Vertex Buffer Object para coordenadas de textura
		size_t vertex_count_ = 0;	// Número de vértices na malha
		std::vector<glm::vec3> v_{};	// Armazena a malha (vértices) do objeto
		std::vector<glm::vec2> vt_{};	// Armazena as coordenadas de textura da malha
		std::vector<glm::vec3> vn_{};	// Armazena as normais da malha
		float min_x_ = 0.0f, max_x_ = 0.0f; // Extensão da malha no eixo X
		float min_y_ = 0.0f, max_y_ = 0.0f; // Extensão da malha no eixo Y
		float min_z_ = 0.0f, max_z_ = 0.0f; // Extensão da malha no eixo Z
	};
}