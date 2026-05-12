#pragma once


// =============================================================
// Ficheiro:    Shader.h
// Descrição:   Declaração da classe Shader
// Autor:       Duarte Duque
// Data:        21/07/2025
// Versão:      1.0.0
// Dependências: GLEW, OpenGL, GLM, Windows SDK
// Observações:
// Esta classe gere shaders OpenGL, permitindo carregar, compilar e usar shaders de vertex e fragment (entre outros).
// A classe utiliza a biblioteca GLEW para gerir extensões OpenGL.
// Esta implementação garante que os shaders são independentes da textura e podem ser usados com diferentes materiais.
// =============================================================

#define GLEW_STATIC // Definição necessária, antes de incluir 'GL\glew.h', sempre que se usa GLEW como uma biblioteca estática
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp> // Necessário para usar glm::value_ptr()
#include <type_traits> // Necessário para std::is_same
#include <string>
#include <vector>
#include <map>


namespace game_engine_p3d {

	// Estrutura que define a fonte de um shader
	// Contém o tipo do shader (vertex, fragment, etc.) e o caminho para o ficheiro do shader
	struct ShaderSource {
		unsigned int type;		// Tipo do shader (GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, etc.). Utilizamos o tipo 'unsigned int' em vez de 'GLenum' para maior compatibilidade.
		std::string filename;	// Caminho do ficheiro do shader.
		unsigned int shader;	// Identificador do shader OpenGL. Utilizamos 'unsigned int' em vez de 'GLuint' para maior compatibilidade.
	};

	class Shader {
	public:
		// ------------------------------------------------------------
		// Construtores e destrutores
		// ------------------------------------------------------------
		Shader(const std::vector<ShaderSource>& sources, const std::string name = {}); // Construtor que recebe um vetor de fontes de shader, lê e compila os shaders. Nome a atribuir ao programa shader é opcional.
		~Shader();
		// ------------------------------------------------------------
		// Accessors e mutators
		// ------------------------------------------------------------
		unsigned int shader_program() const { return shader_program_; } // Retorna o ID do programa de shader compilado
		std::string name() const { return name_; } // Retorna o nome do shader
		// ------------------------------------------------------------
		// Outras funções-membro
		// ------------------------------------------------------------
		void Use() const;	// Ativa o shader para uso
		// Função template genérica para configurar uniforms no shader
		template <typename T>
		void SetUniform(const std::string& name, const T& value) const {
			// Usa o programa shader
			glUseProgram(shader_program_);
			// Obtém o local do uniform no programa shader
			GLint location = glGetProgramResourceLocation(shader_program_, GL_UNIFORM, name.c_str()); // Para versões >= 4.3
			if (location == -1) {
				LOG("Error: Uniform '" << name << "' not found in shader program.");
				return; // Se o uniform não for encontrado, não faz nada
			}
			// Define o valor do uniform de acordo com o tipo T)
			if (std::is_same<T, float>::value) {
				glProgramUniform1f(shader_program_, location, *reinterpret_cast<const float*>(&value)); // O compilador precisa gerar código para todos os tipos possíveis de T, e não consegue garantir que T seja sempre conversível para float. Daí a necessidade do cast com 'reinterpret_cast'.	
			}
			else if (std::is_same<T, int>::value) {
				glProgramUniform1i(shader_program_, location, *reinterpret_cast<const int*>(&value));
			}
			else if (std::is_same<T, glm::vec2>::value) {
				glProgramUniform2fv(shader_program_, location, 1, glm::value_ptr(*reinterpret_cast<const glm::vec2*>(&value)));
			}
			else if (std::is_same<T, glm::vec3>::value) {
				glProgramUniform3fv(shader_program_, location, 1, glm::value_ptr(*reinterpret_cast<const glm::vec3*>(&value)));
			}
			else if (std::is_same<T, glm::vec4>::value) {
				glProgramUniform4fv(shader_program_, location, 1, glm::value_ptr(*reinterpret_cast<const glm::vec4*>(&value)));
			}
			else if (std::is_same<T, glm::mat3>::value) {
				glProgramUniformMatrix3fv(shader_program_, location, 1, GL_FALSE, glm::value_ptr(*reinterpret_cast<const glm::mat3*>(&value)));
			}
			else if (std::is_same<T, glm::mat4>::value) {
				glProgramUniformMatrix4fv(shader_program_, location, 1, GL_FALSE, glm::value_ptr(*reinterpret_cast<const glm::mat4*>(&value)));
			}
			else {
				LOG("Error: Unsupported uniform type for '" << name << "'.");
				return; // Tipo não suportado
			}
		}

	private:
		// ------------------------------------------------------------
		// Funções-membro privadas
		// ------------------------------------------------------------
		unsigned int LoadShaders(std::vector<ShaderSource>& sources);
		const char* ReadShader(const char* filename);

		// ------------------------------------------------------------
		// Dados-membro privados
		// ------------------------------------------------------------
		std::vector<ShaderSource> sources_{};	// Armazena os shaders (vertex, fragment, etc.) a serem compilados
		unsigned int shader_program_ = 0;		// Programa final compilado do shader
		std::string name_;						// Nome do shader, opcional para identificação
	};
}

// ---------------
// Exemplo de uso:
// ---------------
// 
// std::vector<ShaderSource> sources = {
//	{GL_VERTEX_SHADER, "vertex.vert"},
//	{GL_FRAGMENT_SHADER, "fragment.frag"},
//	{GL_GEOMETRY_SHADER, "geometry.geom"} // Opcional
// };
// Shader shader(sources);
// shader.Use();