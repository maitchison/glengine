#pragma once

#include "stdinc.h"

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
    GLenum filtering = GL_LINEAR;
    bool useMapmaps = true;
public:
    Material();
    Material(Color color) : Material() { ambient = diffuse = color; };
    Material(GLuint textureId) : Material() { this->textureId = textureId; };
    void Apply(void);


};
