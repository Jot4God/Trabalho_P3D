#pragma once


// ==============================================================
// Oscilator.h
// Descrição:   Declaração da classe Oscilator
// Autor:       Duarte Duque
// Data:        21/07/2025
// Versão:      1.0.0
// Dependências: Behaviour.h, Input.h
// Compilação:	g++ Oscilator.h -o Oscilator.o -lglew32s -lglfw3 -lopengl32
// Observações:
// Esta classe representa um comportamento de oscilador que pode ser aplicado a um objeto no jogo.
// Ela herda da classe Behaviour e implementa os métodos Start e Update para controlar o comportamento de oscilação.
// ==============================================================


#include <iostream>		// Para std::cout

#include "Game.h"		// Necessário para acessar o método Pause() do jogo associado ao objeto
#include "Behaviour.h"	// Necessário para herdar da classe Behaviour
#include "WindowSystem.h"	// Necessário para aceder às funções de entrada do teclado e rato


using namespace game_engine_p3d; // Usar o namespace do motor de jogo para evitar prefixos longos


class Oscilator : public Behaviour {
private:
	float tempo_ = 0.0f;		// Tempo acumulado
	float velocidade_ = 1.0f;	// Velocidade de oscilação
	bool pause = false;			// Indica se o oscilador está pausado

public:
	// Indica que a função está a substituir (overriding) uma função virtual, com o mesmo nome, na classe base.
	void Start(Object& object) override {
		tempo_ = 0; // Inicializa o tempo acumulado
		LOG("[Oscilator] Start() invoked -> tempo_ = " << tempo_); // Regista a inicialização do oscilador
	}

	void Update(Object& object) override {
		Object* obj = object.game()->FindObjectByName("Objecto (2)"); // Encontra o objeto "Objecto (2)" no jogo
		obj->model().Translate(0.0f, 0.0f, -0.01f); // Move o objeto "Objecto (2)" ligeiramente ao longo do eixo Z em cada frame
		
		tempo_ += 0.1f * velocidade_; // Incrementa o tempo acumulado
		//LOG("[Oscilator] Update() invoked -> tempo_ = " << tempo_);

		// Atualiza a posição do objeto "Objecto (1)" para oscilar em torno do eixo Y com base no tempo acumulado
		object.model().Translate(
			0.0f,
			0.5f * sin(tempo_), // Oscila em torno do eixo Y
			0.0f
		); // Atualiza a posição do objeto com base no tempo acumulado

		// Exemplo de uso de WindowSystem
		/*if (WindowSystem::GetKey(GLFW_KEY_SPACE) == true && WindowSystem::GetKey(GLFW_KEY_LEFT_CONTROL) == true) {
			LOG("[Oscilator] Space key + Left Ctrl are pressed.");
		}*/

		// Exemplo de Pausa do jogo
		if (WindowSystem::GetKey(GLFW_KEY_ESCAPE) == true) {
			LOG("[Oscilator] Escape key pressed. Toggling pause state.");

			// Aqui poderia chamar uma função para pausar o jogo, se implementada
			pause = !pause; // Alterna o estado de pausa
			if (pause) {
				LOG("[Oscilator] Game paused.");
			}
			else {
				LOG("[Oscilator] Game resumed.");
			}
		}

		auto mouse_pos = WindowSystem::GetMousePosition(); // Obtém a posição do rato
		LOG("[Oscilator] Mouse position: (" << mouse_pos.first << ", " << mouse_pos.second << ")");
	}
};
