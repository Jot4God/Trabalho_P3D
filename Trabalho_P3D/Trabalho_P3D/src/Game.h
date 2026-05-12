#pragma once


// ==============================================================
// Ficheiro:    Game.h
// Descrição:   Declaração da classe Game
// Autor:       Duarte Duque
// Data:        21/07/2025
// Versão:      1.0.0
// Dependências: Common.h
// Observações: Esta classe representa o jogo e gerencia o estado do jogo, objetos, entrada e renderização.
// Ela contém o loop principal do jogo e controla a transição entre os diferentes estados do jogo.
// A classe 'Camera' será utilizada para manipular as câmaras no jogo.
// A classe 'Light' será utilizada para manipular as luzes no jogo.
// A classe 'Object' será utilizada para manipular os objetos no jogo.
// A classe 'WindowSystem' será utilizada para processar a entrada do utilizador.
// ==============================================================


#include <iostream>
#include <vector>

#include "Common.h"			// Inclui definições comuns e macros para o motor de jogo (neste caso, é necessário para utilização de 'kWindowDefaultWidth' e 'kWindowDefaultHeight').
#include "Camera.h"			// Inclui a classe 'Camera' para manipulação de câmaras no jogo.
#include "Light.h"			// Inclui a classe 'Light' para manipulação de luzes no jogo.
#include "Object.h"			// Inclui a classe 'Object' para manipulação de objetos no jogo.
#include "WindowSystem.h"	// Inclui a classe 'WindowSystem' para manipulação da janela da aplicação e entrada do utilizador (teclado e rato).


namespace game_engine_p3d {

	class Game {
	public:
		// ------------------------------------------------------------
		// Tipos de dados
		// ------------------------------------------------------------
		// Enumeração para definir os estados do jogo.
		// Esta enumeração foi definida dentro da classe 'Game' para encapsular o estado do jogo e evitar poluição do namespace global.
		enum class GameState {
			kGameStateInitialization,	// Inicialização (estado inicial do jogo).
			kGameStateGameLoop,			// Game Loop (estado principal do jogo).
			kGameStateShutdown			// Saída do jogo (shutdown).
		};

		// ------------------------------------------------------------
		// Construtores e destrutores
		// ------------------------------------------------------------
		Game(unsigned int width = kWindowDefaultWidth, unsigned int height = kWindowDefaultHeight, const char* title = "Game Window"); // Construtor com parâmetros.
		~Game(); // Destrutor (optamos por o definir explicitamente para posteriormente encerrarmos o sistema de 'WindowSystem', quando se destruir um objeto desta classe).
		// ------------------------------------------------------------
		// Accessors e mutators
		// ------------------------------------------------------------
		GameState state() const { return state_; }		// Função acessora (em inglês, 'accessor') para aceder (ler) o estado atual do jogo.
		unsigned int width() const { return width_; }	// Função acessora para obter a largura da janela do jogo.
		unsigned int height() const { return height_; }	// Função acessora para obter a altura da janela do jogo.
		// ------------------------------------------------------------
		// Outras funções-membro
		// ------------------------------------------------------------
		void AddCamera(Camera* camera); // Função para adicionar uma câmara ao jogo.
		void AddLight(Light* light);	// Função para adicionar uma luz ao jogo.
		void AddObject(Object* object);	// Função para adicionar um objeto ao jogo.
		Object* FindObjectByName(const std::string& name); // Função para encontrar um objeto pelo nome.
		void Run();		// Função que contém o game loop e que processa a entrada do utilizador.
		void Exit();	// Função para sair do jogo.

	private:
		// ------------------------------------------------------------
		// Funções-membro privadas
		// ------------------------------------------------------------
		void FrameRateControl(double fps); // Função para controlar a taxa de frames do jogo.

		// ------------------------------------------------------------
		// Dados-membro privados
		// ------------------------------------------------------------
		GameState state_ = GameState::kGameStateInitialization;	// Estado inicial do jogo.
		unsigned long int frameCount_ = 0;	// Contador de frames.
		double delta_time_ = 0.0;			// Tempo decorrido desde o último frame.
		// NOTE: Relação de "composição" de 'Game' com 'WindowSystem' (i.e., 'Game' é composto por uma instância de 'WindowSystem', que só existe enquanto existir o objeto 'Game').
		WindowSystem window_system_;		// Instância da classe 'WindowSystem' para processar a entrada do utilizador.
		unsigned int width_;				// Largura da janela do jogo.
		unsigned int height_;				// Altura da janela do jogo.
		char const* title_;					// Título da janela do jogo.
		// NOTE: Relação de "agregação" de 'Game' com 'Camera' (i.e., 'Game' possui uma coleção de câmaras do jogo). As câmaras podem existir independentemente do jogo.
		std::vector<Camera*> cameras_{};		// Vetor de apontadores para câmaras no jogo (um jogo pode ter várias câmaras). A primeira câmara é a câmara principal ("Main Camera").
		// NOTE: Relação de "agregação" de 'Game' com 'Light' (i.e., 'Game' possui uma coleção de luzes do jogo).
		std::vector<Light*> lights_{};			// Vetor de apontadores para luzes no jogo (um jogo pode ter várias luzes).
		// NOTE: Relação de "agregação" de 'Game' com 'Object' (i.e., 'Game' possui uma coleção de objetos do jogo). Os objetos podem existir independentemente do jogo.
		std::vector<Object*> objects_{};		// Vetor de apontadores para objetos no jogo.
	};
}