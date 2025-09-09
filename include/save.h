#ifndef SAVE_H_
#define SAVE_H_
#include "types.h"
#include "stdlib.h"

#define SAVE_FILE 			"./.save.bin" // TODO change later

const char *get_save_path();

void ensure_save_folder();

void save_game(world_t *world, player_t *player, char *name);

void save_player(player_t *player, char *name);
void save_world(world_t *world, char *name);
void save_enemies(enemy_t *enemy, char *name);
void save_rooms(room_t *room, char *name);

void load_player(player_t *player, char *name);

#endif
