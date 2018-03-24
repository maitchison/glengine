/*

Animated objects

1. Turtle
2. Bird (moves around path)
3. World and moons
4. Vase (surface of revolution, maybe it spins?)
5. Opening Chest




today:

[done] build house
[done] surface of revolution
[done] skybox
put 5 objects in house


collision detection
simple shadows

later (for fun):
proper day / night cycle
physics based movement (i.e. jumping, walking up blocks etc)



roaming animals (turtles and birds)
graphic for sun (just a yellow sphere)
alternative camera view (from turtles perspective)

resources
earth: lab3
moon: texture.com

*/

/*
Sources

Skybox is from https://reije081.home.xs4all.nl/skyboxes/ [edited]
*/

#include "stdinc.h"
#include "loadTGA.h"

#include "PerlinNoise.hpp"
#include "utils.h"

#include "scenegraph.h"
#include "customobjects.h"

float lastFrameTime = 0.0f;
float currentTime = 0.0f;
int counter = 0;
int frameOn = 0;

GLuint grass_texture;
GLuint water_texture;
GLuint globe_texture;
GLuint moon_texture;
siv::PerlinNoise noise = siv::PerlinNoise(123);

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

Camera camera = Camera();
SceneGraph graph = SceneGraph();
Light* sunLight;
Object* bird;

GLuint texId[6];

void initTextures(void)
{
	grass_texture = loadTexture("grass.png");
	water_texture = loadTexture("water.png");
    globe_texture = loadTexture("earth.png");
	moon_texture = loadTexture("moon.png");    

}

void loadSkyBoxTexture(char* filename, GLuint texId)
{
    printf("loading %s\n", filename);
    glBindTexture(GL_TEXTURE_2D, texId);
	loadTGA(filename);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

void initSkyBox()
{
	glGenTextures(6, texId); 		
	loadSkyBoxTexture("sky_left.tga", texId[0]);
	loadSkyBoxTexture("sky_front.tga", texId[1]);
	loadSkyBoxTexture("sky_right.tga", texId[2]);
	loadSkyBoxTexture("sky_back.tga", texId[3]);
	loadSkyBoxTexture("sky_top.tga", texId[4]);
	loadSkyBoxTexture("sky_down.tga", texId[5]);

    graph.Add(new Skybox(texId));

}

void initAnimatedModels()
{
    // table for items
    Material* tableMaterial = new Material(Color(0.5,0.5,1.0));    
    tableMaterial->ambient = Color(Color(0.5,0.5,1.0));
    tableMaterial->diffuse = Color(Color(0.5,0.5,1.0));
    tableMaterial->specular = Color(Color(0.0,0.0,0.0));

    Object* tableBase = new Cube(1,1,1);
    tableBase->material = tableMaterial;
    tableBase->position = Vec3(4,0,0);    
    graph.Add(tableBase);

    Object* table = new Cube(2.5,0.1,2);
    table->material = tableMaterial;
    table->position = Vec3(4,0.5,0);    
    
    graph.Add(table);

    // chest of gold
    Object* chest = new Chest();
    chest->position = Vec3(4-0.5,0.5+0.15,0+.5);
    chest->rotation.y = 180;
    chest->scale = Vec3(0.15,0.15,0.15);
    graph.Add(chest);

    // globe
    Object* globe = new GlobeAndMoon(globe_texture, moon_texture);
    globe->position = Vec3(4+0.5,0.5+0.25,0+.5);
    globe->scale = Vec3(0.15,0.15,0.15);
    graph.Add(globe);

    // turtle
    Object* turtle = new Turtle();
    turtle->position = Vec3(4+0.5,0.5+0.15,0-0.5);
    turtle->rotation.y = 160;
    graph.Add(turtle);

    // toy lighthouse
    Object* toy = new ToyLighthouse();
    toy->position = Vec3(4-0.5,0.5,0-0.5);    
    graph.Add(toy);

    // flying bird
    bird = new Bird();
    bird->position = Vec3(4,1.5,0);    
    bird->scale = Vec3(0.1,0.1,0.1);
    bird->rotation.x = 90;

    graph.Add(bird);


}

void initLights(void)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	sunLight = new Light(GL_LIGHT0);
	sunLight->position = Vec3(-500, +70, -500);
	graph.Add(sunLight);
}

void initTerrain(void)
{
	// we generate blocks as follows...
	// first calculate the height values.
	// then put grass on top, and dirt under
	// sand is used for water level.


	Material* grass = new Material(Color(0x009933));
	Material* dirt = new Material(Color(0x996633));
	Material* sand = new Material(Color(0xcccc00));
	Material* water = new Material(Color(0x2964c4));

	water->diffuse.a = 0.95f;
	water->shininess = 40;

	for (int x = 0; x < 41; x ++) {
		for (int z = 0; z < 41; z ++) {

			int height = (int)(10.0*noise.octaveNoise(x/100.0, z/100.0, 8) - 3.0);

			if (height <= 1) {
				// grass on top, dirt under neith
				Cube* cube = new Cube();
				cube->position = Vec3(x-20,height,z-20);
				cube->material = sand;
				graph.Add(cube);
			} else {
				// grass on top, dirt under neith
				Cube* cube = new Cube();
				cube->position = Vec3(x-20,height,z-20);
				cube->material = grass;
				graph.Add(cube);

				cube = new Cube();
				cube->position = Vec3(x-20,height - 1,z-20);
				cube->material = dirt;
				graph.Add(cube);
			}
		}
	}

	// add water plane
	Cube* ocean = new Cube();
	ocean->position = Vec3(0,-1.25,0);
	ocean->scale = Vec3(41.1,5,41.1);
	ocean->material = water;
	graph.Add(ocean);

}

void initHouse(void)
{
	House* house = new House();
	graph.Add(house);
}

void initCamera()
{
    camera.x = 7.37;
    camera.y = 1.0;
    camera.z = 1.5;
    camera.yAngle = -20;    
}

void initialize(void)
{
	glClearColor(0.1f, 0.0f, 0.5f, 1.0f);
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
	initLights();
	initTextures();
    initCamera();
    //initTerrain();
    initSkyBox();
    initAnimatedModels();	
	initHouse();

}

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1, 1, 1);

	// setup perspective
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1, 2000);

	graph.Render(camera);

	glutSwapBuffers();
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
		printf("fps %f pos (%f,%f,%f)\n", 1.0 / elapsed, camera.x, camera.y, camera.z);
	}

    graph.Update(elapsed);

    // update the birds location
    float theta = currentTime;
    float phi = 3 * currentTime;
    float radius = 1.0;
    bird->position = Vec3(4 + sin(theta) * radius, 1.5 + sin(phi) * 0.1, 0 + cos(theta) * radius);
    bird->rotation.z = -90 - theta / (M_PI / 180);


	
	glutPostRedisplay();
	lastFrameTime = currentTime;

}

void keyboard(unsigned char key, int x, int y)
{
    float moveSpeed = 0.2f;
    float turnSpeed = 0.09f;
	switch (key)
	{
	case 'w':
		camera.move(moveSpeed);
		break;
	case 's':
		camera.move(-moveSpeed);
		break;
	case 'q':
		camera.yAngle -= turnSpeed;
		break;
	case 'e':
		camera.yAngle += turnSpeed;
		break;
	case 'a':
		camera.pan(-moveSpeed);
		break;
	case 'd':
		camera.pan(moveSpeed);
		break;
	case ' ':
		camera.y += moveSpeed;
		break;
	case 'c':
		camera.y -= moveSpeed;
		break;
	}
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
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
