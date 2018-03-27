#include "stdinc.h"

#include "utils.h"
#include "scenegraph.h"

Material DEFAULT_MATERIAL = Material();

int DIVISONS_MULTIPLIER = 1;

//------------------------------------------------------
// helper functions
//------------------------------------------------------

void drawSubdividedQuad(int divisionsX, int divisionsY) 
{
    for (int y = 0; y < (divisionsY); y++)
    {
        float s;
        float t;        
        
        glBegin(GL_QUAD_STRIP);
        for (int x = 0; x < divisionsX+1; x++) {
            s = (float)x / (divisionsX);
            t = (float)y / (divisionsY);                
            glTexCoord2f(s,t);
            glNormal3f(0,1,0);
            glVertex3f(s-0.5, 0, t-0.5);
            t = ((float)y+1) / (divisionsY);    
            glTexCoord2f(s,t);
            glNormal3f(0,1,0);
            glVertex3f(s-0.5, 0, t-0.5);
        }
        glEnd();
    }
}
//------------------------------------------------------
// scenegraph
//------------------------------------------------------

void SceneGraph::Add(Object* object)
{
    objects.push_back(object);    
}

void SceneGraph::AddLight(Light* light)
{
    lights.push_back(light);
}

void SceneGraph::Render(Camera camera, bool renderSolid, bool renderWire)
{
    camera.apply();

    for (int i = 0; i < lights.size(); i++)
    {
        // this doesn't actually draw the lights, just places them.
        lights[i]->Update(0);
        lights[i]->Draw();
    }
    
    glEnable(GL_CULL_FACE);

    // draw the scene normally
    if (renderSolid) {
        Material::M_ENABLE_TEXTURE = true;
        glPolygonMode(GL_FRONT,GL_FILL);    
        glEnable(GL_LIGHTING);        
        for (int i = 0; i < objects.size(); i++) objects[i]->Draw();    
    }

    if (renderWire) {
        // draw again but with edges, and shift the depth values forward a little.
        Material::M_ENABLE_TEXTURE = false;
        
        glPolygonOffset(-0.1,-0.1);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glDisable(GL_LIGHTING);
        
        glPolygonMode(GL_FRONT,GL_LINE);    
        for (int i = 0; i < objects.size(); i++) objects[i]->Draw();

        glDisable(GL_POLYGON_OFFSET_LINE);
    }

}

void SceneGraph::Update(float elapsed)
{
    for (int i = 0; i < objects.size(); i++)
    {
        objects[i]->Update(elapsed);
    }
}

Object* SceneGraph::getObjectAtPosition(Vec3 pos)
{

    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i]->isInside(pos)) {
            return objects[i];
        }
    }
    return NULL;
}

//------------------------------------------------------
// objects
//------------------------------------------------------

Object::Object()
{
    anchor = Vec3(0,0,0);
    position = Vec3(0,0,0);
    rotation = Vec3(0,0,0);
    scale = Vec3(1,1,1);
    color = Color(1,1,1);
}

Object::Object(Vec3 position) : Object()
{
    this->position = position;
}

void Object::Add(Object* object)
{
    children.push_back(object);
    object->parent = this;
}

void Object::Draw(void)
{
    if (!visible) {
        return;
    }

    // apply our Material
    if (material) { 
        material->Apply(); 
    } else DEFAULT_MATERIAL.Apply();
        
    // setup out transforms
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(position.x, position.y, position.z);

    //glTranslatef(anchor.x, anchor.y, anchor.z);

    glRotatef(rotation.x, 1, 0, 0);
    glRotatef(rotation.y, 0, 1, 0);
    glRotatef(rotation.z, 0, 0, 1);
    glScalef(scale.x, scale.y, scale.z);
        
    glTranslatef(-anchor.x, -anchor.y, -anchor.z);
    
    glColor4f(color.r, color.g, color.b, color.a);

    // draw us
    drawObject();

    // draw children
    for (int i = 0; i < children.size(); i++) {
        children[i]->Draw();
    }

    // clean up
    glPopMatrix();
}

void Object::Update(float elapsed)
{    
    // update us
    updateObject(elapsed);
    // update children
    for (int i = 0; i < children.size(); i++) {
        children[i]->Update(elapsed);
    }
}

void Object::drawObject(void)
{
}

void Object::updateObject(float elapsed)
{
}

void Cube::drawObject(void)
{    
    // custom subdivided cube with texture co-rds
    for (int i = 0; i < 4; i ++) {
        glPushMatrix();
        glRotatef(90*i,1,0,0);
        glTranslatef(0,0.5,0);
        drawSubdividedQuad(divisionsX * DIVISONS_MULTIPLIER,divisionsY * DIVISONS_MULTIPLIER);
        glPopMatrix();        
    }

    // front and back faces

    glPushMatrix();
    glRotatef(90,0,0,1);
    glTranslatef(0,0.5,0);
    drawSubdividedQuad(divisionsX * DIVISONS_MULTIPLIER,divisionsY * DIVISONS_MULTIPLIER);
    glPopMatrix();
    glPushMatrix();
    glRotatef(-90,0,0,1);
    glTranslatef(0,0.5,0);
    drawSubdividedQuad(divisionsX * DIVISONS_MULTIPLIER,divisionsY * DIVISONS_MULTIPLIER);
    glPopMatrix();
    
}

Sphere::Sphere() : Object()
{
    q = gluNewQuadric();	
	gluQuadricDrawStyle(q, GLU_FILL );
	gluQuadricNormals(q, GLU_SMOOTH );
	gluQuadricTexture(q, GL_TRUE);
}

void Sphere::drawObject(void)
{
    gluSphere(q, 1.0, 64, 64);
}

void Light::drawObject(void)
{
    float lightPos[4] = {0.0, 0.0, 0.0, 1.0};
    float lightAmbient[4] = {0.05,0.05,0.05,0.1};
    float lightColor[4] = {color.r, color.g, color.b, 1.0};    
    float lightSpec[4] = {color.r, color.g, color.b, 1.0};    

    glLightfv(lightid, GL_POSITION, lightPos);
    glLightfv(lightid, GL_AMBIENT, lightAmbient);
    glLightfv(lightid, GL_DIFFUSE, lightColor);
    glLightfv(lightid, GL_SPECULAR, lightSpec);

    if (spot) {
        glLightf(lightid, GL_SPOT_CUTOFF, 15.0f); //spot    
        glLightf(lightid, GL_SPOT_EXPONENT, 10.0f);
        float spotDirection[4] = {0.8,-1,0,0};
        glLightfv(lightid, GL_SPOT_DIRECTION, spotDirection);
    }
    else {
        glLightf(lightid, GL_SPOT_CUTOFF, 180.0f); //omni    
    }
    
    
    
    if (attenuate)
        glLightf(lightid, GL_LINEAR_ATTENUATION, 0.5);
    else
        glLightf(lightid, GL_LINEAR_ATTENUATION, 0);                
}

void Light::updateObject(float elapsed)
{
    if (visible)
        glEnable(lightid);
    else
        glDisable(lightid);
}

void Quad::drawObject(void)
{
    glBegin(GL_QUADS);
    glTexCoord2f(0,0);
    glVertex3f(-0.5,-0.5,0);
    glTexCoord2f(1,0);
    glVertex3f(+0.5,-0.5,0);
    glTexCoord2f(1,1);
    glVertex3f(+0.5,+0.5,0);
    glTexCoord2f(0,1);
    glVertex3f(-0.5,+0.5,0);
    glEnd();        
}


void Plane::drawObject(void)
{
    drawSubdividedQuad(divisionsX, divisionsY);    
}

//------------------------------------------------------
// surface of resolution
//------------------------------------------------------

SurfaceOfRevolution::SurfaceOfRevolution(): Object()
{
    // pass
}

SurfaceOfRevolution::SurfaceOfRevolution(std::vector<Vec2> points): SurfaceOfRevolution()
{
    for (int i = 0; i < points.size(); i++) {
        this->points.push_back(points[i]);
    }
    
}

void SurfaceOfRevolution::drawObject(void)
{
    // rotate around axis
    float theta;
    float x;
    float y;
    float z;
    float abs2;
    float abs;

    int n = points.size();

    for (int j = 0; j < (n-1); j++) {
        glBegin(GL_QUAD_STRIP);            

        // note the normal calculation here is not quite right, the y component needs adjusting.
        // this should be hard to notice in many cases though.
        
        // cap a partialy swept surface
        if (sweepAngle != 360) {
            theta = (2.0 * M_PI * sweepAngle / 360.0);

            x = sin(theta) * points[(j+1) % n].x;
            z = cos(theta) * points[(j+1) % n].x;
            y =  points[(j+1) % n].y;
            abs2 = x*x+y*y+z*z;
            abs = (abs2 == 0) ? 1 : sqrt(abs2);
            glNormal3f(x/abs,0,z/abs);
            glVertex3f(x,y,z);
            x = sin(theta) * points[j].x;
            z = cos(theta) * points[j].x;
            y =  points[j].y;
            abs2 = x*x+y*y+z*z;
            abs = (abs2 == 0) ? 1 : sqrt(abs2);
            glNormal3f(x/abs,0,z/abs);
            glVertex3f(x,y,z);            
        }

        for (int i = 0; i < slices; i++) {            
            theta = (float)i / (slices-1) * (2.0 * M_PI * sweepAngle / 360.0);
            
            x = sin(theta) * points[(j+1) % n].x;
            z = cos(theta) * points[(j+1) % n].x;
            y =  points[j+1].y;
            abs2 = x*x+y*y+z*z;
            abs = (abs2 == 0) ? 1 : sqrt(abs2);
            glNormal3f(x/abs,0,z/abs);
            glVertex3f(x,y,z);
            x = sin(theta) * points[j].x;
            z = cos(theta) * points[j].x;
            y =  points[j].y;
            abs2 = x*x+y*y+z*z;
            abs = (abs2 == 0) ? 1 : sqrt(abs2);
            glNormal3f(x/abs,0,z/abs);
            glVertex3f(x,y,z);
            
        }        
        

        glEnd();
    }
    
}

//------------------------------------------------------
// camera
//------------------------------------------------------

void Camera::apply(void)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	gluLookAt(x, y, z, x + sin(yAngle), y + sin(xAngle), z + -cos(yAngle), 0, 1, 0);
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