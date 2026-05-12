#ifndef VUE_CONTROLLER_H
#define VUE_CONTROLLER_H

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "Model/model.h"

int init_window(Game* game);
void setup(Game* game);
void process_input(Game* game);
void update(Game* game);
void update_planets(Planet* planet, Star* sun, double time);
void render(Game* game);
void destroy_window();


#endif