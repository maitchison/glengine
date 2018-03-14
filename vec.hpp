#pragma once

/**
vector classes
*/

/** 3d vector */
struct Vec3
{
public:
    float x, y, z;
    Vec3(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Vec3()
    {
        x = y = z = 0;
    }
};

/** 2d vector */
struct Vec2
{
public:
    float x, y;

    Vec2()
    {
        x = y = 0;    
    }

    Vec2(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

};

/** color */
struct Color
{
public:

    union {
        struct {
            float r,g,b,a;
        };
        float values[4];
    };


    Color(float r, float g, float b, float a=1)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    Color()
    {
        r = g = b = 1;
        a = 1;
    }

    Color(uint hex)
    {
        b = (hex & 0xff) / 255.0;
        g = ((hex >> 8) & 0xff) / 255.0;
        r = ((hex >> 16) & 0xff) / 255.0;
        a = 1.0;
    }

};
