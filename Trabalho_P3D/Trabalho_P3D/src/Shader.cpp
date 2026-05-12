// É necessário incluir o header glew.h antes de incluir qualquer header OpenGL específico
// Neste caso, temos de incluir o header do GLEW antes de incluir o header OpenGL do Windows SDK (que é o 'GL.h' e foi incluído em 'Shaders.h').
#define GLEW_STATIC // Definição necessária, antes de incluir 'GL\glew.h', sempre que se usa GLEW como uma biblioteca estática
#include <GL\glew.h>
#include <fstream>
#include <algorithm> // std::any_of

#include "Common.h" // Para a macro LOG
#include "Shader.h"


namespace game_engine_p3d {

	static const char* ShaderTypeToString(unsigned int type) {
		switch (type) {
		case GL_VERTEX_SHADER: return "GL_VERTEX_SHADER";
		case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
		case GL_GEOMETRY_SHADER: return "GL_GEOMETRY_SHADER";
		case GL_COMPUTE_SHADER: return "GL_COMPUTE_SHADER";
		case GL_TESS_CONTROL_SHADER: return "GL_TESS_CONTROL_SHADER";
		case GL_TESS_EVALUATION_SHADER: return "GL_TESS_EVALUATION_SHADER";
		default: return "UNKNOWN_SHADER_TYPE";
		}
	}


	static const char* UniformTypeToString(GLenum type) {
		switch (type) {
			// Tipos básicos
		case GL_FLOAT: return "GL_FLOAT";
		case GL_DOUBLE: return "GL_DOUBLE";
		case GL_INT: return "GL_INT";
		case GL_UNSIGNED_INT: return "GL_UNSIGNED_INT";
		case GL_BOOL: return "GL_BOOL";

			// Vetores float
		case GL_FLOAT_VEC2: return "GL_FLOAT_VEC2";
		case GL_FLOAT_VEC3: return "GL_FLOAT_VEC3";
		case GL_FLOAT_VEC4: return "GL_FLOAT_VEC4";

			// Vetores int
		case GL_INT_VEC2: return "GL_INT_VEC2";
		case GL_INT_VEC3: return "GL_INT_VEC3";
		case GL_INT_VEC4: return "GL_INT_VEC4";

			// Vetores unsigned int
		case GL_UNSIGNED_INT_VEC2: return "GL_UNSIGNED_INT_VEC2";
		case GL_UNSIGNED_INT_VEC3: return "GL_UNSIGNED_INT_VEC3";
		case GL_UNSIGNED_INT_VEC4: return "GL_UNSIGNED_INT_VEC4";

			// Vetores bool
		case GL_BOOL_VEC2: return "GL_BOOL_VEC2";
		case GL_BOOL_VEC3: return "GL_BOOL_VEC3";
		case GL_BOOL_VEC4: return "GL_BOOL_VEC4";

			// Matrizes float
		case GL_FLOAT_MAT2: return "GL_FLOAT_MAT2";
		case GL_FLOAT_MAT3: return "GL_FLOAT_MAT3";
		case GL_FLOAT_MAT4: return "GL_FLOAT_MAT4";
		case GL_FLOAT_MAT2x3: return "GL_FLOAT_MAT2x3";
		case GL_FLOAT_MAT2x4: return "GL_FLOAT_MAT2x4";
		case GL_FLOAT_MAT3x2: return "GL_FLOAT_MAT3x2";
		case GL_FLOAT_MAT3x4: return "GL_FLOAT_MAT3x4";
		case GL_FLOAT_MAT4x2: return "GL_FLOAT_MAT4x2";
		case GL_FLOAT_MAT4x3: return "GL_FLOAT_MAT4x3";

			// Samplers
		case GL_SAMPLER_2D: return "GL_SAMPLER_2D";
		case GL_SAMPLER_CUBE: return "GL_SAMPLER_CUBE";
		case GL_SAMPLER_3D: return "GL_SAMPLER_3D";
		case GL_SAMPLER_2D_SHADOW: return "GL_SAMPLER_2D_SHADOW";
		case GL_SAMPLER_2D_ARRAY: return "GL_SAMPLER_2D_ARRAY";
		case GL_SAMPLER_CUBE_SHADOW: return "GL_SAMPLER_CUBE_SHADOW";

		default: return "UNKNOWN_TYPE";
		}
	}


	Shader::Shader(const std::vector<ShaderSource>& sources, const std::string name)
		: sources_{ sources }, name_{ name } {

		if (kDebugMode) {
			if(name.empty())
				LOG("Creating shader program with the following sources:");
			else
				LOG("Creating shader program '" << name_ << "' with the following sources : ");
		}

		// Verifica se a lista de fontes de shader está vazia ou se algum caminho (filename) está vazio
		if (sources_.empty() || std::any_of(sources_.begin(), sources_.end(), [](const ShaderSource& source_) { return source_.filename.empty(); })) {
			LOG("Error: Cannot create shader program without valid sources.");
			exit(EXIT_FAILURE); // Encerra o programa com erro
		}

		// Adiciona ao nome do ficheiro a pasta do caminho base, isto é, 'shaders/'
		for (auto& source : sources_) {
			source.filename = kShaderPath + source.filename; // Adiciona o caminho base dos shaders
			LOG("  Using shader '" << source.filename << "' of type '" << ShaderTypeToString(source.type) << "'");
		}

		// Carrega e compila os shaders
		shader_program_ = LoadShaders(sources_);
		if (shader_program_ == 0) {
			LOG("Shader program creation failed.");
		}
		else {
			LOG("Shader program created successfully with ID: " << shader_program_);
		}
	}

	Shader::~Shader() {
		// Destrói o programa de shader
		if (shader_program_ != 0) {
			LOG("Destroying shader program with ID : " << shader_program_ << " ...");
			glDeleteProgram(shader_program_);
			LOG("Shader program destroyed.");
			shader_program_ = 0; // Reseta o programa de shader
		}
		else {
			LOG("No shader program to destroy.");
		}
	}

	void Shader::Use() const {
		LOG("Activating shader...");
		// Ativa o programa de shader para uso
		if (shader_program_ != 0) {
			glUseProgram(shader_program_);
			LOG("Shader program " << shader_program_ << " activated.");
		}
		else {
			LOG("Shader program activation failed.");
		}
	}

	/*****************************************************************************
						 GLuint LoadShaders(ShaderInfo*);

	Descrição:
	----------
	Recebe um array de estruturas ShaderInfo.
	Cada estrutura contém:
	- tipo de shader. No OpenGL 4.x poderá ser um dos seguintes valores:
	  - GL_COMPUTE_SHADER
	  - GL_VERTEX_SHADER
	  - GL_TESS_CONTROL_SHADER
	  - GL_TESS_EVALUATION_SHADER
	  - GL_GEOMETRY_SHADER
	  - GL_FRAGMENT_SHADER
	- apontador para uma C-string, que contém o nome do ficheiro com código do shader
	- valor que referencia o objeto shader criado

	Exemplo de uso:
	ShaderInfo  shaders[] = {
		{ GL_VERTEX_SHADER, "triangles.vert" },
		{ GL_FRAGMENT_SHADER, "triangles.frag" }
	};

	Retorno:
	--------
	Em caso de sucesso, a função retorna o valore que referencia o objeto programa.
	Em caso de erro, será retornado o valor zero (0).
	*****************************************************************************/

	GLuint Shader::LoadShaders(std::vector<ShaderSource>& sources) {
		LOG("Loading and compiling shaders..");
		if (sources.empty()) {
			LOG("No shaders were provided.");
			return 0;
		}

		// Cria um objeto de programa
		GLuint program = glCreateProgram();

		for (int i = 0; i < sources.size(); i++) {
			// Cria um objeto shader
			sources[i].shader = glCreateShader(sources[i].type);

			// Garante o locale para leitura de ficheiros de texto com números decimais
			// Salva a locale atual do programa
			auto old = std::locale();
			// Define a locale para "C" para garantir que o ponto seja usado como separador decimal
			std::locale::global(std::locale("C"));

			// Efetua a leitura do código do shader
			const GLchar* source_code = ReadShader(sources[i].filename.c_str());
			// Se não conseguir ler o código
			if (source_code == NULL) {
				// Destrói os shaders que tinham sido criados
				for (int j = 0; j <= i; j++) {
					// Se tem um shader válido (i.e., != 0)
					if (sources[j].shader != 0)
						glDeleteShader(sources[j].shader);
					sources[j].shader = 0;
				}

				return 0;
			}

			// Restaura a locale original do programa
			std::locale::global(old);

			// Carrega o código do shader
			glShaderSource(sources[i].shader, 1, &source_code, NULL);
			LOG("Shader source code:\n" << source_code);
			delete[] source_code;

			// Compila o shader
			glCompileShader(sources[i].shader);

			// Verifica o estado da compilação
			GLint compiled;
			glGetShaderiv(sources[i].shader, GL_COMPILE_STATUS, &compiled);
			// Em caso de erro na compilação
			if (!compiled) {
				GLsizei len;
				glGetShaderiv(sources[i].shader, GL_INFO_LOG_LENGTH, &len);

				GLchar* log = new GLchar[len + 1];
				glGetShaderInfoLog(sources[i].shader, len, &len, log);
				LOG("Shader compilation failed: " << log);
				delete[] log;

				// Destrói os shaders que tinham sido criados
				for (int j = 0; j <= i; j++) {
					// Se tem um shader válido (i.e., != 0)
					if (sources[j].shader != 0)
						glDeleteShader(sources[j].shader);
					sources[j].shader = 0;
				}

				return 0;
			}

			// Anexa o shader ao programa
			glAttachShader(program, sources[i].shader);
		}

		// Linka o programa
		glLinkProgram(program);

		// Verifica o estado do processo de linkagem
		GLint linked;
		glGetProgramiv(program, GL_LINK_STATUS, &linked);
		// Em caso de erro na linkagem
		if (!linked) {
			GLsizei len;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

			GLchar* log = new GLchar[len + 1];
			glGetProgramInfoLog(program, len, &len, log);
			LOG("Shader linking failed: " << log);
			delete[] log;

			// Destrói os shaders que tinham sido criados
			for (int i = 0; i < sources.size(); i++) {
				// Se tem um shader válido (i.e., != 0)
				if (sources[i].shader != 0)
					glDeleteShader(sources[i].shader);
				sources[i].shader = 0;
			}

			return 0;
		}

		// Listar todos os uniforms ativos do shader (para debug)
		if (kDebugMode) {
			GLint numActiveUniforms = 0;
			// Nota que número de uniforms ativos é diferente do número total de uniforms declarados no shader, pois o compilador pode otimizar e eliminar uniforms que não são usados no código do shader.
			glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numActiveUniforms);
			LOG("Active uniforms in shader: " << numActiveUniforms);
			for (GLint i = 0; i < numActiveUniforms; ++i) {
				const GLsizei bufSize = 64; // Tamanho do buffer para o nome do uniform
				GLchar name[bufSize];       // Buffer para o nome do uniform
				GLsizei length;             // Comprimento do nome do uniform
				GLint size;                 // Tamanho do uniform (número de elementos)
				GLenum type;                // Tipo do uniform (GL_FLOAT, GL_INT, etc.)
				glGetActiveUniform(program, i, bufSize, &length, &size, &type, name);
				LOG("  Uniform #" << i << ": Name: " << name << ", Type: " << UniformTypeToString(type) << ", Size: " << size);
			}
		}
		// Listar todos os atributos ativos do shader (para debug)
		if (kDebugMode) {
			GLint numActiveAttribs = 0;
			glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);
			LOG("Active attributes in shader: " << numActiveAttribs);
			for (GLint i = 0; i < numActiveAttribs; ++i) {
				const GLsizei bufSize = 64; // Tamanho do buffer para o nome do atributo
				GLchar name[bufSize];       // Buffer para o nome do atributo
				GLsizei length;             // Comprimento do nome do atributo
				GLint size;                 // Tamanho do atributo (número de elementos)
				GLenum type;                // Tipo do atributo (GL_FLOAT, GL_INT, etc.)
				glGetActiveAttrib(program, i, bufSize, &length, &size, &type, name);
				LOG("  Attribute #" << i << ": Name: " << name << ", Type: " << UniformTypeToString(type) << ", Size: " << size);
			}
		}

		return program;
	}

	const GLchar* Shader::ReadShader(const char* filename) {
		// Abre o ficheiro 'filename' em binário, e coloca-se na última posição do ficheiro.
		std::ifstream ficheiro(filename, std::ifstream::ate | std::ifstream::binary);
		// Se o ficheiro foi aberto.
		if (ficheiro.is_open()) {
			// Leitura da próxima posição de leitura.
			std::streampos tamanhoDoFicheiroEmBytes = ficheiro.tellg();
			// Reposiciona a leitura do ficheiro no seu início.
			ficheiro.seekg(0, std::ios::beg);

			// Alocação de espaço de memória para dados do ficheiro.
			GLchar* source = new GLchar[int(tamanhoDoFicheiroEmBytes) + 1];
			// Leitura do ficheiro para o array 'source'.
			ficheiro.read(source, tamanhoDoFicheiroEmBytes);
			// Fecha a string.
			source[tamanhoDoFicheiroEmBytes] = 0;

			// Fecha o ficheiro.
			ficheiro.close();

			// Retorna o endereço da string alocada.
			return const_cast<const GLchar*>(source);
		}
		else {
			LOG("An error occurred while opening the file '" << filename << "'");
		}

		return nullptr;
	}
}