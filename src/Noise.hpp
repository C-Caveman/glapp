// Perlin noise function
#pragma once

#include <glm/glm.hpp>

// call as e.g. Noise::Noise1(...)
namespace Noise {
	// 3D to 3D hash function
	glm::uvec3 Hash3(glm::uvec3 v);

	// return float noise result in (-1,1) given 3D position P
	float Noise1(glm::vec3 P);

	// return vector noise result in (-1,1) given 3D position P
	glm::vec3 Noise3(glm::vec3 P);
};