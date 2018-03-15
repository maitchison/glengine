/*

today:

build house
[done] surface of revolution
skybox

later (for fun):
proper day / night cycle
physics based movement (i.e. jumping, walking up blocks etc)
textures on terrain
5 objects inside house

roaming animals (turtles and birds)
graphic for sun (just a yellow sphere)
alternative camera view (from turtles perspective)
skybox


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
siv::PerlinNoise noise = siv::PerlinNoise(123);

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

Camera camera = Camera();
SceneGraph graph = SceneGraph();
Light* sunLight;

//---------------------------------------------------------------

// house object

class House: public Object
{
private:
	// create a new block
	void block(Vec3 bottomLeft, Vec3 topRight, Material* material = NULL)
	{
        Vec3 mid = Vec3((bottomLeft.x + topRight.x) / 2, (bottomLeft.y + topRight.y) / 2, (bottomLeft.z + topRight.z) / 2);
        Vec3 size = Vec3(-(bottomLeft.x - topRight.x), -(bottomLeft.y - topRight.y), -(bottomLeft.z - topRight.z));
		Object* block = new Cube();
        block->scale = size;
		block->position = mid;
		block->material = material;
		Add(block);
	}

    void marker(Vec3 pos, Color color = Color(1,0,0))
    {
        float size = 1;
        Object* block = new Cube();
        block->scale = Vec3(size,size,size);
		block->position = pos;
		block->material = new Material();
        block->material->emission = color;
		Add(block);
    }

    void wall(Vec2 p1, Vec2 p2, float thickness, float startHeight, float endHeight, Material* material = NULL)
    {
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        float wallLength = sqrtf(dx*dx + dy*dy);
        float wallAngle = atan2(dy, dx) / M_PI * 180.0 + 90;

        Object* wall = new Cube();
        wall->anchor = Vec3(0, -0.5, +0.5);
        wall->position = Vec3(p1.x, startHeight, p1.y);
        wall->scale = Vec3(thickness, endHeight - startHeight, wallLength);
        wall->rotation = Vec3(0, -wallAngle, 0);
        wall->material = material;
        Add(wall);

    }

public:
	House()
	{

        
		Object* tower = new Cylinder(2, 21);
        Add(tower);

		// the light
		Object* light = new Cylinder(1.5, 3);		
		light->position.y = 21;
		light->material = new Material();
		light->material->emission = Color(1,0.9,0.7);
		Add(light);

		// the cap
		Object* cap = new Cylinder(2, 1.5);		
		cap->position.y = 23;
		Add(cap);

		// tower rings
		Material* ringMaterial = new Material();
		ringMaterial->diffuse = Color(0.5,0.5,0.5);
		ringMaterial->specular = Color(0,0,0);
		
		for (int i = 0; i < 4; i ++) 
		{
			Object* ring = new Cylinder(2.1,0.4);
			ring->position.y = i * 7;
			ring->material = ringMaterial;
			Add(ring);
		}
        
		// building walls

		Material* wallMaterial = new Material();
		wallMaterial->diffuse = Color(1,0,0);

        float wallThickness = 0.15;
        float houseLength = 8.0;
        float houseWidth = 3.0;
        float houseHeight = 3.0;

        // -----------------------------------
		// main room
        
        // back wall
        wall(Vec2(0,-3), Vec2(houseLength,-3), wallThickness, 0, 3);

        // front wall
        wall(Vec2(0,3), Vec2(8,3), wallThickness, 0, 0.5);
        wall(Vec2(0,3), Vec2(8,3), wallThickness, 2.5, 3);
        wall(Vec2(0,3), Vec2(1,3), wallThickness, 0.5, 2.5);
        wall(Vec2(7,3), Vec2(8,3), wallThickness, 0.5, 2.5);

        // entrance wall
        wall(Vec2(8,-3), Vec2(8,-2), wallThickness, 0, 3);
        wall(Vec2(8,-2), Vec2(8,2), wallThickness, 2.2, 3);
        wall(Vec2(8,2), Vec2(8,3), wallThickness, 0, 3);
        
        // far wall
        wall(Vec2(0,-3), Vec2(0,3), wallThickness, 0, 3);

        // floor
        block(Vec3(0, 0,-3), Vec3(8,wallThickness, 3));    

        // roof
        block(Vec3(0, 3,-3), Vec3(8,3-wallThickness, 3));    

        // pillars (cover the joins)
        Add(new Cylinder(Vec3(0,0,-3), wallThickness, 3));
        Add(new Cylinder(Vec3(0,0,+3), wallThickness, 3));
        Add(new Cylinder(Vec3(8,0,-3), wallThickness, 3));
        Add(new Cylinder(Vec3(8,0,+3), wallThickness, 3));


		
	}    
};


//---------------------------------------------------------------

void initTextures(void)
{
	grass_texture = loadTexture("grass.png");
	water_texture = loadTexture("water.png");
}

void initLights(void)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_COLOR_MATERIAL);

	sunLight = new Light(GL_LIGHT0);
	sunLight->position = Vec3(10, 20, 0);
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

	initHouse();

}

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1, 1, 1);

	// setup perspective
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1, 1000);

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

	// stub: update light position
	sunLight->position = Vec3(sin(currentTime)*30, cos(currentTime)*30,0);

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
