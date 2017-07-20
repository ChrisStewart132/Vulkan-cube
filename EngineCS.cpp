#include "EngineCS.h"



EngineCS::EngineCS()
{
}


EngineCS::~EngineCS()
{
}


void EngineCS::rotate(glm::vec3& _vector, float _angle, char axis, glm::vec3 centreDistance)
{
	angle = _angle*(3.14159265359 / 180.0f);//3.14 radians = 180degrees
	glm::mat3 cwMatX(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, cos(angle), -sin(angle)), glm::vec3(0.0f, sin(angle), cos(angle)));//rotate around x
	glm::mat3 cwMatY(glm::vec3(cos(angle), 0.0f, sin(angle)), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(-sin(angle), 0.0f, cos(angle)));//rotate around y
	glm::mat3 cwMatZ(glm::vec3(cos(angle), sin(angle), 0.0f), glm::vec3(-sin(angle), cos(angle), 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));//rotate around z

	_vector -= centreDistance;
	if (axis == 'x' || axis == 'X') {
		_vector = cwMatX * _vector;
	}
	else if (axis == 'y' || axis == 'Y') {
		_vector = cwMatY * _vector;
	}
	else if (axis == 'z' || axis == 'Z') {
		_vector = cwMatZ * _vector;
	}
	else {
		std::cout << "rotate: invalid axis\n";
	}
	_vector += centreDistance;
}


