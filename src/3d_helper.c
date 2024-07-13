#include "../inc/rendering.h"

/**
 * calculate_corrected_distance - Calculate the corrected distance
 * @distance: Original distance
 * @ray_angle: Angle of the ray
 * @player_angle: Angle of the player
 *
 * Return: Corrected distance
 */
float calculate_corrected_distance(float distance, float ray_angle,
		float player_angle)
{
	return (distance * cos(ray_angle - player_angle));
}

/**
 * calculate_line_height - Calculate the height of the line
 * @corrected_distance: Corrected distance
 *
 * Return: Line height
 */
float calculate_line_height(float corrected_distance)
{
	return ((TILE_SIZE / corrected_distance) * PROJECTION_PLANE_DISTANCE);
}

/**
 * calculate_line_offset - Calculate the vertical offset of the line
 * @line_height: Height of the line
 *
 * Return: Line offset
 */
float calculate_line_offset(float line_height)
{
	return ((WINDOW_HEIGHT - line_height) / 2);
}

/**
 * setup_wall_strip - Set up the SDL_Rect for the wall strip
 * @ray_index: Index of the current ray
 * @line_offset: Vertical offset of the line
 * @line_height: Height of the line
 *
 * Return: SDL_Rect structure for the wall strip
 */
SDL_Rect setup_wall_strip(int ray_index, float line_offset, float line_height)
{
	int strip_width = WINDOW_WIDTH / NUM_RAYS;

	SDL_Rect wall_strip = {
		.x = ray_index * strip_width,
		.y = line_offset,
		.w = strip_width,
		.h = line_height
	};

	return (wall_strip);
}
