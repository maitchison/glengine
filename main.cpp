/*

todo:

[done] terrain
better keyboard controls
house
lights
camera movement 

5 objects
[done] turtle
bird

turtle in water
bird in sky

shadows
skybox

animate the water (plasma)

*/

#define _USE_MATH_DEFINES

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <stdint.h>
#include <algorithm>
#include <limits>
#include <vector>
#include <time.h>

#include "PerlinNoise.hpp"
#include "utils.h"

float lastFrameTime = 0.0f;
float currentTime = 0.0f;
int counter = 0;
int frameOn = 0;

GLuint grass_texture;
GLuint water_texture;

const int TERRAIN_DIVISIONS = 50;

float terrain[TERRAIN_DIVISIONS * TERRAIN_DIVISIONS] = {};

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

siv::PerlinNoise perlin = siv::PerlinNoise();

struct Camera
{
	float x;
	float y;
	float z;

	// camera yAngle in radians.
	float yAngle;

	void apply(void)
	{
		gluLookAt(x, y, z, x + sin(yAngle), y, z + -cos(yAngle), 0, 1, 0);
	}

	void move(float amount)
	{
		x += sin(yAngle) * amount;
		z -= cos(yAngle) * amount;
	}

	void pan(float amount)
	{
		x += cos(yAngle) * amount;
		z += sin(yAngle) * amount;
	}

	Camera()
	{
		x = 0;
		y = 0;
		z = 0;
		yAngle = 0;
	}
};

Camera camera = Camera();

// generates our terrain
void initTerrain(void)
{
	for (int z = 0; z < TERRAIN_DIVISIONS; z++) {
		for (int x = 0; x < TERRAIN_DIVISIONS; x++) {
			terrain[x + z * TERRAIN_DIVISIONS] = 5.0f*perlin.octaveNoise((float)x/21*(100.0 / TERRAIN_DIVISIONS), (float)z / 21 * (100.0 / TERRAIN_DIVISIONS), 32) 
				- 7.0 * (float)x/TERRAIN_DIVISIONS - 3.0 * z / TERRAIN_DIVISIONS + 5.0f;
		}
	}
}

void initTextures(void)
{
	grass_texture = loadTexture("grass.png");
	water_texture = loadTexture("water.png");
}

void initialize(void)
{
	glClearColor(0.1f, 0.0f, 0.5f, 1.0f);
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
	initTextures();
	initTerrain();
}

const float GRID_SPACE = 1.0f / TERRAIN_DIVISIONS * 100;

void terrainPoint(int x, int z)
{
	float height = terrain[x + z * TERRAIN_DIVISIONS];
	//glColor3f(0, height / 5, 0);
	glColor3f(height, height + 0.5, height + 0.5);
	glTexCoord2f((float)x / TERRAIN_DIVISIONS * 10, (float)z / TERRAIN_DIVISIONS * 10);
	glVertex3f((x - (TERRAIN_DIVISIONS / 2)) * GRID_SPACE, height, (z - (TERRAIN_DIVISIONS / 2)) * GRID_SPACE);
}

void drawBox(float x, float y, float z)
{
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x, y, z);
	for (int i = 0; i < 4; i++) 
	{
		glColor3f(0.3*i, 0.7, 0.5);
		glPushMatrix();
		glRotatef(i * 90, 0, 1, 0);
		glTranslatef(0, 0, -0.4);
		glScalef(1, 0.5, 0.1);
		glutSolidCube(1.0);
		glPopMatrix();

		glColor3f(1, 0, 0);
		glPushMatrix();
		glRotatef(i * 90, 0, 1, 0);
		glTranslatef(-0.4, 0, -0.4);
		glScalef(0.25, 0.6, 0.25);
		glutSolidCube(1.1);
		glPopMatrix();
	}
	glPopMatrix();
}

void drawHouse(float x, float y, float z)
{
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x, y, z);

	glScalef(10,10,10);

	//todo : doors and windows, maybe do this as polygons. (plan out on paper)

	for (int i = 0; i < 4; i++)
	{
		glColor3f(0.3*i, 0.7, 0.5);
		glPushMatrix();
		glRotatef(i * 90, 0, 1, 0);
		glTranslatef(0, 0, -0.4);
		glScalef(1, 0.5, 0.1);
		glutSolidCube(1.0);
		glPopMatrix();

		glColor3f(1, 0, 0);
		glPushMatrix();
		glRotatef(i * 90, 0, 1, 0);
		glTranslatef(-0.4, 0, -0.4);
		glScalef(0.25, 0.6, 0.25);
		glutSolidCube(1.1);
		glPopMatrix();
	}

	glPopMatrix();
}

/** swept axis object. */
void drawTrinket(float x, float y, float z)
{
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x, y, z);
	glPopMatrix();
}


void drawTurtle(float x, float y, float z)
{
	glMatrixMode(GL_MODELVIEW);

	// shell
	glLoadIdentity();
	glTranslatef(x, y, z);
	glColor3f(0, 0.5, 0);
	glScalef(1, 0.3, 1);
	glutSolidCube(1.0);

	// body
	glLoadIdentity();
	glTranslatef(x, y-0.2, z);
	glColor3f(0.65, 0.55, 0.60);
	glScalef(0.8, 0.2, 0.9);
	glutSolidCube(1.0);

	// arms
	glLoadIdentity();
	glTranslatef(x - 0.5, y - 0.2, z);
	glColor3f(0.65, 0.95, 0.60);
	glScalef(0.6, 0.1, 0.2);
	glutSolidCube(1.0);

	glLoadIdentity();
	glTranslatef(x + 0.5, y - 0.2, z);
	glColor3f(0.65, 0.95, 0.60);
	glScalef(0.6, 0.1, 0.2);
	glutSolidCube(1.0);

	// head
	glLoadIdentity();
	glTranslatef(x, y - 0.2, z + 0.6);
	glColor3f(0.65, 0.95, 0.60);
	glScalef(0.25, 0.25, 0.25);
	glutSolidCube(1.0);

}

void drawTerrain(void)
{

	glPushMatrix();
	glTranslatef(0, -1.5, 0);

	// the water
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, water_texture);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-50, 0, -50);
	glTexCoord2f(7, 0);
	glVertex3f(+50, 0, -50);
	glTexCoord2f(7, 7);
	glVertex3f(+50, 0, +50);
	glTexCoord2f(0, 7);
	glVertex3f(-50, 0, +50);
	glDisable(GL_TEXTURE_2D);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

	// the ground

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, grass_texture);

	glBegin(GL_QUADS);
	for (int z = 0; z < TERRAIN_DIVISIONS-1; z++) {
		for (int x = 0; x < TERRAIN_DIVISIONS-1; x++) {
			terrainPoint(x, z);
			terrainPoint(x + 1, z);
			terrainPoint(x + 1, z + 1);
			terrainPoint(x, z + 1);			
		}				
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1, 1, 1);

	// setup perspective
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1, 1000);

	// setup camera transformation	
	camera.apply();

	// draw the scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, -20, -100);
	drawTerrain();

	drawHouse(0, 0, 0);

	//drawTurtle(0, 0, -10);
	drawBox(0,0,-10);

	glFlush();
}

void update(void)
{
	currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float elapsed = currentTime - lastFrameTime;

	if (elapsed < (10.0 / 1000.0)) {
		return;
	}

	counter++;

	if (counter % 100 == 0) {
		printf("fps %f\n", 1.0 / elapsed);
	}

	glutPostRedisplay();
	lastFrameTime = currentTime;

}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		camera.move(1.0f);
		break;
	case 's':
		camera.move(-1.0f);
		break;
	case 'q':		
		camera.yAngle -= 0.1f;
		break;
	case 'e':
		camera.yAngle += 0.1f;
		break;
	case 'a':
		camera.pan(-1.0f);
		break;
	case 'd':
		camera.pan(1.0f);
		break;
	case ' ':
		camera.y += 1.0f;
		break;
	case 'c':
		camera.y -= 1.0f;
		break;
	}
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("MYHOUSE");
	initialize();
	glutDisplayFunc(display);
	glutIdleFunc(update);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}