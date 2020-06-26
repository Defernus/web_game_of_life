#include "shader_utils.h"

GLuint loadAndCompileShader(const char *path, GLuint type) {
	std::ifstream file;
	file.open(path);
	std::stringstream sstream;
	sstream << file.rdbuf();
	std::string shader_str = sstream.str();

	if(shader_str == "") {
		//TODO
		printf("failed t load shader, file %s not found.\n", path);
		return 0;
	}

	GLuint shader = glCreateShader(type);

	const char* src = shader_str.c_str();
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE) {
		GLchar log[1024];
		GLsizei length;
		glGetShaderInfoLog(shader, 1024, &length, log);
		printf("failed to compile shader \n%s\n", log);
	}

	return shader; 
}



int loadProgram(const char *vs, const char *fs, GLuint &program) {
	printf("compiling vertex shader\n");
	GLuint vertex_shader = loadAndCompileShader(vs, GL_VERTEX_SHADER);

	printf("compiling fragment shader\n");
	GLuint fragment_shader = loadAndCompileShader(fs, GL_FRAGMENT_SHADER);
	
	program = glCreateProgram();

	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	glLinkProgram(program);
	glUseProgram(program);
	return 0;
}
