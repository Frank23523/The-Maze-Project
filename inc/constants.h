#ifndef CONSTANTS_H
#define CONSTANTS_H


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WINDOW_TITLE "The Maze Project"
#define GREY_COLOR 128
#define PLAYER_SIZE 10
#define YELLOW_COLOR (255, 255, 0)
#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define MAP_WALL 1
#define MAP_FLOOR 0
#define TILE_SIZE 128
#define SCREEN_WIDTH (MAP_WIDTH * TILE_SIZE)
#define SCREEN_HEIGHT (MAP_HEIGHT * TILE_SIZE)
#define DEGREE_TO_RADIAN(degree) ((degree) * (M_PI / 180.0))
#define FOV_HALF_RADIAN (DEGREE_TO_RADIAN(FOV_DEGREES / 2))
#define HALF_WINDOW_WIDTH (WINDOW_WIDTH / 2)
#define PROJECTION_PLANE_DISTANCE (HALF_WINDOW_WIDTH / tan(FOV_HALF_RADIAN))
#define PI 3.14159265
#define FOV_DEGREES 60
#define NUM_RAYS WINDOW_WIDTH
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 500
#define MAX_LINE_HEIGHT 500
#define MAP_WALL_CHAR '1'
#define MAP_FLOOR_CHAR '0'
#define PLAYER_SPEED 200.0f
#define ROTATION_SPEED 2.0f

#endif
