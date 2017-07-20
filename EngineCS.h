#pragma once
#include <glm/glm.hpp>						//vectors mats
#include <glm/gtc/matrix_transform.hpp>		//transformation matrix
#include <iostream>
#include <math.h>

class EngineCS
{
private:
	float angle = 0.0f;

public:
	EngineCS();
	~EngineCS();

	glm::vec3 worldRotation = { 0.0f,0.0f,0.0f };

	void rotate(glm::vec3& _vector, float _angle, char axis, glm::vec3 centreDistance = { 0.0f,0.0f,0.0f });
	
};

