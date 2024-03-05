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
 * Map
 */

// Map constants

int mapX = 8, mapY = 8, mapS = 64;
int map[] =
{
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 1, 0, 0, 1, 0, 1,
	1, 0, 1, 0, 0, 0, 0, 1,
	1, 0, 1, 0, 0, 0, 0, 1,
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
	/*
	 * Rotation keys (D and A)
	 */
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) 
	{
		pa -= 1; pa = fixAngle(pa); pdx = cos(toRadian(pa)); pdy = sin(toRadian(pa)); 
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
	{
		pa += 1; pa = fixAngle(pa); pdx = cos(toRadian(pa)); pdy = sin(toRadian(pa)); 
	}

	/*
	 * Movement keys (W and S)
	 */
	int xOff, yOff;
	if (pdx > 0) { xOff = 20; } else { xOff = -20; }
	// there is a flip because of the Y coordinates
	if (pdy > 0) { yOff = 20; } else { yOff = -20; }
	int pxLoc = px / 64, pxAddOffLoc = (px + xOff) / 64, pxSubOffLoc = (px - xOff) / 64;
	// This is due to the flipped Y coord of the map (This should probably be done differently in the future)
	int pyLoc = 8 - (py / 64), pyAddOffLoc = 8 - (py + yOff) / 64, pySubOffLoc = 8 - (py - yOff) / 64;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		if (map[pyLoc * mapX + pxAddOffLoc] == 0) { px += pdx; }	// if map index in x direction is 0, move
		if (map[pyAddOffLoc * mapX + pxLoc] == 0) { py += pdy; }	// if map index in y direction is 0, move
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		if (map[pyLoc * mapX + pxSubOffLoc] == 0) { px -= pdx; }	// if map index in x direction is 0, move
		if (map[pySubOffLoc * mapX + pxLoc] == 0) { py -= pdy; }	// if map index in y direction is 0, move
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
	glLineWidth(1);
	glDrawArrays(GL_LINES, 0, 2);
	VAO1.Unbind();
	VAO1.Delete(); playerLine.Delete();
}

/*
 * Drawing the rays
 */

float distance(float x1, float y1, float x2, float y2, float angle)
{
	//return cos(toRadian(angle)) * (x2 - x1) - sin(toRadian(angle)) * (y2 - y1);
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void drawRays()
{
	int mx, my, mp, dof, side; float vx, vy, rx, ry, ra, yOff, xOff, Tan, aTan, disV, disH, color;
	ra = pa;  rx = 0.0f; ry = 0.0f; xOff = 0.0f; yOff = 0.0f; color = 0.7f;
	ra = fixAngle(pa + 30);
	for (int r = 0; r < 60; r++)
	{
		/*
		 * Checking Verticals
		 */
		dof = 0; disV = 10000000;
		Tan = tan(toRadian(ra));
		if (cos(toRadian(ra)) > 0.0001f) { rx = (((int)px >> 6) << 6) + 64; ry = (rx - px) * Tan + py; xOff = 64; yOff = xOff * Tan; }//looking left
		else if (cos(toRadian(ra)) < -0.0001f) { rx = (((int)px >> 6) << 6) - 0.0001f; ry = (rx - px) * Tan + py; xOff = -64; yOff = xOff * Tan;	}//looking right
		else { rx = px; ry = py; dof = mapX; }                                                  //looking up or down. no hit  
		while (dof < mapX)
		{
			/* 
			 * DEBUG HELL: mp was messing everything up because the mp index was calculated based on(0, 0)
			 * being the bottom left corner of the map, when in reality the array is designed with a flipped Y 
			 * axis for legibility.  Just needed to invert the Y axis (-1 because indexing starts at 0).
			 */
			mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = (mapY - my - 1) * mapX + mx;
			if (mp > 0 && mp < mapX * mapY && map[mp] == 1) { dof = mapX; disV = distance(px, py, rx, ry, ra); }//hit         
			else { rx += xOff; ry += yOff; dof += 1; }                                               //check next vertical
		}
		vx = rx; vy = ry;
		/*
		 * Checking horizontals
		 */
		dof = 0; disH = 100000000;
		aTan = 1.0f / Tan;
		if (sin(toRadian(ra)) > 0.0001f) { ry = (((int)(py) >> 6) << 6) + 64; rx = (ry - py) * aTan + px; yOff = 64; xOff = yOff * aTan; }
		else if (sin(toRadian(ra)) < -0.0001f) { ry = (((int)(py) >> 6) << 6) - 0.0001f; rx = (ry - py) * aTan + px; yOff = -64; xOff = yOff * aTan; }
		else { rx = px; ry = py; dof = 8; }
		while (dof < mapY)
		{
			mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = (mapY - my - 1) * mapX + mx;
			if (mp > 0 && mp < mapX * mapY && map[mp] == 1) { dof = mapY; disH = distance(px, py, rx, ry, ra); }//hit         
			else { rx += xOff; ry += yOff; dof += 1; }                                               //check next horizontal
		}

		if (disV - disH < 0.000000001) { rx = vx; ry = vy; disH = disV; color = 0.9f; }

		/*
		 * Drawing the Rays
		 */
		GLfloat line[] =
		{
			pixelToVertex(px, 0), pixelToVertex(py, 1), 1.0f,	1.0f, 0.0f, 0.0f,
			pixelToVertex(rx, 0), pixelToVertex(ry, 1), 1.0f,	1.0f, 0.0f, 0.0f
		};

		VAO VAO2;
		VAO2.Bind();
		VBO VBO2(line, sizeof(line));
		VAO2.LinkAttrib(VBO2, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
		VAO2.LinkAttrib(VBO2, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		VBO2.Unbind();
		glLineWidth(1);
		glDrawArrays(GL_LINES, 0, 2);
		VAO2.Unbind();
		VAO2.Delete(); VBO2.Delete();


		/*
		 * Drawing Walls
		 */
		
		int ca = fixAngle(pa - ra); disH = disH * cos(toRadian(ca));			// fix fisheye
		int lineH = (mapS * 320) / disH; if (lineH > 320) { lineH = 320; }		// line height with limit
		int lineOff = 160 - (lineH >> 1);										// line offset

		GLfloat vertices[] =
		{
			pixelToVertex(r * 8 + 530, 0), pixelToVertex(lineOff, 1), 1.0f,	color, 0.0f, 0.0f,
			pixelToVertex(r * 8 + 530, 0), pixelToVertex(lineOff + lineH, 1), 1.0f,	color, 0.0f, 0.0f
		};

		VAO VAO1; 
		VAO1.Bind(); 
		VBO VBO1(vertices, sizeof(vertices)); 
		VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0); 
		VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float))); 
		VBO1.Unbind(); 
		glLineWidth(8); 
		glDrawArrays(GL_LINES, 0, 2); 
		VAO1.Unbind(); 
		VAO1.Delete(); VBO1.Delete(); 

		ra = fixAngle(ra - 1);
	}
}


/*
 * Display functions
 */

void init()
{
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	px = 280; py = 100; pa = 90;
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
		drawRays();
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