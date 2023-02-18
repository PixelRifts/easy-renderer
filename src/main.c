#include "defines.h"
#include "os/os.h"
#include "os/window.h"
#include "os/input.h"
#include "base/tctx.h"
#include "core/backend.h"
#include "core/resources.h"

#include "render_demo.h"
#include "render_ext.h"

int main() {
	OS_Init();
	
	ThreadContext context = {0};
	tctx_init(&context);
	
	M_Arena global_arena;
	arena_init(&global_arena);
	
	OS_Window* window = OS_WindowCreate(1080, 720, str_lit("This should work"));
	B_BackendInit(window);
	OS_WindowShow(window);
	
	Renderer renderer;
	Render_Init(&renderer);
	Renderer_Extension r_ext;
	Render_Extension_Init(&r_ext);
	r_ext.culling_quad = rect_init(0, 0, 1080.f / 2.f, 720.f);
	
	u32 white_texture = Render_GetWhiteTexture();
	Render_FontInfo inconsolata_font = {0};
	Render_FontLoad(&inconsolata_font, str_lit("res/Inconsolata.ttf"), 32);
	
	u32 pepper = Render_TextureLoad(str_lit("res/pepper.png"));
	
	while (OS_WindowIsOpen(window)) {
		OS_PollEvents();
		
		Render_Begin_Frame(&renderer);
		Render_Push_Quad_T(&renderer, &r_ext, rect_init(485, 400, 100, 100), Color_White, pepper);
		Render_Push_String(&renderer, &r_ext, &inconsolata_font, str_lit("Text Rendering!"), vec2_init(500, 100), Color_Yellow);
		Render_End_Frame(&renderer);
		
		B_BackendSwapchainNext(window);
	}
	
	Render_Free(&renderer);
	
	B_BackendFree(window);
	
	OS_WindowClose(window);
	
	arena_free(&global_arena);
	tctx_free(&context);
}
