#pragma once

/**
3d Objects used as part of a scene graph
*/

#include <vector>

#include "stdinc.h"

class Object;
class Camera;
class Light;

class SceneGraph
{
protected:
    std::vector<Object*> objects;
    std::vector<Light*> lights;
public:
    /** Adds object to scenegraph. */
    void Add(Object* object);

    void AddLight(Light* light);

    /** Draws all objects in the given scenegraph as seen by this camera. */
    void Render(Camera camera);

    /** Updates all objects animation over the given number seconds. */
    void Update(float elapsed);
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
    GLuint textureId;
    bool disableLighting;    
public:
    Material();
    Material(Color color) : Material() { ambient = diffuse = color; };
    Material(GLuint textureId) : Material() { this->textureId = textureId; };
    void Apply(void);
};


class Object
{
protected:
    /** Handles custom part of object draw. */
    virtual void drawObject(void);

    /** Handles custom part of object update. */
    virtual void updateObject(float elapsed);

    /** Children of this scenegraph object. */
    std::vector<Object*> children;

public:
    
    /** Anchor point (in object space). */
    Vec3 anchor;

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

    /** update the object, elapsed is in seconds since last update. */
    virtual void Update(float elapsed);

public:
    Object();
    Object(Vec3 position);
    void Add(Object* object);

};

class Light: public Object
{

protected:
    GLuint lightid;
    
    void drawObject(void) override;

public:
    bool attenuate = false;

    Light(GLuint lightid) : Object() { this->lightid = lightid; }
};

class Cube: public Object
{
    void drawObject(void) override;
public:
    Cube() : Object() {};
    Cube(float width, float height, float depth) : Cube() { this->scale = Vec3(width, height, depth); };
};

class Quad: public Object
{
    void drawObject(void) override;    
public:
    Quad() : Object() {};
    Quad(Vec3 position, float width, float height) : Object() { this->position = position; this->scale = Vec3(width, height, 1); };
};

// Like a quad but with sub divisions.
class Plane: public Object
{
    void drawObject(void) override;    
public:

    int divisionsX = 16;
    int divisionsY = 16;


    Plane() : Object() {};
    Plane(Vec3 position, float width, float height) : Object() { this->position = position; this->scale = Vec3(width, height, 1); };
};


class Sphere: public Object
{
private:
    GLUquadricObj*	q;
protected:
    void drawObject(void) override;    
public:
    Sphere();
};

class SurfaceOfRevolution: public Object
{
protected:
    std::vector<Vec2> points;
    int slices = 16;
        
public:

    // number of degrees to sweep across.
    float sweepAngle = 360;

    SurfaceOfRevolution();
    SurfaceOfRevolution(std::vector<Vec2> points);

protected:
     void drawObject(void) override;
};

class Cylinder: public SurfaceOfRevolution
{
public:
    
    Cylinder(float radius, float height, bool capped=true, int divisions = -1) : SurfaceOfRevolution()
    {
        if (capped) points.push_back(Vec2(0, 0));
        if (divisions == -1) {
            divisions = (int)abs(height) + 2;
        }
        
        for (int i = 0; i < divisions; i++) 
        {
            float y = (i * height / (divisions-1));
            points.push_back(Vec2(radius, y));
        }

        if (capped) points.push_back(Vec2(0, height));
    }
    Cylinder(Vec3 pos, float radius, float height, bool capped=true, int divisions = -1) : Cylinder(radius, height, capped, divisions)
    {
        this->position = pos;
    }
};

