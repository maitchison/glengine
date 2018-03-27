#pragma once

#include "stdinc.h"

class Material
{
public:
    // enabled texturing 
    static bool M_ENABLE_TEXTURE;

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
