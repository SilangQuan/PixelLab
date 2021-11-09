/*
#pragma comment( lib, "..\\..\\Lib\\Win32\\Debug\\AppFramework.lib" )

#include "../include/App.h"
#include "../../SkyEngine/include/SkyEngine.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "../../3rdParty/include/SDL/SDL.h"
#include "../include/shader_utils.h"


#define GLM_FORCE_RADIANS
#include "../../3rdParty/include/GLM/glm.hpp"
#include "../../3rdParty/include/GLM/gtc/matrix_transform.hpp"
#include "../../3rdParty/include/GLM/gtc/type_ptr.hpp"
#include "../../3rdParty/include/GLM/gtc/noise.hpp"


#include "textures.c"

static GLuint program;
static GLint attribute_coord;
static GLint uniform_mvp;
static GLuint texture;
static GLint uniform_texture;
static GLuint cursor_vbo;

static glm::vec3 cameraPosition;
static glm::vec3 cameraForward;
static glm::vec3 cameraRight;
static glm::vec3 cameraUp;
static glm::vec3 cameraLookat;
static glm::vec3 cameraAngle;

static int ww, wh;
static int mx, my, mz;
static int face;
static uint8_t buildtype = 1;

static int now;
static unsigned int keys;



static const int transparent[16] = { 2, 0, 0, 0, 1, 0, 0, 0, 3, 4, 0, 0, 0, 0, 0, 0 };
static const char *blocknames[16] = {
	"air", "dirt", "topsoil", "grass", "leaves", "wood", "stone", "sand",
	"water", "glass", "brick", "ore", "woodrings", "white", "black", "x-y"
};



struct superchunk {
	chunk *c[SCX][SCY][SCZ];
	time_t seed;

	superchunk() {
		seed = time(NULL);
		for (int x = 0; x < SCX; x++)
			for (int y = 0; y < SCY; y++)
				for (int z = 0; z < SCZ; z++)
					c[x][y][z] = new chunk(x - SCX / 2, y - SCY / 2, z - SCZ / 2);

		for (int x = 0; x < SCX; x++)
			for (int y = 0; y < SCY; y++)
				for (int z = 0; z < SCZ; z++) {
					if (x > 0)
						c[x][y][z]->left = c[x - 1][y][z];
					if (x < SCX - 1)
						c[x][y][z]->right = c[x + 1][y][z];
					if (y > 0)
						c[x][y][z]->below = c[x][y - 1][z];
					if (y < SCY - 1)
						c[x][y][z]->above = c[x][y + 1][z];
					if (z > 0)
						c[x][y][z]->front = c[x][y][z - 1];
					if (z < SCZ - 1)
						c[x][y][z]->back = c[x][y][z + 1];
				}
	}

	uint8_t get(int x, int y, int z) const {
		int cx = (x + CX * (SCX / 2)) / CX;
		int cy = (y + CY * (SCY / 2)) / CY;
		int cz = (z + CZ * (SCZ / 2)) / CZ;

		if (cx < 0 || cx >= SCX || cy < 0 || cy >= SCY || cz <= 0 || cz >= SCZ)
			return 0;

		return c[cx][cy][cz]->get(x & (CX - 1), y & (CY - 1), z & (CZ - 1));
	}

	void set(int x, int y, int z, uint8_t type) {
		int cx = (x + CX * (SCX / 2)) / CX;
		int cy = (y + CY * (SCY / 2)) / CY;
		int cz = (z + CZ * (SCZ / 2)) / CZ;

		if (cx < 0 || cx >= SCX || cy < 0 || cy >= SCY || cz <= 0 || cz >= SCZ)
			return;

		c[cx][cy][cz]->set(x & (CX - 1), y & (CY - 1), z & (CZ - 1), type);
	}

	void render(const glm::mat4 &pv) {
		float ud = Mathf::Infinity;
		int ux = -1;
		int uy = -1;
		int uz = -1;

		for (int x = 0; x < SCX; x++) {
			for (int y = 0; y < SCY; y++) {
				for (int z = 0; z < SCZ; z++) {
					glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(c[x][y][z]->ax * CX, c[x][y][z]->ay * CY, c[x][y][z]->az * CZ));
					glm::mat4 mvp = pv * model;

					// Is this chunk on the screen?
					glm::vec4 center = mvp * glm::vec4(CX / 2, CY / 2, CZ / 2, 1);

					float d = glm::length(center);
					center.x /= center.w;
					center.y /= center.w;

					// If it is behind the camera, don't bother drawing it
					if (center.z < -CY / 2)
						continue;

					// If it is outside the screen, don't bother drawing it
					if (fabsf(center.x) > 1 + fabsf(CY * 2 / center.w) || fabsf(center.y) > 1 + fabsf(CY * 2 / center.w))
						continue;

					// If this chunk is not initialized, skip it
					if (!c[x][y][z]->initialized) {
						// But if it is the closest to the camera, mark it for initialization
						if (ux < 0 || d < ud) {
							ud = d;
							ux = x;
							uy = y;
							uz = z;
						}
						continue;
					}

					glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

					c[x][y][z]->render();
				}
			}
		}

		if (ux >= 0) {
			c[ux][uy][uz]->noise(seed);
			if (c[ux][uy][uz]->left)
				c[ux][uy][uz]->left->noise(seed);
			if (c[ux][uy][uz]->right)
				c[ux][uy][uz]->right->noise(seed);
			if (c[ux][uy][uz]->below)
				c[ux][uy][uz]->below->noise(seed);
			if (c[ux][uy][uz]->above)
				c[ux][uy][uz]->above->noise(seed);
			if (c[ux][uy][uz]->front)
				c[ux][uy][uz]->front->noise(seed);
			if (c[ux][uy][uz]->back)
				c[ux][uy][uz]->back->noise(seed);
			c[ux][uy][uz]->initialized = true;
		}
	}
};

static superchunk *world;

// Calculate the forward, right and lookat vectors from the angle vector
static void update_vectors() {
	cameraForward.x = sinf(cameraAngle.x);
	cameraForward.y = 0;
	cameraForward.z = cosf(cameraAngle.x);

	cameraRight.x = -cosf(cameraAngle.x);
	cameraRight.y = 0;
	cameraRight.z = sinf(cameraAngle.x);

	cameraLookat.x = sinf(cameraAngle.x) * cosf(cameraAngle.y);
	cameraLookat.y = sinf(cameraAngle.y);
	cameraLookat.z = cosf(cameraAngle.x) * cosf(cameraAngle.y);

	cameraUp = glm::cross(cameraRight, cameraLookat);
}

static int init_resources() {
	/ * Create shaders * /

	program = create_program("../GLCraft/assets/glescraft.v.glsl", "../GLCraft/assets/glescraft.f.glsl");

	if (program == 0)
		return 0;

	attribute_coord = get_attrib(program, "coord");
	uniform_mvp = get_uniform(program, "mvp");

	if (attribute_coord == -1 || uniform_mvp == -1)
		return 0;

	/ * Create and upload the texture * /

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textures.width, textures.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textures.pixel_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	/ * Create the world * /

	world = new superchunk;

	cameraPosition = glm::vec3(0, CY + 1, 0);
	cameraAngle = glm::vec3(0, -0.5, 0);
	update_vectors();

	/ * Create a VBO for the cursor * /

	glGenBuffers(1, &cursor_vbo);

	/ * OpenGL settings that do not change while running this program * /

	glUseProgram(program);
	glUniform1i(uniform_texture, 0);
	glClearColor(0.6, 0.8, 1.0, 0.0);
	glEnable(GL_CULL_FACE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Use GL_NEAREST_MIPMAP_LINEAR if you want to use mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glPolygonOffset(1, 1);

	glEnableVertexAttribArray(attribute_coord);

	return 1;
}

static void reshape(int w, int h) {
	ww = w;
	wh = h;
	glViewport(0, 0, w, h);
}

static void render() {
	glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraLookat, cameraUp);
	glm::mat4 projection = glm::perspective(45.0f, 1.0f*ww / wh, 0.01f, 1000.0f);

	glm::mat4 mvp = projection * view;

	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);

	/ * Then draw chunks * /

	world->render(mvp);

	/ * At which voxel are we looking? * /
	/ * Very naive ray casting algorithm to find out which block we are looking at * /

	glm::vec3 testpos = cameraPosition;
	glm::vec3 prevpos = cameraPosition;

	for (int i = 0; i < 100; i++) {
		/ * Advance from our currect position to the direction we are looking at, in small steps * /

		prevpos = testpos;
		testpos += cameraLookat * 0.1f;

		mx = floorf(testpos.x);
		my = floorf(testpos.y);
		mz = floorf(testpos.z);

		/ * If we find a block that is not air, we are done * /

		if (world->get(mx, my, mz))
			break;
	}

	/ * Find out which face of the block we are looking at * /

	int px = floorf(prevpos.x);
	int py = floorf(prevpos.y);
	int pz = floorf(prevpos.z);

	if (px > mx)
		face = 0;
	else if (px < mx)
		face = 3;
	else if (py > my)
		face = 1;
	else if (py < my)
		face = 4;
	else if (pz > mz)
		face = 2;
	else if (pz < mz)
		face = 5;

	/ * If we are looking at air, move the cursor out of sight * /

	if (!world->get(mx, my, mz))
		mx = my = mz = 99999;

	float bx = mx;
	float by = my;
	float bz = mz;

	/ * Render a box around the block we are pointing at * /

	float box[24][4] = {
		{ bx + 0, by + 0, bz + 0, 14 },
		{ bx + 1, by + 0, bz + 0, 14 },
		{ bx + 0, by + 1, bz + 0, 14 },
		{ bx + 1, by + 1, bz + 0, 14 },
		{ bx + 0, by + 0, bz + 1, 14 },
		{ bx + 1, by + 0, bz + 1, 14 },
		{ bx + 0, by + 1, bz + 1, 14 },
		{ bx + 1, by + 1, bz + 1, 14 },

		{ bx + 0, by + 0, bz + 0, 14 },
		{ bx + 0, by + 1, bz + 0, 14 },
		{ bx + 1, by + 0, bz + 0, 14 },
		{ bx + 1, by + 1, bz + 0, 14 },
		{ bx + 0, by + 0, bz + 1, 14 },
		{ bx + 0, by + 1, bz + 1, 14 },
		{ bx + 1, by + 0, bz + 1, 14 },
		{ bx + 1, by + 1, bz + 1, 14 },

		{ bx + 0, by + 0, bz + 0, 14 },
		{ bx + 0, by + 0, bz + 1, 14 },
		{ bx + 1, by + 0, bz + 0, 14 },
		{ bx + 1, by + 0, bz + 1, 14 },
		{ bx + 0, by + 1, bz + 0, 14 },
		{ bx + 0, by + 1, bz + 1, 14 },
		{ bx + 1, by + 1, bz + 0, 14 },
		{ bx + 1, by + 1, bz + 1, 14 },
	};

	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_CULL_FACE);
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	glBindBuffer(GL_ARRAY_BUFFER, cursor_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINES, 0, 24);

	/ * Draw a cross in the center of the screen * /

	float cross[4][4] = {
		{ -0.05, 0, 0, 13 },
		{ +0.05, 0, 0, 13 },
		{ 0, -0.05, 0, 13 },
		{ 0, +0.05, 0, 13 },
	};

	glDisable(GL_DEPTH_TEST);
	glm::mat4 one(1);
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(one));
	glBufferData(GL_ARRAY_BUFFER, sizeof cross, cross, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINES, 0, 4);

	/ * And we are done * /
}

static void keyDown(SDL_KeyboardEvent *ev) {
	switch (ev->keysym.scancode) {
	case SDL_SCANCODE_LEFT:
	case SDL_SCANCODE_A:
		keys |= 1;
		break;
	case SDL_SCANCODE_RIGHT:
	case SDL_SCANCODE_D:
		keys |= 2;
		break;
	case SDL_SCANCODE_UP:
	case SDL_SCANCODE_W:
		keys |= 4;
		break;
	case SDL_SCANCODE_DOWN:
	case SDL_SCANCODE_S:
		keys |= 8;
		break;
	case SDL_SCANCODE_PAGEUP:
	case SDL_SCANCODE_SPACE:
		keys |= 16;
		break;
	case SDL_SCANCODE_PAGEDOWN:
	case SDL_SCANCODE_C:
		keys |= 32;
		break;
	case SDL_SCANCODE_HOME:
		cameraPosition = glm::vec3(0, CY + 1, 0);
		cameraAngle = glm::vec3(0, -0.5, 0);
		update_vectors();
		break;
	case SDL_SCANCODE_END:
		cameraPosition = glm::vec3(0, CX * SCX, 0);
		cameraAngle = glm::vec3(0, -M_PI * 0.49, 0);
		update_vectors();
		break;
	default:
		break;
	}
}

static void keyUp(SDL_KeyboardEvent *ev) {
	switch (ev->keysym.scancode) {
	case SDL_SCANCODE_LEFT:
	case SDL_SCANCODE_A:
		keys &= ~1;
		break;
	case SDL_SCANCODE_RIGHT:
	case SDL_SCANCODE_D:
		keys &= ~2;
		break;
	case SDL_SCANCODE_UP:
	case SDL_SCANCODE_W:
		keys &= ~4;
		break;
	case SDL_SCANCODE_DOWN:
	case SDL_SCANCODE_S:
		keys &= ~8;
		break;
	case SDL_SCANCODE_PAGEUP:
	case SDL_SCANCODE_SPACE:
		keys &= ~16;
		break;
	case SDL_SCANCODE_PAGEDOWN:
	case SDL_SCANCODE_C:
		keys &= ~32;
		break;
	default:
		break;
	}
}

static void physics() {
	static int prev = 0;
	static const float movespeed = 10;

	now = SDL_GetTicks();
	float dt = (now - prev) * 1.0e-3;
	prev = now;

	if (keys & 1)
		cameraPosition -= cameraRight * movespeed * dt;
	if (keys & 2)
		cameraPosition += cameraRight * movespeed * dt;
	if (keys & 4)
		cameraPosition += cameraForward * movespeed * dt;
	if (keys & 8)
		cameraPosition -= cameraForward * movespeed * dt;
	if (keys & 16)
		cameraPosition.y += movespeed * dt;
	if (keys & 32)
		cameraPosition.y -= movespeed * dt;
}

static void mouseMotion(SDL_MouseMotionEvent *ev) {
	static const float mousespeed = 0.001;

	cameraAngle.x -= ev->xrel * mousespeed;
	cameraAngle.y -= ev->yrel * mousespeed;

	if (cameraAngle.x < -M_PI)
		cameraAngle.x += M_PI * 2;
	if (cameraAngle.x > M_PI)
		cameraAngle.x -= M_PI * 2;
	if (cameraAngle.y < -M_PI / 2)
		cameraAngle.y = -M_PI / 2;
	if (cameraAngle.y > M_PI / 2)
		cameraAngle.y = M_PI / 2;

	update_vectors();
}

static void mouseWheel(SDL_MouseWheelEvent *ev) {
	if (ev->y) {
		if (ev->y > 0)
			buildtype--;
		else
			buildtype++;

		buildtype &= 0xf;
		fprintf(stderr, "Building blocks of type %u (%s)\n", buildtype, blocknames[buildtype]);
		return;
	}
}

static void mouseButtonDown(SDL_MouseButtonEvent *ev) {
	fprintf(stderr, "Clicked on %d, %d, %d, face %d, button %d\n", mx, my, mz, face, ev->button);

	if (ev->button == SDL_BUTTON_LEFT) {
		if (face == 0)
			mx++;
		if (face == 3)
			mx--;
		if (face == 1)
			my++;
		if (face == 4)
			my--;
		if (face == 2)
			mz++;
		if (face == 5)
			mz--;
		world->set(mx, my, mz, buildtype);
	}
	else if (ev->button == SDL_BUTTON_RIGHT) {
		world->set(mx, my, mz, 0);
	}
}

static void windowEvent(SDL_WindowEvent *ev) {
	switch (ev->event) {
	case SDL_WINDOWEVENT_SIZE_CHANGED:
		reshape(ev->data1, ev->data2);
		break;
	default:
		break;
	}
}

static void free_resources() {
	glDeleteProgram(program);
}

static void mainLoop(SDL_Window *window) {
	while (true) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
			case SDL_QUIT:
				return;
			case SDL_KEYDOWN:
				keyDown(&ev.key);
				break;
			case SDL_KEYUP:
				keyUp(&ev.key);
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouseButtonDown(&ev.button);
				break;
			case SDL_MOUSEWHEEL:
				mouseWheel(&ev.wheel);
				break;
			case SDL_MOUSEMOTION:
				mouseMotion(&ev.motion);
				break;
			case SDL_WINDOWEVENT:
				windowEvent(&ev.window);
				break;
			default:
				break;
			}
		}

		physics();
		render();
		SDL_GL_SwapWindow(window);
	}
}

bool App::CreateWorld()
{
	print_opengl_info();
	printf("Use the mouse to look around.\n");
	printf("Use cursor keys, pageup and pagedown to move around.\n");
	printf("Use home and end to go to two predetermined positions.\n");
	printf("Press the left mouse button to build a block.\n");
	printf("Press the right mouse button to remove a block.\n");
	printf("Use the scrollwheel to select different types of blocks.\n");

	return init_resources();
	
}

void App::DestroyWorld()
{
	free_resources();
}

void App::FrameMove()
{

}
void App::RenderWorld()
{
	reshape(400, 300);
	physics();
	render();
}

/ *
int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);

	// Select an OpenGL ES 2.0 profile.
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_Window *window = SDL_CreateWindow("GLEScraft",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		640, 480,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

	// Set relative mouse mode, this will grab the cursor.
	SDL_SetRelativeMouseMode(SDL_TRUE);

	SDL_GLContext context = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1);
	reshape(640, 480);

	print_opengl_info();
	printf("Use the mouse to look around.\n");
	printf("Use cursor keys, pageup and pagedown to move around.\n");
	printf("Use home and end to go to two predetermined positions.\n");
	printf("Press the left mouse button to build a block.\n");
	printf("Press the right mouse button to remove a block.\n");
	printf("Use the scrollwheel to select different types of blocks.\n");

	if (!init_resources())
		return EXIT_FAILURE;

	mainLoop(window);

	free_resources();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
* /*/