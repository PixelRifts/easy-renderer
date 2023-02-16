/* date = February 13th 2023 8:11 pm */

#ifndef RENDER_DEMO_H
#define RENDER_DEMO_H

#include "base/str.h"
#include "base/vmath.h"
#include <stb/stb_truetype.h>

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

typedef struct Render_FontInfo {
	u32 font_texture;
    stbtt_packedchar cdata[95];
    f32 scale;
    f32 font_size;
    i32 ascent;
    i32 descent;
    i32 baseline;
} Render_FontInfo;

void Render_Init(Renderer* r);
void Render_Free(Renderer* r);
void Render_Begin_Frame(Renderer* r);
void Render_End_Frame(Renderer* r);
void Render_Push_Triangle(Renderer* r,
						  vec2 a, vec2 b, vec2 c,
						  vec4 a_color, vec4 b_color, vec4 c_color,
						  vec2 a_uv, vec2 b_uv, vec2 c_uv,
						  u32 texture);

void Render_Push_Quad_T(Renderer* r, rect quad, vec4 tint, u32 texture);
void Render_Push_Quad_ST(Renderer* r, rect quad, rect uv_quad, u32 texture, vec4 tint);
void Render_Push_Quad_STFlipped(Renderer* r, rect quad, rect uv_quad, u32 texture, vec4 tint);
void Render_Push_String(Renderer* r, Render_FontInfo* fontinfo, string str, vec2 pos, vec4 color);

// Helpers
u32  Render_GetWhiteTexture();
void Render_FontLoad(Render_FontInfo* fontinfo, string filename, f32 size);

void Render_FontLoad(Render_FontInfo* fontinfo, string filename, f32 size);
void Render_FontFree(Render_FontInfo* fontinfo);

#endif //RENDER_DEMO_H
