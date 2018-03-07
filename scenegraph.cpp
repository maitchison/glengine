#include "stdinc.h"

#include "utils.h"
#include "scenegraph.h"

//------------------------------------------------------
// scenegraph
//------------------------------------------------------

void SceneGraph::Add(Object* object)
{
    objects.push_back(object);
}

void SceneGraph::Render(Camera camera)
{
    camera.apply();
    for (int i = 0; i < objects.size(); i++)
    {
        objects[i]->Draw();
    }
}

//------------------------------------------------------
// material
//------------------------------------------------------

Material::Material()
{
    ambient = Color(0.1,0.1,0.1);
    diffuse = Color(1,1,1);
    specular = Color(1,1,1);
    emission = Color(0,0,0);
    shininess = 40;
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
    }
}

//------------------------------------------------------
// objects
//------------------------------------------------------

Object::Object()
{
    position = Vec3(0,0,0);
    rotation = Vec3(0,0,0);
    scale = Vec3(1,1,1);
    color = Color(1,1,1);
}

Object::Object(Vec3 position) : Object()
{
    this->position = position;
}

void Object::Draw(void)
{
    // apply our Material
    if (material) material->Apply();
    // setup out transforms
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glRotatef(rotation.x, 1, 0, 0);
    glRotatef(rotation.y, 0, 1, 0);
    glRotatef(rotation.z, 0, 0, 1);
    glScalef(scale.x, scale.y, scale.z);
    glColor4f(color.r, color.g, color.b, color.a);
    drawObject();
    glPopMatrix();
}

void Object::drawObject(void)
{
}

void Cube::drawObject(void)
{

    glutSolidCube(1.0);
}

void Sphere::drawObject(void)
{
    glutSolidSphere(1.0, 64, 64);
}

void Light::drawObject(void)
{
    float lightPos[4] = {0,0,0, 1};
    glLightfv(lightid, GL_POSITION, lightPos);

    //stub: draw light in world
    glutSolidSphere(1.0, 4, 4);
}

//------------------------------------------------------
// surface of resolution
//------------------------------------------------------

SurfaceOfRevolution::SurfaceOfRevolution(Vec2 points): Object()
{
    this->points = points;
}

SurfaceOfRevolution::drawObject(void)
{
    // rotate around axis
    for (int i = 0; i < slices; i++) {
        float x = sin(theta) * points[i];
        float z = cos(theta) * points[i];
    }
}

//------------------------------------------------------
// camera
//------------------------------------------------------

void Camera::apply(void)
{
	gluLookAt(x, y, z, x + sin(yAngle), y, z + -cos(yAngle), 0, 1, 0);
}

void Camera::move(float amount)
{
	x += sin(yAngle) * amount;
	z -= cos(yAngle) * amount;
}

void Camera::pan(float amount)
{
	x += cos(yAngle) * amount;
	z += sin(yAngle) * amount;
}

Camera::Camera()
{
	x = 0;
	y = 0;
	z = 0;
	yAngle = 0;
}
