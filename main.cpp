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

#include "stdinc.h"

#include "PerlinNoise.hpp"
#include "utils.h"

#include "scenegraph.h"

float lastFrameTime = 0.0f;
float currentTime = 0.0f;
int counter = 0;
int frameOn = 0;

GLuint grass_texture;
GLuint water_texture;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

Camera camera = Camera();
SceneGraph graph = SceneGraph();

void initTextures(void)
{
	grass_texture = loadTexture("grass.png");
	water_texture = loadTexture("water.png");
}

void initTerrain(void)
{
	graph.Add(new Cube());
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

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1, 1, 1);

	// setup perspective
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1, 1000);

	// setup camera transformation
	graph.Render(camera);

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
