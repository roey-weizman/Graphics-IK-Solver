#include <list>
#include <iostream>
#include "display.h"
#include "mesh.h"
#include "shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <cube.h>
#include <SceneData.h>
#include <stdlib.h>     /* srand, rand */
#include <iostream>
#include <fstream>
#include <string> 
#include "glm/gtx/matrix_interpolation.hpp"
#include "stb_image.h"


#define M_PI 3.14159265358979323846
#define relation DISPLAY_WIDTH/DISPLAY_HEIGHT;

using namespace glm;
static const int m_RGB = 3;
static const int DISPLAY_WIDTH = 800;
static const int DISPLAY_HEIGHT = 800;

mat4 rotations[5];
mat4 translations[5];
mat4 box_transformations[5];

mat4 rotateX[5];
mat4 rotateZ[5];
mat4 rotateZ2[5];

mat4 lower = translate(vec3(0, 0, -2)), raise = translate(vec3(0, 0, 2));

cube *myCube;
SceneData* myScene;

float prevY = 0;
float prevX = 0;
float transX;
float transY;
float depth;
bool stopped = true;
double curX = 0;
double curY = 0;
int pressed_index = -1;

void draw_lines() {		/*draws the axis of every box in the chain*/
	glBegin(GL_LINES);
	glVertex3f(-10.0f, 0.0f, -2.0f);
	glVertex3f(10.0f, 0.0f, -2.0f);

	glBegin(GL_LINES);
	glVertex3f(0.0f, 10.0f, -2.0f);
	glVertex3f(0.0f, -10.0f, -2.0f);

	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 8.0f);
	glVertex3f(0.0f, 0.0f, -12.0f);
	glEnd();

}

void ik_solver() {		/*IK solver by the CCD algorithm */

	float threshold = 0.1f;
	vec4 e = box_transformations[3] *raise* vec4(1, 1, 1, 1);	/*top of last part in chain*/
	vec4 d = box_transformations[4]  * translate(vec3(-2, 0, -2)) * vec4(1, 1, 1, 1);	/*destination*/
	vec4 bottom = box_transformations[0] * lower* vec4(1, 1, 1, 1);

	if (distance(d,e) > threshold && !stopped && distance(d, bottom) < 16) {
		
		for (int i = 3; i >= 0; i--) /*for every part in the chain rotate it a bit according to the algorithm*/
		{
			vec4 r = box_transformations[i] * lower * vec4(1, 1, 1, 1);
			vec4 re = normalize(e - r);
			vec4 rd = normalize(d - r);

			rotations[i] = lower*rotate( degrees(acos(clamp(dot(re, rd),-1.0f,1.0f)))/25, normalize(cross((vec3)re, (vec3)rd)))*raise*rotations[i];
		
			e = box_transformations[3] * raise* vec4(1, 1, 1, 1); 
		}
	} 
	else if(distance(d, bottom) > 16) {
		std::cout << "cannot reach" << std::endl;
	}
	else if (distance(d, e) <= threshold) {
		std::cout << distance(d, e) << std::endl;
	}
}

void rotate_arrow(int axis, int dir) {		/*rotations according to arrows*/
	float angle = 3;

	switch (pressed_index) {
	case 0:
		if (axis)
			rotateX[0] = lower*rotate(dir*angle, vec3(1, 0, 0))*raise*rotateX[0];
		else {
			rotateZ[0] = lower*rotate(dir*angle, vec3(0, 0, 1))*raise*rotateZ[0];
			rotateZ2[0] = lower*rotate(dir*angle, vec3(0, 0, -1))*raise*rotateZ2[0];
		}
		break;
	case 10:
		if (axis)
			rotateX[1] = lower*rotate(dir*angle, vec3(1, 0, 0))*raise*rotateX[1];
		else {
			rotateZ[1] = lower*rotate(dir*angle, vec3(0, 0, 1))*raise*rotateZ[1];
			rotateZ2[1] = lower*rotate(dir*angle, vec3(0, 0, -1))*raise*rotateZ2[1];
		}
		break;

	case 40:
		if (axis)
			rotateX[2] = lower*rotate(dir*angle, vec3(1, 0, 0))*raise*rotateX[2];
		else {
			rotateZ[2] = lower*rotate(dir*angle, vec3(0, 0, 1))*raise*rotateZ[2];
			rotateZ2[2] = lower*rotate(dir*angle, vec3(0, 0, -1))*raise*rotateZ2[2];
		}
		break;

	case 90:
		if (axis)
			rotateX[3] = lower*rotate(dir*angle, vec3(1, 0, 0))*raise*rotateX[3];
		else {
			rotateZ[3] = lower*rotate(dir*angle, vec3(0, 0, 1))*raise*rotateZ[3];
			rotateZ2[3] = lower*rotate(dir*angle, vec3(0, 0, -1))*raise*rotateZ2[3];
		}
		break;

	case -1: /*scene rotation*/
		if (axis)
			myScene->setProjection(rotate(myScene->getProjection(), dir*angle, vec3(1, 0, 0)));
		else
			myScene->setProjection(rotate(myScene->getProjection(), dir*angle, vec3(0, 0, 1)));

		break;
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	case GLFW_KEY_UP:
		rotate_arrow(1, 1);
		break;

	case GLFW_KEY_DOWN:
		rotate_arrow(1, -1);
		break;

	case GLFW_KEY_RIGHT:
		rotate_arrow(0, 1);
		break;

	case GLFW_KEY_LEFT:
		rotate_arrow(0, -1);
		break;

	case GLFW_KEY_SPACE:
		if (action == GLFW_PRESS) {
			stopped = !stopped;
			ik_solver();
		}
		break;
	}
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) { /*mouse draging, left for rotation and right mouse for translations*/

	prevY = curY;
	prevX = curX;
	curX = xpos;
	curY = ypos;

	float xrel = prevX - curX;
	float yrel = prevY - curY;

	float z = 100.0f + depth*(0.1f - 100.0f);
	transX = ((float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT)*(xrel) / (float)(DISPLAY_HEIGHT)*0.1*2.0*tan(60.0f * M_PI / 360.0)*(100.0f / z);
	transY = ((float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT)*(yrel) / (float)(DISPLAY_WIDTH)*0.1*2.0*tan(60.0f * M_PI / 360.0)*(100.0f / z);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
		float angle = 0.5;

		switch (pressed_index) {
		case 0:
			rotateX[0] = lower*rotate((float)(curY - prevY)*angle, vec3(1, 0, 0))*raise*rotateX[0];
			rotateZ[0] = lower*rotate((float)(curX - prevX)*angle, vec3(0, 0, 1))*raise*rotateZ[0];
			rotateZ2[0] = lower*rotate((float)(curX - prevX)*angle, vec3(0, 0, -1))*raise*rotateZ2[0];
			break;

		case 10:
			rotateX[1] = lower*rotate((float)(curY - prevY)*angle, vec3(1, 0, 0))*raise*rotateX[1];
			rotateZ[1] = lower*rotate((float)(curX - prevX)*angle, vec3(0, 0, 1))*raise*rotateZ[1];
			rotateZ2[1] = lower*rotate((float)(curX - prevX)*angle, vec3(0, 0, -1))*raise*rotateZ2[1];
			break;

		case 40:
			rotateX[2] = lower*rotate((float)(curY - prevY)*angle, vec3(1, 0, 0))*raise*rotateX[2];
			rotateZ[2] = lower*rotate((float)(curX - prevX)*angle, vec3(0, 0, 1))*raise*rotateZ[2];
			rotateZ2[2] = lower*rotate((float)(curX - prevX)*angle, vec3(0, 0, -1))*raise*rotateZ2[2];
			break;

		case 90:
			rotateX[3] = lower*rotate((float)(curY - prevY)*angle, vec3(1, 0, 0))*raise*rotateX[3];
			rotateZ[3] = lower*rotate((float)(curX - prevX)*angle, vec3(0, 0, 1))*raise*rotateZ[3];
			rotateZ2[3] = lower*rotate((float)(curX - prevX)*angle, vec3(0, 0, -1))*raise*rotateZ2[3];
			break;

		case -1:
				myScene->setProjection(rotate(myScene->getProjection(), (float)(curY - prevY)*angle, vec3(1, 0, 0)));
				myScene->setProjection(rotate(myScene->getProjection(), (float)(curX - prevX)*angle, vec3(0, 0, 1)));
			break;
		}
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {

		if (pressed_index>=0 && pressed_index <=90) 
			translations[0] = translate(vec3(transX, 0, transY)) * translations[0];
		else if(pressed_index == 160)
			translations[4] = translate(vec3(transX, 0, transY*2)) * translations[4] ;
		else {
			translations[0] = translate(vec3(transX, 0, transY)) * translations[0];
			translations[4] = translate(vec3(transX, 0, transY)) * translations[4];
		}
	} 
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) /*zoom in and out*/
{
	double xpos, ypos;

	glfwGetCursorPos(window, &xpos, &ypos);

	int dir1 = yoffset >= 0 ? -1 : 1;
	switch (pressed_index) {
	case 0:
		translations[0] = translations[0] * translate(vec3(0, dir1, 0));
		break;
	case 10:
		translations[0] = translations[0] * translate(vec3(0, dir1, 0));
		break;
	case 40:
		translations[0] = translations[0] * translate(vec3(0, dir1, 0));
		break;
	case 90:
		translations[0] = translations[0] * translate(vec3(0, dir1, 0));
		break;

	case 160:
		translations[4] = translations[4] * translate(vec3(0, dir1, 0));
		break;
	}
}

void mouse_callback(GLFWwindow* window, int button, int action, int mods) { /*for clicking/choosing on objects in the scene*/
	double xpos, ypos;

	glfwGetCursorPos(window, &xpos, &ypos);

	Shader pickingShader("./res/shaders/pickingShader");

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) || glfwSetScrollCallback(window, scroll_callback)) {
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (int k = 0; k < 5; k++) {
			mat4 t = mat4(1.0);
			if (k > 0 && k < 4) {
				t = translate(t, vec3(0.0f, 0.0f, 4 * 1.0f));
				box_transformations[k] = box_transformations[k - 1] * t * translations[k] * rotations[k] * rotateZ2[k] * rotateX[k] * rotateZ[k];
			}
			else if (k == 4) {
				t = translate(t, vec3(5.0f, 0.0f, 0.0f));
				t = scale(t, vec3(1, 1, 0.5f));
				box_transformations[k] = t*translations[k] * rotations[k] * rotateZ2[k] * rotateX[k] * rotateZ[k];
			}

			else if (k == 0) {
				box_transformations[k] = t*translations[k] * rotations[k] * rotateZ2[k] * rotateX[k] * rotateZ[k];
			}

			myScene->setMainMat(box_transformations[k]);

			myScene->muliplyMVP();

			pickingShader.Bind();
			pickingShader.Update(mat4(myScene->getMVP()), mat4(myScene->getMainMat()), k*k * 10);

			myCube->Draw();
		}

		glFlush();
		glFinish();

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		unsigned char data[4];
		glReadPixels(xpos, DISPLAY_HEIGHT - ypos - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glReadPixels(xpos, DISPLAY_HEIGHT - ypos - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

		int pickedID =
			data[0] +
			data[1] * 256 +
			data[2] * 256 * 256;

		if (pickedID == 0x00ffffff) { // Full white, must be the background !
			pressed_index = -1;
		}
		else {
			pressed_index = pickedID;
		}
	}
}

int main(int argc, char** argv)
{
	Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OpenGL");
	Shader shader("./res/shaders/basicShader");

	myScene = new SceneData(vec3(0, 0, -35), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
	myScene->setPerspectiveProjection(60.0f, (float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT, 0.1f, 100.0f);
	myScene->setProjection(mat4((myScene->getProjection()))*lookAt(vec3(myScene->getPos()), vec3(myScene->getPos() + myScene->getForward()), vec3(myScene->getUp())));
	myScene->muliplyMVP();

	myCube = new cube(0, 0, 0);
	myScene->setProjection(rotate(myScene->getProjection(), 90.0f, vec3(-1, 0, 0)));

	glfwSetKeyCallback(display.m_window, key_callback);
	glfwSetCursorPosCallback(display.m_window, cursor_position_callback);
	glfwSetMouseButtonCallback(display.m_window, mouse_callback);

	/* init */
	for (int k = 0; k < 5; ++k) {
		rotations[k] = mat4(1.0);
		translations[k] = mat4(1.0);
		box_transformations[k] = mat4(1.0);
		rotateX[k] = mat4(1.0);
		rotateZ[k] = mat4(1.0);
		rotateZ2[k] = mat4(1.0);
	}
	int TextureSize, TextureSize1;
	int numComponents, numComponents1;
	unsigned char* image0 = stbi_load("./res/textures/box0.bmp", &TextureSize, &TextureSize, &numComponents, m_RGB);
	unsigned char* image1 = stbi_load("./res/textures/grass.bmp", &TextureSize1, &TextureSize1, &numComponents1, m_RGB);

	while (!glfwWindowShouldClose(display.m_window))
	{
		Sleep(10);
		display.Clear(1.0f, 1.0f, 1.0f, 1.0f);

		for (int k = 0; k < 5; k++) {
			mat4 t = mat4(1.0);

			if (k > 0 && k < 4) {		//chain cube other then the base
				t = translate(t, vec3(0.0f, 0.0f, 4 * 1.0f));
				box_transformations[k] =  box_transformations[k - 1] * t * translations[k] * rotations[k] * rotateZ2[k] * rotateX[k] * rotateZ[k];
				shader.Texture(image0, TextureSize, numComponents);

			}
			else if (k == 4) {			//side cube
				t = translate(t, vec3(5.0f, 0.0f, 0.0f));
				t = scale(t, vec3(1, 1, 0.5f));
				box_transformations[k] =  t * translations[k] * rotations[k] * rotateZ2[k] * rotateX[k] * rotateZ[k];
				shader.Texture(image1, TextureSize1, numComponents1);
			}
			else if (k == 0) {			//base chain
				box_transformations[k] = t * translations[k]  * rotations[k] * rotateZ2[k] * rotateX[k] * rotateZ[k] ;
				shader.Texture(image0, TextureSize, numComponents);

			}
			myScene->setMainMat(box_transformations[k]);

			myScene->muliplyMVP();

			shader.Bind();
			
			shader.Update(mat4(myScene->getMVP()), mat4(myScene->getMainMat()), k);
				
			myCube->Draw();

			if (k < 4)
				draw_lines();
		}
		if (!stopped) 
			ik_solver();
		
		display.SwapBuffers();
		glfwPollEvents();
	}
	return 0;
}