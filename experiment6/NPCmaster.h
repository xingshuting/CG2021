#pragma once

#include "../base/model.h"
#include <memory>
#include "../base/shader.h"


class AxisAlignedBox {
public:
	AxisAlignedBox();
	AxisAlignedBox(glm::vec3  minimum, glm::vec3  maximum);

	void translate_box(glm::vec3 temp);
	glm::vec3 get_minimum();
	glm::vec3 get_maximun();

private:

	glm::vec3  _minimum;
	glm::vec3  _maximum;

};


class NPCmaster {
public:
	NPCmaster(Model * model);
	
	void drawNPC(Shader const& _shader);

	std::vector<glm::mat4> getModelMatrix();

	void detectRay(glm::vec3 world_ray, glm::vec3 origin);

	void export_npc();
	

private:
	std::unique_ptr<Model> _model;


	int _amount = 20;

	std::vector<glm::mat4> _modelMatrices;

	std::vector<AxisAlignedBox> AABB_box;

};



