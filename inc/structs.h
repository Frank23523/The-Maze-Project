#ifndef STRUCTS_H
#define STRUCTS_H

#include "constants.h"

/**
 * struct Player - Player structure
 * @x: x-coordinate of the player
 * @y: y-coordinate of the player
 * @dirX: x direction of the player
 * @dirY: y direction of the player
 * @angle: angle of the player
 *
 * Description: This structure holds the position, direction,
 * and angle of the player in the game.
 */
struct Player
{
	float x;
	float y;
	float dirX;
	float dirY;
	float angle;
};

/**
 * struct Game - Game structure
 * @window: pointer to SDL_Window
 * @renderer: pointer to SDL_Renderer
 * @player: Player structure containing player information
 * @map: integer array representing the game map
 * @rotation_speed: float representing the rotation speed of the player
 * @show_map: boolean to show/hide the map
 * @wall_texture: pointer to SDL_Texture for the wall
 * @sky_texture: pointer to SDL_Texture for the sky
 * @ground_texture: pointer to SDL_Texture for the ground
 * @pistol_texture: pointer to SDL_Texture for the pistol
 * @shotgun_texture: pointer to SDL_Texture for the shotgun
 * @rifle_texture: pointer to SDL_Texture for the rifle
 * @current_weapon: integer representing the current weapon
 * @keyboard_state: pointer to the current state of the keyboard
 * @previous_time: Uint32 representing the previous time
 * @current_time: Uint32 representing the current time
 * @delta_time: float representing the change in time
 *
 * Description: This structure holds all the necessary information
 * for the game, including the window, renderer, player, map, textures,
 * and timing information.
 */
struct Game
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	struct Player player;
	int map[MAP_WIDTH * MAP_HEIGHT];
	float rotation_speed;
	bool show_map;
	SDL_Texture *wall_texture;
	SDL_Texture *sky_texture;
	SDL_Texture *ground_texture;
	SDL_Texture *pistol_texture;
	SDL_Texture *shotgun_texture;
	SDL_Texture *rifle_texture;
	int current_weapon;
	const Uint8 *keyboard_state;
	Uint32 previous_time;
	Uint32 current_time;
	float delta_time;
};

#endif /* STRUCTS_H */
