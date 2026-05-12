#pragma once


// ==============================================================
// Ficheiro:    Renderer.h
// Descrição:   Declaração da classe Renderer
// Autor:       Duarte Duque
// Data:        21/07/2025
// Versão:      1.0.0
// Dependências: Shader.h, Material.h, Mesh.h
// Observações:
// Esta classe representa um renderizador (em inglês, "renderer") que pode ser usado para desenhar (em inglês, "render") objetos 3D no jogo.
// Ela contém um material, que é responsável por definir o shader, a textura e as propriedades físicas do objeto.
// A classe 'Renderer' é responsável por configurar o material e desenhar a malha associada ao objeto.
// ==============================================================


#include <iostream>
#include <vector>
#include <string>

#include "Material.h"   // Inclui a classe 'Material', que define as propriedades do material
#include "Mesh.h"       // Inclui a classe 'Mesh', que define a malha do objeto 3D
#include "Light.h"      // Inclui a classe 'Light', que define uma luz no jogo


namespace game_engine_p3d {

	class Shader; // Declaração antecipada da classe 'Shader'


    class Renderer {
    public:
		// ------------------------------------------------------------
		// Construtores e destrutores
		// ------------------------------------------------------------
        Renderer(const Shader* shader, const std::string obj_filename);
        virtual ~Renderer() = default;
		// ------------------------------------------------------------
		// Outras funções-membro
		// ------------------------------------------------------------
		void Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection, std::string layer, std::vector<Light*> lights);    // Função que desenha o objeto usando o material e o shader associado

    private:
		Material material_; // Material associado ao objeto, que contém o shader, as texturas e as propriedades do material
		Mesh mesh_;         // Malha (mesh) do objeto 3D, a partir do ficheiro OBJ
    };
}