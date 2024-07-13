#ifndef RENDERING_H
#define RENDERING_H

#include "structs.h"

void draw_3d_view(struct Game *game, float ray_angle, float distance,
		int ray_index, int is_vertical);
float calculate_corrected_distance(float distance, float ray_angle,
		float player_angle);
float calculate_line_height(float corrected_distance);
float calculate_line_offset(float line_height);
SDL_Rect setup_wall_strip(int ray_index, float line_offset, float line_height);
SDL_Rect calculate_texture_rect(int texture_width,
		float hit_x, float texture_y_start, float line_height, float step);
void apply_shading(SDL_Texture *texture, float corrected_distance);
void draw_weapon(struct Game *game);

#endif
