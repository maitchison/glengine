/**
Include only vector class
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
};

/** color */
struct Color
{
public:
    float r,g,b,a;
};
