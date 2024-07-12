#include "../inc/raycasting.h"

/**
 * cast_rays - Casts rays to render the 3D view of the game scene.
 * @game: Pointer to the Game structure containing game state.
 * @ray_ends_x: Array to store x coordinates of ray endpoints.
 * @ray_ends_y: Array to store y coordinates of ray endpoints.
 *
 * This function calculates the direction of each ray and casts them to detect
 * intersections with walls. It updates ray_ends_x and ray_ends_y with the
 * coordinates where each ray intersects a wall
 * or reaches the maximum render distance.
 */
void cast_rays(struct Game *game, float ray_ends_x[NUM_RAYS],
		float ray_ends_y[NUM_RAYS])
{
	int i;
	float ray_angle = game->player.angle - DEGREE_TO_RADIAN(FOV_DEGREES / 2);
	float angle_step = DEGREE_TO_RADIAN(FOV_DEGREES) / (NUM_RAYS - 1);

	for (i = 0; i < NUM_RAYS; i++)
	{
		cast_single_ray(game, ray_angle, i, &ray_ends_x[i], &ray_ends_y[i]);
		ray_angle += angle_step;
	}
}

/**
 * cast_horizontal_ray - Casts a horizontal ray and
 *			determines its intersection with walls.
 * @game: Pointer to the Game structure containing game state.
 * @ray_angle: Angle of the ray to cast.
 * @facing_down: Flag indicating whether the ray is facing downwards.
 * @facing_right: Flag indicating whether the ray is facing rightwards.
 * @horiz_x: Pointer to store the x coordinate
 *		of the horizontal ray intersection.
 * @horiz_y: Pointer to store the y coordinate
 *		of the horizontal ray intersection.
 * @horiz_distance: Pointer to store the distance
 *		from the player to the horizontal ray intersection.
 *
 * This function casts a horizontal ray and calculates
 * where it intersects with the nearest wall.
 * It updates horiz_x, horiz_y, and horiz_distance
 * with the intersection point and distance.
 */
void cast_horizontal_ray(struct Game *game, float ray_angle, int facing_down,
		int facing_right, float *horiz_x, float *horiz_y,
		float *horiz_distance)
{
	float y_intercept, x_intercept, y_step, x_step;
	float next_horiz_x, next_horiz_y, x_to_check, y_to_check;
	int map_x, map_y;

	y_intercept = floor(game->player.y / TILE_SIZE) * TILE_SIZE;
	y_intercept += facing_down ? TILE_SIZE : 0;
	x_intercept = (game->player.x + (y_intercept - game->player.y)
			/ tan(ray_angle));

	y_step = TILE_SIZE;
	y_step *= facing_down ? 1 : -1;
	x_step = TILE_SIZE / tan(ray_angle);
	x_step *= (facing_right && x_step < 0) ? -1 : 1;
	x_step *= (!facing_right && x_step > 0) ? -1 : 1;

	next_horiz_x = x_intercept;
	next_horiz_y = y_intercept;

	while (next_horiz_x >= 0 && next_horiz_x < SCREEN_WIDTH &&
			next_horiz_y >= 0 && next_horiz_y < SCREEN_HEIGHT)
	{
		x_to_check = next_horiz_x;
		y_to_check = next_horiz_y + (facing_down ? 0 : -1);
		if (x_to_check < 0 || x_to_check >= MAP_WIDTH * TILE_SIZE ||
				y_to_check < 0 || y_to_check >= MAP_HEIGHT * TILE_SIZE)
			break;
		map_x = (int)(x_to_check / TILE_SIZE);
		map_y = (int)(y_to_check / TILE_SIZE);
		if (game->map[map_y * MAP_WIDTH + map_x] == MAP_WALL)
		{
			*horiz_x = next_horiz_x;
			*horiz_y = next_horiz_y;
			*horiz_distance = sqrt(pow(*horiz_x - game->player.x, 2) +
					pow(*horiz_y - game->player.y, 2));
			return;
		}
		next_horiz_x += x_step;
		next_horiz_y += y_step;
	}
	*horiz_distance = INFINITY;
}

/**
 * cast_vertical_ray - Casts a vertical ray
 *			and determines its intersection with walls.
 * @game: Pointer to the Game structure containing game state.
 * @ray_angle: Angle of the ray to cast.
 * @facing_down: Flag indicating whether the ray is facing downwards.
 * @facing_right: Flag indicating whether the ray is facing rightwards.
 * @vert_x: Pointer to store the x coordinate of the vertical ray intersection.
 * @vert_y: Pointer to store the y coordinate of the vertical ray intersection.
 * @vert_distance: Pointer to store the distance
 *		from the player to the vertical ray intersection.
 *
 * This function casts a vertical ray and calculates
 * where it intersects with the nearest wall.
 * It updates vert_x, vert_y, and vert_distance
 * with the intersection point and distance.
 */
void cast_vertical_ray(struct Game *game, float ray_angle, int facing_down,
		int facing_right, float *vert_x, float *vert_y,
		float *vert_distance)
{
	float x_intercept, y_intercept, x_step, y_step;
	float next_vert_x, next_vert_y, x_to_check, y_to_check;
	int map_x, map_y;

	x_intercept = floor(game->player.x / TILE_SIZE) * TILE_SIZE;
	x_intercept += facing_right ? TILE_SIZE : 0;
	y_intercept = (game->player.y + (x_intercept - game->player.x)
			* tan(ray_angle));
	x_step = TILE_SIZE;
	x_step *= facing_right ? 1 : -1;
	y_step = TILE_SIZE * tan(ray_angle);
	y_step *= (facing_down && y_step < 0) ? -1 : 1;
	y_step *= (!facing_down && y_step > 0) ? -1 : 1;

	next_vert_x = x_intercept;
	next_vert_y = y_intercept;

	while (next_vert_x >= 0 && next_vert_x < SCREEN_WIDTH &&
			next_vert_y >= 0 && next_vert_y < SCREEN_HEIGHT)
	{
		x_to_check = next_vert_x + (facing_right ? 0 : -1);
		y_to_check = next_vert_y;

		if (x_to_check < 0 || x_to_check >= MAP_WIDTH * TILE_SIZE ||
				y_to_check < 0 || y_to_check >= MAP_HEIGHT * TILE_SIZE)
			break;
		map_x = (int)(x_to_check / TILE_SIZE);
		map_y = (int)(y_to_check / TILE_SIZE);
		if (game->map[map_y * MAP_WIDTH + map_x] == MAP_WALL)
		{
			*vert_x = next_vert_x;
			*vert_y = next_vert_y;
			*vert_distance = sqrt(pow(*vert_x - game->player.x, 2) +
					pow(*vert_y - game->player.y, 2));
			return;
		}
		next_vert_x += x_step;
		next_vert_y += y_step;
	}
	*vert_distance = INFINITY;
}

/**
 * cast_single_ray - Casts a single ray
 *		and determines its intersection with walls.
 * @game: Pointer to the Game structure containing game state.
 * @ray_angle: Angle of the ray to cast.
 * @ray_index: Index of the ray in the array.
 * @ray_end_x: Pointer to store the x coordinate of the ray's endpoint.
 * @ray_end_y: Pointer to store the y coordinate of the ray's endpoint.
 *
 * This function casts a single ray and calculates
 * where it intersects with the nearest wall
 * in both horizontal and vertical directions.
 * It updates ray_end_x and ray_end_y with the
 * coordinates of the intersection point based on the shorter distance.
 */
void cast_single_ray(struct Game *game, float ray_angle, int ray_index,
		float *ray_end_x, float *ray_end_y)
{
	float horiz_x, horiz_y, vert_x, vert_y, distance;
	float horiz_distance = INFINITY, vert_distance = INFINITY;
	int is_vertical, facing_down, facing_right;

	ray_angle = fmodf(ray_angle, 2 * PI);
	if (ray_angle < 0)
		ray_angle += 2 * PI;

	facing_down = (ray_angle > 0 && ray_angle < PI);
	facing_right = (ray_angle < 0.5 * PI || ray_angle > 1.5 * PI);

	cast_horizontal_ray(game, ray_angle, facing_down, facing_right,
			&horiz_x, &horiz_y, &horiz_distance);
	cast_vertical_ray(game, ray_angle, facing_down, facing_right,
			&vert_x, &vert_y, &vert_distance);

	if (horiz_distance < vert_distance)
	{
		*ray_end_x = horiz_x;
		*ray_end_y = horiz_y;
		distance = horiz_distance;
		is_vertical = 0;
	}
	else
	{
		*ray_end_x = vert_x;
		*ray_end_y = vert_y;
		distance = vert_distance;
		is_vertical = 1;
	}

	draw_3d_view(game, ray_angle, distance, ray_index, is_vertical);
}
