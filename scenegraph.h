#pragma once

/**
3d Objects used as part of a scene graph
*/

#include <vector>

#include "stdinc.h"

class Object;
class Camera;

class SceneGraph
{
protected:
    std::vector<Object*> objects;
public:
    /** Adds object to scenegraph. */
    void Add(Object* object);

    /** Draws all objects in the given scenegraph as seen by this camera. */
    void Render(Camera camera);
};

/** Defines a camera. */
struct Camera
{

public:
	float x;
	float y;
	float z;

	// camera yAngle in radians.
	float yAngle;

	void apply(void);

	void move(float amount);

	void pan(float amount);

	Camera();

};

class Material
{
public:
    Color ambient;
    Color diffuse;
    Color specular;
    Color emission;
    float shininess;
public:
    Material();
    Material(Color color) : Material() { ambient = diffuse = color; };
    void Apply(void);
};


class Object
{
protected:
    /** Handles custom part of object draw. */
    virtual void drawObject(void);

public:
    /** Location of the object. */
    Vec3 position;

    /** euler rotations. */
    Vec3 rotation;

    /** scales. */
    Vec3 scale;

    /** color, used when no material is provided. */
    Color color;

    /** material, maybe null */
    Material* material = NULL;

    /** Draws the object to current view. */
    virtual void Draw(void);

public:
    Object();
    Object(Vec3 position);

};

class Light: public Object
{

protected:
    GLuint lightid;

protected:
    void drawObject(void) override;

public:
    Light(GLuint lightid) : Object() { this->lightid = lightid; }
};

class Cube: public Object
{
    void drawObject(void) override;
public:
    Cube() : Object() {};
};

class Sphere: public Object
{
    void drawObject(void) override;
};

class SurfaceOfRevolution: public Object
{
protected:
    std::vector<Vec2> points;
    int slices = 16;

public:

    SurfaceOfRevolution();
     SurfaceOfRevolution(std::vector<Vec2> points);

protected:
     void drawObject(void) override;
};
