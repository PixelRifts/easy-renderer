# easy-renderer

A Very simple Renderer API written in C.
I'm using my [C-Codebase](https://github.com/PixelRifts/c-codebase) but the use is pretty minimal.
Mainly used for the string helpers for loading files and logging to the console.

The only major thing to see here is the render_demo.c file which has the implementation.
Compile the examples/tetris.c to check out the tetris example.

I explain everything about this renderer in this video: [A Quick, Easy and Extendable OpenGL Renderer](https://www.youtube.com/watch?v=NPnQF4yABwg)

Currently windows only, but it really doesn't have to be. Only thing to add is virtual memory stuff and backend OpenGL context stuff for linux since I'm not using glfw or any platform agnostic windowing library. I just don't have a linux machine to test this on :D. So contributions are welcome there.
