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

GLuint texId[6];

//---------------------------------------------------------------

// house object

class Turtle: public Object
{
public:
    Turtle() : Object()
    {
        // shell
        Object* shell = new Cube();
        shell->material = new Material(Color(0, 0.5, 0));
	    shell->scale = Vec3(1, 0.3, 1);	    
        Add(shell);

	    // body
        Object* body = new Cube();
	    body->position = Vec3(0, -0.2, 0);
	    body->material = new Material(Color(0.65, 0.55, 0.60));
	    body->scale = Vec3(0.8, 0.2, 0.9);
        Add(body);
	            
        // arms
        Object* leftArm = new Cube();
        leftArm->position = Vec3(- 0.5, - 0.2, 0);
        leftArm->material = new Material(Color(0.65, 0.95, 0.60));
	    leftArm->scale = Vec3(0.6, 0.1, 0.2);
        Add(leftArm);

        Object* rightArm = new Cube();
        rightArm->position = Vec3(+0.5, - 0.2, 0);
        rightArm->material = new Material(Color(0.65, 0.95, 0.60));
	    rightArm->scale = Vec3(0.6, 0.1, 0.2);
        Add(rightArm);

        // head
        Object* head = new Cube();            
        head->position = Vec3(0, - 0.2, 0.6);
        head->material = new Material(Color(0.65, 0.95, 0.60));
        head->scale = Vec3(0.25, 0.25, 0.25);        
        Add(head);

    }
};

class Bird: public Object
{
public:
    Bird() : Object()
    {
        // body
        Object* body = new Cube();
        body->material = new Material(Color(0.9, 0.5, 0.45));
	    body->scale = Vec3(1, 2, 0.8);	    
        Add(body);
	    
        // head
        Object* head = new Cube();            
        head->position = Vec3(0, +1.0, 0.6);
        head->material = new Material(Color(0.9, 0.65, 0.60));
        head->scale = Vec3(0.65, 0.45, 0.95);        
        head->rotation.x = 15;
        Add(head);

        // wings
        Object* leftWing = new Cube();            
        leftWing->position = Vec3(0.5, 0.35, 0.2);
        leftWing->material = new Material(Color(0.2, 0.6, 0.2));
        leftWing->scale = Vec3(0.15, 1.5, 0.75);                
        leftWing->anchor = Vec3(0,-0.5, -0.5);
        leftWing->rotation.x = -155;
        Add(leftWing);

        Object* rightWing = new Cube();            
        rightWing->position = Vec3(-0.5, 0.35, 0.2);
        rightWing->material = new Material(Color(0.2, 0.6, 0.2));
        rightWing->scale = Vec3(0.15, 1.5, 0.75);                
        rightWing->anchor = Vec3(0,-0.5, -0.5);
        rightWing->rotation.x = -155;
        Add(rightWing);

        scale = Vec3(0.3,0.3,0.3);

    }

};

class FancyBox: public Object
{
public:
    FancyBox() : Object()
    {
        // box
        Object* box = new Cube();
        box->material = new Material(Color(0.5, 0.5, 0.5));	    
        Add(box);
	    
        // spheres
        Object* sphere1 = new Sphere();
        sphere1->position = Vec3(-2, 0, 0);
        sphere1->scale = Vec3(0.2,0.2,0.2);
        Add(sphere1);

        Object* sphere2 = new Sphere();
        sphere2->position = Vec3(+2, 0, 0);
        sphere2->scale = Vec3(0.2,0.2,0.2);
        Add(sphere2);

        Object* sphere3 = new Sphere();
        sphere3->position = Vec3(0, -2, 0);
        sphere3->scale = Vec3(0.2,0.2,0.2);
        Add(sphere3);        

    }

};



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

// Renders skybox on a very far away cube.
class Skybox : public Object
{
public:
    Skybox(GLuint textures[6])
    {
        Quad* quad;

        float size = 1000;
        
        //front
        quad = new Quad(Vec3(0,0,size),size*2,size*2);
        quad->rotation.y = 180;
        quad->material = new Material(textures[1]);         
        Add(quad);
        //left
        quad = new Quad(Vec3(+size,0,0),size*2,size*2);
        quad->rotation.y = -90;
        quad->material = new Material(textures[0]);        
        Add(quad);
        //right
        quad = new Quad(Vec3(-size,0,0),size*2,size*2);
        quad->rotation.y = +90;
        quad->material = new Material(textures[2]);
        Add(quad);
        //back
        quad = new Quad(Vec3(0,0,-size),size*2,size*2);
        quad->rotation.y = 0;
        quad->material = new Material(textures[3]);
        Add(quad);
        //top
        quad = new Quad(Vec3(0,size,0),-size*2,size*2);
        quad->rotation.x = -90;
        quad->rotation.z = 0;
        quad->material = new Material(textures[4]);
        Add(quad);
        //bottom
        quad = new Quad(Vec3(0,-size,0),-size*2,size*2);
        quad->rotation.x = -90;
        quad->rotation.z = 0;
        quad->material = new Material(textures[5]);
        Add(quad);
    }
};

//---------------------------------------------------------------

void initTextures(void)
{
	grass_texture = loadTexture("grass.png");
	water_texture = loadTexture("water.png");
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
    /*
    Turtle* turtle = new Turtle();
    turtle->position = Vec3(0,0,0);
    graph.Add(turtle);
    */

    /*
    Bird* bird = new Bird();
    bird->position = Vec3(0,0,0);
    graph.Add(bird);
    */
    
    FancyBox* box = new FancyBox();
    graph.Add(box);

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
	
	//initHouse();

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
