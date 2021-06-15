#include "shading_tutorial.h"

Shading_Tutorial::Shading_Tutorial() {

	// init lights
	_directionalLight.reset(new DirectionalLight);
	_directionalLight->position = glm::vec3(10.0f, 10.0f, 10.0f);
	_directionalLight->rotation = glm::angleAxis(glm::radians(45.0f), -glm::vec3(1.0f, 1.0f, 1.0f));

	_spotLight.reset(new SpotLight);
	_spotLight->position = glm::vec3(0.0f, 0.0f, 5.0f);
	_spotLight->rotation = glm::vec3(0.0f, 0.0f, 0.0f);


	PhongVertCode = 
		"#version 330 core\n"
		"layout(location = 0) in vec3 aPosition;\n"
		"layout(location = 1) in vec3 aNormal;\n"
		"layout(location = 2) in vec2 aTexCoord;\n"
		"out vec3 FragPos;\n"
		"out vec3 Normal;\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"void main() {\n"
		"	FragPos = vec3(model * vec4(aPosition, 1.0f));\n"
		"	Normal = mat3(transpose(inverse(model))) * aNormal;\n"
		"	gl_Position = projection * view * model * vec4(aPosition, 1.0f);\n"
		"}\n";

	PhongFragCode = 
		"#version 330 core\n"
		"in vec3 FragPos;\n"
		"in vec3 Normal;\n"
		"out vec4 color;\n"


		"// material data structure declaration\n"
		"struct Material {\n"
		"	vec3 albedo;\n"
		"	float ka;\n"
		"	vec3 kd;\n"
		"	vec3 ks;\n"
		"	float ns;\n"
		"};\n"

		"// directional light data structure declaration\n"
		"struct DirectionalLight {\n"
		"	vec3 direction;\n"
		"	float intensity;\n"
		"	vec3 color;\n"
		"};\n"

		"// spot light data structure declaration\n"
		"struct SpotLight {\n"
		"	vec3 position;\n"
		"	vec3 direction;\n"
		"	float intensity;\n"
		"	vec3 color;\n"
		"	float angle;\n"
		"	float kc;\n"
		"	float kl;\n"
		"	float kq;\n"
		"};\n"

		"// uniform variables\n"
		"uniform Material material;\n"
		"uniform DirectionalLight directionalLight;\n"
		"uniform SpotLight spotLight;\n"
		"uniform vec3 viewPos;\n"

		"vec3 calcDirectionalLight(vec3 normal) {\n"
		"	vec3 lightDir = normalize(-directionalLight.direction);\n"
		"	// diffuse color\n"
		"	vec3 diffuse = directionalLight.color * max(dot(lightDir, normal), 0.0f) * material.kd;\n"
		"	// specular color\n"
		"	vec3 viewDir = normalize(viewPos - FragPos);\n"
		"	vec3 reflectDir = reflect(-lightDir, normal);\n"
		"	vec3 specular = material.ns * directionalLight.color * pow(max(dot(viewDir, reflectDir), 0.0f),8) * material.ks ;\n"
		"	return directionalLight.intensity *( diffuse + specular);\n"
		"}\n"

		"vec3 calcSpotLight(vec3 normal) {\n"
		"	vec3 lightDir = normalize(spotLight.position - FragPos);\n"
		"	float theta = acos(-dot(lightDir, normalize(spotLight.direction)));\n"
		"	if (theta > spotLight.angle) {\n"
		"		return vec3(0.0f, 0.0f, 0.0f);\n"
		"	}\n"
		"	vec3 diffuse = spotLight.color * max(dot(lightDir, normal), 0.0f) * material.kd;\n"
		"	vec3 viewDir = normalize(viewPos - FragPos);\n"
		"	vec3 reflectDir = reflect(-lightDir, normal);\n"
		"	vec3 specular = material.ns * spotLight.color * pow( max(dot(viewDir, reflectDir), 0.0f), 8 ) * material.ks ;\n"

		"	float distance = length(spotLight.position - FragPos);\n"
		"	float attenuation = 1.0f / (spotLight.kc + spotLight.kl * distance + spotLight.kq * distance * distance);\n"
		"	return spotLight.intensity * attenuation * (diffuse + specular);\n"
		"}\n"

		"void main() {\n"
		"	vec3 normal = normalize(Normal);\n"
		"	// ambient color\n"
		"	vec3 ambient = material.ka * material.albedo;\n"
		"	// diffuse + specular color\n"
		"	vec3 DiffusePlazSpecular = calcDirectionalLight(normal) + calcSpotLight(normal);\n"
		"	color = vec4(ambient + DiffusePlazSpecular, 1.0f);\n"
		"}\n";

	
	_phongShader.reset(new Shader(PhongVertCode, PhongFragCode));
	
}



void Shading_Tutorial::setPhongShader(glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec3 camera_pos) {
	_phongShader->use();
	// 1. transfer mvp matrix to the shader
	_phongShader->setMat4("projection", projection);
	_phongShader->setMat4("view", view);
	_phongShader->setMat4("model", model);
	// 2. transfer material attributes to the shader
	_phongShader->setVec3("material.albedo", _albedo);
	_phongShader->setFloat("material.ka", _ka);
	_phongShader->setVec3("material.kd", _kd);
	_phongShader->setVec3("material.ks", _ks);
	_phongShader->setFloat("material.ns", _ns);
	// 3. transfer light attributes to the shader
	_phongShader->setVec3("viewPos", camera_pos);
	_phongShader->setVec3("spotLight.position", _spotLight->position);
	_phongShader->setVec3("spotLight.direction", _spotLight->getFront());
	_phongShader->setFloat("spotLight.intensity", _spotLight->intensity);
	_phongShader->setVec3("spotLight.color", _spotLight->color);
	_phongShader->setFloat("spotLight.angle", _spotLight->angle);
	_phongShader->setFloat("spotLight.kc", _spotLight->kc);
	_phongShader->setFloat("spotLight.kl", _spotLight->kl);
	_phongShader->setFloat("spotLight.kq", _spotLight->kq);
	_phongShader->setVec3("directionalLight.direction", _directionalLight->getFront());
	_phongShader->setFloat("directionalLight.intensity", _directionalLight->intensity);
	_phongShader->setVec3("directionalLight.color", _directionalLight->color);

	
	//_npcInstancedShader->use();
	// 1. transfer mvp matrix to the shader
	//_npcInstancedShader->setMat4("projection", projection);
	//_npcInstancedShader->setMat4("view", view);
	//_npcInstancedShader->setMat4("model", model);
	// 2. transfer material attributes to the shader
	//_npcInstancedShader->setVec3("material.albedo", _albedo);
	//_npcInstancedShader->setFloat("material.ka", _ka);
	//_npcInstancedShader->setVec3("material.kd", _kd);
	//_npcInstancedShader->setVec3("material.ks", _ks);
	//_npcInstancedShader->setFloat("material.ns", _ns);
	//// 3. transfer light attributes to the shader
	//_npcInstancedShader->setVec3("viewPos", camera_pos);
	//_npcInstancedShader->setVec3("spotLight.position", _spotLight->position);
	//_npcInstancedShader->setVec3("spotLight.direction", _spotLight->getFront());
	//_npcInstancedShader->setFloat("spotLight.intensity", _spotLight->intensity);
	//_npcInstancedShader->setVec3("spotLight.color", _spotLight->color);
	//_npcInstancedShader->setFloat("spotLight.angle", _spotLight->angle);
	//_npcInstancedShader->setFloat("spotLight.kc", _spotLight->kc);
	//_npcInstancedShader->setFloat("spotLight.kl", _spotLight->kl);
	//_npcInstancedShader->setFloat("spotLight.kq", _spotLight->kq);
	//_npcInstancedShader->setVec3("directionalLight.direction", _directionalLight->getFront());
	//_npcInstancedShader->setFloat("directionalLight.intensity", _directionalLight->intensity);
	//_npcInstancedShader->setVec3("directionalLight.color", _directionalLight->color);


}


void Shading_Tutorial::setNpcShader(Shader const& _shader, glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos) {
	

	 //1. transfer mvp matrix to the shader
	_shader.setMat4("projection", projection);
	_shader.setMat4("view", view);
	//_shader.setMat4("model", model[0]);

	 //2. transfer material attributes to the shader
	_shader.setVec3("material.albedo", _albedo);
	_shader.setFloat("material.ka", _ka);
	_shader.setVec3("material.kd", _kd);
	_shader.setVec3("material.ks", _ks);
	_shader.setFloat("material.ns", _ns);

	// 3. transfer light attributes to the shader
	_shader.setVec3("viewPos", camera_pos);
	_shader.setVec3("spotLight.position", _spotLight->position);
	_shader.setVec3("spotLight.direction", _spotLight->getFront());
	_shader.setFloat("spotLight.intensity", _spotLight->intensity);
	_shader.setVec3("spotLight.color", _spotLight->color);
	_shader.setFloat("spotLight.angle", _spotLight->angle);
	_shader.setFloat("spotLight.kc", _spotLight->kc);
	_shader.setFloat("spotLight.kl", _spotLight->kl);
	_shader.setFloat("spotLight.kq", _spotLight->kq);
	_shader.setVec3("directionalLight.direction", _directionalLight->getFront());
	_shader.setFloat("directionalLight.intensity", _directionalLight->intensity);
	_shader.setVec3("directionalLight.color", _directionalLight->color);



}