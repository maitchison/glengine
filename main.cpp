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
[done] put 5 objects in house


[done] collision detection
[done] trilinear filtering
[done] collisions and simple physics
[done] camera modes

[...] rotating spotlight on lighthouse
[...] simple shadows ?

*/

/*
Sources
earth: lab3
moon: texture.com
Skybox is from https://reije081.home.xs4all.nl/skyboxes/ [edited]
*/

#include "stdinc.h"
#include "loadTGA.h"

#include "PerlinNoise.hpp"
#include "utils.h"

#include "scenegraph.h"
#include "customobjects.h"
#include "player.h"

float lastFrameTime = 0.0f;
float currentTime = 0.0f;
int counter = 0;
int frameOn = 0;

// records if key is down
bool key[256];
// this handle special keys
bool specialKey[256];

GLuint grass_texture;
GLuint water_texture;
GLuint sand_texture;
GLuint dirt_texture;
GLuint globe_texture;
GLuint moon_texture;

siv::PerlinNoise noise = siv::PerlinNoise(123);

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

Camera camera = Camera();
SceneGraph graph = SceneGraph();
Player player = Player(&graph);
Light* sunLight;
Light* cameraLight;
Light* spotLight;

Object* skybox;
Object* bird;
WaterPlane* ocean;

float vVel = 0.0;

const int CM_PLAYER = 0;
const int CM_SECURITY = 1;
const int CM_BIRD = 2;

const int RM_NORMAL = 0;
const int RM_OVERLAY_WIRE = 1;
const int RM_WIRE = 2;

const int LM_DAY = 0;
const int LM_NIGHT = 1;

int cameraMode = CM_PLAYER;
int renderMode = RM_NORMAL;
int lightsMode = LM_DAY;
bool hqMode = false;

GLuint texId[6];

void initTextures(void)
{
	grass_texture = loadTexture("grass.png");
	water_texture = loadTexture("water.png");
    globe_texture = loadTexture("earth.png");
	moon_texture = loadTexture("moon.png");    
    sand_texture = loadTexture("sand.png");    
    dirt_texture = loadTexture("dirt.png");    
}

void loadSkyBoxTexture(char* filename, GLuint texId)
{
    printf("loading %s\n", filename);
    glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	loadTGA(filename);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
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

    skybox = new Skybox(texId);
    graph.Add(skybox);

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
    glEnable(GL_LIGHT1);

    // took ages to find this!  0.2 default ambient is really not the best.
    float ambientLight[4] = {0,0,0,1};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

    // this helps things a bit by correctly calculating the eye view when translating.
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, -1);
    
	sunLight = new Light(GL_LIGHT0);
	sunLight->position = Vec3(-500, +70, -500);
    sunLight->color = Color(1,1,1);    
	graph.AddLight(sunLight);
    
    cameraLight = new Light(GL_LIGHT1);
    cameraLight->color = Color(1,1,1);
    cameraLight->attenuate = true;
	graph.AddLight(cameraLight);

    spotLight = new Light(GL_LIGHT2);
    spotLight->color = Color(1,1,0.5);
    spotLight->spot = true;
    spotLight->position = Vec3(1.5, 25, 0.5);
	graph.AddLight(spotLight);
    
}

void initTerrain(void)
{
	// we generate blocks as follows...
	// first calculate the height values.
	// then put grass on top, and dirt under
	// sand is used for water level.

	Material* grass = new Material(grass_texture);
	Material* dirt = new Material(dirt_texture);
	Material* sand = new Material(sand_texture);
	Material* water = new Material(water_texture);

    water->diffuse.a = 0.95f;	

    Object* terrain = new Object();

	for (int x = 0; x < 41; x ++) {
		for (int z = 0; z < 41; z ++) {

			int height = (int)(10.0*noise.octaveNoise(x/100.0, z/100.0, 8) - 3.0);

            // make sure height is correct around the house
            if ((abs(x-24) <= 4) && (abs(z-20) <= 5)) {
                height = 1;
            }

			if (height <= 1) {
				// sand at low heights
				Cube* cube = new Cube();
				cube->position = Vec3(x-20,height,z-20);
				cube->material = sand;
                cube->solid = true;
				terrain->Add(cube);
            
			} else {
				// grass on top, dirt under 
				Cube* cube = new Cube();
				cube->position = Vec3(x-20,height,z-20);
				cube->material = grass;
                cube->solid = true;
				terrain->Add(cube);

				cube = new Cube();
				cube->position = Vec3(x-20,height - 1,z-20);
				cube->material = dirt;
                cube->solid = true;
				terrain->Add(cube);
			}

            // add a little depth around the boarder
            if (x == 0 || z == 0 || x == 40 || z == 40) {
                for (int y = height-1; y > -5; y--) {
                    Cube* cube = new Cube();
                    cube->position = Vec3(x-20,y,z-20);
                    cube->material = sand;
                    cube->solid = true;
                    terrain->Add(cube);
                }                    
            }

		}
	}
    
    terrain->position.y = -1.5;
    terrain->solid = true;
    graph.Add(terrain);

	// add water plane
	ocean = new WaterPlane();
	ocean->position = Vec3(0,-4,0);
	ocean->scale = Vec3(200,2,200);
    ocean->material = water;
    ocean->divisionsX = 64;
    ocean->divisionsY = 64;
    graph.Add(ocean);

    // add safety floor (so we don't fall through)
    Object* floor = new Cube(200,2,200);
    floor->position = Vec3(0,-5,0);
    floor->visible = false;
    floor->solid = true;
	
    graph.Add(floor);
    

}

void initHouse(void)
{
	House* house = new House();
	graph.Add(house);
}

void initPlayer()
{
    player.position = Vec3(11, 5.0, 1.2);
    player.yAngle = -20;    
}

void initialize(void)
{
	glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
 	initLights();
	initTextures();
    initPlayer();
    initTerrain();
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

	graph.Render(camera, renderMode != RM_WIRE, renderMode != RM_NORMAL);

	glutSwapBuffers();
}

void updateInput()
{
    float moveSpeed = 0.1f;
    float turnSpeed = 0.05f;    
    if (key['w'] || specialKey[GLUT_KEY_UP]) player.moveForward(moveSpeed);
    if (key['s'] || specialKey[GLUT_KEY_DOWN]) player.moveForward(-moveSpeed);
	if (key['q'] || specialKey[GLUT_KEY_LEFT]) player.turn(-turnSpeed);
    if (key['e'] || specialKey[GLUT_KEY_RIGHT]) player.turn(+turnSpeed);
    if (key['a']) player.pan(-moveSpeed);
    if (key['d']) player.pan(+moveSpeed);
    if (key['c']) player.height = 0.5; else player.height = 1.2;
    if (key[' ']) player.jump();	
}


void update(void)
{
	currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float elapsed = currentTime - lastFrameTime;

	if (elapsed < (10.0 / 1000.0)) {
		return;
	}

    // limit step to a tenth of a second.  Otherwise we get problems if the OS swaps us out for a few seconds.
    if (elapsed > 0.1) {
        elapsed = 0.1;
	}

	counter++;

	if (counter % 100 == 0) {
		printf("fps %f pos (%f,%f,%f)\n", 1.0 / elapsed, camera.x, camera.y, camera.z);
	}    

    // update the camera, make sure same is behind player a little.
    switch (cameraMode) {
        case CM_PLAYER:
            updateInput();
            player.update(elapsed);
            camera.x = player.position.x + sin(-player.yAngle) * 0.2;
            camera.y = player.position.y + player.height;
            camera.z = player.position.z + cos(-player.yAngle) * 0.2;
            camera.yAngle = player.yAngle;
            break;
        case CM_SECURITY:
            camera.x = 7.4;
            camera.y = 2;
            camera.z = -2.5;
            camera.yAngle = sin(currentTime / 2) * 0.6 + 4;
            camera.xAngle = -0.1;
            break;
        case CM_BIRD:
            // sit just behind bird
            camera.yAngle = (bird->rotation.z + 180) * (M_PI/180);            
            camera.x = bird->position.x - sin(camera.yAngle) * 0.5;
            camera.y = bird->position.y+0.03f;
            camera.z = bird->position.z + cos(camera.yAngle) * 0.5;            
            break;
    }

    // setup our lights
    switch (lightsMode) {
        case LM_DAY: 
            sunLight->visible = true;
            skybox->visible = true;
            break;
        case LM_NIGHT:
            sunLight->visible = false;            
            skybox->visible = false;
            break;
    }

    // update spotlight
    spotLight->rotation.y += elapsed * 10;
    
    graph.Update(elapsed);

    // update the birds location
    if (bird) {
        float theta = currentTime;
        float phi = 3 * currentTime;
        float radius = 1.0;
        bird->position = Vec3(4 + sin(theta) * radius, 1.5 + sin(phi) * 0.1, 0 + cos(theta) * radius);
        bird->rotation.z = -90 - theta / (M_PI / 180);    
    }

    // update camera light position
    cameraLight->position = player.position;
    cameraLight->position.y += player.height;
    
	glutPostRedisplay();
	lastFrameTime = currentTime;

}

void keyboardDown(unsigned char code, int x, int y)
{
    key[code] = true;
}

void keyboardSpecialDown(int code, int x, int y)
{
    // process special function keys.
    specialKey[code & 0xff] = true;
    switch (code) {
        case GLUT_KEY_F1:            
            cameraMode = (cameraMode + 1) % 3;
            printf("Switching camera mode to %d\n", cameraMode);
            break;
        case GLUT_KEY_F2:            
            lightsMode = (lightsMode + 1) % 2;
            printf("Switching lighting mode to %d\n", lightsMode);
            break;
        case GLUT_KEY_F3:            
            renderMode = (renderMode + 1) % 3;
            printf("Switching render mode to %d\n", renderMode);
            break;
        case GLUT_KEY_F4:            
            player.flying = ! player.flying;
            printf("Flying mode %d\n", player.flying);
            break;
        case GLUT_KEY_F5:            
            hqMode = !hqMode;
            printf("HQ mode %d\n", hqMode);
            DIVISONS_MULTIPLIER = hqMode ? 3 : 1;
            // ocean is actually really slow, need to optimize this more to increase the resolution.
            ocean->divisionsX = ocean->divisionsY = hqMode ? 64 : 32;        
	
            break;
    }
}

void keyboardUp(unsigned char code, int x, int y)
{
    key[code] = false;
}

void keyboardSpecialUp(int code, int x, int y)
{
    specialKey[code & 0xff] = false;
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Lighthouse");
	initialize();
	glutDisplayFunc(display);
	glutIdleFunc(update);
    glutSpecialFunc(keyboardSpecialDown);
    glutSpecialUpFunc(keyboardSpecialUp);
	glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutMainLoop();
	return 0;
}
