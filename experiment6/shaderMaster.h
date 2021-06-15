#pragma once
#ifndef SHADERMASTER_H_ 
#define SHADERMASTER_H_


const char* const PhongVertCode =
	"#version 330 core\n"
	"layout(location = 0) in vec3 aPosition;\n"
	"layout(location = 1) in vec3 aNormal;\n"
	"layout(location = 2) in vec2 aTexCoord;\n"
	"out vec3 FragPos;\n"
	"out vec3 Normal;\n"
	"out vec2 TexCoord;\n"
	"uniform mat4 model;\n"
	"uniform mat4 view;\n"
	"uniform mat4 projection;\n"
	"void main() {\n"
	"	TexCoord = aTexCoord;\n"
	"	FragPos = vec3(model * vec4(aPosition, 1.0f));\n"
	"	Normal = mat3(transpose(inverse(model))) * aNormal;\n"
	"	gl_Position = projection * view * model * vec4(aPosition, 1.0f);\n"
	"}\n";

const char* const PhongFragCode = 
	"#version 330 core\n"
	"in vec3 FragPos;\n"
	"in vec3 Normal;\n"
	"in vec2 TexCoord;\n"
	"out vec4 color;\n"
	"uniform sampler2D mapKd;\n"


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
	//"	color = vec4(ambient + DiffusePlazSpecular, 1.0f);\n"
	"	color = texture(mapKd, TexCoord) *  vec4(ambient + DiffusePlazSpecular, 1.0f);\n"
	"}\n";


#endif