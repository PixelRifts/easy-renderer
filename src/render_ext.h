/* date = February 18th 2023 0:39 pm */

//  This file is here so that I can keep render_demo.h as slim as possible
//  It can easily be merged with render_demo

#ifndef RENDER_EXT_H
#define RENDER_EXT_H

#include <stb/stb_truetype.h>
#include <stb/stb_image.h>

typedef struct Renderer_Extension {
	rect culling_quad;
} Renderer_Extension;

void Render_Extension_Init(Renderer_Extension* ext);

typedef struct Render_FontInfo {
	u32 font_texture;
    stbtt_packedchar cdata[95];
    f32 scale;
    f32 font_size;
    i32 ascent;
    i32 descent;
    i32 baseline;
} Render_FontInfo;

u32  Render_TextureLoad(string filepath);
void Render_TextureFree(u32 handle);

void Render_FontLoad(Render_FontInfo* fontinfo, string filename, f32 size);
void Render_FontFree(Render_FontInfo* fontinfo);

void Render_Push_Quad_C(Renderer* r, Renderer_Extension* ext, rect quad, vec4 color);
void Render_Push_Quad_T(Renderer* r, Renderer_Extension* ext, rect quad, vec4 tint, u32 texture);
void Render_Push_Quad_ST(Renderer* r, Renderer_Extension* ext, rect quad, rect uv_quad, u32 texture, vec4 tint);
void Render_Push_Quad_STFlipped(Renderer* r, Renderer_Extension* ext, rect quad, rect uv_quad, u32 texture, vec4 tint);
void Render_Push_String(Renderer* r, Renderer_Extension* ext, Render_FontInfo* fontinfo, string str, vec2 pos, vec4 color);

#endif //RENDER_EXT_H
