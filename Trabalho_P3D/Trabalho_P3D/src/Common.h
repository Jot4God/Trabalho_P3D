#pragma once


// =============================================================
// Ficheiro:    Common.h
// Descrição:   Definições comuns e macros para o jogo
// Autor:       Duarte Duque
// Data:        21/07/2025
// Versão:      1.0.0
// Dependências: Não possui dependências externas
// Observações:
// Esta é uma coleção de definições, constantes e macros que podem ser usadas em todo o projeto.
// =============================================================


// -------------------------------------------------------------
// Indicação das bibliotecas necessárias para o projeto
// -------------------------------------------------------------
#ifdef _WIN32
// O comando #pragma comment é usado para informar ao linker quais bibliotecas devem ser vinculadas ao projeto.
#pragma comment(lib, "glew32s.lib")		// Biblioteca 'GLEW' para extensões OpenGL.
#pragma comment(lib, "glfw3.lib")		// Biblioteca 'GLFW' para manipulação de janelas e entrada.
#pragma comment(lib, "opengl32.lib")	// Biblioteca 'OpenGL' padrão do Windows SDK.
#elif __APPLE__
    // macOS - linking será feito via CMake ou Xcode.
    // OpenGL framework é incluído automaticamente.
    // GLFW e GLEW devem ser instalados via Homebrew ou vcpkg.
#elif __linux__
    // Linux - linking será feito via CMake ou Makefile.
#endif


// ------------------------------------------------------------
// Configurações específicas da plataforma
// ------------------------------------------------------------
#ifdef __APPLE__
    // macOS requer OpenGL Core Profile e Forward Compatibility.
    // No macOS, a versão máxima suportada do OpenGL é 4.1 (GLSL 4.10).
constexpr int kOpenGLMajorVersion = 4;
constexpr int kOpenGLMinorVersion = 1;
constexpr bool kRequiresCoreProfile = true;
constexpr bool kRequiresForwardCompat = true;
#else
    // Windows e Linux podem usar versões mais recentes.
constexpr int kOpenGLMajorVersion = 4;
constexpr int kOpenGLMinorVersion = 6;
constexpr bool kRequiresCoreProfile = false;
constexpr bool kRequiresForwardCompat = false;
#endif


// ------------------------------------------------------------
// Inclusão de cabeçalhos padrão do C++
// ------------------------------------------------------------
#include <iostream>


// ------------------------------------------------------------
// Configuração de debug (deve-se usar #define para funcionar com #if)
// ------------------------------------------------------------
#define kDebugMode 1								// Modo de depuração (1=ativado, 0=desativado).


// ------------------------------------------------------------
// Tipos de dados comuns
// ------------------------------------------------------------
using Layer = std::string;                          // Tipo para representar layers (camadas) de objetos no jogo.


// ------------------------------------------------------------
// Constantes do projeto
// ------------------------------------------------------------
// CONFIGURAÇÕES DO MOTOR DE JOGO
constexpr const char* kGameVersion = "1.0.0";		// Major.Minor.Patch
constexpr int kMaxFPS = 60;							// Taxa alvo de frames por segundo.
constexpr int kMaxCameras = 4;						// Número máximo de câmaras que podem ser adicionadas ao jogo.
constexpr int kMaxLights = 8;						// Número máximo de luzes que podem ser adicionadas ao jogo.
constexpr int kMaxObjects = 1000;					// Número máximo de objetos que podem ser criados no jogo.
// CONFIGURAÇÕES DE RENDERIZAÇÃO
constexpr int kWindowDefaultWidth = 1280;			// Largura padrão da janela do jogo.
constexpr int kWindowDefaultHeight = 720;			// Altura padrão da janela do jogo.
// CONFIGURAÇÕES DE RECURSOS
constexpr const char* kAssetPath = "assets/";		// Caminho base para os assets (texturas, modelos, etc.).
constexpr const char* kShaderPath = "shaders/";		// Caminho base para os shaders (ficheiros .vert e .frag).
// CONFIGURAÇÕES DE FÍSICA
constexpr float kPhysicsGravity = 9.81f;			// Aceleração da gravidade em m/s².
constexpr float kPhysicsTimeStep = 1.0f / kMaxFPS;	// 1/FPS em Segundos.
constexpr int kMaxCollisionObjects = 100;			// Número máximo de objetos que podem ser usados para colisões.
constexpr int kMaxTriggerObjects = 100;				// Número máximo de objetos que podem ser usados como gatilhos (triggers).


// ------------------------------------------------------------
// Macro para logging condicional
// ------------------------------------------------------------
#if kDebugMode
#include <iostream>
// Define a macro LOG que imprime mensagens de log com o nome da função atual.
// Nota que __func__ é uma macro padrão do C++ (C++11 e superior) que permite obter o nome da função atual.
#define LOG(message) std::cout << "[" << __func__ << "] -> " << message << std::endl
#else
#define LOG(message)  // LOG desativado em release.
#endif


// ------------------------------------------------------------
// Atributos padrão para os shaders
// ------------------------------------------------------------
// Nome de atributos padrão (hardcoded) nos shaders
constexpr const char* kVertexPositionName = "vPosition";    // Nome do atributo de posição dos vértices.
constexpr const char* kVertexNormalName = "vNormal";		// Nome do atributo de normal dos vértices.
constexpr const char* kVertexTexCoordName = "vTexture";     // Nome do atributo de coordenadas de textura dos vértices.
// Nome dos uniforms padrão (hardcoded) nos shaders
constexpr const char* kModelMatrixName = "Model";			// Nome do uniform da matriz Model.
constexpr const char* kViewMatrixName = "View";			    // Nome do uniform da matriz View.
constexpr const char* kModelViewMatrixName = "ModelView";	// Nome do uniform da matriz ModelView.
constexpr const char* kProjectionMatrixName = "Projection"; // Nome do uniform da matriz Projection.
// Nome do uniform da textura
constexpr const char* kTextureSamplerName = "texture1";	    // Nome do uniform do sampler de textura.
// Nome dos uniforms de propriedades do material
constexpr const char* kMaterialAmbientName = "material.ambient";	    // Nome do uniform da propriedade ambient do material.
constexpr const char* kMaterialDiffuseName = "material.diffuse";	    // Nome do uniform da propriedade diffuse do material.
constexpr const char* kMaterialSpecularName = "material.specular";	    // Nome do uniform da propriedade specular do material.
constexpr const char* kMaterialEmissiveName = "material.emissive";	    // Nome do uniform da propriedade emissive do material.
constexpr const char* kMaterialShininessName = "material.shininess";    // Nome do uniform da propriedade shininess do material.
constexpr const char* kMaterialIndexOfRefractionName = "material.indexOfRefraction"; // Nome do uniform da propriedade indexOfRefraction do material.
constexpr const char* kMaterialOpacityName = "material.opacity";          // Nome do uniform da propriedade opacity do material.
constexpr const char* kMaterialIlluminationModelName = "material.illuminationModel"; // Nome do uniform da propriedade illuminationModel do material.
// Nome dos uniforms de propriedades da luz
constexpr const char* kLightPositionName = "light.position";	// Nome do uniform da propriedade position da luz.
constexpr const char* kLightAmbientName = "light.ambient";		// Nome do uniform da propriedade ambient da luz.
constexpr const char* kLightDiffuseName = "light.diffuse";		// Nome do uniform da propriedade diffuse da luz.
constexpr const char* kLightSpecularName = "light.specular";	// Nome do uniform da propriedade specular da luz.