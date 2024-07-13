#include "../inc/rendering.h"

/**
 * draw_weapon - Draws the current weapon on the screen.
 * @game: Pointer to the Game structure.
 */
void draw_weapon(struct Game *game)
{
	int texture_width, texture_height;
	int weapon_width, weapon_height, weapon_x, weapon_y;
	float scale_factor = 0.3;
	SDL_Texture *weapon_texture;
	SDL_Rect *weapon_rect = &(SDL_Rect){0};

	switch (game->current_weapon)
	{
		case 0:
			weapon_texture = game->pistol_texture;
			break;
		case 1:
			weapon_texture = game->shotgun_texture;
			break;
		case 2:
			weapon_texture = game->rifle_texture;
			break;
		default:
			return;
	}

	SDL_QueryTexture(weapon_texture, NULL, NULL, &texture_width, &texture_height);
	weapon_width = (int)(WINDOW_WIDTH * scale_factor);
	weapon_height = weapon_width * texture_height / texture_width;
	weapon_x = (WINDOW_WIDTH - weapon_width) / 2;
	weapon_y = WINDOW_HEIGHT - weapon_height;

	*weapon_rect = (SDL_Rect){weapon_x, weapon_y, weapon_width, weapon_height};

	SDL_RenderCopy(game->renderer, weapon_texture, NULL, weapon_rect);
}
