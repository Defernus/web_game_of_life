#ifndef SHADER_UTIL
#define SHADER_UTIL

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>

#include <SDL/SDL_opengl.h>

GLuint loadAndCompileShader(const char *path, GLuint type);

int loadProgram(const char *vs, const char *fs, GLuint &program);
#endif
