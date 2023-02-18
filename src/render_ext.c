#include "base/base.h"

#include "core/impl/gl_functions.h"
#include "render_demo.h"
#include "render_ext.h"

//~ Render_Extension Basics

void Render_Extension_Init(Renderer_Extension* ext) {
	// @hardcoded @resize
	ext->culling_quad = rect_init(0, 0, 1080, 720);
}

//~ Rendering Extended Helper functions

void Render_Push_Quad_C(Renderer* r, Renderer_Extension* ext, rect quad, vec4 color) {
	u32 texture = Render_GetWhiteTexture();
	Render_Push_Triangle(r,
						 vec2_clamp(vec2_init(quad.x, quad.y), ext->culling_quad),
						 vec2_clamp(vec2_init(quad.x + quad.w, quad.y), ext->culling_quad),
						 vec2_clamp(vec2_init(quad.x + quad.w, quad.y + quad.h), ext->culling_quad),
						 color, color, color,
						 vec2_init(0, 0), vec2_init(1, 0), vec2_init(1, 1), texture);
	Render_Push_Triangle(r,
						 vec2_clamp(vec2_init(quad.x, quad.y), ext->culling_quad),
						 vec2_clamp(vec2_init(quad.x + quad.w, quad.y + quad.h), ext->culling_quad),
						 vec2_clamp(vec2_init(quad.x, quad.y + quad.h), ext->culling_quad),
						 color, color, color,
						 vec2_init(0, 0), vec2_init(1, 0), vec2_init(1, 1), texture);
}

void Render_Push_Quad_T(Renderer* r, Renderer_Extension* ext, rect quad, vec4 tint, u32 texture) {
	rect uv_culled = rect_uv_cull(quad, rect_init(0, 0, 1, 1), ext->culling_quad);
	Render_Push_Triangle(r,
						 vec2_clamp(vec2_init(quad.x, quad.y), ext->culling_quad),
						 vec2_clamp(vec2_init(quad.x + quad.w, quad.y), ext->culling_quad),
						 vec2_clamp(vec2_init(quad.x + quad.w, quad.y + quad.h), ext->culling_quad),
						 tint, tint, tint,
						 vec2_init(uv_culled.x, uv_culled.y),
						 vec2_init(uv_culled.x + uv_culled.w, uv_culled.y), vec2_init(uv_culled.x + uv_culled.w, uv_culled.y + uv_culled.h),texture);
	Render_Push_Triangle(r,
						 vec2_clamp(vec2_init(quad.x, quad.y), ext->culling_quad),
						 vec2_clamp(vec2_init(quad.x + quad.w, quad.y + quad.h), ext->culling_quad),
						 vec2_clamp(vec2_init(quad.x, quad.y + quad.h), ext->culling_quad),
						 tint, tint, tint,
						 vec2_init(uv_culled.x, uv_culled.y),
						 vec2_init(uv_culled.x + uv_culled.w, uv_culled.y + uv_culled.h),
						 vec2_init(uv_culled.x, uv_culled.y + uv_culled.h), texture);
}

void Render_Push_Quad_ST(Renderer* r, Renderer_Extension* ext, rect quad, rect uv_quad, u32 texture, vec4 tint) {
	rect uv_culled = rect_uv_cull(quad, uv_quad, ext->culling_quad);
	Render_Push_Triangle(r,
						 vec2_clamp(vec2_init(quad.x, quad.y), ext->culling_quad),
						 vec2_clamp(vec2_init(quad.x + quad.w, quad.y), ext->culling_quad),
						 vec2_clamp(vec2_init(quad.x + quad.w, quad.y + quad.h), ext->culling_quad),
						 tint, tint, tint,
						 vec2_init(uv_culled.x, uv_culled.y),
						 vec2_init(uv_culled.x + uv_culled.w, uv_culled.y), vec2_init(uv_culled.x + uv_culled.w, uv_culled.y + uv_culled.h), texture);
	Render_Push_Triangle(r,
						 vec2_clamp(vec2_init(quad.x, quad.y), ext->culling_quad), vec2_clamp(vec2_init(quad.x + quad.w, quad.y + quad.h), ext->culling_quad), vec2_clamp(vec2_init(quad.x, quad.y + quad.h), ext->culling_quad),
						 tint, tint, tint,
						 vec2_init(uv_culled.x, uv_culled.y),
						 vec2_init(uv_culled.x + uv_culled.w, uv_culled.y + uv_culled.h),
						 vec2_init(uv_culled.x, uv_culled.y + uv_culled.h), texture);
}

void Render_Push_Quad_STFlipped(Renderer* r, Renderer_Extension* ext, rect quad, rect uv_quad, u32 texture, vec4 tint) {
	rect uv_culled = rect_uv_cull(quad, uv_quad, ext->culling_quad);
	Render_Push_Triangle(r,
						 vec2_clamp(vec2_init(quad.x, quad.y), ext->culling_quad), vec2_clamp(vec2_init(quad.x + quad.w, quad.y), ext->culling_quad),
						 vec2_clamp(vec2_init(quad.x + quad.w, quad.y + quad.h), ext->culling_quad),
						 tint, tint, tint,
						 vec2_init(uv_culled.x, uv_culled.y + uv_culled.h),
						 vec2_init(uv_culled.x + uv_culled.w, uv_culled.y + uv_culled.h), vec2_init(uv_culled.x + uv_culled.w, uv_culled.y), texture);
	Render_Push_Triangle(r,
						 vec2_clamp(vec2_init(quad.x, quad.y), ext->culling_quad), vec2_clamp(vec2_init(quad.x + quad.w, quad.y + quad.h), ext->culling_quad), vec2_clamp(vec2_init(quad.x, quad.y + quad.h), ext->culling_quad),
						 tint, tint, tint,
						 vec2_init(uv_culled.x, uv_culled.y + uv_culled.h),
						 vec2_init(uv_culled.x + uv_culled.w, uv_culled.y),
						 vec2_init(uv_culled.x, uv_culled.y), texture);
}

void Render_Push_String(Renderer* r, Renderer_Extension* ext, Render_FontInfo* fontinfo, string str, vec2 pos, vec4 color) {
	for (u32 i = 0; i < str.size; i++) {
		if (str.str[i] >= 32 && str.str[i] < 128) {
			stbtt_packedchar* info = &fontinfo->cdata[str.str[i] - 32];
			rect uvs = {
				info->x0 / 512.f, info->y0 / 512.f,
				(info->x1 - info->x0) / 512.f, (info->y1 - info->y0) / 512.f
			};
			rect loc = {
				pos.x + info->xoff, pos.y - info->yoff2,
				info->x1 - info->x0, info->y1 - info->y0
			};
			Render_Push_Quad_STFlipped(r, ext, loc, uvs, fontinfo->font_texture, color);
			pos.x += info->xadvance;
		}
	}
}

//~ Asset Stuff

u32 Render_TextureLoad(string filepath) {
	i32 width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	u8* data = stbi_load((const char*)filepath.str, &width, &height, &channels, 0);
	
	// @hardcoded I don't want abstraction for different filters. This is just a demo.
	
	u32 id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	
	if (channels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	if (channels == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	if (channels == 3)
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	else if (channels == 4)
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	
	stbi_image_free(data);
	return id;
}

void Render_TextureFree(u32 handle) {
	glDeleteTextures(1, &handle);
}

void Render_FontLoad(Render_FontInfo* fontinfo, string filename, f32 size) {
	FILE* ttfile = fopen((char*)filename.str, "rb");
	AssertTrue(ttfile, "Font file '%.*s' couldn't be opened", str_expand(filename));
	fseek(ttfile, 0, SEEK_END);
	u64 length = ftell(ttfile);
	rewind(ttfile);
	u8 buffer[length * sizeof(u8)];
	fread(buffer, length, 1, ttfile);
	fclose(ttfile);
	
	u8 temp_bitmap[512 * 512];
	
	stbtt_fontinfo finfo;
	stbtt_pack_context packctx;
	stbtt_InitFont(&finfo, buffer, 0);
	stbtt_PackBegin(&packctx, temp_bitmap, 512, 512, 0, 1, 0);
	stbtt_PackSetOversampling(&packctx, 1, 1);
	stbtt_PackFontRange(&packctx, buffer, 0, size, 32, 95, fontinfo->cdata);
	stbtt_PackEnd(&packctx);
	
	glGenTextures(1, &fontinfo->font_texture);
	glBindTexture(GL_TEXTURE_2D, fontinfo->font_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	GLint swizzles[4] = { GL_ONE, GL_ONE, GL_ONE, GL_RED };
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzles);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 512, 512, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
	
	fontinfo->scale = stbtt_ScaleForPixelHeight(&finfo, size);
	stbtt_GetFontVMetrics(&finfo, &fontinfo->ascent, &fontinfo->descent, nullptr);
	fontinfo->baseline = (i32) (fontinfo->ascent * fontinfo->scale);
	fontinfo->font_size = size;
}

void Render_FontFree(Render_FontInfo* fontinfo) {
	glDeleteTextures(1, &fontinfo->font_texture);
}
