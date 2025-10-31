#ifndef ENEMY_H_

#define ENEMY_H_

#include <stdbool.h>
#include "types.h"

//TODO need move functions and check functions so enemies don't leave the map
// shouldn't happen but just in case

enum trait enemy_get_trait(const char *name);

enemy_t *enemy_spawn(enemy_type_t type, const enemy_data_t *enemy_data, int x, int y, int global_x, int global_y, biome_t biome);

const char *enemy_get_name(enemy_type_t type);

enemy_type_t enemy_get_type(const char *name);

enemy_type_t enemy_generate_type(unsigned int *seed, enemy_data_t *enemy_data, biome_t biome);

void load_enemy_data(enemy_data_t *enemy_data);

void load_enemy_drop_data(enemy_data_t *enemy_data);

void load_biome_data(enemy_data_t *enemy_data);

biome_t get_biome(const char *name);

void enemy_kill(enemy_t *enemy, world_t *world);

void enemy_handle_death_drops(enemy_t *enemy, enemy_data_t *enemy_data, item_data_t *item_data, tile_t *tile);

bool enemy_damage(enemy_t *enemy, world_t *world, int amount);
bool enemy_damage_ignore_armor(enemy_t *enemy, world_t *world, int amount);

void enemy_attack(enemy_t *enemy, player_t *player, world_t *world);

void enemy_decide_move(enemy_t *enemy, world_t *world, player_t *player);

void enemy_decide_move_passive(enemy_t *enemy, world_t *world, player_t *player);

void enemy_decide_move_aggressive(enemy_t *enemy, world_t *world, player_t *player);

void enemy_decide_move_dark_centered(enemy_t *enemy, world_t *world, player_t *player);

void enemy_decide_move_light_centered(enemy_t *enemy, world_t *world, player_t *player);

void enemy_decide_move_survival(enemy_t *enemy, world_t *world, player_t *player);

void enemy_move_toward_location(enemy_t *enemy, world_t *world, player_t *player, int y, int x, bool exact);

int enemy_attempt_attack(enemy_t *enemy, world_t *world, player_t *player);

char enemy_check_dir(enemy_t *enemy, world_t *world, player_t *player, direction_t dir);

int enemy_can_move_dir(enemy_t *enemy, world_t *world, player_t *player, direction_t dir);

enemy_t *enemy_create_temp(world_t *world);

int enemy_calc_max_health(const enemy_t *enemy);

void find_suitable_tile_away_from_pos(const enemy_t *enemy, const room_t *room, const player_t *player, int range, int start_y, int start_x, int *end_y, int *end_x);

int find_spot_near(const enemy_t *enemy, const world_t *world, const player_t *player, int *y, int *x);

bool enemy_can_walk(char symbol);

void enemy_create_path_lists(enemy_t *enemy);

void enemy_append_to_olist(enemy_t *enemy, path_node_t node);

void enemy_remove_from_olist(enemy_t *enemy, int index);

path_node_t enemy_pop_from_olist(enemy_t *enemy);

path_node_t *enemy_find_next_node(enemy_t *enemy);

int get_path_node_neighbors(path_node_t node, path_node_t neighbors[4], enemy_t *enemy, room_t *room, player_t *player);

void enemy_find_path_to_target(enemy_t *enemy, room_t *room, player_t *player, int endy, int endx);

void enemy_free_path_lists(enemy_t *enemy);
#endif
