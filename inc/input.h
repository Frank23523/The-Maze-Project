#ifndef INPUT_H
#define INPUT_H

#include "structs.h"
#include "game.h"

void handle_input(struct Game *game);
void handle_rotation(struct Game *game, float rotation_speed);
void handle_movement(struct Game *game, float movement_speed);
void update_direction(struct Player *player);
bool check_collision(const struct Game *game, float newX, float newY);
void keep_player_in_bounds(struct Player *player);

#endif
