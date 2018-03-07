#pragma once

/*
Standard includes.
Includes selecting an appropriate opengl with respect to the operating system.
*/


#define _USE_MATH_DEFINES

#ifdef __linux__
#include <GL/freeglut.h>
#elif __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <stdint.h>
#include <algorithm>
#include <limits>
#include <vector>
#include <time.h>

#include "vec.hpp"
