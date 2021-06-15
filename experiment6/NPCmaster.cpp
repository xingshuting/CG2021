#include "NPCmaster.h"


AxisAlignedBox::AxisAlignedBox() {
	_minimum = {0.0f,0.0f,0.0f};
	_maximum = { 0.0f,0.0f,0.0f };

}

AxisAlignedBox::AxisAlignedBox(glm::vec3  minimum, glm::vec3  maximum) {
	_minimum = minimum;
	_maximum = maximum;
}

void AxisAlignedBox::translate_box(glm::vec3 temp) {
	_minimum += temp;
	_maximum += temp;
}

glm::vec3 AxisAlignedBox::get_minimum() {
	return _minimum;
}
glm::vec3 AxisAlignedBox::get_maximun() {
	return _maximum;
}


NPCmaster::NPCmaster(Model* model) {

	_model.reset(model);

	const float radius = 50.0f;
	const float offset = 10.0f;
	for (int i = 0; i < _amount; ++i) {
		glm::mat4 model(1.0f);

		AxisAlignedBox _box(_model->_min_vec,_model->_max_vec);

		// translate
		float angle = (float)i / (float)_amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;

		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.2f;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, 1.0f, z));

		//std::cout << "min: " << .x << " " << min.y << " " << min.z << std::endl;
		_box.translate_box(glm::vec3(x, 1.0f, z));
		

		//// scale
		//float scale = (rand() % 20) / 100.0f + 0.05;
		//model = glm::scale(model, glm::vec3(scale));

		//// rotate
		//float rotAngle = (rand() % 360);
		//model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));


		AABB_box.push_back(_box);
		_modelMatrices.push_back(model);
	}

}


std::vector<glm::mat4> NPCmaster::getModelMatrix() {
	return _modelMatrices;
}

void NPCmaster::drawNPC(Shader const& _shader) {
	
	for (int i = 0; i < _amount; ++i) {
		_shader.setMat4("model", _modelMatrices[i]);
		_model->draw();
	}
}

void NPCmaster::export_npc() {

	_model->export_model();

}

void NPCmaster::detectRay(glm::vec3 world_ray, glm::vec3 origin) {
	std::cout << "detectRay" << std::endl;

	glm::vec3 ray_dir = normalize( world_ray - origin );
	//glm::vec3 ray_dir = origin - world_ray;

	std::cout << origin.x << " " << origin.y << " " << origin.z << std::endl;
	std::cout << world_ray.x << " " << world_ray.y << " " << world_ray.z << std::endl;
	std::cout << ray_dir.x <<" " <<ray_dir.y <<" " <<ray_dir.z << std::endl;


	static const float EPS = 1e-6f;
	// 因为是线段 所以参数t取值在0和1之间
	float tmin = 0.0f;
	float tmax = 50.0f;

	for (int j = 0; j < AABB_box.size(); j++) {
		glm::vec3 min = AABB_box[j].get_minimum();
		glm::vec3 max = AABB_box[j].get_maximun();

		std::cout <<"min: " << min.x << " " << min.y << " " << min.z << std::endl;
		std::cout << "max: " << max.x << " " << max.y << " " << max.z << std::endl;
		bool tinit = false;

		for (int i = 0; i < 3; i++)
		{
			// 如果光线某一个轴分量为 0，且在包围盒这个轴分量之外，那么直接判定不相交 
			if (fabsf(ray_dir[i]) < EPS && (origin[i] < min[i] || origin[i] > max[i]))
			{
				std::cout << "0.0f" << std::endl;
				continue;

			}
			else
			{
				const float ood = 1.0f / ray_dir[i];
				// 计算参数t 并令 t1为较小值 t2为较大值
				float t1 = (min[i] - origin[i]) * ood;
				float t2 = (max[i] - origin[i]) * ood;
				if (t1 > t2) {
					float tmp = t1; t1 = t2; t2 = tmp;
				}
				

				if (t1 > tmin) tmin = t1;
				if (t2 < tmax) tmax = t2;

				// 判定不相交
				if (tmin > tmax)
					continue;

				else {
					std::cout << j << std::endl;
					_modelMatrices[j] = glm::translate(_modelMatrices[j], glm::vec3(0.0f, 5.0f, 0.0f));
					break;
				}


			}


		}
	}



}