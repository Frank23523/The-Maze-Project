#include "../inc/game.h"

static void handle_event(struct Game *game, SDL_Event *event);
static void update_game_state(struct Game *game);
static void render_game(struct Game *game, float ray_ends_x[],
		float ray_ends_y[]);

/**
 * main - Entry point for the game.
 * @argc: argument count
 * @argv: argument array
 *
 * Return: Exit status.
 */
int main(int argc, char *argv[])
{
	struct Game game = {0};
	float ray_ends_x[NUM_RAYS];
	float ray_ends_y[NUM_RAYS];
	SDL_Event event;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <map_file_path>\n", argv[0]);
		return (EXIT_FAILURE);
	}
	game.previous_time = SDL_GetTicks();
	putenv("SDL_AUDIODRIVER=alsa");

	if (sdl_initialize(&game, argv[1]))
		game_cleanup(&game, EXIT_FAILURE);

	game.keyboard_state = SDL_GetKeyboardState(NULL);

	while (true)
	{
		update_game_state(&game);

		while (SDL_PollEvent(&event))
			handle_event(&game, &event);

		handle_input(&game);
		render_game(&game, ray_ends_x, ray_ends_y);
		SDL_Delay(16);
	}
	game_cleanup(&game, EXIT_SUCCESS);
	return (0);
}


/**
 * handle_event - Handles SDL events.
 * @game: Pointer to the Game structure.
 * @event: Pointer to the SDL_Event to handle.
 */
static void handle_event(struct Game *game, SDL_Event *event)
{
	if (event->type == SDL_QUIT)
		game_cleanup(game, EXIT_SUCCESS);
	else if (event->type == SDL_KEYDOWN)
	{
		switch (event->key.keysym.scancode)
		{
			case SDL_SCANCODE_ESCAPE:
				game_cleanup(game, EXIT_SUCCESS);
				break;
			case SDL_SCANCODE_M:
				game->show_map = !game->show_map;
				break;
			case SDL_SCANCODE_P:
				game->current_weapon = (game->current_weapon + 1) % 3;
				break;
			default:
				break;
		}
	}
}

/**
 * update_game_state - Updates the game state based on delta time.
 * @game: Pointer to the Game structure.
 */
static void update_game_state(struct Game *game)
{
	game->current_time = SDL_GetTicks();
	game->delta_time = (game->current_time - game->previous_time) / 1000.0f;
	game->previous_time = game->current_time;
}

/**
 * render_game - Renders the game.
 * @game: Pointer to the Game structure.
 * @ray_ends_x: Array of x coordinates where rays end.
 * @ray_ends_y: Array of y coordinates where rays end.
 */
static void render_game(struct Game *game, float ray_ends_x[],
		float ray_ends_y[])
{
	SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
	SDL_RenderClear(game->renderer);

	cast_rays(game, ray_ends_x, ray_ends_y);

	SDL_RenderSetViewport(game->renderer, NULL);

	if (game->show_map)
		draw_2d_map(game);

	draw_weapon(game);
	SDL_RenderPresent(game->renderer);
}
