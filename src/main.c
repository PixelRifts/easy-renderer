#include "defines.h"
#include "os/os.h"
#include "os/window.h"
#include "os/input.h"
#include "base/tctx.h"
#include "core/backend.h"
#include "core/resources.h"

#include "render_demo.h"

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
	
	u32 white_texture = Render_GetWhiteTexture();
	Render_FontInfo inconsolata_font = {0};
	Render_FontLoad(&inconsolata_font, str_lit("res/Inconsolata.ttf"), 32);
	
	while (OS_WindowIsOpen(window)) {
		OS_PollEvents();
		
		Render_Begin_Frame(&renderer);
		Render_Push_Triangle(&renderer,
							 vec2_init(100, 100), vec2_init(400, 100), vec2_init(400, 400),
							 Color_Red, Color_Blue, Color_Green,
							 vec2_init(0, 0), vec2_init(1, 0), vec2_init(1, 1),
							 white_texture);
		Render_Push_Quad_T(&renderer, rect_init(400, 400, 100, 100), Color_Magenta, white_texture);
		Render_Push_String(&renderer, &inconsolata_font, str_lit("Text Rendering!"), vec2_init(500, 100), Color_Yellow);
		Render_End_Frame(&renderer);
		
		B_BackendSwapchainNext(window);
	}
	
	Render_Free(&renderer);
	
	B_BackendFree(window);
	
	OS_WindowClose(window);
	
	arena_free(&global_arena);
	tctx_free(&context);
}
