#ifndef ITEMS_H_
#define ITEMS_H_
#include "types.h"

// takes a item id and returns it's name
char *get_item_name(item_ids_t item);

// returns 1 on success and 0 on fail
int use_item(player_t *player);

int use_teleport_scroll(player_t *player);
int use_health_potion(player_t *player);
int use_apple(player_t *player);
int use_chicken_dinner(player_t *player);

void remove_item(player_t *player);

#endif
