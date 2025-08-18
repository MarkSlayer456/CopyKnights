#ifndef ITEMS_H_
#define ITEMS_H_
#include "types.h"

item_ids_t item_get_id(const char *name);

armor_type_t armor_get_type(const char *name);

stats_t get_stat(const char *name);

rarity_t get_rarity(const char *name);

// returns 1 on success and 0 on fail
int use_item(player_t *player);

int use_teleport_scroll(player_t *player);
int use_health_potion(player_t *player);
int use_apple(player_t *player);
int use_chicken_dinner(player_t *player);

void remove_item(player_t *player);

void load_armor_spawn_data(world_t *world);
void load_armor_effects(world_t *world);
void load_armor_data(world_t *world);

#endif
