#pragma once


// =============================================================
// Ficheiro:    Camera.h
// Descrição:   Classe para representar uma câmara 3D
// Autor:       Duarte Duque
// Data:        21/07/2025
// Versão:      1.0.0
// Dependências: Common.h, Transform.h
// Compilação:	g++ Camera.h -o Camera.o -lglew32s -lglfw3 -lopengl32
// Observações:
// Esta classe encapsula a posição, orientação e escala de uma câmara 3D.
// =============================================================


#include <vector>
#include <tuple> // std::tuple<>

#include "Common.h" // Inclui definições comuns e macros para o motor de jogo (neste caso, é necessário para utilização de 'WINDOW_DEFAULT_WIDTH' e 'WINDOW_DEFAULT_HEIGHT')
#include "Transform.h" // Inclui a classe 'Transform' para manipulação de matrizes


namespace game_engine_p3d {

	class Camera {
	public:
		// ------------------------------------------------------------
		// Tipos de dados
		// ------------------------------------------------------------
		// Enumeração para definir o tipo de câmara (perspectiva ou ortográfica)
		// Foi definida dentro da classe 'Camera' para encapsular o tipo de câmara e evitar poluição do namespace global.
		enum class CameraType {
			kNone = -1,		// Nenhum tipo de câmara definido
			kPerspective,	// Câmara perspetiva
			kOrthographic	// Câmara ortográfica
		};

		// ------------------------------------------------------------
		// Construtores e destrutores
		// ------------------------------------------------------------
		Camera() = default;		// Construtor padrão (indico ao compilador para gerar o construtor padrão)
		~Camera() = default;	// Destrutor padrão (indico ao compilador para gerar o destrutor padrão)
		// ------------------------------------------------------------
		// Wrappers para funções OpenGL e GLM
		// ------------------------------------------------------------
		void LookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up); // Wrapper para glm::lookAt
		void Prespective(float fov, float aspect, float near_plane, float far_plane); // Wrapper para glm::perspective
		void Orthographic(float left, float right, float bottom, float top, float near_plane, float far_plane); // Wrapper para glm::ortho
		void Viewport(int width, int height, int x, int y); // Wrapper para a função viewport do OpenGL
		void Viewport() const; // Wrapper para a função viewport do OpenGL com os valores atribuídos aos dados-membro da classe Camera
		// ------------------------------------------------------------
		// Accessors e mutators
		// ------------------------------------------------------------
		Transform& view() { return view_; } // Retorna a matriz de visualização da câmara
		void set_view(const Transform& view) { view_ = view; } // Define a matriz de visualização da câmara
		Transform& projection() { return projection_; } // Retorna a matriz de projeção da câmara
		void set_projection(const Transform& projection) { projection_ = projection; } // Define a matriz de projeção da câmara
		glm::vec4 background_color() const { return background_color_; } // Retorna a cor de fundo da câmara
		void set_background_color(const glm::vec4& color) { background_color_ = color; } // Define a cor de fundo da câmara
		std::vector<Layer>& culling_mask() { return culling_mask_; } // Retorna a máscara de culling da câmara
		// ------------------------------------------------------------
		// Outras funções-membro
		// ------------------------------------------------------------
		void AddLayerToCullingMask(Layer layer); // Adiciona uma layer à máscara de culling da câmara
		// Função que retorna o viewport da câmara, ou seja, as dimensões e posição da viewport associada à câmara.
		std::tuple<int, int, int, int> GetViewport() const {
			return std::make_tuple(viewport_.width, viewport_.height, viewport_.x, viewport_.y);
		}

	private:
		// ------------------------------------------------------------
		// Dados-membro privados
		// ------------------------------------------------------------

		// Cor de fundo da câmara
		glm::vec4 background_color_ = glm::vec4(0.0f);
		// Matriz de visualização da câmara (matriz View)
		Transform view_;
		// Matriz de projeção da câmara (matriz Projection)
		Transform projection_;

		// Tipo de câmara (perspectiva ou ortográfica)
		CameraType camera_type_ = CameraType::kNone;	// Tipo de câmara (inicialmente definido como 'kNone', ou seja, nenhum tipo de câmara definido)
		// Propriedades da câmara
		float prespective_field_of_view_ = 45.0f;	// Campo de visão da câmara (apenas para câmaras perspetivas) em graus
		float orthographic_size_ = 10.0f;			// Tamanho ortográfico da câmara (apenas para câmaras ortográficas) top e bottom. Neste exemplo, o tamanho é 10, então top = 10 e bottom = -10. Left e roght são calculados com base na proporção (aspect ratio) da viewport. Consideramos valores simétricos para simplificar a implementação.

		// Planos de recorte (clipping planes)
		struct ClippingPlanes {
			float near_plane = 0.1f;	// Plano próximo da câmara
			float far_plane = 100.0f;	// Plano distante da câmara
		} clipping_planes_; // Estrutura para armazenar os planos de recorte	

		// Viewport (dimensão e posição)
		struct Viewport {
			int width = kWindowDefaultWidth;	// Largura da viewport (por defeito, a largura da janela do jogo)
			int height = kWindowDefaultHeight;	// Altura da viewport (por defeito, a altura da janela do jogo)
			int x = 0;	// Posição (offset em) X da viewport (por defeito, 0)
			int y = 0;	// Posição (offset em) Y da viewport (por defeito, 0)
		} viewport_; // Estrutura para armazenar as dimensões e posição da viewport

		// Máscara de culling (layers que a câmara deve renderizar)
		std::vector<Layer> culling_mask_{ "Default" }; // Por defeito renderiza apenas a layer "Default"
	};
}