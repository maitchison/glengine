#include "customobjects.h"

//---------------------------------------------------------------
// Turtle
//---------------------------------------------------------------

Turtle::Turtle()
{
    // shell
    Object* shell = new Cube();
    shell->material = new Material(Color(0, 0.5, 0));
    shell->scale = Vec3(1, 0.3, 1);	    
    Add(shell);

    // body
    Object* body = new Cube();
    body->position = Vec3(0, -0.2, 0);
    body->material = new Material(Color(0.65, 0.55, 0.60));
    body->scale = Vec3(0.8, 0.2, 0.9);
    Add(body);
            
    // arms
    Object* leftArm = new Cube();
    leftArm->position = Vec3(- 0.5, - 0.2, 0);
    leftArm->material = new Material(Color(0.65, 0.95, 0.60));
    leftArm->scale = Vec3(0.6, 0.1, 0.2);
    Add(leftArm);

    Object* rightArm = new Cube();
    rightArm->position = Vec3(+0.5, - 0.2, 0);
    rightArm->material = new Material(Color(0.65, 0.95, 0.60));
    rightArm->scale = Vec3(0.6, 0.1, 0.2);
    Add(rightArm);

    // head
    Object* head = new Cube();            
    head->position = Vec3(0, - 0.2, 0.6);
    head->material = new Material(Color(0.65, 0.95, 0.60));
    head->scale = Vec3(0.25, 0.25, 0.25);        
    Add(head);

}

//---------------------------------------------------------------
// Bird
//---------------------------------------------------------------

Bird::Bird() : Object()
{
    // body
    Object* body = new Cube();
    body->material = new Material(Color(0.9, 0.5, 0.45));
    body->scale = Vec3(1, 2, 0.8);	    
    Add(body);
    
    // head
    Object* head = new Cube();            
    head->position = Vec3(0, +1.0, 0.6);
    head->material = new Material(Color(0.9, 0.65, 0.60));
    head->scale = Vec3(0.65, 0.45, 0.95);        
    head->rotation.x = 15;
    Add(head);

    // wings
    Object* leftWing = new Cube();            
    leftWing->position = Vec3(0.5, 0.35, 0.2);
    leftWing->material = new Material(Color(0.2, 0.6, 0.2));
    leftWing->scale = Vec3(0.15, 1.5, 0.75);                
    leftWing->anchor = Vec3(0,-0.5, -0.5);
    leftWing->rotation.x = -155;
    Add(leftWing);

    Object* rightWing = new Cube();            
    rightWing->position = Vec3(-0.5, 0.35, 0.2);
    rightWing->material = new Material(Color(0.2, 0.6, 0.2));
    rightWing->scale = Vec3(0.15, 1.5, 0.75);                
    rightWing->anchor = Vec3(0,-0.5, -0.5);
    rightWing->rotation.x = -155;
    Add(rightWing);

    scale = Vec3(0.3,0.3,0.3);

}

//---------------------------------------------------------------
// Toy Lighthouse
//---------------------------------------------------------------

ToyLighthouse::ToyLighthouse() : Object()
{
    // main base as object of revolution

    std::vector<Vec2> pts = {
        Vec2(2,0),
        Vec2(0.5,4),
        Vec2(0.5,5),
        Vec2(0.1,5),
        Vec2(0.1,5.5),
        Vec2(0.5,5.5),
        Vec2(0.5,5.6),
        Vec2(0,5.6)
    };

    Object* base = new SurfaceOfRevolution(pts);

    Add(base);

    // add the light part
    light = new Cylinder(0.4, 0.5);
    light->material = new Material();
    light->material->ambient = Color(1,0,0);
    light->position.y = 5;
    Add(light);
}

void ToyLighthouse::updateObject(float elapsed)
{
    light->rotation.y += elapsed;
    //todo: update spotlight  actually this can probably just be attached??
}


//---------------------------------------------------------------
// House
//---------------------------------------------------------------


// create a new block
void House::block(Vec3 bottomLeft, Vec3 topRight, Material* material)
{
    Vec3 mid = Vec3((bottomLeft.x + topRight.x) / 2, (bottomLeft.y + topRight.y) / 2, (bottomLeft.z + topRight.z) / 2);
    Vec3 size = Vec3(-(bottomLeft.x - topRight.x), -(bottomLeft.y - topRight.y), -(bottomLeft.z - topRight.z));
    Object* block = new Cube();
    block->scale = size;
    block->position = mid;
    block->material = material;
    Add(block);
}

void House::marker(Vec3 pos, Color color)
{
    float size = 1;
    Object* block = new Cube();
    block->scale = Vec3(size,size,size);
    block->position = pos;
    block->material = new Material();
    block->material->emission = color;
    Add(block);
}

void House::wall(Vec2 p1, Vec2 p2, float thickness, float startHeight, float endHeight, Material* material)
{
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float wallLength = sqrtf(dx*dx + dy*dy);
    float wallAngle = atan2(dy, dx) / M_PI * 180.0 + 90;

    Object* wall = new Cube();
    wall->anchor = Vec3(0, -0.5, +0.5);
    wall->position = Vec3(p1.x, startHeight, p1.y);
    wall->scale = Vec3(thickness, endHeight - startHeight, wallLength);
    wall->rotation = Vec3(0, -wallAngle, 0);
    wall->material = material;
    Add(wall);

}

House::House()
{        
    Object* tower = new Cylinder(2, 21);
    Add(tower);

    // the light
    Object* light = new Cylinder(1.5, 3);		
    light->position.y = 21;
    light->material = new Material();
    light->material->emission = Color(1,0.9,0.7);
    Add(light);

    // the cap
    Object* cap = new Cylinder(2, 1.5);		
    cap->position.y = 23;
    Add(cap);

    // tower rings
    Material* ringMaterial = new Material();
    ringMaterial->diffuse = Color(0.5,0.5,0.5);
    ringMaterial->specular = Color(0,0,0);
    
    for (int i = 0; i < 4; i ++) 
    {
        Object* ring = new Cylinder(2.1,0.4);
        ring->position.y = i * 7;
        ring->material = ringMaterial;
        Add(ring);
    }
    
    // building walls

    Material* wallMaterial = new Material();
    wallMaterial->diffuse = Color(1,0,0);

    float wallThickness = 0.15;
    float houseLength = 8.0;
    float houseWidth = 3.0;
    float houseHeight = 3.0;

    // -----------------------------------
    // main room
    
    // back wall
    wall(Vec2(0,-3), Vec2(houseLength,-3), wallThickness, 0, 3);

    // front wall
    wall(Vec2(0,3), Vec2(8,3), wallThickness, 0, 0.5);
    wall(Vec2(0,3), Vec2(8,3), wallThickness, 2.5, 3);
    wall(Vec2(0,3), Vec2(1,3), wallThickness, 0.5, 2.5);
    wall(Vec2(7,3), Vec2(8,3), wallThickness, 0.5, 2.5);

    // entrance wall
    wall(Vec2(8,-3), Vec2(8,-2), wallThickness, 0, 3);
    wall(Vec2(8,-2), Vec2(8,2), wallThickness, 2.2, 3);
    wall(Vec2(8,2), Vec2(8,3), wallThickness, 0, 3);
    
    // far wall
    wall(Vec2(0,-3), Vec2(0,3), wallThickness, 0, 3);

    // floor
    block(Vec3(0, 0,-3), Vec3(8,wallThickness, 3));    

    // roof
    block(Vec3(0, 3,-3), Vec3(8,3-wallThickness, 3));    

    // pillars (cover the joins)
    Add(new Cylinder(Vec3(0,0,-3), wallThickness, 3));
    Add(new Cylinder(Vec3(0,0,+3), wallThickness, 3));
    Add(new Cylinder(Vec3(8,0,-3), wallThickness, 3));
    Add(new Cylinder(Vec3(8,0,+3), wallThickness, 3));
}    

//---------------------------------------------------------------
// Skybox
//---------------------------------------------------------------

Skybox::Skybox(GLuint textures[6])
{
    Quad* quad;

    float size = 1000;
    
    //front
    quad = new Quad(Vec3(0,0,size),size*2,size*2);
    quad->rotation.y = 180;
    quad->material = new Material(textures[1]);         
    Add(quad);
    //left
    quad = new Quad(Vec3(+size,0,0),size*2,size*2);
    quad->rotation.y = -90;
    quad->material = new Material(textures[0]);        
    Add(quad);
    //right
    quad = new Quad(Vec3(-size,0,0),size*2,size*2);
    quad->rotation.y = +90;
    quad->material = new Material(textures[2]);
    Add(quad);
    //back
    quad = new Quad(Vec3(0,0,-size),size*2,size*2);
    quad->rotation.y = 0;
    quad->material = new Material(textures[3]);
    Add(quad);
    //top
    quad = new Quad(Vec3(0,size,0),-size*2,size*2);
    quad->rotation.x = -90;
    quad->rotation.z = 0;
    quad->material = new Material(textures[4]);
    Add(quad);
    //bottom
    quad = new Quad(Vec3(0,-size,0),-size*2,size*2);
    quad->rotation.x = -90;
    quad->rotation.z = 0;
    quad->material = new Material(textures[5]);
    Add(quad);
}

//---------------------------------------------------------------
// Globe and moon
//---------------------------------------------------------------

// todo: add textures to globe and moon
GlobeAndMoon::GlobeAndMoon(GLuint globeTexture, GLuint moonTexture)
{
    globe = new Sphere();
    moon = new Sphere();    
    moon->position = Vec3(-3, 0, 0);
    moon->scale = Vec3(0.5,0.5,0.5);
    Add(globe);
    globe->Add(moon);
    this->scale = Vec3(0.2,0.2,0.2);
    globe->material = new Material(globeTexture);
    moon->material = new Material(moonTexture);
    globe->rotation.x = 90;
}

void GlobeAndMoon::updateObject(float elapsed)
{
    globe->rotation.z += elapsed * 11;
    moon->rotation.z += elapsed * 17;
}