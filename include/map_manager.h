#ifndef MAP_MANAGER_H_
#define MAP_MANAGER_H_

#include "types.h"
#include <stdbool.h>

void calculate_main_path(unsigned int *seed, world_t *world);

void calculate_door_masks(world_t *world);

void sync_doors(world_t *world, int x, int y);

room_t *generate_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data, item_data_t *item_data, world_t *world);

room_t *load_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data, item_data_t *item_data, world_t *world);

room_t *setup_first_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data, item_data_t *item_data, world_t *world);
#endif
