#pragma once

#include "../base/shader.h"
#include "../base/light.h"
#include <vector>

class Shading_Tutorial
{
public:
	Shading_Tutorial();


	void setPhongShader(glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec3 camera_pos);

	void setNpcShader(Shader const& _shader, glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos);


	const char* PhongVertCode;
	const char* PhongFragCode;
	
	// lights
	std::unique_ptr<DirectionalLight> _directionalLight;
	std::unique_ptr<SpotLight> _spotLight;

	// material coefficients
	// for ambient color calculation
	glm::vec3 _albedo = { 1.0f, 1.0f, 1.0f };
	float _ka = 0.03f;

	// for diffuse color calculation
	glm::vec3 _kd = { 1.0f, 1.0f, 1.0f };

	// for specular color calculation
	glm::vec3 _ks = { 1.0f, 1.0f, 1.0f };
	float _ns = 0.5f;


	std::unique_ptr<Shader> _phongShader;


};

