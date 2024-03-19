@echo off

if not exist "build" mkdir build

set cxxflags=-std=c++23 -sINITIAL_MEMORY=64MB -sUSE_SDL=2 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2
set lddflags=-Iexternal/glm
set warnings=-Wall -Wpedantic -Wsign-conversion -Wno-gnu-anonymous-struct -Wno-nested-anon-types
set debugflags=-sSAFE_HEAP=1 -sSTACK_OVERFLOW_CHECK=2 -fno-omit-frame-pointer -g 
set releaseflags=-DNDEBUG -O3

em++ src/sdlgl_main.cpp -o build/main.js %cxxflags% %lddflags% %debugflags% %warnings%
echo em++ src/sdlgl_main.cpp            -o build/main.js %cxxflags% %lddflags% %releaseflags%
