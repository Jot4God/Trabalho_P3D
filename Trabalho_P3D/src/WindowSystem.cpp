#include "Common.h"
#include "WindowSystem.h"


namespace game_engine_p3d {

	// Declara e inicializa o apontador estático para a janela GLFW
	GLFWwindow* WindowSystem::window_ = nullptr;


	WindowSystem::WindowSystem(int width, int height, const char* title) {
		LOG("Initializing WindowSystem...");

		// Exibe uma mensagem de log com a versão do GLFW
		LOG("GLFW version: " << glfwGetVersionString());

		// Regista a função de callback para gestão de erros do GLFW
		glfwSetErrorCallback([](int error, const char* description) {
			LOG("GLFW Error: " << error << " - " << description);
			});

		// Inicializa a biblioteca GLFW
		// Garantir que a biblioteca GLFW é inicializada antes da biblioteca GLEW
		if (!glfwInit()) {
			LOG("Failed to initialize GLFW.");
			return;
		}

		// ------------------------------------------------
		// CONFIGURAÇÃO ESPECÍFICA PARA VERSÕES OPENGL
		// ------------------------------------------------
		LOG("Configuring OpenGL context for platform...");
		LOG("OpenGL version: " << kOpenGLMajorVersion << "." << kOpenGLMinorVersion);

		// Define a versão mínima do OpenGL
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, kOpenGLMajorVersion);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, kOpenGLMinorVersion);
#ifdef __APPLE__
		// Configurações específicas baseadas na plataforma
		if (kRequiresCoreProfile) {
			LOG("Using Core Profile (required for macOS)");
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}

		if (kRequiresForwardCompat) {
			LOG("Enabling Forward Compatibility (required for macOS)");
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
		}
#endif

		// Cria uma janela GLFW
		window_ = glfwCreateWindow(
			width, height, // Define a resolução da janela
			title, // Define o título da janela
			nullptr, // Define funcionamento em modo janela (não em modo fullscreen)
			nullptr // Não partilha contexto com outra janela
		);
		if (!window_) {
			LOG("Failed to create GLFW window.");
			glfwTerminate();
			return;
		}

		// Define a janela atual como a janela de contexto
		glfwMakeContextCurrent(window_);

		// Define o apontador de utilizador da janela (apontador de uso genérico) para a instância atual de Input
		// Necessário para que a função de callback de teclado possa aceder os dados da instância de Input
		glfwSetWindowUserPointer(window_, this);

		// Define o cursor do rato como visível e normal
		glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Permite que as teclas mantenham o estado pressionado mesmo após serem soltas
		glfwSetInputMode(window_, GLFW_STICKY_KEYS, GLFW_TRUE);
		// Permite que os botões do rato mantenham o estado pressionado mesmo após serem soltos
		glfwSetInputMode(window_, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

		// Regista a função de callback para eventos de teclado
		// NOTE: Para simplificar o código, não vamos implementar a função de callback de teclado aqui.
		// Vamos recorrer apenas às funções de entrada do GLFW para verificar o estado das teclas, tais como glfwGetKey().
		//glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode, int action, int mods) {/*...*/});

		LOG("WindowSystem initialized.");
	}

	void WindowSystem::Finalize() {
		if (window_) {
			LOG("Finalizing WindowSystem...");
			glfwDestroyWindow(window_);
			window_ = nullptr;
		}
		glfwTerminate();
		LOG("WindowSystem finalized.");
	}

	void WindowSystem::ProcessInput() {
		LOG("Processing input...");

		// Processa eventos da janela GLFW, atualizando o estado das teclas e do rato
		glfwPollEvents();
	}

	void WindowSystem::ProcessInputWait() {
		LOG("Processing input with wait...");
		// Processa eventos da janela GLFW e espera por eventos
		// A execução deste método bloqueia até que haja eventos disponíveis, como teclas pressionadas ou movimento do rato
		// Atualiza o estado das teclas e do rato
		glfwWaitEvents();
	}

	// Retorna 'true' se a tecla foi (está) pressionada e 'false' caso contrário
	bool WindowSystem::GetKey(int key) {
		if (glfwGetKey(window_, key) == GLFW_PRESS) {
			LOG("***** Key " << key << " is pressed.");
			return true; // A tecla foi (ou está) pressionada
		}
		else {
			LOG("***** Key " << key << " is not pressed.");
			return false; // A tecla não foi (ou não está) pressionada
		}
	}

	// Retorna 'true' se o botão do rato foi (está) pressionado e 'false' caso contrário
	bool WindowSystem::GetMouseButton(int button) {
		if (glfwGetMouseButton(window_, button) == GLFW_PRESS) {
			LOG("***** Mouse button " << button << " is pressed.");
			return true; // O botão do rato foi (ou está) pressionado
		}
		else {
			LOG("***** Mouse button " << button << " is not pressed.");
			return false; // O botão do rato não foi (ou não está) pressionado
		}
	}

	// Retorna a posição atual do rato na janela como um par de coordenadas (x, y)
	std::pair<float, float> WindowSystem::GetMousePosition() {
		double xpos, ypos;
		glfwGetCursorPos(window_, &xpos, &ypos);
		LOG("***** Mouse position: (" << xpos << ", " << ypos << ")");
		return std::make_pair<float, float>(static_cast<float>(xpos), static_cast<float>(ypos));
	}
}