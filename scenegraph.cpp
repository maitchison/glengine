#include "stdinc.h"

#include "utils.h"
#include "scenegraph.h"

Material DEFAULT_MATERIAL = Material();

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

void SceneGraph::Render(Camera camera)
{
    camera.apply();

    for (int i = 0; i < lights.size(); i++)
    {
        lights[i]->Draw();
    }

    for (int i = 0; i < objects.size(); i++)
    {
        objects[i]->Draw();
    }
}

void SceneGraph::Update(float elapsed)
{
    for (int i = 0; i < objects.size(); i++)
    {
        objects[i]->Update(elapsed);
    }
}

//------------------------------------------------------
// material
//------------------------------------------------------

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
    }

    if (textureId) 
    {
        glEnable(GL_TEXTURE_2D);
        if (disableLighting) 
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);	
        else
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);	
        glBindTexture(GL_TEXTURE_2D, textureId);
    } else {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
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
}

void Object::Draw(void)
{
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
    glutSolidCube(1.0);
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

    //stub: only spec    
    glLightfv(lightid, GL_POSITION, lightPos);
    glLightfv(lightid, GL_AMBIENT, lightAmbient);
    glLightfv(lightid, GL_DIFFUSE, lightColor);
    glLightfv(lightid, GL_SPECULAR, lightSpec);
    glLightf(lightid, GL_SPOT_CUTOFF, 180.0f); //omni    
    
    if (attenuate)
        glLightf(lightid, GL_LINEAR_ATTENUATION, 1.0);
    else
        glLightf(lightid, GL_LINEAR_ATTENUATION, 0);                
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
    
    for (int y = 0; y < (divisionsY-1); y++)
    {
        float s;
        float t;        
        
        glBegin(GL_QUAD_STRIP);
        for (int x = 0; x < divisionsX; x++) {
            s = (float)x / (divisionsX-1);
            t = (float)y / (divisionsY-1);    
            glTexCoord2f(s,t);
            glNormal3f(0,1,0);
            glVertex3f(s-0.5, 0, t-0.5);
            t = ((float)y+1) / (divisionsY-1);    
            glTexCoord2f(s,t);
            glNormal3f(0,1,0);
            glVertex3f(s-0.5, 0, t-0.5);
        }
        glEnd();
    }
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
        
        // cap a partialy swept surface
        if (sweepAngle != 360) {
            theta = (2.0 * M_PI * sweepAngle / 360.0);
            x = sin(theta) * points[j].x;
            z = cos(theta) * points[j].x;
            y =  points[j].y;
            abs2 = x*x+y*y+z*z;
            abs = (abs2 == 0) ? 1 : sqrt(abs2);
            glNormal3f(x/abs,y/abs,z/abs);
            glVertex3f(x,y,z);
            x = sin(theta) * points[(j+1) % n].x;
            z = cos(theta) * points[(j+1) % n].x;
            y =  points[(j+1) % n].y;
            abs2 = x*x+y*y+z*z;
            abs = (abs2 == 0) ? 1 : sqrt(abs2);
            glNormal3f(x/abs,y/abs,z/abs);
            glVertex3f(x,y,z);
        }

        for (int i = 0; i < slices; i++) {            
            theta = (float)i / (slices-1) * (2.0 * M_PI * sweepAngle / 360.0);
            x = sin(theta) * points[j].x;
            z = cos(theta) * points[j].x;
            y =  points[j].y;
            abs2 = x*x+y*y+z*z;
            abs = (abs2 == 0) ? 1 : sqrt(abs2);
            glNormal3f(x/abs,y/abs,z/abs);
            glVertex3f(x,y,z);
            x = sin(theta) * points[(j+1) % n].x;
            z = cos(theta) * points[(j+1) % n].x;
            y =  points[j+1].y;
            abs2 = x*x+y*y+z*z;
            abs = (abs2 == 0) ? 1 : sqrt(abs2);
            glNormal3f(x/abs,y/abs,z/abs);
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