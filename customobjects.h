#pragma once

#include "stdinc.h"
#include "scenegraph.h"

class Bird: public Object
{
public:
    Bird();
};

class Turtle: public Object
{
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
