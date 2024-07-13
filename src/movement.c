#include "../inc/input.h"

/**
 * update_direction - Updates the direction vectors
 *		of the player based on their angle.
 * @player: Pointer to the Player structure.
 */
void update_direction(struct Player *player)
{
	player->dirX = cosf(player->angle);
	player->dirY = sinf(player->angle);
}

/**
 * check_collision - Checks for collision
 *		between the player and walls on the map.
 * @game: Pointer to the Game structure.
 * @newX: Potential new X position of the player.
 * @newY: Potential new Y position of the player.
 *
 * Return: true if no collision, false if collision detected.
 */
bool check_collision(const struct Game *game, float newX, float newY)
{
	float buffer = PLAYER_SIZE / 2.0f;
	int map_x_forward = (int)((newX + buffer * game->player.dirX) / TILE_SIZE);
	int map_y_forward = (int)((newY + buffer * game->player.dirY) / TILE_SIZE);
	int map_x_backward = (int)((newX - buffer * game->player.dirX) / TILE_SIZE);
	int map_y_backward = (int)((newY - buffer * game->player.dirY) / TILE_SIZE);

	return (game->map[map_y_forward * MAP_WIDTH + map_x_forward] == MAP_FLOOR &&
		game->map[map_y_backward * MAP_WIDTH + map_x_backward] == MAP_FLOOR);
}

/**
 * keep_player_in_bounds - Keeps the player within the bounds of the game map.
 * @player: Pointer to the Player structure.
 */
void keep_player_in_bounds(struct Player *player)
{
	player->x = fmaxf(0, fminf(player->x, MAP_WIDTH * TILE_SIZE));
	player->y = fmaxf(0, fminf(player->y, MAP_HEIGHT * TILE_SIZE));
}
