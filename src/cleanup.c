#include "../inc/game.h"

/**
 * game_cleanup - Clean up game resources and exit
 * @game: Pointer to the Game structure
 * @exit_status: Exit status code
 */
void game_cleanup(struct Game *game, int exit_status)
{
	SDL_DestroyTexture(game->pistol_texture);
	SDL_DestroyTexture(game->shotgun_texture);
	SDL_DestroyTexture(game->rifle_texture);
	SDL_DestroyTexture(game->wall_texture);
	SDL_DestroyTexture(game->sky_texture);
	SDL_DestroyTexture(game->ground_texture);
	SDL_DestroyRenderer(game->renderer);
	SDL_DestroyWindow(game->window);
	IMG_Quit();
	SDL_Quit();
	exit(exit_status);
}
