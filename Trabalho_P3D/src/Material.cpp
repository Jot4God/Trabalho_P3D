#include <fstream>
#include <sstream>
#include <algorithm>

#include "Common.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"


namespace game_engine_p3d {

    Material::Material(const Shader* shader, const std::string obj_filename) {
        // Se o shader não for fornecido, encerra o programa com uma mensagem de erro
        if (!shader) {
            LOG("Error: Shader is null. Cannot create Material object without a shader.");
            exit(EXIT_FAILURE); // Encerra o programa com erro
        }
        // Se não for fornecido um caminho para o objeto, encerra o programa com uma mensagem de erro
        if (obj_filename.empty()) {
            LOG("Error: Object filename is empty. Cannot create Material object without an object path.");
            exit(EXIT_FAILURE); // Encerra o programa com erro
        }

		LOG("Creating Material with shader and OBJ file: " + obj_filename);

        // Inicializa o shader com o fornecido ou nulo se não for fornecido
        shader_ = (Shader*)shader;

        // Adiciona o caminho base dos assets ao nome do ficheiro
        std::string obj_filename_full = kAssetPath + obj_filename;

        // A partir do ficheiro OBJ, obtém o nome do ficheiro MTL (fica guardado em 'mtl_filename_')
        if (findMaterialFile(obj_filename_full) == false) {
            LOG("No MTL file found in OBJ file: '" << obj_filename_full << "'");
            // Se não encontrar o ficheiro MTL, não continua
            return;
        }

        // Carrega as propriedades do material (e instancia as Texturas) a partir do ficheiro MTL
        LoadMaterialFromFile();
        // Se a flag de debug estiver ativa, imprime as propriedades do material
        if (kDebugMode) {
            LOG("Material properties:");
            LOG("  Ambient Color: " << ka_.r << " " << ka_.g << " " << ka_.b);
            LOG("  Diffuse Color: " << kd_.r << " " << kd_.g << " " << kd_.b);
            LOG("  Specular Color: " << ks_.r << " " << ks_.g << " " << ks_.b);
            LOG("  Emissive Color: " << ke_.r << " " << ke_.g << " " << ke_.b);
            LOG("  Shininess: " << Ns_);
            LOG("  Index of Refraction: " << Ni_);
            LOG("  Opacity: " << d_);
            LOG("  Illumination Model: " << illum_);
            if (texture_.size() >= 1) LOG("  Texture: '" << texture_.at(0).path() << "'");
            else LOG("  Texture: None");
            if (texture_.size() >= 2) LOG("  Normal Map: '" << texture_.at(1).path() << "'");
            else LOG("  Normal Map: None");
        }

		LOG("Material created with shader and object path: " + obj_filename);
    }

    Material::Material(glm::vec3 ka, glm::vec3 kd, glm::vec3 ks, glm::vec3 ke, float Ns, float Ni, float d, int illum,
        Shader* shader, std::vector<Texture> textures) {
        // Inicializa as propriedades do material com os valores fornecidos ou padrões
        ka_ = ka; // Cor ambiente
        kd_ = kd; // Cor difusa
        ks_ = ks; // Cor especular
        ke_ = ke; // Cor emissiva
        Ns_ = Ns; // Exponente de reflexão especular
        Ni_ = Ni; // Índice de refração
        d_ = d;   // Opacidade
        illum_ = illum;     // Modelo de iluminação
        shader_ = shader;       // Shader associado ao material
        texture_ = textures;    // Textura do material, se fornecida, caso contrário, cria uma textura vazia
    }

    void Material::Use() const {
        // Ativa o shader do material
        if (shader_) {
            shader_->Use();
        }

        // Ativa a textura do material (se existir)
        // FIXME: Atualmente, apenas a primeira textura (difusa) é ativada. Poderia ser melhorado para suportar múltiplas texturas (difusa, especular, normal map, etc.)
        // A Unidade de Textura deveria, nesse caso, coincidir com o índice da textura no vetor 'texture_'.
        if (texture_.size() > 0) {
            texture_.at(0).Use(); // Vincula a primeira textura (difusa)
        }
        
		// Configura os uniforms do material no shader
		// NOTE: Os nomes dos uniforms são hardcoded (ver ficheiro "Common.h") e devem existir no shader
        if (shader_) {
            shader_->SetUniform<glm::vec3>(kMaterialAmbientName, ka_);
            shader_->SetUniform<glm::vec3>(kMaterialDiffuseName, kd_);
            shader_->SetUniform<glm::vec3>(kMaterialSpecularName, ks_);
            shader_->SetUniform<glm::vec3>(kMaterialEmissiveName, ke_);
            shader_->SetUniform<float>(kMaterialShininessName, Ns_);
            shader_->SetUniform<float>(kMaterialIndexOfRefractionName, Ni_);
            shader_->SetUniform<float>(kMaterialOpacityName, d_);
            shader_->SetUniform<int>(kMaterialIlluminationModelName, illum_);
        }

        LOG("Material used.");
        LOG("  Ambient Color: " << ka_.r << " " << ka_.g << " " << ka_.b);
        LOG("  Diffuse Color: " << kd_.r << " " << kd_.g << " " << kd_.b);
        LOG("  Specular Color: " << ks_.r << " " << ks_.g << " " << ks_.b);
        LOG("  Emissive Color: " << ke_.r << " " << ke_.g << " " << ke_.b);
        LOG("  Shininess: " << Ns_);
        LOG("  Index of Refraction: " << Ni_);
        LOG("  Opacity: " << d_);
        LOG("  Illumination Model: " << illum_);
    }

    bool Material::findMaterialFile(const std::string& obj_filename) {
        std::string delimiter = "mtllib ";	// This delimiter precedes the name of the mtl file.
        std::string mtlFile;

        LOG("Looking for MTL file in OBJ file: '" << obj_filename << "'");

        // Abre o ficheiro OBJ para ler o nome do ficheiro MTL
        std::ifstream obj_file(obj_filename);
        // Verifica se o ficheiro OBJ foi aberto corretamente
        if (!obj_file.is_open()) {
            LOG("Failed to open OBJ file: '" << obj_filename << "'");
            return ""; // Retorna uma string vazia se o ficheiro não puder ser aberto
        }
        // Verifica se o ficheiro contém o delimitador "mtllib "
        // Vamos considerar que poderá estar em qualquer parte do ficheiro OBJ, por isso lê o ficheiro linha a linha
        std::string obj_line;
        while (std::getline(obj_file, obj_line)) {
            size_t pos = obj_line.find(delimiter, 0);
            // Se encontrar o delimitador
            if (pos != std::string::npos) {
                // Obtém o nome do ficheiro MTL
                mtlFile = obj_line.substr(pos + delimiter.length(), obj_line.find('\n', pos) - (pos + delimiter.length()));

                // Remove os caracteres '\r' que podem estar presentes antes do '\n'
                mtlFile.erase(std::remove(mtlFile.begin(), mtlFile.end(), '\r'), mtlFile.end());

                // Fecha o ficheiro
                obj_file.close();

                // Guarda o nome do ficheiro MTL encontrado
                mtl_filename_ = mtlFile;

                // Retorna verdadeiro se encontrar o ficheiro MTL
                return true;
            }
        }

        // Retorna falso se não encontrar o ficheiro MTL
        return false;
    }

    void Material::LoadMaterialFromFile(void) {

        // Adiciona o caminho base dos assets ao nome do ficheiro
        std::string mtl_filename_full = kAssetPath + mtl_filename_;

        // Carrega as propriedades do material a partir do ficheiro MTL
        LOG("Loading material properties from file: '" << mtl_filename_full << "'");

        // Abrir e ler o ficheiro MTL
        std::ifstream file(mtl_filename_full);
        if (!file.is_open()) {
            LOG("Failed to open MTL file: '" << mtl_filename_full << "'");
            return;
        }

        //NOVO
        auto old = std::locale();
        // Define a locale para "C" para garantir que os números decimais sejam lidos corretamente (com ponto como separador decimal))
		std::locale::global(std::locale("C")); 


        // Faz o parse do ficheiro linha a linha
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "Ka") { // Ambient color
                iss >> ka_.r >> ka_.g >> ka_.b;
            }
            else if (prefix == "Kd") { // Diffuse color
                iss >> kd_.r >> kd_.g >> kd_.b;
            }
            else if (prefix == "Ks") { // Specular color
                iss >> ks_.r >> ks_.g >> ks_.b;
            }
            else if (prefix == "Ke") { // Emissive color
                iss >> ke_.r >> ke_.g >> ke_.b;
            }
            else if (prefix == "Ns") { // Shininess
                iss >> Ns_;
            }
            else if (prefix == "Ni") { // Index of refraction
                iss >> Ni_;
            }
            else if (prefix == "d") {  // Dissolve
                iss >> d_;
            }
            else if (prefix == "illum") { // Illumination model
                iss >> illum_;
            }
            else if (prefix == "map_Kd") { // Diffuse texture map
                std::string texture_path;
                iss >> texture_path;
                texture_.push_back(Texture(texture_path)); // Não estranhe que seja criada uma nova textura aqui, que depois seja destruída após inserir no vetor.
            }
            else if (prefix == "map_Ks") { // Specular texture map
                std::string specular_map_path;
                iss >> specular_map_path;
                texture_.push_back(Texture(specular_map_path));
            }
            else if (prefix == "map_Ke") { // Emissive texture map
                std::string emissive_map_path;
                iss >> emissive_map_path;
                texture_.push_back(Texture(emissive_map_path));
            }
            else if (prefix == "map_Bump") { // Normal map
                std::string normal_map_path;
                iss >> normal_map_path;
                texture_.push_back(Texture(normal_map_path));
            }
        }

        // NOVO
		std::locale::global(old);
        

        file.close();
        LOG("Material properties loaded.");
    }
}