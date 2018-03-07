


/*
void drawBox(float x, float y, float z)
{
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x, y, z);
	for (int i = 0; i < 4; i++)
	{
		glColor3f(0.3*i, 0.7, 0.5);
		glPushMatrix();
		glRotatef(i * 90, 0, 1, 0);
		glTranslatef(0, 0, -0.4);
		glScalef(1, 0.5, 0.1);
		glutSolidCube(1.0);
		glPopMatrix();

		glColor3f(1, 0, 0);
		glPushMatrix();
		glRotatef(i * 90, 0, 1, 0);
		glTranslatef(-0.4, 0, -0.4);
		glScalef(0.25, 0.6, 0.25);
		glutSolidCube(1.1);
		glPopMatrix();
	}
	glPopMatrix();
}

void drawHouse(float x, float y, float z)
{
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x, y, z);

	glScalef(10,10,10);

	//todo : doors and windows, maybe do this as polygons. (plan out on paper)

	for (int i = 0; i < 4; i++)
	{
		glColor3f(0.3*i, 0.7, 0.5);
		glPushMatrix();
		glRotatef(i * 90, 0, 1, 0);
		glTranslatef(0, 0, -0.4);
		glScalef(1, 0.5, 0.1);
		glutSolidCube(1.0);
		glPopMatrix();

		glColor3f(1, 0, 0);
		glPushMatrix();
		glRotatef(i * 90, 0, 1, 0);
		glTranslatef(-0.4, 0, -0.4);
		glScalef(0.25, 0.6, 0.25);
		glutSolidCube(1.1);
		glPopMatrix();
	}

	glPopMatrix();
}

// swept axis object.
void drawTrinket(float x, float y, float z)
{
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x, y, z);
	glPopMatrix();
}


void drawTurtle(float x, float y, float z)
{
	glMatrixMode(GL_MODELVIEW);

	// shell
	glLoadIdentity();
	glTranslatef(x, y, z);
	glColor3f(0, 0.5, 0);
	glScalef(1, 0.3, 1);
	glutSolidCube(1.0);

	// body
	glLoadIdentity();
	glTranslatef(x, y-0.2, z);
	glColor3f(0.65, 0.55, 0.60);
	glScalef(0.8, 0.2, 0.9);
	glutSolidCube(1.0);

	// arms
	glLoadIdentity();
	glTranslatef(x - 0.5, y - 0.2, z);
	glColor3f(0.65, 0.95, 0.60);
	glScalef(0.6, 0.1, 0.2);
	glutSolidCube(1.0);

	glLoadIdentity();
	glTranslatef(x + 0.5, y - 0.2, z);
	glColor3f(0.65, 0.95, 0.60);
	glScalef(0.6, 0.1, 0.2);
	glutSolidCube(1.0);

	// head
	glLoadIdentity();
	glTranslatef(x, y - 0.2, z + 0.6);
	glColor3f(0.65, 0.95, 0.60);
	glScalef(0.25, 0.25, 0.25);
	glutSolidCube(1.0);

}
*/
