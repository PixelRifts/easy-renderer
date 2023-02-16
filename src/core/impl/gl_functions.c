#include "defines.h"
#include "gl_functions.h"

typedef i64 long_func();
typedef long_func* loader_func(const char* name);

#define X(Name, Return, Args) GL_##Name##_Func* Name;
GL_FUNCTIONS
#undef X

void __LoadGLFunctions(loader_func* load_proc, loader_func* fallback) {
#define X(Name, Return, Args) \
Name = (GL_##Name##_Func *) load_proc(#Name);\
if (!Name) Name = (GL_##Name##_Func *) fallback(#Name);
	
	GL_FUNCTIONS
		
#undef X
}
