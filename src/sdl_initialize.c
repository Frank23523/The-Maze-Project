#include "../inc/game.h"

/**
 * initialize_sdl_and_img - Initializes SDL and SDL_image.
 *
 * Return: true if initialization is successful, otherwise false.
 */
bool initialize_sdl_and_img(void)
{
	int imgFlags = IMG_INIT_JPG;

	if (SDL_Init(SDL_INIT_EVERYTHING))
	{
		fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
		return (false);
	}

	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		fprintf(stderr, "SDL_image could not initialize! Error: %s\n",
				IMG_GetError());
		return (false);
	}

	return (true);
}

/**
 * create_window_and_renderer - Creates an SDL window and renderer.
 * @game: Pointer to the Game structure containing game state.
 *
 * Return: true if window and renderer creation is successful, else false.
 */
bool create_window_and_renderer(struct Game *game)
{
	game->window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	if (!game->window)
	{
		fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
		return (false);
	}

	game->renderer = SDL_CreateRenderer(game->window, -1, 0);
	if (!game->renderer)
	{
		fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
		return (false);
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	return (true);
}

/**
 * load_texture - Loads a texture from an image file.
 * @renderer: The SDL renderer to be used for creating the texture.
 * @path: Path to the image file.
 *
 * Return: The created SDL texture, or NULL if loading fails.
 */
SDL_Texture *load_texture(SDL_Renderer *renderer, const char *path)
{
	SDL_Surface *loaded_surface = IMG_Load(path);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);

	if (loaded_surface == NULL)
	{
		fprintf(stderr, "Unable to load image %s! Error: %s\n",
				path, IMG_GetError());
		return (NULL);
	}

	SDL_FreeSurface(loaded_surface);
	if (texture == NULL)
		fprintf(stderr, "Unable to create texture from %s! Error: %s\n",
				path, SDL_GetError());

	return (texture);
}

/**
 * sdl_initialize - Initializes SDL and game resources.
 * @game: Pointer to the Game structure containing game state.
 * @map_file_path: Path to the map file to be loaded.
 *
 * Return: true if an error occurs, otherwise false.
 */
bool sdl_initialize(struct Game *game, const char *map_file_path)
{
	if (!initialize_sdl_and_img() || !create_window_and_renderer(game))
		return (true);

	game->wall_texture = load_texture(game->renderer,
			"./textures/grasstexture1.jpg");
	game->ground_texture = load_texture(game->renderer,
			"./textures/groundtexture4.jpg");
	game->pistol_texture = load_texture(game->renderer,
			"./textures/pistol.png");
	game->shotgun_texture = load_texture(game->renderer,
			"./textures/shotgun.png");
	game->rifle_texture = load_texture(game->renderer,
			"./textures/rifle.png");

	if (!game->wall_texture || !game->ground_texture || !game->pistol_texture ||
			!game->shotgun_texture || !game->rifle_texture)
		return (true);

	game->current_weapon = 0;

	game->player.x = TILE_SIZE + TILE_SIZE / 2;
	game->player.y = TILE_SIZE + TILE_SIZE / 2;
	game->player.angle = 0;
	game->player.dirX = cos(game->player.angle);
	game->player.dirY = sin(game->player.angle);

	if (!parse_map_file(game, map_file_path))
		return (true);

	game->rotation_speed = 0.05f;
	game->show_map = false;

	return (false);
}
