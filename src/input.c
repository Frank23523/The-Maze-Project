#include "../inc/input.h"

/**
 * handle_input - Handles player input for movement and rotation.
 * @game: Pointer to the Game structure.
 */
void handle_input(struct Game *game)
{
	float movement_speed = PLAYER_SPEED * game->delta_time;
	float rotation_speed = ROTATION_SPEED * game->delta_time;

	handle_rotation(game, rotation_speed);
	handle_movement(game, movement_speed);
	update_direction(&game->player);
	keep_player_in_bounds(&game->player);
}

/**
 * handle_rotation - Handles rotation of the player based on input.
 * @game: Pointer to the Game structure.
 * @rotation_speed: Speed of rotation.
 */
void handle_rotation(struct Game *game, float rotation_speed)
{
	if (game->keyboard_state[SDL_SCANCODE_LEFT])
	{
		game->player.angle -= rotation_speed;
		if (game->player.angle < 0)
			game->player.angle += 2 * PI;
	}
	if (game->keyboard_state[SDL_SCANCODE_RIGHT])
	{
		game->player.angle += rotation_speed;
		if (game->player.angle > 2 * PI)
			game->player.angle -= 2 * PI;
	}
}

/**
 * handle_movement - Handles movement of the player based on input.
 * @game: Pointer to the Game structure.
 * @movement_speed: Speed of movement.
 */
void handle_movement(struct Game *game, float movement_speed)
{
	float newX = game->player.x;
	float newY = game->player.y;

	if (game->keyboard_state[SDL_SCANCODE_W])
	{
		newX += game->player.dirX * movement_speed;
		newY += game->player.dirY * movement_speed;
	}
	if (game->keyboard_state[SDL_SCANCODE_S])
	{
		newX -= game->player.dirX * movement_speed;
		newY -= game->player.dirY * movement_speed;
	}
	if (game->keyboard_state[SDL_SCANCODE_D])
	{
		newX -= game->player.dirY * movement_speed;
		newY += game->player.dirX * movement_speed;
	}
	if (game->keyboard_state[SDL_SCANCODE_A])
	{
		newX += game->player.dirY * movement_speed;
		newY -= game->player.dirX * movement_speed;
	}

	if (check_collision(game, newX, newY))
	{
		game->player.x = newX;
		game->player.y = newY;
	}
}
