#ifndef ITEMS_H_
#define ITEMS_H_
#include "types.h"

item_ids_t item_get_id(const char *name);

stats_t get_stat(const char *name);

rarity_t get_rarity(const char *name);

// returns 1 on success and 0 on fail
int use_item(player_t *player);

int use_teleport_scroll(player_t *player);
int use_health_potion(player_t *player);
int use_apple(player_t *player);
int use_chicken_dinner(player_t *player);

void remove_item(player_t *player);

void item_spawn(item_ids_t id, biome_t biome, tile_t *tile, item_data_t *item_data);
item_ids_t item_generate_type(unsigned int *seed, item_data_t *item_data, biome_t biome);

#endif
