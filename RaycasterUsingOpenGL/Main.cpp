#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "ShaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"


/*
 * Important constants
 */
const int WINDOW_WIDTH = 1024, WINDOW_HEIGHT = 512;
const float PI = 3.14159265359f;

/*
 * Global Variables
 */

//Shader shaderProgram("default.vert", "default.frag");

/*
 * Map
 */

// Map constants

const int mapX = 8, mapY = 8, mapS = 64;
const int map[] =
{
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 1, 0, 0, 0, 0, 1,
	1, 0, 1, 0, 0, 1, 0, 1,
	1, 0, 1, 0, 0, 1, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 1, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1
};
GLuint indices[] =
{
	0, 2, 1, // Upper triangle
	0, 3, 2 // Lower triangle
};

// Map functions

GLfloat pixelToVertex(int pixel, int direction)
{
	// x direction == 0
	// y direction == 1
	if (direction == 0) { return 2 * (GLfloat)pixel / WINDOW_WIDTH - 1; }
	else if (direction == 1) { return 2 * (GLfloat)pixel / WINDOW_HEIGHT - 1; }
	else { return 0.0f; }
}

void drawMap()
{
	float c;
	int x0, y0;
	for (int x = 0; x < mapX; x++)
	{
		for (int y = 0; y < mapY; y++)
		{
			// set the color based on value in array
			if (map[y * mapX + x] == 1)
			{
				c = 1.0;
			}
			else
			{
				c = 0.0;
			}
			// Calculate vertices
			x0 = x * mapS; y0 = y * mapS;
			GLfloat vertices[] =
			{
				pixelToVertex(0 + x0 + 1, 0),		pixelToVertex(WINDOW_HEIGHT - (y0 + 1), 1),			0.0f,	c, c, c,	0.0f, 0.0f, // Lower left corner
				pixelToVertex(0 + x0 + 1, 0),		pixelToVertex(WINDOW_HEIGHT - (mapS + y0 - 1), 1),	0.0f,   c, c, c,	0.0f, 1.0f, // Upper left corner
				pixelToVertex(mapS + x0 - 1, 0),	pixelToVertex(WINDOW_HEIGHT - (mapS + y0 - 1), 1),	0.0f,   c, c, c,	1.0f, 1.0f, // Upper right corner
				pixelToVertex(mapS + x0 - 1, 0),	pixelToVertex(WINDOW_HEIGHT - (y0 + 1), 1),			0.0f,	c, c, c,	1.0f, 0.0f  // Lower right corner
			};

			// Generates Vertex Array Object and binds it
			VAO VAO1;
			VAO1.Bind();

			// Generates Vertex Buffer Object and links it to vertices
			VBO VBO1(vertices, sizeof(vertices));
			// Generates Element Buffer Object and links it to indices
			EBO EBO1(indices, sizeof(indices));

			// Links VBO attributes such as coordinates and colors to VAO
			VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
			VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			// Unbind all to prevent accidentally modifying them
			VAO1.Unbind();
			VBO1.Unbind();
			EBO1.Unbind();

			// Bind the VAO so OpenGL knows to use it
			VAO1.Bind(); 
			// Draw primitives, number of indices, datatype of indices, index of indices
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); 
			VAO1.Unbind();

			VAO1.Delete(); VBO1.Delete(); EBO1.Delete();
		}
	}
}


/*
 * Player
 */

// math functions

int fixAngle(int a)
{
	if (a > 359)
	{
		a -= 360;
	}
	if (a < 0)
	{
		a += 360;
	}
	return a;
}

float toRadian(float deg)
{
	return deg * PI / 180.0;
}

// variables
float px, py, pdx, pdy, pa;

void movePlayer(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		//px += pdx * 2; py += pdy * 2;
		px += pdx; py += pdy;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		//pa -= 5;  pa = fixAngle(pa); pdx = cos(toRadian(pa)); pdx = -sin(toRadian(pa));
		pa += 1; pa = fixAngle(pa); pdx = cos(toRadian(pa)); pdy = sin(toRadian(pa));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		//px -= pdx * 2; py -= pdy * 2;
		px -= pdx; py -= pdy;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		//pa += 5; pa = fixAngle(pa); pdx = cos(toRadian(pa)); pdx = -sin(toRadian(pa));
		pa -= 1; pa = fixAngle(pa); pdx = cos(toRadian(pa)); pdy = sin(toRadian(pa));
	}
}

void drawPlayer()
{
	glPointSize(20);
	GLfloat pos[] =
	{
		pixelToVertex(px, 0), pixelToVertex(py, 1), 1.0f,		1.0f, 1.0f, 0.0f
	};

	VAO playerVAO;
	playerVAO.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	VBO playerVBO(pos, sizeof(pos));

	// Links VBO attributes such as coordinates and colors to VAO
	playerVAO.LinkAttrib(playerVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	playerVAO.LinkAttrib(playerVBO, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)(3 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	playerVBO.Unbind();

	// Draw primitives, number of indices, datatype of indices, index of indices
	glDrawArrays(GL_POINTS, 0, 1);
	playerVAO.Unbind();
	playerVAO.Delete(); playerVBO.Delete();

	// player line

	GLfloat vertices[] =
	{
		pixelToVertex(px, 0), pixelToVertex(py, 1),
		pixelToVertex(px + pdx * 10, 0), pixelToVertex(py + pdy * 10, 1)
	};

	VAO VAO1;
	VAO1.Bind();
	VBO playerLine(vertices, sizeof(vertices));
	VAO1.LinkAttrib(playerLine, 0, 2, GL_FLOAT, 2 * sizeof(float), (void*)0);
	playerLine.Unbind();
	glDrawArrays(GL_LINES, 0, 2);
	VAO1.Unbind();
	VAO1.Delete(); playerLine.Delete();
}

/*
 * Display functions
 */

void init()
{
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	px = 150; py = 400; pa = 90;
	pdx = cos(toRadian(pa)); pdy = sin(toRadian(pa));
}

/*
 * Main function
 */
int main()
{
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "YoutubeOpenGL", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");

	init();

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();
		// Draws the diagram of the map
		drawMap();
		movePlayer(window);
		drawPlayer();
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}