@ECHO off
SetLocal EnableDelayedExpansion

IF NOT EXIST bin mkdir bin

SET cc=clang

REM ==============
REM Gets list of all C files
SET c_filenames= 
FOR %%f in (src\*.c) do SET c_filenames=!c_filenames! %%f
FOR %%f in (src\base\*.c) do SET c_filenames=!c_filenames! %%f
FOR %%f in (src\impl\*.c) do SET c_filenames=!c_filenames! %%f
FOR %%f in (src\core\*.c) do SET c_filenames=!c_filenames! %%f
FOR %%f in (src\os\*.c) do SET c_filenames=!c_filenames! %%f
REM ==============


REM ==============
SET compiler_flags=-Wall -Wvarargs -Werror -Wno-unused-function -Wno-format-security -Wno-incompatible-pointer-types-discards-qualifiers -Wno-unused-but-set-variable -Wno-int-to-void-pointer-cast -Wno-unused-variable
SET include_flags=-Isrc -Ithird_party/include -Ithird_party/source
SET linker_flags=-g -lshell32 -luser32 -lwinmm -luserenv -lgdi32 -Lthird_party/lib
SET defines=-D_DEBUG -D_CRT_SECURE_NO_WARNINGS
SET output=-obin/renderer_demo.exe
SET backend=-DBACKEND_GL33
REM ==============

REM SET compiler_flags=!compiler_flags! -fsanitize=address

ECHO "Building renderer_demo.exe..."
%cc% %c_filenames% %compiler_flags% %defines% %backend% %include_flags% %linker_flags% %output%
