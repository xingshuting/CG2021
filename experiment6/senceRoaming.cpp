#include <unordered_map>

#include <tiny_obj_loader.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "senceRoaming.h"


const std::string npcTexturePath = "../data/ApocalypseInteriors_Texture.png";

SenceRoaming::SenceRoaming(const std::string& basedir){

	 _aspect = 1.0f * _windowWidth / _windowHeight;
	// init model
	//_cube.reset(importModel(basedir + "/" + "cube6.obj"));
	
	_cube.reset(new Model(basedir + "/" + "cube7.obj","cube7.obj"));

	//init npc
	//_npc.reset(new NPCmaster(importModel(basedir + "/" + "bed2.obj")));
	_npc.reset(new NPCmaster(new Model(basedir + "/" + "bed2.obj", "bed2.obj" )));


	//_npc->export_npc();

	_npcShader.reset(new Shader(PhongVertCode, PhongFragCode));
	_phongShader.reset(new Shader(PhongVertCode, PhongFragCode));


	// init camera
	_camera.reset(new PerspectiveCamera(glm::radians(50.0f), 1.0f * _windowWidth / _windowHeight, 0.1f, 10000.0f));
	_camera->position.z = 5.0f;
	_camera->position.x = 5.0f;
	
	// init lights
	_directionalLight.reset(new DirectionalLight);
	_directionalLight->position = glm::vec3(10.0f, 10.0f, 10.0f);
	_directionalLight->rotation = glm::angleAxis(glm::radians(45.0f), -glm::vec3(1.0f, 1.0f, 1.0f));

	_spotLight.reset(new SpotLight);
	
	_spotLight->position = glm::vec3(10.0f, 10.0f, 10.0f);;
	_spotLight->rotation = glm::angleAxis(glm::radians(45.0f), -glm::vec3(1.0f, 1.0f, 1.0f));

	//init texture
	_npcTextures.reset(new Texture2D(npcTexturePath));

	// init imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(_window, true);
	ImGui_ImplOpenGL3_Init();
}

SenceRoaming::~SenceRoaming() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

Model* SenceRoaming::importModel(const std::string& filepath) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;

	std::string::size_type index = filepath.find_last_of("/");
	std::string mtlBaseDir = filepath.substr(0, index + 1);

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filepath.c_str(), mtlBaseDir.c_str())) {
		throw std::runtime_error("load " + filepath + " failure: " + err);
	}

	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::unordered_map<Vertex, uint32_t> uniqueVertices;

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			vertex.position.x = attrib.vertices[3 * index.vertex_index + 0];
			vertex.position.y = attrib.vertices[3 * index.vertex_index + 1];
			vertex.position.z = attrib.vertices[3 * index.vertex_index + 2];

			if (index.normal_index >= 0) {
				vertex.normal.x = attrib.normals[3 * index.normal_index + 0];
				vertex.normal.y = attrib.normals[3 * index.normal_index + 1];
				vertex.normal.z = attrib.normals[3 * index.normal_index + 2];
			}

			// not used for this experiment
			if (index.texcoord_index >= 0) {
				vertex.texCoord.x = attrib.texcoords[2 * index.texcoord_index + 0];
				vertex.texCoord.y = attrib.texcoords[2 * index.texcoord_index + 1];
			}

			// check if the vertex appeared before to reduce redundant data
			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}

	// in this experiment we will not process with any material...
	// no more code for material preprocess
	return new Model(vertices, indices);
}


void SenceRoaming::handleInput() {
	if (_keyboardInput.keyStates[GLFW_KEY_ESCAPE] != GLFW_RELEASE) {
		glfwSetWindowShouldClose(_window, true);
		return;
	}

	if (_keyboardInput.keyStates[GLFW_KEY_W] != GLFW_RELEASE) {
		_camera->position += _camera->getFront() * _cameraMoveSpeed * (float)_deltaTime;
		_spotLight->position += _spotLight->getFront() * _cameraMoveSpeed * (float)_deltaTime;
	}

	if (_keyboardInput.keyStates[GLFW_KEY_A] != GLFW_RELEASE) {
		_camera->position -= _camera->getRight() * _cameraMoveSpeed * (float)_deltaTime;
		_spotLight->position -= _spotLight->getRight() * _cameraMoveSpeed * (float)_deltaTime;
	}

	if (_keyboardInput.keyStates[GLFW_KEY_S] != GLFW_RELEASE) {
		_camera->position -= _camera->getFront() * _cameraMoveSpeed * (float)_deltaTime;
		_spotLight->position -= _spotLight->getFront() * _cameraMoveSpeed * (float)_deltaTime;
	}

	if (_keyboardInput.keyStates[GLFW_KEY_D] != GLFW_RELEASE) {
		_camera->position += _camera->getRight() * _cameraMoveSpeed * (float)_deltaTime;
		_spotLight->position += _spotLight->getRight() * _cameraMoveSpeed * (float)_deltaTime;
	}

	if (_mouseInput.move.xCurrent != _mouseInput.move.xOld) {
	
		const float angle = -_cameraRotateSpeed * _deltaTime * (_mouseInput.move.xCurrent - _mouseInput.move.xOld);
		const glm::vec3 axis = { 0.0f, 1.0f, 0.0f };
	

		glm::quat q = glm::quat(cos(angle / 2), axis[0] * sin(angle / 2), axis[1] * sin(angle / 2), axis[2] * sin(angle / 2));

		_camera->rotation = q * _camera->rotation;
		_spotLight->rotation = q * _spotLight->rotation;

		_mouseInput.move.xOld = _mouseInput.move.xCurrent;
	}

	if (_mouseInput.move.yCurrent != _mouseInput.move.yOld) {
	

		const float angle = -_cameraRotateSpeed * _deltaTime * (_mouseInput.move.yCurrent - _mouseInput.move.yOld);
		const glm::vec3 axis = _camera->getRight();

		glm::quat q = glm::quat(cos(angle / 2), axis[0] * sin(angle / 2), axis[1] * sin(angle / 2), axis[2] * sin(angle / 2));

		_camera->rotation = q * _camera->rotation;
		_spotLight->rotation = q * _spotLight->rotation;

		_mouseInput.move.yOld = _mouseInput.move.yCurrent;
	}

	if (_mouseInput.click.left == true) {
		std::cout << "mouse left click  " << _mouseInput.move.xCurrent <<" " << _mouseInput.move.yCurrent <<std::endl;

		float x = (2.0f * _mouseInput.move.xCurrent) / _windowWidth - 1.0f;
		float y = 1.0f - (2.0f * _mouseInput.move.yCurrent) / _windowHeight;
		float z = 1.0f;
		glm::vec3 ray_nds = glm::vec3(x, y, z);
		glm::vec4 ray_clip = glm::vec4(x,y,z, 1.0);
		glm::vec4 ray_eye = inverse(_camera->getProjectionMatrix()) * ray_clip;
		glm::vec4 ray_world = inverse(_camera->getViewMatrix()) * ray_eye;


		if (ray_world.w != 0.0)
		{
			ray_world.x /= ray_world.w;
			ray_world.y /= ray_world.w;
			ray_world.z /= ray_world.w;
		}


		//_npc->detectRay(glm::vec3(ray_world.x, ray_world.y, ray_world.z), _camera->position);
		
	}
	/*add mouse scroll*/
	/*鼠标滚动放大缩小场景*/
	
	_camera->position.y = 30.0f;
}

void SenceRoaming::renderFrame() {
	// some options related to imGUI
	static bool wireframe = false;

	showFpsInWindowTitle();

	glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	if (wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	//set cube shader
	_phongShader->use();
	setShader(*_phongShader);

	//set npc shader
	_npcShader->use();
	setShader(*_npcShader);
	glActiveTexture(GL_TEXTURE0);
	_npcTextures->bind();
	
	//draw the cube
	_cube->draw();

	// draw the npc 
	_npc->drawNPC(*_npcShader);


	// draw ui elements
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	const auto flags =
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoSavedSettings;

	if (!ImGui::Begin("Control Panel", nullptr, flags)) {
		ImGui::End();
	} 
	else {
		ImGui::Text("directional light");
		ImGui::Separator();
		ImGui::SliderFloat("intensity##1", &_directionalLight->intensity, 0.0f, 2.0f);
		ImGui::ColorEdit3("color##1", (float*)&_directionalLight->color);
		ImGui::NewLine();

		ImGui::Text("spot light");
		ImGui::Separator();
		ImGui::SliderFloat("intensity##2", &_spotLight->intensity, 0.0f, 10.0f);
		ImGui::ColorEdit3("color##2", (float*)&_spotLight->color);
		ImGui::SliderFloat("angle##2", (float*)&_spotLight->angle, 0.0f, glm::radians(180.0f), "%f rad");
		ImGui::NewLine();

		ImGui::Text("Material");
		ImGui::Separator();
		ImGui::ColorEdit3("albedo", (float*)&_albedo);
		ImGui::SliderFloat("ka", &_ka, 0.0f, 1.0f);
		ImGui::ColorEdit3("kd", (float*)&_kd);
		ImGui::ColorEdit3("ks", (float*)&_ks);
		ImGui::SliderFloat("ns", &_ns, 0.0f, 50.0f);

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void SenceRoaming::setShader(Shader const& _shader) {
	//1. transfer mvp matrix to the shader
	_shader.setMat4("projection", _camera->getProjectionMatrix());
	_shader.setMat4("view", _camera->getViewMatrix());
	//_shader.setMat4("model", model[0]);

	 //2. transfer material attributes to the shader
	_shader.setVec3("material.albedo", _albedo);
	_shader.setFloat("material.ka", _ka);
	_shader.setVec3("material.kd", _kd);
	_shader.setVec3("material.ks", _ks);
	_shader.setFloat("material.ns", _ns);

	// 3. transfer light attributes to the shader
	_shader.setVec3("viewPos", _camera->position);
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