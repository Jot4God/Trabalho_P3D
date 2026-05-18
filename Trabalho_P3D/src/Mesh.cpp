#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <array>
#include <string>
#include <glm\glm.hpp>
#define GLEW_STATIC
#include <GL\glew.h>

#include "Common.h" // Para definições comuns e macros
#include "Shader.h"
#include "Mesh.h"


namespace game_engine_p3d {

	template<class T> T op_StringToVec(std::string str);
	template<class T> T op_StringToArrayOfVec(std::string str);

	Mesh::Mesh(const Shader* shader, const std::string& obj_filename) {
		// Se o shader não for fornecido, encerra o programa com uma mensagem de erro
		if(!shader) {
			LOG("Error: Shader is null. Cannot create Mesh without a shader.");
			exit(EXIT_FAILURE); // Encerra o programa com erro
		}
		// Se não for fornecido um caminho para o objeto, encerra o programa com uma mensagem de erro
		if(obj_filename.empty()) {
			LOG("Error: OBJ filename is empty. Cannot create Mesh without an OBJ path.");
			exit(EXIT_FAILURE); // Encerra o programa com erro
		}

		LOG("Creating Mesh with OBJ file: " + obj_filename);

		// Adiciona o caminho base dos assets ao nome do ficheiro
		std::string obj_filename_full = kAssetPath + obj_filename;
		
		// Garante o locale para leitura de ficheiros de texto com números decimais
		// Salva a locale atual do programa
		auto old = std::locale();
		// Define a locale para "C" para garantir que o ponto seja usado como separador decimal
		std::locale::global(std::locale("C"));

		// Carrega a malha (para a RAM da CPU) a partir do ficheiro OBJ
		ObjLoader(obj_filename_full);

		// Restaura a locale original do programa
		std::locale::global(old);

		// Carrega os dados da malha (vértices, normais, coordenadas de textura) para os buffers OpenGL (VBOs) da GPU
		{
			// Criação do Vertex Buffer Object (VBO) para vértices
			if (!v_.empty()) {
				LOG("Vertices provided for the mesh.");
				// VBO para vértices
				glGenBuffers(1, &vbo_vertices_); // Gera um (1) nome para o VBO
				glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices_); // Vincula o VBO ao contexto OpenGL
				glBufferData(GL_ARRAY_BUFFER, v_.size() * sizeof(glm::vec3), v_.data(), GL_STATIC_DRAW); // Copia os dados dos vértices para o VBO
			}
			else {
				LOG("Warning: No vertices provided for the mesh. Mesh will not be rendered.");
			}
			// Criação do VBO para normais
			if (!vn_.empty()) {
				LOG("Normals provided for the mesh.");
				// VBO para normais
				glGenBuffers(1, &vbo_normals_);
				glBindBuffer(GL_ARRAY_BUFFER, vbo_normals_);
				glBufferData(GL_ARRAY_BUFFER, vn_.size() * sizeof(glm::vec3), vn_.data(), GL_STATIC_DRAW);
			}
			else {
				LOG("Warning: No normals provided for the mesh. Normals will not be used.");
			}
			// Criação do VBO para coordenadas de textura
			if (!vt_.empty()) {
				LOG("Texture coordinates provided for the mesh.");
				// VBO para coordenadas de textura
				glGenBuffers(1, &vbo_texcoords_);
				glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_);
				glBufferData(GL_ARRAY_BUFFER, vt_.size() * sizeof(glm::vec2), vt_.data(), GL_STATIC_DRAW);
			}
			else {
				LOG("Warning: No texture coordinates provided for the mesh. Texture coordinates will not be used.");
			}
		}

		// Obtém o ID do programa shader
		GLuint programa = shader->shader_program();

		// Criação do Vertex Array Object (VAO) e configuração dos atributos
		{
			glGenVertexArrays(1, &vao_); // Gera o nome do VAO
			glBindVertexArray(vao_); // Vincula o VAO ao contexto OpenGL
			// Configuração dos atributos do VAO (associar os VBOs aos atributos do shader)
			// Atributo: Vértices
			if (!v_.empty()) {
				// VBO para vértices
				// Liga o atributo ao 'índice' do VAO (e por consequência à 'location' do programa shader)
				// NOTE: Os nomes dos atributos estão hardcoded (ver ficheiro "Common.h") e devem existir no shader
				glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices_); // Vincula o VBO ao contexto OpenGL
				GLint coordsId = glGetProgramResourceLocation(programa, GL_PROGRAM_INPUT, kVertexPositionName); // Obtém a localização do atributo no programa shader (para versões >= 4.3)
				if(coordsId == -1) {
					LOG("Error: Attribute 'vPosition' not found in shader program. Mesh cannot be rendered.");
					exit(EXIT_FAILURE); // Encerra o programa com erro (sem vértices, a malha não pode ser renderizada)
				}
				else {
					glVertexAttribPointer(coordsId, 3, GL_FLOAT, GL_FALSE, 0, nullptr); // Define o formato dos dados dos vértices (índice 'coordsId' no VAO, 3 elementos por vértice, do tipo float, sem normalização, sem espaçamento entre os vértices, e sem deslocamento)
					glEnableVertexAttribArray(coordsId); // Ativa o atributo de vértices no VAO (o VBO associado a este índice do VAO irá passar a fornecer os dados de vértices ao shader)
				}
			}
			// Atributo: Normais
			if (!vn_.empty()) {
				// Liga o atributo ao 'índice' do VAO (e por consequência à 'location' do programa shader)
				glBindBuffer(GL_ARRAY_BUFFER, vbo_normals_);
				GLint normalId = glGetProgramResourceLocation(programa, GL_PROGRAM_INPUT, kVertexNormalName);
				if(normalId == -1) {
					LOG("Warning: Attribute 'vNormal' not found in shader program. Normals will not be used.");
				}
				else {
					glVertexAttribPointer(normalId, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
					glEnableVertexAttribArray(normalId);
				}
			}
			// Atributo: Coordenadas de textura
			if (!vt_.empty()) {
				// Liga o atributo ao 'índice' do VAO (e por consequência à 'location' do programa shader)
				glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_);
				GLint textureId = glGetProgramResourceLocation(programa, GL_PROGRAM_INPUT, kVertexTexCoordName);
				if(textureId == -1) {
					LOG("Warning: Attribute 'vTexture' not found in shader program. Texture coordinates will not be used.");
				}
				else {
					glVertexAttribPointer(textureId, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
					glEnableVertexAttribArray(textureId);
				}
			}
			glBindVertexArray(0); // Desvincula o VAO
		}

		LOG("Mesh created.");
	}

	Mesh::~Mesh() {
		LOG("Destroying Mesh...");
		LOG("Destroying VAO with ID: " + std::to_string(vao_));
		
		if (vao_) glDeleteVertexArrays(1, &vao_);
		if (vbo_vertices_) glDeleteBuffers(1, &vbo_vertices_);
		if (vbo_normals_) glDeleteBuffers(1, &vbo_normals_);
		if (vbo_texcoords_) glDeleteBuffers(1, &vbo_texcoords_);

		LOG("Mesh destroyed.");
	}

	void Mesh::Use(void) const {
		// Ativa o VAO para renderização
		glBindVertexArray(vao_);
	}

	void Mesh::ObjLoader(const std::string& obj_filename) {
		// If it is not an OBJ file, it returns error.
		if (!isObjFile(obj_filename)) {
			LOG("'" + obj_filename + "'" + " is not an '.obj' file!");
			return;
		}

		// If the OBJ file can not be opened, it returns an error.
		const char* data = loadDataFromObjFile(obj_filename);
		if (data == nullptr) {
			LOG("Error on opening file '" + obj_filename + "'!");
			return;
		}

		// Extract attributes to a vector of strings.
		std::vector<std::string> attributeVectorV = extractObjAttribute(data, "\nv ");		// vertex position
		std::vector<std::string> attributeVectorVt = extractObjAttribute(data, "\nvt ");	// texture coordinates
		std::vector<std::string> attributeVectorVn = extractObjAttribute(data, "\nvn ");	// vertex normal
		std::vector<std::string> attributeVectorF = extractObjAttribute(data, "\nf ");		// face index

		// Allocate vectors to store all attribute data.
		std::vector<glm::vec4>* vTmp = new std::vector<glm::vec4>(attributeVectorV.size(), glm::vec4(1.0)); // Default value 1.0
		std::vector<glm::vec3>* vtTmp = new std::vector<glm::vec3>(attributeVectorVt.size(), glm::vec4(0.0)); // Default value 0.0
		std::vector<glm::vec3>* vnTmp = new std::vector<glm::vec3>(attributeVectorVn.size());
		std::vector<std::array<glm::ivec3, 3>>* fTmp = new std::vector<std::array<glm::ivec3, 3>>(attributeVectorF.size());

		// Extracts attribute values from the string vector and loads them into the corresponding attribute vector.
		std::transform(attributeVectorV.begin(), attributeVectorV.end(), vTmp->begin(), op_StringToVec<glm::vec4>);
		std::transform(attributeVectorVt.begin(), attributeVectorVt.end(), vtTmp->begin(), op_StringToVec<glm::vec3>);
		std::transform(attributeVectorVn.begin(), attributeVectorVn.end(), vnTmp->begin(), op_StringToVec<glm::vec3>);
		std::transform(attributeVectorF.begin(), attributeVectorF.end(), fTmp->begin(), op_StringToArrayOfVec<std::array<glm::ivec3 /*3 vertex per face*/, 3 /*each vertex has: v, vt, vn*/>>);

		// Face elements
		// f v1[/vt1][/vn1] v2[/vt2][/vn2] v3[/vt3][/vn3]
		// Examples:
		// f v1 v2 v3
		// f v1/vt1 v2/vt2 v3/vt3
		// f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
		// f v1//vn1 v2//vn2 v3//vn3

		for (std::vector<std::array<glm::ivec3, 3>>::iterator it = fTmp->begin(); it != fTmp->end(); it++) {
			std::array<glm::ivec3, 3> arrayOfVec = *it;

			// Vertices of a triangle
			glm::ivec3 vertex[3];
			for (int i = 0; i < 3; i++) { vertex[i] = arrayOfVec[i]; }

			// For each face we have v, vt and vn
			int indexOfVertexPositionForVertex[3];
			int indexOfTextureCoordinateForVertex[3];
			int indexOfVertexNormalForVertex[3];
			for (int i = 0; i < 3; i++) {
				// The index in 'face' start at the value '1', so we need to subtract one.
				indexOfVertexPositionForVertex[i] = vertex[i][0/*position*/] - 1;
				indexOfTextureCoordinateForVertex[i] = vertex[i][1/*texture*/] - 1;
				indexOfVertexNormalForVertex[i] = vertex[i][2/*normal*/] - 1;

				// Load values to the attribute vectors
				v_.push_back(vTmp->at(indexOfVertexPositionForVertex[i]));		// position of vertex #0
				vt_.push_back(vtTmp->at(indexOfTextureCoordinateForVertex[i]));	// texture coordinate of vertex #0
				vn_.push_back(vnTmp->at(indexOfVertexNormalForVertex[i]));		// normal of vertex #0
			}
		}

		// Encontra os valores máximo e mínimo dos vértices
		glm::vec3 maxV, minV;
		FindMaxMinFromVec(v_, maxV, minV);
		min_x_ = minV.x; max_x_ = maxV.x;
		LOG("Mesh X axis extent: min_x = " << std::to_string(min_x_) << ", max_x = " << std::to_string(max_x_));
		min_y_ = minV.y; max_y_ = maxV.y;
		LOG("Mesh Y axis extent: min_y = " << std::to_string(min_y_) << ", max_y = " << std::to_string(max_y_));
		min_z_ = minV.z; max_z_ = maxV.z;
		LOG("Mesh Z axis extent: min_z = " << std::to_string(min_z_) << ", max_z = " << std::to_string(max_z_));

		LOG("Total number of vertex = " << std::to_string(v_.size()));

		delete vTmp;
		delete vtTmp;
		delete vnTmp;

		delete[] data;

		return;
	}

	bool Mesh::isObjFile(const std::string file) {
		// Return false if it is not an '.obj' file
		if (file.substr(file.size() - 4, 4) != ".obj") return false;
		return true;
	}

	const char* Mesh::loadDataFromObjFile(const std::string file) {
		// Abre o ficheiro 'file' em binário, e coloca-se na última posição do ficheiro.
		std::ifstream ficheiro(file, std::ifstream::ate | std::ifstream::binary);
		// Se o ficheiro foi aberto.
		if (ficheiro.is_open()) {
			// Leitura da próxima posição de leitura.
			std::streampos tamanhoDoFicheiroEmBytes = ficheiro.tellg();
			// Reposiciona a leitura do ficheiro no seu início.
			ficheiro.seekg(0, std::ios::beg);

			// Alocação de espaço de memória para dados do ficheiro.
			char* data = new char[int(tamanhoDoFicheiroEmBytes) + 1];
			// Leitura do ficheiro para o array 'source'.
			ficheiro.read(data, tamanhoDoFicheiroEmBytes);
			// Fecha a string.
			data[tamanhoDoFicheiroEmBytes] = 0;

			// Fecha o ficheiro.
			ficheiro.close();

			// Retorna o endereço da string alocada.
			return const_cast<const char*>(data);
		}
		else {
			return nullptr;
		}
	}

	std::vector<std::string> Mesh::extractObjAttribute(const char* data, std::string AttributeDelimiter) {
		std::vector<std::string> retAttributeVector;
		std::string s(data);
		std::string delimiter = AttributeDelimiter;

		// Removes the '\r' characters that may be present before each '\n'
		s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());

		size_t lastOffset = s.find(delimiter, 0); // 1st delimiter
		if (lastOffset == std::string::npos) return retAttributeVector; // If no delimiter was found
		while (true)
		{
			size_t offset = s.find(delimiter, lastOffset + 1); // next delimiter (add one to skip the delimiter)
			retAttributeVector.push_back(s.substr(lastOffset + delimiter.size(), offset - lastOffset - delimiter.size()));
			//std::cout << "* " << s.substr(lastOffset + delimiter.size(), offset - lastOffset - delimiter.size()) << " +\n";

			// If no delimiter was found
			if (offset == std::string::npos) break;

			lastOffset = offset;
		}

		if (kDebugMode) {
			delimiter.erase(std::remove(delimiter.begin(), delimiter.end(), '\r'), delimiter.end());
			delimiter.erase(std::remove(delimiter.begin(), delimiter.end(), '\n'), delimiter.end());
			delimiter.erase(std::remove(delimiter.begin(), delimiter.end(), ' '), delimiter.end());
			LOG("Number of '" + delimiter + "' elements " + " = " + std::to_string(retAttributeVector.size()));
		}

		return retAttributeVector;
	}

	void Mesh::FindMaxMinFromVec(std::vector<glm::vec3> vec, glm::vec3& max, glm::vec3& min) {
		if (vec.empty()) return;
		max = vec[0];
		min = vec[0];
		for (std::vector<glm::vec3>::iterator it = vec.begin(); it != vec.end(); it++) {
			glm::vec3 v = *it;
			if (v.x > max.x) max.x = v.x;
			if (v.y > max.y) max.y = v.y;
			if (v.z > max.z) max.z = v.z;
			if (v.x < min.x) min.x = v.x;
			if (v.y < min.y) min.y = v.y;
			if (v.z < min.z) min.z = v.z;
		}
	}

	template<class T>
	T op_StringToVec(std::string str) {
		T vec(0);
		std::istringstream iss(str);
		int vecElement = 0;
		while (iss && vecElement < vec.length()) {
			iss >> vec[vecElement++];
		}
		return vec;
	}

	template<class T>
	T op_StringToArrayOfVec(std::string str) {
		T arrayOfVec{};
		replace(str.begin(), str.end(), '/', ' ');
		std::istringstream iss(str);
		for (int arrayIndex = 0; arrayIndex < arrayOfVec.size(); arrayIndex++) {
			for (int vecElement = 0; vecElement < arrayOfVec[0].length(); vecElement++) {
				iss >> arrayOfVec[arrayIndex][vecElement];
			}
		}
		return arrayOfVec;
	}
}