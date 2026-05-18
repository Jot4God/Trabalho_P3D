#include "Common.h"
#include "Light.h"


namespace game_engine_p3d {

	Light::Light(const glm::vec3& ambient) :
		type_{ LightType::kAmbient },
		ambient_{ ambient }
	{
		// Construtor para luz ambiente
		LOG("Ambient light created with color: (" << ambient.r << ", " << ambient.g << ", " << ambient.b << ")");
	}

	Light::Light(const glm::vec3& direction,
		const glm::vec3& ambient,
		const glm::vec3& diffuse,
		const glm::vec3& specular
	) :
		type_{ LightType::kDirectional },
		direction_{ glm::normalize(direction) }, // Normaliza a direção
		ambient_{ ambient },
		diffuse_{ diffuse },
		specular_{ specular }
	{
		// Construtor para luz direcional
		LOG("Directional light created with direction: (" << direction_.x << ", " << direction_.y << ", " << direction_.z << ")");
	}

	Light::Light(const glm::vec3& position,
		const glm::vec3& ambient,
		const glm::vec3& diffuse,
		const glm::vec3& specular,
		float constant,
		float linear,
		float quadratic
	) :
		type_{ LightType::kPoint },
		position_{ position },
		ambient_{ ambient },
		diffuse_{ diffuse },
		specular_{ specular },
		constant_{ constant },
		linear_{ linear },
		quadratic_{ quadratic }
	{
		// Construtor para luz pontual
		LOG("Point light created at position: (" << position_.x << ", " << position_.y << ", " << position_.z << ")");
	}

	Light::Light(const glm::vec3& position,
		const glm::vec3& direction,
		const glm::vec3& ambient,
		const glm::vec3& diffuse,
		const glm::vec3& specular,
		float constant,
		float linear,
		float quadratic,
		float cutOff,
		float outerCutOff
	) :
		type_{ LightType::kSpotlight },
		position_{ position },
		direction_{ glm::normalize(direction) }, // Normaliza a direção
		ambient_{ ambient },
		diffuse_{ diffuse },
		specular_{ specular },
		constant_{ constant },
		linear_{ linear },
		quadratic_{ quadratic },
		cutOff_{ cutOff },
		outerCutOff_{ outerCutOff }
	{
		// Construtor para luz cónica (spotlight)
		LOG("Spotlight created at position: (" << position_.x << ", " << position_.y << ", " << position_.z << ") with direction: (" << direction_.x << ", " << direction_.y << ", " << direction_.z << ")");
	}

	std::string Light::type_string() const {
		// Retorna o tipo da luz como string (ex: "Ambient", "Directional", etc.)
		switch (type_) {
		case LightType::kAmbient:
			return "Ambient";
		case LightType::kDirectional:
			return "Directional";
		case LightType::kPoint:
			return "Point";
		case LightType::kSpotlight:
			return "Spotlight";
		default:
			return "Unknown";
		}
	}

	bool Light::IsInLayer(Layer layer) const {
		// Verifica se a layer fornecida está na lista de layers que esta luz afeta
		for (const auto& existing_layer : light_layers_) {
			if (existing_layer == layer) {
				return true; // A layer foi encontrada
			}
		}
		return false; // A layer não foi encontrada
	}

	void Light::AddLayerToLightLayers(Layer layer) {
		// Adiciona uma layer à lista de layers que esta luz afeta
		if (layer.empty()) {
			LOG("Error: Layer name cannot be empty.");
			return; // Não adiciona uma layer vazia
		}
		// Verifica se a layer já está na lista
		for (const auto& existing_layer : light_layers_) {
			if (existing_layer == layer) {
				LOG("Layer '" << layer << "' is already in the light layers list.");
				return; // Não adiciona duplicados
			}
		}
		light_layers_.push_back(layer);
		LOG("Layer '" << layer << "' added to light layers.");
	}
}