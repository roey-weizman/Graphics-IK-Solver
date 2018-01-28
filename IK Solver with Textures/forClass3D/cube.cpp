#include "cube.h"
#include "shader.h"

unsigned int indices[] = { 0, 1, 2,
0, 2, 3,

6, 5, 4,
7, 6, 4,

10, 9, 8,
11, 10, 8,

12, 13, 14,
12, 14, 15,

16, 17, 18,
16, 18, 19,

22, 21, 20,
23, 22, 20
};

cube::cube(float x, float y, float z)
{
	myVertex = (Vertex*)malloc(sizeof(Vertex) * 24 * 44);
	cubeVertex = init_cube(x, y, z);
}

Vertex * cube::init_cube(float x, float y, float z)
{

	myVertex[0] = Vertex(glm::vec3(-1 + x, -1 + y, -2 + z), glm::vec2(1, 0), glm::vec3(0, 0, -1), glm::vec3(1, 1, 0));			//yellow
	myVertex[1] = Vertex(glm::vec3(-1 + x, 1 + y, -2 + z), glm::vec2(0, 0), glm::vec3(0, 0, -1), glm::vec3(1, 1, 0));
	myVertex[2] = Vertex(glm::vec3(1 + x, 1 + y, -2 + z), glm::vec2(0, 1), glm::vec3(0, 0, -1), glm::vec3(1, 1, 0));
	myVertex[3] = Vertex(glm::vec3(1 + x, -1 + y, -2 + z), glm::vec2(1, 1), glm::vec3(0, 0, -1), glm::vec3(1, 1, 0));

	myVertex[4] = Vertex(glm::vec3(-1 + x, -1 + y, 2 + z), glm::vec2(1, 0), glm::vec3(0, 0, 1), glm::vec3(1, 1, 1));			//white
	myVertex[5] = Vertex(glm::vec3(-1 + x, 1 + y, 2 + z), glm::vec2(0, 0), glm::vec3(0, 0, 1), glm::vec3(1, 1, 1));
	myVertex[6] = Vertex(glm::vec3(1 + x, 1 + y, 2 + z), glm::vec2(0, 1), glm::vec3(0, 0, 1), glm::vec3(1, 1, 1));
	myVertex[7] = Vertex(glm::vec3(1 + x, -1 + y, 2 + z), glm::vec2(1, 1), glm::vec3(0, 0, 1), glm::vec3(1, 1, 1));

	myVertex[8] = Vertex(glm::vec3(-1 + x, -1 + y, -2 + z), glm::vec2(0, 1), glm::vec3(0, -1, 0), glm::vec3(1, 0.5, 0));		//orange
	myVertex[9] = Vertex(glm::vec3(-1 + x, -1 + y, 2 + z), glm::vec2(1, 1), glm::vec3(0, -1, 0), glm::vec3(1, 0.5, 0));
	myVertex[10] = Vertex(glm::vec3(1 + x, -1 + y, 2 + z), glm::vec2(1, 0), glm::vec3(0, -1, 0), glm::vec3(1, 0.5, 0));
	myVertex[11] = Vertex(glm::vec3(1 + x, -1 + y, -2 + z), glm::vec2(0, 0), glm::vec3(0, -1, 0), glm::vec3(1, 0.5, 0));

	myVertex[12] = Vertex(glm::vec3(-1 + x, 1 + y, -2 + z), glm::vec2(0, 1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));			//red
	myVertex[13] = Vertex(glm::vec3(-1 + x, 1 + y, 2 + z), glm::vec2(1, 1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
	myVertex[14] = Vertex(glm::vec3(1 + x, 1 + y, 2 + z), glm::vec2(1, 0), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
	myVertex[15] = Vertex(glm::vec3(1 + x, 1 + y, -2 + z), glm::vec2(0, 0), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));

	myVertex[16] = Vertex(glm::vec3(-1 + x, -1 + y, -2 + z), glm::vec2(1, 1), glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1));			//blue
	myVertex[17] = Vertex(glm::vec3(-1 + x, -1 + y, 2 + z), glm::vec2(1, 0), glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1));
	myVertex[18] = Vertex(glm::vec3(-1 + x, 1 + y, 2 + z), glm::vec2(0, 0), glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1));
	myVertex[19] = Vertex(glm::vec3(-1 + x, 1 + y, -2 + z), glm::vec2(0, 1), glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1));

	myVertex[20] = Vertex(glm::vec3(1 + x, -1 + y, -2 + z), glm::vec2(1, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));			//green
	myVertex[21] = Vertex(glm::vec3(1 + x, -1 + y, 2 + z), glm::vec2(1, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
	myVertex[22] = Vertex(glm::vec3(1 + x, 1 + y, 2 + z), glm::vec2(0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
	myVertex[23] = Vertex(glm::vec3(1 + x, 1 + y, -2 + z), glm::vec2(0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));

	return myVertex;
}

void cube::Draw() {

	Mesh mesh(cubeVertex, sizeof(cubeVertex) * 44 * 24 / sizeof((cubeVertex)[0]), indices, sizeof(indices) / sizeof(indices[0]));
	mesh.Draw();
}

cube::~cube()
{
	delete cubeVertex;
}
