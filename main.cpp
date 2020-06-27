#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>

#include <SDL2/SDL.h>
#include <emscripten.h>

#include <SDL/SDL_opengl.h>

#include "shader_utils.h"

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 1024;

const int GRID_WIDTH = 1024;
const int GRID_HEIGHT = 1024;

int min_draw_radius = 0;
int max_draw_radius = 128;
int draw_radius = 10;

Uint32 last_time_ms;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_GLContext gl_ctx;

GLuint program;
GLuint grid_program;
GLuint grid_texture;
GLuint grid_texture_processor;
GLuint grid_framebuffer;
GLuint grid_renderbuffer;
GLuint grid_texture_display_id;
GLuint grid_texture_processor_id;
GLuint offset_id;

void draw() {
	/*Uint32 current_time_ms = SDL_GetTicks();
	Uint32 dt_ms = current_time_ms - last_time_ms;
	double dt = double(dt_ms)/1000.;
	double fps = 1./dt;*/

	//process grid
	glBindFramebuffer(GL_FRAMEBUFFER, grid_framebuffer);
	glViewport(0, 0, GRID_WIDTH, GRID_HEIGHT);

	glUseProgram(grid_program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grid_texture_processor);
	GLubyte *data = new GLubyte[GRID_WIDTH*GRID_HEIGHT*4];
	glReadPixels(0, 0, GRID_WIDTH, GRID_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, data);
	//handle input
	
	int mx, my;

	Uint32 m_state = SDL_GetMouseState(&mx, &my);
	my = SCREEN_HEIGHT-1-my;

	if(m_state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
		int x = mx*GRID_WIDTH/SCREEN_WIDTH;	
		int y = my*GRID_HEIGHT/SCREEN_HEIGHT;	

		for(int i = -draw_radius; i != draw_radius+1; ++i) {
			for(int j = -draw_radius; j != draw_radius+1; ++j) {
				int gx = x+i;
				gx = (GRID_WIDTH + (gx%GRID_WIDTH)) % GRID_WIDTH;
				int gy = y+j;
				gy = (GRID_HEIGHT + (gy%GRID_HEIGHT)) % GRID_HEIGHT;
				int index = (gx + gy*GRID_WIDTH)*4;
				*(data + index) = (rand()%2)*255;
			}
		}
	}

	//--
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GRID_WIDTH, GRID_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	delete[] data;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glUniform1i(grid_texture_processor_id, 0);
	
	glUniform2f(offset_id, 1./GRID_WIDTH, 1./GRID_HEIGHT);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);


	//draw grid
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glUseProgram(program);

	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grid_texture);
	glUniform1i(grid_texture_display_id, 0);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	SDL_GL_SwapWindow(window);
}


int main() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("failed to init SDL: %s\n", SDL_GetError());
		return -1;
	}

	if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL, &window, &renderer) != 0) {
		printf("failed to create window and renderer: %s\n", SDL_GetError());
		return -1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetSwapInterval(0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	gl_ctx = SDL_GL_CreateContext(window);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	GLuint vbo;
	glGenBuffers(1, &vbo);
	
	GLfloat vertices[] = {
		-1., -1.,
		1., -1.,
		1., 1.,

		1., 1.,
		-1., 1.,
		-1., -1.
	};
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	loadProgram("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl", program);
	grid_texture_display_id = glGetAttribLocation(program, "grid");

	loadProgram("assets/shaders/grid_vertex.glsl", "assets/shaders/grid_fragment.glsl", grid_program);
	grid_texture_processor_id = glGetUniformLocation(grid_program, "grid");
	offset_id = glGetUniformLocation(grid_program, "offset");
	
	GLint posAttrib = glGetAttribLocation(program, "aPosition");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);


	//preparing grid framebuffer
	grid_framebuffer = 0;
	glGenFramebuffers(1, &grid_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, grid_framebuffer);


	glGenTextures(1, &grid_texture_processor);
	glGenTextures(1, &grid_texture);

	glBindTexture(GL_TEXTURE_2D, grid_texture);

	GLubyte *inp_data = new GLubyte[GRID_WIDTH*GRID_HEIGHT*4];

	for(GLubyte *i = inp_data; i != inp_data+GRID_WIDTH*GRID_HEIGHT*4; i+=4) {
		*(i + 0) = 0;
		*(i + 1) = 0;
		*(i + 2) = 0;
		*(i + 3) = 255;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GRID_WIDTH, GRID_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, inp_data);
	delete[] inp_data;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, grid_texture, 0);
	/*GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers);*/
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("framebuffer error!");
		return -1;
	}

	printf("version: %s\n", glGetString(GL_VERSION));

	last_time_ms = SDL_GetTicks();

	printf("starting loop...");
	emscripten_set_main_loop(draw, -1, 1);

	SDL_Quit();
	return 0;
}

