
//an example using the PNG loading function:

#define _USE_MATH_DEFINES

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#endif

#include <iostream>
#include <fstream>

#include "utils.h"

#include "picoPNG.hpp"

float randf(void)
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

void loadFile(std::vector<unsigned char>& buffer, const std::string& filename) //designed for loading files from hard disk in an std::vector
{
	std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

	//get filesize
	std::streamsize size = 0;
	if (file.seekg(0, std::ios::end).good()) size = file.tellg();
	if (file.seekg(0, std::ios::beg).good()) size -= file.tellg();

	//read contents of the file into the vector
	if (size > 0)
	{
		buffer.resize((size_t)size);
		file.read((char*)(&buffer[0]), size);
	}
	else buffer.clear();
}

// taken from https://stackoverflow.com/questions/12084889/how-can-i-use-a-texture-jpg-image-for-an-opengl-background-window-in-mac
GLuint loadTexture(char* filename)
{	
	//load and decode
	std::vector<unsigned char> buffer, image;
	loadFile(buffer, filename);
	unsigned long w, h;
	int error = decodePNG(image, w, h, buffer.empty() ? 0 : &buffer[0], (unsigned long)buffer.size());

	//if there's an error, display it
	if (error != 0) std::cout << "error: " << error << std::endl;

	//the pixels are now in the vector "image", use it as texture, draw it, ...
	if (image.size() > 4) std::cout << "width: " << w << " height: " << h << " first pixel: " << std::hex << int(image[0]) << int(image[1]) << int(image[2]) << int(image[3]) << std::endl;

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
	//gluBuild2DMipmaps(); 

	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}