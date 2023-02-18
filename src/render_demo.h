/* date = February 13th 2023 8:11 pm */

#ifndef RENDER_DEMO_H
#define RENDER_DEMO_H

#include "base/str.h"
#include "base/vmath.h"

#define MAX_TRIANGLES 2048
#define MAX_VERTICES  MAX_TRIANGLES * 3

typedef struct Render_Vertex {
	vec2 pos;
	vec4 color;
	vec2 uv;
	float tex_index;
} Render_Vertex;

typedef struct Renderer {
	// The required OpenGL objects
	u32 vao;
	u32 vbo;
	u32 shader;
	
	mat4 projection;
	
	// Tightly packed triangle data. This is a cpu side mirror of the buffer
	Render_Vertex triangle_data[MAX_VERTICES];
	u32 triangle_count;
	
	// Texture stuff
	u32 textures[8];
	u32 texture_count;
} Renderer;

void Render_Init(Renderer* r);
void Render_Free(Renderer* r);
void Render_Begin_Frame(Renderer* r);
void Render_End_Frame(Renderer* r);
void Render_Push_Triangle(Renderer* r,
						  vec2 a, vec2 b, vec2 c,
						  vec4 a_color, vec4 b_color, vec4 c_color,
						  vec2 a_uv, vec2 b_uv, vec2 c_uv,
						  u32 texture);

// Helpers
u32  Render_GetWhiteTexture();

#endif //RENDER_DEMO_H
