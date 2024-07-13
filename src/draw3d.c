#include "../inc/rendering.h"

/**
 * calculate_texture_rect - Calculate the SDL_Rect for the texture
 * @texture_width: Width of the texture
 * @texture_height: Height of the texture
 * @hit_x: Horizontal hit position
 * @texture_y_start: Starting y position of the texture
 * @line_height: Height of the line
 * @step: Step value for texture
 *
 * Return: SDL_Rect structure for the texture
 */
SDL_Rect calculate_texture_rect(int texture_width,
		float hit_x, float texture_y_start, float line_height, float step)
{
	int texture_x = (int)(hit_x / TILE_SIZE * texture_width);

	SDL_Rect texture_rect = {
		.x = texture_x,
		.y = (int)texture_y_start,
		.w = 1,
		.h = (int)(line_height * step)
	};

	return (texture_rect);
}

/**
 * apply_shading - Apply shading to the texture based on distance
 * @texture: Pointer to the SDL_Texture
 * @corrected_distance: Corrected distance for shading calculation
 */
void apply_shading(SDL_Texture *texture, float corrected_distance)
{
	float shade_factor = 1.0 - (corrected_distance / (MAP_WIDTH * TILE_SIZE));

	shade_factor = fmaxf(0.3f, shade_factor);
	if (shade_factor > 1)
		shade_factor = 1;

	SDL_SetTextureColorMod(texture, (Uint8)(255 * shade_factor),
			(Uint8)(255 * shade_factor), (Uint8)(255 * shade_factor));
}

/**
 * draw_3d_view - Render the 3D view for the game
 * @game: Pointer to the Game structure
 * @ray_angle: Angle of the current ray
 * @distance: Distance from the player to the wall
 * @ray_index: Index of the current ray
 * @is_vertical: Flag indicating if the hit was vertical
 */
void draw_3d_view(struct Game *game, float ray_angle, float distance,
		int ray_index, int is_vertical)
{
	float step, texture_y_start, hit_x;
	float corrected_distance = calculate_corrected_distance(distance, ray_angle,
			game->player.angle);
	float line_height = calculate_line_height(corrected_distance);
	float line_offset = calculate_line_offset(line_height);
	int texture_width, texture_height;
	SDL_Rect wall_strip, texture_rect;
	SDL_Rect ground_rect = {
		.x = ray_index * (WINDOW_WIDTH / NUM_RAYS),
		.y = WINDOW_HEIGHT / 2,
		.w = WINDOW_WIDTH / NUM_RAYS,
		.h = WINDOW_HEIGHT / 2
	};

	SDL_QueryTexture(game->wall_texture, NULL, NULL,
			&texture_width, &texture_height);
	step = (float)texture_height / line_height;
	texture_y_start = 0;
	if (line_offset < 0)
	{
		texture_y_start = -line_offset * step;
		line_offset = 0;
	}
	if (line_height > WINDOW_HEIGHT)
		line_height = WINDOW_HEIGHT;

	wall_strip = setup_wall_strip(ray_index, line_offset, line_height);
	hit_x = is_vertical ? fmod(game->player.y + distance * sin(ray_angle),
			TILE_SIZE)
		: fmod(game->player.x + distance * cos(ray_angle), TILE_SIZE);
	texture_rect = calculate_texture_rect(texture_width,
			hit_x, texture_y_start, line_height, step);

	SDL_RenderCopy(game->renderer, game->ground_texture, NULL, &ground_rect);
	apply_shading(game->wall_texture, corrected_distance);
	SDL_RenderCopy(game->renderer, game->wall_texture,
			&texture_rect, &wall_strip);
}
