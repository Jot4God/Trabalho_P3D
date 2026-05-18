#include <vector>
#include <thread> // std::this_thread::sleep_for()
#include <chrono> // std::chrono::duration<>()
#include <tuple>  // std::tie()

#include "Common.h" // Para definições comuns e macros
#include "Game.h"
#include <GLFW/glfw3.h> 


namespace game_engine_p3d {

	Game::Game(unsigned int width, unsigned int height, const char* title)
		: width_{ width }, height_{ height }, title_{ title }, state_{ GameState::kGameStateInitialization } {
		LOG("Game initialization will proceed using the following parameters:");
		LOG("  Game version: " << kGameVersion);
		LOG("  Debug mode: " << (kDebugMode ? "Enabled" : "Disabled"));
		LOG("  Max cameras allowed: " << kMaxCameras);
		LOG("  Max objects allowed: " << kMaxObjects);
		LOG("  Max FPS: " << kMaxFPS);
		LOG("  Window dimensions: " << width_ << "x" << height_);
		LOG("  Asset path: '" << kAssetPath << "'");
		LOG("  Shader path: '" << kShaderPath << "'");
		LOG("  Physics gravity: " << kPhysicsGravity);
		LOG("  Physics time step: " << kPhysicsTimeStep);
		LOG("  Max collision objects: " << kMaxCollisionObjects);
		LOG("  Max trigger objects: " << kMaxTriggerObjects);

		// Aqui inicializa as bibliotecas GLFW e GLEW
		// GLFW -> Cria e configura o contexto OpenGL, o sistema de entrada, a janela da aplicação, etc.
		// GLEW -> Permite usar as extensões do OpenGL Moderno
		LOG("Initializing libraries...");
		{
			// Inicializa a biblioteca GLFW (i.e., o sistema gestão de janelas e de entrada).
			window_system_ = WindowSystem(width_, height_, title_);

			// Inicializa o GLEW (só o posso fazer uma vez e apenas depois de criar uma janela GLFW)
			// Para utilizar as funções do OpenGL Moderno é necessário inicializar o GLEW
			// Caso contrário, funções como glCreateProgram, glCreateShader, etc. não estarão disponíveis
			// Nota que glewInit() deve ser executado após a criação e vinculação (binding) de um contexto OpenGL válido.
			// Ou seja, primeiro inicializa-se o GLFW, cria-se a janela e chama-se glfwMakeContextCurrent(window). Só então se pode chamar glewInit().
			// Se glewInit() for chamado antes de vincular o contexto, ele não conseguirá carregar as extensões do OpenGL corretamente, resultando em falhas ou comportamentos inesperados.
			glewExperimental = GL_TRUE;
			LOG("Initializing GLEW...");
			if (glewInit() != GLEW_OK) {
				LOG("GLEW initialization failed.");
				return;
			}
			LOG("GLEW initialized successfully.");

			LOG("Libraries initialization complete.");
		}

		LOG("Game instance created successfully.");
	}

	Game::~Game() {
		LOG("Game instance is being destroyed...");

		// Finaliza o sistema de entrada, liberando recursos alocados
		window_system_.Finalize();

		LOG("Game instance destroyed.");
	}

	void Game::AddCamera(Camera* camera) {
		// Verifica se o número máximo de câmaras foi atingido
		if (cameras_.size() >= kMaxCameras) {
			LOG("Error: Maximum number of cameras reached.");
			return;
		}

		// Se for a primeira câmara, indica que se trata da câmara principal
		if (cameras_.empty()) {
			LOG("Setting the first camera as the Main Camera.");
		}

		// Adiciona a câmara ao vetor de câmaras do jogo
		cameras_.push_back(camera);
		LOG("Camera " << cameras_.size() << " added successfully.");
	}

	void Game::AddLight(Light* light) {
		if (lights_.size() >= kMaxLights) {
			LOG("Error: Maximum number of lights reached.");
			return;
		}

		// Aqui adiciona a luz ao vetor de luzes do jogo
		lights_.push_back(light);
		LOG("Light " << lights_.size() << " added successfully.");
	}

	void Game::AddObject(Object* object) {
		if (objects_.size() >= kMaxObjects) {
			LOG("Error: Maximum number of objects reached.");
			return;
		}
		LOG("Adding object with ID: " << object->id());
		// Verifica se o objeto já existe
		for (const auto& obj : objects_) {
			if (obj->id() == object->id()) {
				LOG("Error: Object with ID " << object->id() << " already exists.");
				return;
			}
		}
		// Se não existir, adiciona o objeto ao vetor
		LOG("Object with ID " << object->id() << " added successfully to the game.");
		// Associa a instância do jogo ao objeto
		object->set_game(this);
		// Adiciona o objeto ao vetor de objetos
		objects_.push_back(object);
	}

	Object* Game::FindObjectByName(const std::string& name) {
		for (auto object : objects_) {
			if (object->name() == name) {
				return object; // Retorna o ponteiro para o objeto encontrado
			}
		}
		return nullptr; // Retorna nullptr se o objeto não for encontrado
	}
	//novo
	void Game::ProcessLightInput() {
		GLFWwindow* window = window_system_.window();

		auto ToggleLightByType = [this](LightType type) {
			for (auto light : lights_) {
				if (light != nullptr && light->type() == type) {
					light->Toggle();

					std::cout << light->type_string()
						<< " light: "
						<< (light->enabled() ? "ON" : "OFF")
						<< std::endl;
				}
			}
			};

		bool key1 = glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS;
		bool key2 = glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS;
		bool key3 = glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS;
		bool key4 = glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS;

		if (key1 && !key_1_was_pressed_) {
			ToggleLightByType(LightType::kAmbient);
		}

		if (key2 && !key_2_was_pressed_) {
			ToggleLightByType(LightType::kDirectional);
		}

		if (key3 && !key_3_was_pressed_) {
			ToggleLightByType(LightType::kPoint);
		}

		if (key4 && !key_4_was_pressed_) {
			ToggleLightByType(LightType::kSpotlight);
		}

		key_1_was_pressed_ = key1;
		key_2_was_pressed_ = key2;
		key_3_was_pressed_ = key3;
		key_4_was_pressed_ = key4;
	}
	void Game::Run() {
		while (state_ != GameState::kGameStateShutdown) {
			switch (state_) {
			case GameState::kGameStateInitialization:
				LOG("Game is loading...");
				state_ = GameState::kGameStateGameLoop;

				// Aqui carrega recursos, inicializa objetos, etc.

				LOG("Game loading complete.");

				break;
			case GameState::kGameStateGameLoop:
				//LOG("Game is running...");
				LOG("Rendering frame " << frameCount_);

				// Aqui adiciona a lógica para atualizar o jogo, processar entrada, renderizar, etc.

				// 1. Processa a entrada do utilizador
				window_system_.ProcessInput();
				//novo
				ProcessLightInput();

				// 2. Atualização do jogo (comportamentos, física, etc.)
				{
					for (auto object : objects_) {
						// Atualiza cada objeto
						object->Update();
					}
					for (auto object : objects_) {
						// Atualiza cada objeto com física
						object->FixedUpdate();
					}
					for (auto object : objects_) {
						// Atualiza a física do objeto
						object->PhysicsUpdate();
					}
					for (auto object : objects_) {
						// Atualiza cada objeto na fase final
						object->LateUpdate();
					}
					for (auto object : objects_) {
						// Simula colisões e gatilhos
						object->OnCollisionEnter(*object);	// Exemplo de colisão
						object->OnTriggerEnter(*object);	// Exemplo de gatilho
					}
					for (auto object : objects_) {
						// Simula saída de colisões e gatilhos
						object->OnCollisionExit(*object);	// Exemplo de saída de colisão
						object->OnTriggerExit(*object);		// Exemplo de saída de gatilho
					}
				}

				// 3. Renderização
				{
					// Limpa todo o buffer de cor (Color Buffer) com uma cor de fundo
					// Nota que para a limpeza do buffer de cor usamos os parâmetros da primeira câmara (índice 0), isto é, a "Main Camera"
					float red = cameras_.empty() ? 0.0f : cameras_[0]->background_color().r;
					float green = cameras_.empty() ? 0.0f : cameras_[0]->background_color().g;
					float blue = cameras_.empty() ? 0.0f : cameras_[0]->background_color().b;
					float alpha = cameras_.empty() ? 1.0f : cameras_[0]->background_color().a;
					// Limpa todo o ecrã (isto é, todo o Color Buffer) com uma cor de fundo definida pela "Main Camera"
					glClearBufferfv(GL_COLOR, 0, glm::value_ptr(glm::vec4(red, green, blue, alpha)));
					// Em alternantiva, se quiser usar 'glClearColor()' e 'glClear()':
					/*glClearColor(red, green, blue, alpha); // Define a cor de fundo
					glClear(GL_COLOR_BUFFER_BIT); // Limpa o ecrã (com uma cor de fundo)*/

					// Para cada câmara, renderiza os objetos do jogo
					for (int camera_index = 0; camera_index < cameras_.size(); ++camera_index)
					{
						LOG("Rendering with Camera " << camera_index);

						// Para as restantes câmaras (i.e., não a "Main Camera"), usa-se o Scissor Test para apagar apenas a região da viewport da câmara atual, e não o ecrã inteiro.
						if (camera_index > 0) {
							// Ativa o Scissor Test
							glEnable(GL_SCISSOR_TEST);
							int viewport_width, viewport_height, viewport_x, viewport_y;
							// Obtém os valores da viewport da câmara atual usando a função GetViewport() da classe 'Camera'
							std::tie(viewport_width, viewport_height, viewport_x, viewport_y) = cameras_[camera_index]->GetViewport();
							// Define a região do Scissor Test com os valores da viewport da câmara atual
							glScissor(viewport_x, viewport_y, viewport_width, viewport_height);
							// Define a cor de fundo da câmara atual para a região do Scissor Test
							red = cameras_[camera_index]->background_color().r;
							green = cameras_[camera_index]->background_color().g;
							blue = cameras_[camera_index]->background_color().b;
							alpha = cameras_[camera_index]->background_color().a;
							// Limpa a região da câmara atual (com a cor de fundo definida)
							glClearBufferfv(GL_COLOR, 0, glm::value_ptr(glm::vec4(red, green, blue, alpha)));
							// Desativa o Scissor Test para as próximas operações de renderização
							glDisable(GL_SCISSOR_TEST);
						}

						// Para cada layer na máscara de culling da câmara
						for (const auto& layer : cameras_[camera_index]->culling_mask()) {
							LOG("  Rendering layer: " << layer);

							// O buffer de profundidade é limpo (com valor 1.0f, que é o máximo)
							static const GLfloat one[] = { 1.0f };
							glClearBufferfv(GL_DEPTH, 0, one);
							// Em alternativa, se quiser usar 'glClear()':
							/*glClear(GL_DEPTH_BUFFER_BIT);*/

							// ****************************************************
							// Outros parâmetros do OpenGL
							// ****************************************************
							//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
							glEnable(GL_DEPTH_TEST);
							glEnable(GL_CULL_FACE); // Ativa o culling de faces dos poligonos (por defeito está desativado)
							glCullFace(GL_BACK); // "Culla" (i.e., não renderiza) as faces traseiras. Opções possíveis: GL_FRONT, GL_BACK, GL_FRONT_AND_BACK

							// Define a viewport da câmara atual
							cameras_[camera_index]->Viewport(); // Aplica para a viewport os valores atribuídos aos dados-membro da classe Camera

							// Para cada objecto
							for (auto object : objects_) {
								// Verifica se o objeto está na layer que a câmara deve renderizar
								// Isto faz com que apenas os objetos nas layers especificadas sejam renderizados
								if (object->layer() == layer) {
									// Chama a função de renderização do objeto
									// Passa a câmara atual para o método Draw do objeto, para que ele possa usar a matriz de visualização (View) e projeção (Projection) da câmara
									// Passa também a lista de luzes do jogo, para que o objeto possa usar as luzes na sua renderização
									object->Draw(*cameras_[camera_index], lights_);
								}
							}

						}
					}
					// Troca os buffers do ecrã
					glfwSwapBuffers(window_system_.window());
				}

				LOG("Frame " << frameCount_ << " rendered successfully.");

				// 4. Controla a taxa de frames
				FrameRateControl(1.0 / kMaxFPS);

				break;
			default:
				LOG("Game is exiting...");
				state_ = GameState::kGameStateShutdown;

				// Lógica para salvar o estado do jogo (por exemplo, para ficheiro), etc.
				// Aqui...

				// Finaliza o sistema de entrada
				LOG("Game loop finished.");

				break;
			}

			// Verifica se a janela foi fechada ou se o utilizador pediu para sair do jogo (função da biblioteca GLFW)
			if (glfwWindowShouldClose(window_system_.window())) {
				LOG("Window close requested. Exiting game loop.");
				state_ = GameState::kGameStateShutdown;
			}
		}
		std::cout << "Game exiting..." << std::endl;
	}

	void Game::Exit() {
		LOG("Exiting game...");
		state_ = GameState::kGameStateShutdown;

		// Lógica para salvar o estado do jogo (por exemplo, para ficheiro), etc.
		// Aqui...
	}

	// Controla o framerate do jogo
	void Game::FrameRateControl(double fps) {
		// Calcula o tempo de espera necessário para manter a taxa de frames desejada
		// FIXME: Está a usar uma taxa de frames fixa, mas pode ser melhorado para permitir taxas de frames variáveis (com funções da biblioteca GLFW)
		double segundos = 1.0 / kMaxFPS; // Pausa por 1/kMaxFPS segundos
		std::this_thread::sleep_for(std::chrono::duration<double>(segundos));

		// Incrementa o contador de frames
		frameCount_++;
	}
}