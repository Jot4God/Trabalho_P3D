#include <iostream>
#include <array>

#include "Game.h"
#include "Camera.h"
#include "Light.h"
#include "Object.h"
#include "Shader.h"
#include "Renderer.h" // Inclui a classe 'Renderer', que define o renderizador a associar a cada objeto do jogo~~
#include "BallMovement.h"

// ------------------------------------------------------------
// É aqui que se incluem as suas classes de comportamento personalizados
// ------------------------------------------------------------
#include "Oscilator.h" // Inclui a classe 'Oscilator', que define o comportamento do objeto oscilador


// Usar um namespace mais curto para facilitar a escrita do código, e.g., 'gep3d::Game' em vez de 'game_engine_p3d::Game'
namespace gep3d = game_engine_p3d;


int main() {
	//using namespace gep3d;

	// NOTA: Ao definir para PT as definições regionais, alteramos a forma como o programa interpreta os números decimais (e.g., o separador decimal passa a ser a vírgula ',' em vez do ponto '.').
	//       Tal pode causar problemas na leitura de ficheiros de texto que contenham números decimais, como os shaders ou os modelos 3D (ficheiros .obj e outros).
	//       Por exemplo, se um shader ou modelo 3D contiver um número decimal como '0.5', o programa pode interpretar isso como '0,5' e não conseguir ler corretamente o valor, levando a erros de compilação do shader ou de carregamento do modelo.
	//       Assim, nas funções que realizam a leitura de ficheiros de texto que contenham números decimais, é importante garantir que o programa esteja a utilizar a locale correta.
	//       Devemos guardar a locale que estamos a utilizar no programa, e definir explicitamente a locale para "C" ou "en_US.UTF-8" (que usam o ponto como separador decimal) antes de ler os ficheiros de texto, e depois restaurar a locale original do programa.
	// Definições regionais (locale)
	try {
		// locale para português de Portugal
#ifdef __linux__
		std::locale::global(std::locale("pt_PT"));
#else
		std::locale::global(std::locale("pt-PT"));
#endif
	}
	catch (const std::exception& e) {
		std::cerr << "Erro ao definir locale: " << e.what() << std::endl;
	}

	// --------------------------------------------------
	// Cria uma instância do jogo
	// --------------------------------------------------
	gep3d::Game game(1280, 720);

	// --------------------------------------------------
	// Preparação da(s) câmara(s)
	// --------------------------------------------------
	// Instancia a câmara
	gep3d::Camera camera;
	// Define a cor de fundo da câmara
	camera.set_background_color(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
	// Define a posição da câmara e o ponto de vista
	camera.LookAt(
		glm::vec3(10.0f, 4.0f, 12.0f),
		glm::vec3(0.0f, -2.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	// Define a projeção perspetiva ºda câmara
	camera.Prespective(15.0f, static_cast<float>(game.width()) / game.height(), 0.1f, 100.0f);
	//camera.Orthographic(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f); // Define a projeção ortográfica
	// Define a viewport da câmara
	camera.Viewport(game.width(), game.height(), 0, 0);
	// Adiciona layers à máscara de culling da câmara
	std::array<std::string, 3> layers = { "Default", "Environment", "UI" };
	for (const auto& layer : layers) {
		camera.AddLayerToCullingMask(layer); // Adiciona a layer à máscara de culling da câmara
	}

	// Adiciona a câmara ao jogo (a primeira câmara adicionada é considerada a "Main Camera")
	game.AddCamera(&camera);

	// --------------------------------------------------
	// Preparação da(s) luzes(es)
	// --------------------------------------------------
	// Luz ambiente global fraca.
// Serve só para as zonas em sombra não ficarem completamente pretas.
	gep3d::Light* ambient_light = new gep3d::Light(
		glm::vec3(0.15f, 0.15f, 0.15f) // Cor da luz ambiente, um cinzento escuro para criar um ambiente mais realista, sem iluminar tudo como uma luz global forte.
	);

	// Luz direcional suave, tipo luz geral da sala.
	// A direção aponta de cima para baixo e ligeiramente de lado.
	gep3d::Light* directional_light = new gep3d::Light(
		glm::vec3(2.0f, -2.0f, -1.0f),	// Direção da luz
		glm::vec3(1.0f, 1.0f, 1.0f),    // Componente ambiente
		glm::vec3(1.0f, 1.0f, 1.0f),    // Componente difusa
		glm::vec3(1.0f, 1.0f, 1.0f)     // Componente especular
	);

	// Luz pontual por cima da mesa.
	// Como a mesa está em y = -2, esta luz fica bastante acima dela.
	gep3d::Light* point_light = new gep3d::Light(
		glm::vec3(-1.2f, 1.8f, 0.6f),     // Posição: acima da mesa e ligeiramente de lado
		glm::vec3(0.08f, 0.08f, 0.07f),   // Ambiente fraco
		glm::vec3(1.00f, 0.92f, 0.75f),   // Difusa quente, tipo lâmpada
		glm::vec3(1.00f, 0.95f, 0.85f),   // Brilho quente nas bolas
		1.0f,                             // Constante
		0.12f,                            // Linear
		0.045f                            // Quadrática
	);

	// Luz cónica tipo candeeiro de bilhar.
	// Fica por cima da mesa e aponta diretamente para baixo.
	gep3d::Light* spot_light = new gep3d::Light(
		glm::vec3(0.0f, 2.0f, 0.0f),      // Posição: por cima do centro da mesa
		glm::vec3(0.0f, -1.0f, 0.0f),     // Direção: aponta para baixo, para a mesa

		glm::vec3(0.0f, 0.0f, 0.0f),      // Ambiente: 0 para não iluminar tudo como luz global
		glm::vec3(1.0f, 0.92f, 0.75f),    // Difusa: luz quente tipo candeeiro
		glm::vec3(1.0f, 0.95f, 0.85f),    // Especular: brilho nas bolas

		1.0f,                             // Constante de atenuação
		0.09f,                            // Linear de atenuação
		0.025f,                           // Quadrática de atenuação

		25.0f,                            // CutOff: cone interior
		40.0f                             // OuterCutOff: cone exterior mais aberto
	);
	ambient_light->set_enabled(true);
	directional_light->set_enabled(false);
	point_light->set_enabled(false);
	spot_light->set_enabled(false);

	// Adiciona a luz ambiente ao jogo
	game.AddLight(ambient_light);
	// Adiciona a luz direcional ao jogo
	game.AddLight(directional_light);
	// Adiciona a luz pontual ao jogo
	game.AddLight(point_light);
	// Adiciona a luz cónica ao jogo
	game.AddLight(spot_light);

	// --------------------------------------------------
	// Preparação do(s) programa(s) shader
	// --------------------------------------------------
	// Para um programa shader, indica os tipos de shaders que serão usados e os respetivos caminhos para os ficheiros de código shader
	std::vector<ShaderSource> sources = {
		{GL_VERTEX_SHADER, "light.vert" /*"default_shader.vert"*/},
		{GL_FRAGMENT_SHADER, "light.frag" /*"default_shader.frag"*/}
	};
	// Cria o programa shader (lê e compila os shaders de um programa shader, a partir dos ficheiros especificados em 'sources')
	// O nome do shader é opcional, mas pode ser útil para identificação
	Shader* shader = new Shader(sources, "DefaultShader");

	// --------------------------------------------------
	// Preparação do(s) renderizador(es)
	// --------------------------------------------------
	// Cria o renderizador com o shader especificado e o caminho do modelo 3D (ficheiro OBJ)
	Renderer* tablerenderer = new Renderer(shader, "table.obj");
	Renderer* ballrenderer1 = new Renderer(shader, "Ball1.obj");
	Renderer* ballrenderer2 = new Renderer(shader, "Ball2.obj");
	Renderer* ballrenderer3 = new Renderer(shader, "Ball3.obj");
	Renderer* ballrenderer4 = new Renderer(shader, "Ball4.obj");
	Renderer* ballrenderer5 = new Renderer(shader, "Ball5.obj");
	Renderer* ballrenderer6 = new Renderer(shader, "Ball6.obj");
	Renderer* ballrenderer7 = new Renderer(shader, "Ball7.obj");
	Renderer* ballrenderer8 = new Renderer(shader, "Ball8.obj");
	Renderer* ballrenderer9 = new Renderer(shader, "Ball9.obj");
	Renderer* ballrenderer10 = new Renderer(shader, "Ball10.obj");
	Renderer* ballrenderer11 = new Renderer(shader, "Ball11.obj");
	Renderer* ballrenderer12 = new Renderer(shader, "Ball12.obj");
	Renderer* ballrenderer13 = new Renderer(shader, "Ball13.obj");
	Renderer* ballrenderer14 = new Renderer(shader, "Ball14.obj");
	Renderer* ballrenderer15 = new Renderer(shader, "Ball15.obj");


	// --------------------------------------------------
	// Preparação do(s) comportamento(s) do(s) objeto(s)
	// --------------------------------------------------
	// Cria uma instância do comportamento Oscilator
	Oscilator* oscilator = new Oscilator();
	BallMovement* ballMovement = new BallMovement();

	// --------------------------------------------------
	// Preparação do(s) objeto(s) do jogo
	// --------------------------------------------------
	// Instancia um objeto do jogo
	// Cria um objeto com nome "Objecto (1)" e layer padrão ("" = "Default")
	// Atribui um comportamento 'oscilator' ao objeto, que será executado no ciclo de atualização do jogo
	// Atribui um renderizador ao objeto, que será usado para renderizar o objeto no jogo
	// Define a posição do objeto como (0, -4, 0), no sistema de coordenadas local, com orientação e escala padrão
	gep3d::Object* table = new gep3d::Object("table", "", nullptr, tablerenderer, 0.0f, -2.0f, 0.0f);

	// FILA 1
	gep3d::Object* ball1 = new gep3d::Object("Ball1", "", ballMovement, ballrenderer1, -0.8f, -1.90f, -0.18f);

	// FILA 2
	gep3d::Object* ball2 = new gep3d::Object("Ball2", "", nullptr, ballrenderer2, -0.4f, -1.90f, -0.38f);
	gep3d::Object* ball3 = new gep3d::Object("Ball3", "", nullptr, ballrenderer3, -0.4f, -1.90f, 0.02f);

	// FILA 3
	gep3d::Object* ball4 = new gep3d::Object("Ball4", "", nullptr, ballrenderer4, 0.0f, -1.90f, -0.58f);
	gep3d::Object* ball5 = new gep3d::Object("Ball5", "", nullptr, ballrenderer5, 0.0f, -1.90f, -0.18f);
	gep3d::Object* ball6 = new gep3d::Object("Ball6", "", nullptr, ballrenderer6, 0.0f, -1.90f, 0.22f);

	// FILA 4
	gep3d::Object* ball7 = new gep3d::Object("Ball7", "", nullptr, ballrenderer7, 0.4f, -1.90f, -0.78f);
	gep3d::Object* ball8 = new gep3d::Object("Ball8", "", nullptr, ballrenderer8, 0.4f, -1.90f, -0.38f);
	gep3d::Object* ball9 = new gep3d::Object("Ball9", "", nullptr, ballrenderer9, 0.4f, -1.90f, 0.02f);
	gep3d::Object* ball10 = new gep3d::Object("Ball10", "", nullptr, ballrenderer10, 0.4f, -1.90f, 0.42f);

	// FILA 5
	gep3d::Object* ball11 = new gep3d::Object("Ball11", "", nullptr, ballrenderer11, 0.8f, -1.90f, -0.98f);
	gep3d::Object* ball12 = new gep3d::Object("Ball12", "", nullptr, ballrenderer12, 0.8f, -1.90f, -0.58f);
	gep3d::Object* ball13 = new gep3d::Object("Ball13", "", nullptr, ballrenderer13, 0.8f, -1.90f, -0.18f);
	gep3d::Object* ball14 = new gep3d::Object("Ball14", "", nullptr, ballrenderer14, 0.8f, -1.90f, 0.22f);
	gep3d::Object* ball15 = new gep3d::Object("Ball15", "", nullptr, ballrenderer15, 0.8f, -1.90f, 0.62f);

	// Tranformar o tamanho  das bolas 
	ball1->model().Scale(0.1f, 0.1f, 0.1f);
	ball2->model().Scale(0.1f, 0.1f, 0.1f);
	ball3->model().Scale(0.1f, 0.1f, 0.1f);
	ball4->model().Scale(0.1f, 0.1f, 0.1f);
	ball5->model().Scale(0.1f, 0.1f, 0.1f);
	ball6->model().Scale(0.1f, 0.1f, 0.1f);
	ball7->model().Scale(0.1f, 0.1f, 0.1f);
	ball8->model().Scale(0.1f, 0.1f, 0.1f);
	ball9->model().Scale(0.1f, 0.1f, 0.1f);
	ball10->model().Scale(0.1f, 0.1f, 0.1f);
	ball11->model().Scale(0.1f, 0.1f, 0.1f);
	ball12->model().Scale(0.1f, 0.1f, 0.1f);
	ball13->model().Scale(0.1f, 0.1f, 0.1f);
	ball14->model().Scale(0.1f, 0.1f, 0.1f);
	ball15->model().Scale(0.1f, 0.1f, 0.1f);

	// Cria um segundo objeto com nome "Objecto (2)" e layer padrão, sem comportamento, mas com o mesmo renderizador do primeiro objeto, e posiciona-o em (0, -2, 0)

	LOG("Object created with ID: " << table->id() << " at position: (0, -2, 0).");
	LOG("Object created with ID: " << ball1->id() << " at position: (-2, -1.75, 0).");

	// --------------------------------------------------
	// Adiciona o(s) objeto(s) ao jogo
	// --------------------------------------------------
	game.AddObject(table);
	game.AddObject(ball1);
	game.AddObject(ball2);
	game.AddObject(ball3);
	game.AddObject(ball4);
	game.AddObject(ball5);
	game.AddObject(ball6);
	game.AddObject(ball7);
	game.AddObject(ball8);
	game.AddObject(ball9);
	game.AddObject(ball10);
	game.AddObject(ball11);
	game.AddObject(ball12);
	game.AddObject(ball13);
	game.AddObject(ball14);
	game.AddObject(ball15);

	// --------------------------------------------------
	// Inicia o loop do jogo
	// --------------------------------------------------
	game.Run();

	// --------------------------------------------------
	// Liberta a memória alocada para os recursos do jogo
	// --------------------------------------------------
	delete shader;		// Liberta a memória alocada para o shader	
	delete tablerenderer; // Liberta a memória alocada para o renderizador
	delete ballrenderer1;
	delete oscilator;	// Liberta a memória alocada para o comportamento
	delete ballMovement;
	delete table;	// Liberta a memória alocada para o objeto
	// Liberta a memória alocada para o objeto

	LOG("Exit!");

	return 0;
}