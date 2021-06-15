#pragma once

#include <memory>
#include <string>

#include <imgui.h>

#include "../base/application.h"
#include "../base/shader.h"
#include "../base/model.h"
#include "../base/light.h"
#include "../base/texture.h"
#include "../base/camera.h"
#include "NPCmaster.h"
#include "shaderMaster.h"


class SenceRoaming : public Application {
public:
	SenceRoaming(const std::string& basedir);

	~SenceRoaming();

	Model* importModel(const std::string& filepath);

private:
	
	// model
	std::unique_ptr<Model> _cube;
	std::unique_ptr <NPCmaster> _npc;

	float _aspect;

	// lights
	std::unique_ptr<DirectionalLight> _directionalLight;
	std::unique_ptr<SpotLight> _spotLight;

	// shaders
	std::unique_ptr<Shader> _npcShader;
	std::unique_ptr<Shader> _phongShader;

	void setShader(Shader const& _shader);

	// for ambient color calculation
	glm::vec3 _albedo = { 1.0f, 1.0f, 1.0f };
	float _ka = 0.03f;

	// for diffuse color calculation
	glm::vec3 _kd = { 1.0f, 1.0f, 1.0f };

	// for specular color calculation
	glm::vec3 _ks = { 1.0f, 1.0f, 1.0f };
	float _ns = 0.5f;

	std::unique_ptr<Texture> _npcTextures;



	// camera
	std::unique_ptr<PerspectiveCamera> _camera;

	const float _cameraMoveSpeed = 10.0f;
	const float _cameraRotateSpeed = 0.01f;



	void handleInput() override;

	void renderFrame() override;
};