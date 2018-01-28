#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <Windows.h>
#include <list>
#include <iostream>
#include "display.h"
#include "mesh.h"

using namespace glm;
class cube
{

public:
	void Draw();
	cube(float x, float y, float z);
	~cube();
	Vertex* init_cube(float x, float y, float z);
	Vertex* cubeVertex;
private:
	Vertex* myVertex;
};


