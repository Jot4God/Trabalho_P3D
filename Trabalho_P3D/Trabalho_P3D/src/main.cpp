#include <iostream>
#include <array>

#include "Game.h"
#include "Camera.h"
#include "Light.h"
#include "Object.h"
#include "Shader.h"
#include "Renderer.h" // Inclui a classe 'Renderer', que define o renderizador a associar a cada objeto do jogo

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
	// Define a projeção perspetiva da câmara
	camera.Prespective(10.0f, static_cast<float>(game.width()) / game.height(), 0.1f, 100.0f);
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
	// Cria uma luz ambiente
	gep3d::Light* ambient_light = new gep3d::Light(glm::vec3(0.1f, 0.1f, 0.1f)); // Cor da luz ambiente
	// Cria uma luz direcional (ex: sol)
	gep3d::Light* directional_light = new gep3d::Light(
		glm::vec3(0.0f, 0.0f, -1.0f),	// Direção da luz
		glm::vec3(1.0f, 1.0f, 1.0f),    // Componente ambiente
		glm::vec3(1.0f, 1.0f, 1.0f),    // Componente difusa
		glm::vec3(1.0f, 1.0f, 1.0f)     // Componente especular
	);
	// Cria uma luz pontual (ex: lâmpada)
	gep3d::Light* point_light = new gep3d::Light(
		glm::vec3(0.0f, 0.0f, -1.0f),	// Posição da luz
		glm::vec3(1.0f, 1.0f, 1.0f),    // Componente ambiente
		glm::vec3(1.0f, 1.0f, 1.0f),    // Componente difusa
		glm::vec3(1.0f, 1.0f, 1.0f),    // Componente especular
		1.0f,                           // Constante de atenuação
		0.09f,                          // Linear de atenuação
		0.032f                          // Quadrática de atenuação
	);
	// Cria uma luz cónica (spotlight)
	gep3d::Light* spot_light = new gep3d::Light(
		glm::vec3(0.0f, 0.0f, -1.0f),  // Posição da luz
		glm::vec3(0.0f, 0.0f, -1.0f),   // Direção da luz
		glm::vec3(0.0f, 0.0f, 0.0f),    // Componente ambiente
		glm::vec3(1.0f, 1.0f, 1.0f),    // Componente difusa
		glm::vec3(1.0f, 1.0f, 1.0f),    // Componente especular
		1.0f,                           // Constante de atenuação
		0.09f,                          // Linear de atenuação
		0.032f,                         // Quadrática de atenuação
		10.0f,							// CutOff (ângulo interno, em graus)
		20.0f							// OuterCutOff (ângulo externo, em graus)
	);
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
	Renderer* renderer = new Renderer(shader, "table.obj");

	// --------------------------------------------------
	// Preparação do(s) comportamento(s) do(s) objeto(s)
	// --------------------------------------------------
	// Cria uma instância do comportamento Oscilator
	Oscilator* oscilator = new Oscilator();

	// --------------------------------------------------
	// Preparação do(s) objeto(s) do jogo
	// --------------------------------------------------
	// Instancia um objeto do jogo
	// Cria um objeto com nome "Objecto (1)" e layer padrão ("" = "Default")
	// Atribui um comportamento 'oscilator' ao objeto, que será executado no ciclo de atualização do jogo
	// Atribui um renderizador ao objeto, que será usado para renderizar o objeto no jogo
	// Define a posição do objeto como (0, -4, 0), no sistema de coordenadas local, com orientação e escala padrão
	gep3d::Object* table = new gep3d::Object("table", "", nullptr, renderer, 0.0f, -2.0f, 0.0f);
	// Cria um segundo objeto com nome "Objecto (2)" e layer padrão, sem comportamento, mas com o mesmo renderizador do primeiro objeto, e posiciona-o em (0, -2, 0)

	LOG("Object created with ID: " << table->id() << " at position: (0, -2, 0).");
	

	// --------------------------------------------------
	// Adiciona o(s) objeto(s) ao jogo
	// --------------------------------------------------
	game.AddObject(table);
	

	// --------------------------------------------------
	// Inicia o loop do jogo
	// --------------------------------------------------
	game.Run();

	// --------------------------------------------------
	// Liberta a memória alocada para os recursos do jogo
	// --------------------------------------------------
	delete shader;		// Liberta a memória alocada para o shader
	delete renderer;	// Liberta a memória alocada para o renderizador
	delete oscilator;	// Liberta a memória alocada para o comportamento
	delete table;	// Liberta a memória alocada para o objeto
		// Liberta a memória alocada para o objeto

	LOG("Exit!");

	return 0;
}