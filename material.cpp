#include "material.h"

Material::Material()
{
    ambient = Color(1,1,1);
    diffuse = Color(1,1,1);
    specular = Color(1,1,1);
    emission = Color(0,0,0);
    shininess = 40;
    textureId = 0;
    disableLighting = false;
};

void Material::Apply(void)
{
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient.values);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse.values);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular.values);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission.values);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    if (diffuse.a != 1.0) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDisable(GL_BLEND);
    }

    if (textureId) 
    {
        glEnable(GL_TEXTURE_2D);

        // setup our filtering
        switch (filtering) {
            case GL_NEAREST:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, useMapmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, useMapmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
                break;
            case GL_LINEAR:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, useMapmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, useMapmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                break;            
        }

        // setup lighting mode
        if (disableLighting) 
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);	
        else
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);	

        // bind the texture
        glBindTexture(GL_TEXTURE_2D, textureId);
    } else {
        glDisable(GL_TEXTURE_2D);
    }
}
