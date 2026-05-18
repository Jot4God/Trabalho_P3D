#pragma once


// =============================================================
// Ficheiro:    WindowSystem.h
// Descrição:   Declaração da classe 'WindowSystem'
// Autor:       Duarte Duque
// Data:        21/07/2025
// Versão:      1.0.0
// Dependências: GLFW, GLEW
// Compilação:	g++ WindowSystem.h -o WindowSystem.o -lglew32s -lglfw3 -lopengl32
// Observações:
// A classe inicializa uma janela através da biblioteca GLFW.
// Define a resolução da janela e o título, e permite capturar eventos de teclado e rato.
// =============================================================


#include <utility> // std::pair
//--------------------------------------------------------------
#define GLEW_STATIC // Definição necessária, antes de incluir 'GL\glew.h', sempre que se usa GLEW como uma biblioteca estática.
#include <GL/glew.h> // Certifique-se de que a biblioteca GLEW está instalada e configurada corretamente no seu sistema.
//--------------------------------------------------------------
#define GLFW_USE_DWM_SWAP_INTERVAL // Definição necessária para usar o intervalo de troca do DWM (Desktop Window Manager) no GLFW.
#include <GLFW/glfw3.h> // Certifique-se de que a biblioteca GLFW está instalada e configurada corretamente no seu sistema.


namespace game_engine_p3d {

	class WindowSystem {
	public:
		// ------------------------------------------------------------
		// Construtores e destrutores
		// ------------------------------------------------------------
		WindowSystem() = default;								// Construtor padrão (necessário para a instanciação da classe Game).
		WindowSystem(int width, int height, const char* title);	// Construtor, onde se inicializa o sistema de entrada.
		// ------------------------------------------------------------
		// Accessors e mutators
		// ------------------------------------------------------------
		GLFWwindow* window() const { return window_; }		// Retorna o apontador para a janela GLFW, que pode ser usado para outras operações.
		// ------------------------------------------------------------
		// Outras funções-membro
		// ------------------------------------------------------------
		void ProcessInput();			// Processa a entrada do utilizador (utilizar no loop do jogo).
		void ProcessInputWait();		// Processa a entrada do utilizador e espera por eventos (utilizar quando o estado do jogo for de pausa).
		void Finalize();				// Finaliza o sistema de entrada, destruindo a janela e liberando recursos.
		// ------------------------------------------------------------
		// Wrapper para criar uma interface unificada para as funções de entrada do GLFW
		// ------------------------------------------------------------
		static bool GetKey(int key);						// Leitura do estado de uma tecla.
		static bool GetMouseButton(int button);				// Leitura do estado de um botão do rato.
		static std::pair<float, float> GetMousePosition();	// Leitura da posição do rato na janela.

	private:
		static GLFWwindow* window_;
	};
}