#include "../inc/map.h"

/**
 * draw_2d_map - Draws the 2D map including background,
 *		tiles, player, and FOV rays.
 * @game: Pointer to the Game structure.
 */
void draw_2d_map(struct Game *game)
{
	int player_size, player_x, player_y;
	int map_size = WINDOW_HEIGHT / 4;
	int tile_size = map_size / MAP_HEIGHT;
	int map_x = WINDOW_WIDTH - map_size;
	int map_y = WINDOW_HEIGHT - map_size;

	draw_map_background(game, map_x, map_y, map_size);
	draw_map_tiles(game, map_x, map_y, tile_size);

	player_size = tile_size / 4;
	player_x = (map_x + (game->player.x * tile_size / TILE_SIZE)
		- player_size / 2);
	player_y = (map_y + (game->player.y * tile_size / TILE_SIZE)
		- player_size / 2);

	draw_player(game, player_x, player_y, player_size);
	draw_fov_rays(game, map_x, map_y, tile_size, player_x, player_y, player_size);
}

/**
 * draw_map_background - Draws the background of the map.
 * @game: Pointer to the Game structure.
 * @map_x: X-coordinate of the map's top-left corner.
 * @map_y: Y-coordinate of the map's top-left corner.
 * @map_size: Size of the map (square dimension).
 */
void draw_map_background(struct Game *game, int map_x, int map_y, int map_size)
{
	SDL_Rect map_rect = {map_x, map_y, map_size, map_size};

	SDL_SetRenderDrawColor(game->renderer, 100, 100, 100, 128);
	SDL_RenderFillRect(game->renderer, &map_rect);
}

/**
 * draw_map_tiles - Draws the tiles of the map based on the game's map data.
 * @game: Pointer to the Game structure.
 * @map_x: X-coordinate of the map's top-left corner.
 * @map_y: Y-coordinate of the map's top-left corner.
 * @tile_size: Size of each tile in pixels.
 */
void draw_map_tiles(struct Game *game, int map_x, int map_y, int tile_size)
{
	int y, x;
	SDL_Rect *wall_rect = &(SDL_Rect){0};

	for (y = 0; y < MAP_HEIGHT; y++)
	{
		for (x = 0; x < MAP_WIDTH; x++)
		{
			if (game->map[y * MAP_WIDTH + x] == MAP_WALL)
			{
				*wall_rect = (SDL_Rect){
					map_x + x * tile_size,
					map_y + y * tile_size,
					tile_size,
					tile_size
				};
				SDL_SetRenderDrawColor(game->renderer, 200, 200, 200, 255);
				SDL_RenderFillRect(game->renderer, wall_rect);
			}
		}
	}
}

/**
 * draw_player - Draws the player on the 2D map.
 * @game: Pointer to the Game structure.
 * @player_x: X-coordinate of the player's center.
 * @player_y: Y-coordinate of the player's center.
 * @player_size: Size of the player in pixels.
 */
void draw_player(struct Game *game, int player_x,
		int player_y, int player_size)
{
	SDL_Rect player_rect = {player_x, player_y, player_size, player_size};

	SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(game->renderer, &player_rect);
}

/**
 * draw_fov_rays - Draws the FOV (Field of View) rays
 *		from the player's perspective.
 * @game: Pointer to the Game structure.
 * @map_x: X-coordinate of the map's top-left corner.
 * @map_y: Y-coordinate of the map's top-left corner.
 * @tile_size: Size of each tile in pixels.
 * @player_x: X-coordinate of the player's center.
 * @player_y: Y-coordinate of the player's center.
 * @player_size: Size of the player in pixels.
 */
void draw_fov_rays(struct Game *game, int map_x, int map_y, int tile_size,
		int player_x, int player_y, int player_size)
{
	float ray_angle, end_x, end_y, ray_dx, ray_dy, ray_distance, step_x;
	float step_y, ray_x, ray_y;
	float ray_length = tile_size * 3;
	float angle_step = DEGREE_TO_RADIAN(FOV_DEGREES) / 19;
	int i, map_x_check, map_y_check, ray_end_x, ray_end_y;

	SDL_SetRenderDrawColor(game->renderer, 0, 255, 0, 255);
	for (i = -11; i <= 11; i++)
	{
		if (i == 0)
			continue;
		ray_angle = game->player.angle + i * angle_step;
		end_x = game->player.x + cosf(ray_angle) * ray_length;
		end_y = game->player.y + sinf(ray_angle) * ray_length;
		ray_dx = end_x - game->player.x;
		ray_dy = end_y - game->player.y;
		ray_distance = sqrt(ray_dx * ray_dx + ray_dy * ray_dy);
		step_x = ray_dx / ray_distance;
		step_y = ray_dy / ray_distance;
		ray_x = game->player.x;
		ray_y = game->player.y;

		while (ray_x >= 0 && ray_x < MAP_WIDTH * TILE_SIZE &&
				ray_y >= 0 && ray_y < MAP_HEIGHT * TILE_SIZE)
		{
			map_x_check = (int)(ray_x / TILE_SIZE);
			map_y_check = (int)(ray_y / TILE_SIZE);
			if (game->map[map_y_check * MAP_WIDTH + map_x_check] == MAP_WALL)
				break;
			ray_x += step_x;
			ray_y += step_y;
		}
		ray_end_x = map_x + (ray_x * tile_size / TILE_SIZE);
		ray_end_y = map_y + (ray_y * tile_size / TILE_SIZE);
		SDL_RenderDrawLine(game->renderer,
				player_x + player_size / 2,
				player_y + player_size / 2,
				ray_end_x,
				ray_end_y);
	}
}
