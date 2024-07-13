#ifndef RAYCASTING_H
#define RAYCASTING_H

#include "structs.h"
#include "rendering.h"

void cast_single_ray(struct Game *game, float ray_angle,
		int ray_index, float *ray_end_x, float *ray_end_y);
void cast_rays(struct Game *game, float ray_ends_x[NUM_RAYS],
		float ray_ends_y[NUM_RAYS]);
void cast_horizontal_ray(struct Game *game, float ray_angle, int facing_down,
		int facing_right, float *horiz_x, float *horiz_y, float *horiz_distance);
void cast_vertical_ray(struct Game *game, float ray_angle, int facing_down,
		int facing_right, float *vert_x, float *vert_y, float *vert_distance);

#endif
