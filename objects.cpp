#include "stdinc.h"

#include "utils.h"
#include "objects.h"

Object::Object()
{
    position = Vec3(0,0,0);
    rotation = Vec3(0,0,0);
    scale = Vec3(1,1,1);
}

void Object::Draw(void)
{
    // setup out transforms
    glRotatef(rotation.x, 1, 0, 0);
    glRotatef(rotation.y, 0, 1, 0);
    glRotatef(rotation.z, 0, 0, 1);
    glScalef(scale.x, scale.y, scale.z);
}

void Object::drawObject(void)
{
    // pass
}


void Cube::drawObject(void)
{
    // pass
    glutSolidCube(1.0);
}

void Sphere::drawObject(void)
{
    // pass
    glutSolidCube(1.0);
}
