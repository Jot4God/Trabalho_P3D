// GLEW - OpenGL Extension Wrangler Library
#define GLEW_STATIC		// Definição necessária, antes de incluir 'GL\glew.h', sempre que se usa GLEW como uma biblioteca estática
#include <GL/glew.h>	// Necessário para utilizar a função 'glViewport'
#if kDebugMode
#define GLM_ENABLE_EXPERIMENTAL // Necessário para utilizar funcionalidades experimentais do GLM, como 'glm::to_string'
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#endif

#include "Camera.h"


namespace game_engine_p3d {

	void Camera::LookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up) {
		view_.matrix_ = glm::lookAt(eye, center, up);
		LOG("Camera LookAt set with eye: (" << eye.x << ", " << eye.y << ", " << eye.z << "), center: (" << center.x << ", " << center.y << ", " << center.z << "), up: (" << up.x << ", " << up.y << ", " << up.z << ")");
#if !kDebugMode
		// Imprime a matriz de visualização para verificação
		LOG("View Matrix:\n" << glm::to_string(view_.matrix_));
#endif
	}

	void Camera::Prespective(float fov, float aspect, float near_plane, float far_plane) {
		// Exibe aviso se a câmara for do tipo ortográfica
		if (camera_type_ == CameraType::kOrthographic) {
			LOG("Warning: Changing camera type to Perspective.");
			camera_type_ = CameraType::kPerspective;
		}

		projection_.matrix_ = glm::perspective(glm::radians(fov), aspect, near_plane, far_plane);
		LOG("Perspective projection set with FOV: " << fov << ", aspect: " << aspect << ", near: " << near_plane << ", far: " << far_plane);
	}

	void Camera::Orthographic(float left, float right, float bottom, float top, float near_plane, float far_plane) {
		// Exibe aviso se a câmara for do tipo perspetiva
		if (camera_type_ == CameraType::kPerspective) {
			LOG("Warning: Changing camera type to Orthographic.");
			camera_type_ = CameraType::kOrthographic;
		}

		projection_.matrix_ = glm::ortho(left, right, bottom, top, near_plane, far_plane);
		LOG("Orthographic projection set with left: " << left << ", right: " << right << ", bottom: " << bottom << ", top: " << top);
	}

	void Camera::Viewport(int width, int height, int x, int y) {
		viewport_.width = width;
		viewport_.height = height;
		viewport_.x = x;
		viewport_.y = y;

		glViewport(x, y, width, height);
		LOG("Viewport set to: " << width << "x" << height << " at position (" << x << ", " << y << ")");
	}

	void Camera::Viewport() const {
		glViewport(viewport_.x, viewport_.y, viewport_.width, viewport_.height);
		LOG("Viewport set to: " << viewport_.width << "x" << viewport_.height << " at position (" << viewport_.x << ", " << viewport_.y << ")");
	}

	void  Camera::AddLayerToCullingMask(Layer layer) {
		// Verifica se a layer já está na máscara de culling
		if (std::find(culling_mask_.begin(), culling_mask_.end(), layer) == culling_mask_.end()) {
			culling_mask_.push_back(layer);
			LOG("Layer '" << layer << "' added to camera culling mask.");
		}
		else {
			LOG("Layer '" << layer << "' is already in the camera culling mask.");
		}
	}
}