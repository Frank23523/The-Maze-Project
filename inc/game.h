#ifndef GAME_H
#define GAME_H

#include "structs.h"
#include "input.h"
#include "raycasting.h"
#include "map.h"
#include "rendering.h"

void game_cleanup(struct Game *game, int exit_status);
bool sdl_initialize(struct Game *game, const char *map_file_path);
SDL_Texture* load_texture(SDL_Renderer *renderer, const char *path);
bool create_window_and_renderer(struct Game *game);
bool initialize_sdl_and_img(void);

#endif
