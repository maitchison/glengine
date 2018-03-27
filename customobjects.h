#pragma once

#include "stdinc.h"
#include "scenegraph.h"

class Bird: public Object
{
protected:
    float timer = 0;
    Object* leftWing;
    Object* rightWing;
    void updateObject(float elapsed);    
public:
    Bird();

};

class Turtle: public Object
{
private:
    Object* leftArm;
    Object* rightArm;
    float timer = 0;
protected:
    void updateObject(float elapsed);    
public:
    Turtle();
};

class GlobeAndMoon: public Object
{
private:
    Object* globe;
    Object* moon;
protected:
    void updateObject(float elapsed);
public:
    GlobeAndMoon(GLuint globeTexture, GLuint moonTexture);
};

// Toy lighthouse with a rotating spotlight
class ToyLighthouse: public Object
{
private:
    Object* light;
protected:
    void updateObject(float elapsed);
public:
    ToyLighthouse();
};

// A chest that can open and close
class Chest: public Object
{
private:    
    float timer = 0;
    Cylinder* lid;
protected:
    void updateObject(float elapsed);
public:
    Chest();
};

class House: public Object
{
private:
	// create a new block
	void block(Vec3 bottomLeft, Vec3 topRight, Material* material = NULL);
	
    void marker(Vec3 pos, Color color = Color(1,0,0));
    
    void wall(Vec2 p1, Vec2 p2, float thickness, float startHeight, float endHeight, Material* material = NULL);
    
public:
	House();	
};


// Renders skybox on a very far away cube.
class Skybox : public Object
{
public:
    Skybox(GLuint textures[6]);
};

class WaterPlane: public Plane
{
private:
    float timer = 0;
    // get height at given location.
    float sample(float x, float y);

protected:
    void updateObject(float elapsed);
    void drawObject();
};