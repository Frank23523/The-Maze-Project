#ifndef MAP_H
#define MAP_H

#include "structs.h"

void draw_2d_map(struct Game *game);
void draw_map_background(struct Game *game, int map_x,
		int map_y, int map_size);
void draw_map_tiles(struct Game *game, int map_x, int map_y, int tile_size);
void draw_player(struct Game *game, int map_x, int map_y, int tile_size);
void draw_fov_rays(struct Game *game, int map_x, int map_y, int tile_size,
		int player_x, int player_y, int player_size);
bool parse_map_file(struct Game *game, const char *file_path);

#endif
