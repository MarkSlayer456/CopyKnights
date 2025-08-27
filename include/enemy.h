#ifndef ENEMY_H_

#define ENEMY_H_

#include <stdbool.h>
#include "types.h"

//TODO need move functions and check functions so enemies don't leave the map
// shouldn't happen but just in case

enemy_t *enemy_spawn(enemy_type_t type, const enemy_data_t *enemy_data, int x, int y, biome_t biome);

const char *enemy_get_name(enemy_type_t type);

enemy_type_t enemy_get_type(const char *name);

enemy_type_t enemy_generate_type(unsigned int *seed, enemy_data_t *enemy_data, biome_t biome);

void load_enemy_data(enemy_data_t *enemy_data);

void load_biome_data(enemy_data_t *enemy_data);

biome_t get_biome(const char *name);

void enemy_kill(enemy_t *enemy, world_t *world, const player_t *player);

bool enemy_decrease_health(enemy_t *enemy, world_t *world, const player_t *player, int amount);

void enemy_attack(enemy_t *enemy, player_t *player, world_t *world);

void enemy_decide_move(enemy_t *enemy, world_t *world, player_t *player);

char enemy_check_dir(enemy_t *enemy, world_t *world, player_t *player, direction_t dir);

int enemy_can_move_dir(enemy_t *enemy, world_t *world, player_t *player, direction_t dir);

enemy_t *enemy_create_temp(world_t *world);

#endif
