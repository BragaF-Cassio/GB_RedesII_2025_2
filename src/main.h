#ifndef MAIN_H
#define MAIN_H
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <string>
#include <vector>

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h" // Required for std::string overloads
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include <SDL3/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

#include "shared.h"
#include "codificadorDeFonte.h"
#include "codificadorDeCanal.h"
#include "decodificadorDeCanal.h"
#include "decodificadorDeFonte.h"

#endif // MAIN_H // MAIN_H