#include "render_demo.h"

#include "core/impl/gl_functions.h"
#include "base/base.h"
#include "os/os.h"

void Render_Init(Renderer* r) {
	glGenVertexArrays(1, &r->vao);
	glBindVertexArray(r->vao);
	
	glGenBuffers(1, &r->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
	glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(Render_Vertex), nullptr, GL_DYNAMIC_DRAW);
	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Render_Vertex), (void*) offsetof(Render_Vertex, pos));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Render_Vertex), (void*) offsetof(Render_Vertex, color));
	glEnableVertexAttribArray(1);
	
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Render_Vertex), (void*) offsetof(Render_Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Render_Vertex), (void*) offsetof(Render_Vertex, tex_index));
	glEnableVertexAttribArray(3);
	
	r->projection = mat4_ortho(0, 1080, 720, 0, -0.01, 1.0);
	
	r->shader = glCreateProgram();
    u32 vert_module = glCreateShader(GL_VERTEX_SHADER);
    u32 frag_module = glCreateShader(GL_FRAGMENT_SHADER);
	
	// Ignore stuff using arenas, use malloc if you please
	// These are just helpers from this codebase for nicer file reading
	M_Arena arena;
	arena_init(&arena);
	
	string vert_code = OS_FileRead(&arena, str_lit("res/vertexshader.vert.glsl"));
	i32 vert_code_size = vert_code.size;
	glShaderSource(vert_module, 1, (const GLchar *const *)&vert_code.str, &vert_code_size);
	
	string frag_code = OS_FileRead(&arena, str_lit("res/fragmentshader.frag.glsl"));
	i32 frag_code_size = frag_code.size;
	glShaderSource(frag_module, 1, (const GLchar *const *)&frag_code.str, &frag_code_size);
	
	glCompileShader(vert_module);
	glCompileShader(frag_module);
	
	i32 error;
	glGetShaderiv(vert_module, GL_COMPILE_STATUS, &error);
	if (error == GL_FALSE) {
		LogError("Vertex Shader Compilation failed!");
		i32 length = 0;
		glGetShaderiv(vert_module, GL_INFO_LOG_LENGTH, &length);
		
		GLchar* info = arena_alloc(&arena, length * sizeof(GLchar));
		glGetShaderInfoLog(vert_module, length * sizeof(GLchar), nullptr, info);
		LogError("%s", info);
	}
	
	glGetShaderiv(frag_module, GL_COMPILE_STATUS, &error);
	if (error == GL_FALSE) {
		LogError("Fragment Shader Compilation failed!");
		i32 length = 0;
		glGetShaderiv(frag_module, GL_INFO_LOG_LENGTH, &length);
		
		GLchar* info = arena_alloc(&arena, length * sizeof(GLchar));
		glGetShaderInfoLog(frag_module, length * sizeof(GLchar), nullptr, info);
		LogError("%s", info);
	}
	
	glAttachShader(r->shader, vert_module);
	glAttachShader(r->shader, frag_module);
	
	glLinkProgram(r->shader);
	glGetProgramiv(r->shader, GL_LINK_STATUS, &error);
	if (error == GL_FALSE) {
		LogError("Program Linking Failed:\n");
		i32 length = 0;
		glGetProgramiv(r->shader, GL_INFO_LOG_LENGTH, &length);
		
		GLchar* info = arena_alloc(&arena, length * sizeof(GLchar));
		glGetProgramInfoLog(r->shader, length, nullptr, info);
		LogError("%s", info);
	}
	
	glDetachShader(r->shader, vert_module);
	glDetachShader(r->shader, frag_module);
	glDeleteShader(vert_module);
	glDeleteShader(frag_module);
	
	glUseProgram(r->shader);
	u32 proj_loc = glGetUniformLocation(r->shader, "u_proj");
	// the member a here is the underlying array. mat4 is defined as so:
	//    typedef struct mat4 { f32 a[4*4]; } mat4;
	// Arrays implicitly become pointers, so this works nicely
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, r->projection.a);
	
	
	u32 tex_loc = glGetUniformLocation(r->shader, "u_tex");
	s32 textures[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
	glUniform1iv(tex_loc, 8, textures);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	arena_free(&arena);
}

void Render_Free(Renderer* r) {
	glDeleteBuffers(1, &r->vbo);
	glDeleteVertexArrays(1, &r->vao);
	
	glDeleteProgram(r->shader);
}

void Render_Begin_Frame(Renderer* r) {
	glClear(GL_COLOR_BUFFER_BIT);
	
	r->triangle_count = 0;
	r->texture_count = 0;
}

void Render_End_Frame(Renderer* r) {
	
	for (u32 i = 0; i < r->texture_count; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, r->textures[i]);
	}
	
	glUseProgram(r->shader);
	glBindVertexArray(r->vao);
	glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, r->triangle_count * 3 * size_of(Render_Vertex), r->triangle_data);
	
	glDrawArrays(GL_TRIANGLES, 0, r->triangle_count * 3);
}

void Render_Push_Triangle(Renderer* r,
						  vec2 a, vec2 b, vec2 c,
						  vec4 a_color, vec4 b_color, vec4 c_color,
						  vec2 a_uv, vec2 b_uv, vec2 c_uv,
						  u32 texture) {
	
	// 1248 is just an invalid value since this is an unsigned number, -1 doesnt work
	u32 tex_index = 1248;
	for (u32 i = 0; i < r->texture_count; i++) {
		if (r->textures[i] == texture) {
			tex_index = i;
			break;
		}
	}
	
	// r->texture_count < 8 confirms we don't overwrite the texture_count
	if (tex_index == 1248 && r->texture_count < 8) {
		r->textures[r->texture_count] = texture;
		tex_index = r->texture_count;
		r->texture_count += 1;
	}
	
	
	// Flush the batch if it is full. We don't like segfaults on this channel.
	if (r->triangle_count == MAX_TRIANGLES || tex_index == 1248) {
		Render_End_Frame(r);
		Render_Begin_Frame(r);
	}
	
	r->triangle_data[r->triangle_count * 3 + 0].pos = a;
	r->triangle_data[r->triangle_count * 3 + 0].color = a_color;
	r->triangle_data[r->triangle_count * 3 + 1].pos = b;
	r->triangle_data[r->triangle_count * 3 + 1].color = b_color;
	r->triangle_data[r->triangle_count * 3 + 2].pos = c;
	r->triangle_data[r->triangle_count * 3 + 2].color = c_color;
	
	r->triangle_data[r->triangle_count * 3 + 0].uv = a_uv;
	r->triangle_data[r->triangle_count * 3 + 0].tex_index = tex_index;
	r->triangle_data[r->triangle_count * 3 + 1].uv = b_uv;
	r->triangle_data[r->triangle_count * 3 + 1].tex_index = tex_index;
	r->triangle_data[r->triangle_count * 3 + 2].uv = c_uv;
	r->triangle_data[r->triangle_count * 3 + 2].tex_index = tex_index;
	
	r->triangle_count++;
}

void Render_Push_Quad_T(Renderer* r, rect quad, vec4 tint, u32 texture) {
	Render_Push_Triangle(r,
						 vec2_init(quad.x, quad.y), vec2_init(quad.x + quad.w, quad.y), vec2_init(quad.x + quad.w, quad.y + quad.h),
						 tint, tint, tint,
						 vec2_init(0, 0), vec2_init(1, 0), vec2_init(1, 1), texture);
	Render_Push_Triangle(r,
						 vec2_init(quad.x, quad.y), vec2_init(quad.x + quad.w, quad.y + quad.h), vec2_init(quad.x, quad.y + quad.h),
						 tint, tint, tint,
						 vec2_init(0, 0), vec2_init(1, 0), vec2_init(1, 1), texture);
}

void Render_Push_Quad_ST(Renderer* r, rect quad, rect uv_quad, u32 texture, vec4 tint) {
	Render_Push_Triangle(r,
						 vec2_init(quad.x, quad.y), vec2_init(quad.x + quad.w, quad.y),
						 vec2_init(quad.x + quad.w, quad.y + quad.h),
						 tint, tint, tint,
						 vec2_init(uv_quad.x, uv_quad.y),
						 vec2_init(uv_quad.x + uv_quad.w, uv_quad.y), vec2_init(uv_quad.x + uv_quad.w, uv_quad.y + uv_quad.h), texture);
	Render_Push_Triangle(r,
						 vec2_init(quad.x, quad.y), vec2_init(quad.x + quad.w, quad.y + quad.h), vec2_init(quad.x, quad.y + quad.h),
						 tint, tint, tint,
						 vec2_init(uv_quad.x, uv_quad.y),
						 vec2_init(uv_quad.x + uv_quad.w, uv_quad.y + uv_quad.h),
						 vec2_init(uv_quad.x, uv_quad.y + uv_quad.h), texture);
}

void Render_Push_Quad_STFlipped(Renderer* r, rect quad, rect uv_quad, u32 texture, vec4 tint) {
	Render_Push_Triangle(r,
						 vec2_init(quad.x, quad.y), vec2_init(quad.x + quad.w, quad.y),
						 vec2_init(quad.x + quad.w, quad.y + quad.h),
						 tint, tint, tint,
						 vec2_init(uv_quad.x, uv_quad.y + uv_quad.h),
						 vec2_init(uv_quad.x + uv_quad.w, uv_quad.y + uv_quad.h), vec2_init(uv_quad.x + uv_quad.w, uv_quad.y), texture);
	Render_Push_Triangle(r,
						 vec2_init(quad.x, quad.y), vec2_init(quad.x + quad.w, quad.y + quad.h), vec2_init(quad.x, quad.y + quad.h),
						 tint, tint, tint,
						 vec2_init(uv_quad.x, uv_quad.y + uv_quad.h),
						 vec2_init(uv_quad.x + uv_quad.w, uv_quad.y),
						 vec2_init(uv_quad.x, uv_quad.y), texture);
}

void Render_Push_String(Renderer* r, Render_FontInfo* fontinfo, string str, vec2 pos, vec4 color) {
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
			Render_Push_Quad_STFlipped(r, loc, uvs, fontinfo->font_texture, color);
			pos.x += info->xadvance;
		}
	}
}

//~ Helper stuff
u32 _cached_white = 4096;

u32 Render_GetWhiteTexture() {
	if (_cached_white == 4096) {
		u32 tex;
		u8 image[4] = { 255, 255, 255, 255 };
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		_cached_white = tex;
	}
	return _cached_white;
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
