#ifndef SAVE_H_
#define SAVE_H_
#include "types.h"
#include "stdlib.h"

#define SAVE_FILE 			"./.save.bin" // TODO change later

const char *get_save_path();

void ensure_save_folder();

void save_game(world_t *world, player_t *player, char *name);

void load_game(world_t *world, player_t *player, char *name);

void save_player(player_t *player, FILE *file);
void save_world(world_t *world, FILE *file);
void save_room(room_t *room, FILE *file);

void save_buff(buff_t *buff, world_t *world, FILE *file);

void load_buff(buff_t *buff, world_t *world, player_t *player, FILE *file);

void load_player(player_t *player, FILE *file, item_data_t *item_data);

void load_world(world_t *world, player_t *player, FILE *file);

void load_room_save(room_t *room, FILE *file, item_data_t *item_data);

#endif
