#pragma once

/**
3d Objects used as part of a scene graph
*/

#include <vector>

#include "stdinc.h"

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

    /** Draws the object to current view. */
    virtual void Draw(void);

    Object();

};

class Light: Object
{

};

class Cube: Object
{
    void drawObject(void) override;
};

class Sphere: Object
{
    void drawObject(void) override;
};
