#ifndef ITEMS_H_
#define ITEMS_H_
#include "game_manager.h"

#define INV_SIZE		128

#define BLANK_NAME					"Blank"
#define TELEPORT_SCROLL_NAME		"Teleport Scroll"
#define TELEPORT_SCROLL_TELEPORT_LOCATION  ((location_t){ .x = 1, .y = 1, .global_x = 0, .global_y = 0 })
#define HEALTH_POTION_NAME			"Health Potion"
#define HEALTH_POTION_HEAL_AMOUNT	100
#define	APPLE_NAME					"Apple"
#define APPLE_HEAL_AMOUNT			5
#define CHICKEN_DINNER_NAME			"Chicken Dinner"
#define CHICKEN_DINNER_HEAL_AMOUNT	500

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
