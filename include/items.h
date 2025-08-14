#ifndef ITEMS_H_
#define ITEMS_H_
#include "types.h"

item_ids_t item_get_id(const char *name);

// returns 1 on success and 0 on fail
int use_item(player_t *player);

int use_teleport_scroll(player_t *player);
int use_health_potion(player_t *player);
int use_apple(player_t *player);
int use_chicken_dinner(player_t *player);

void remove_item(player_t *player);

void load_armor_data(world_t *world);

#endif
