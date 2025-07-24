#include <stdlib.h>
#include <string.h>
#include "items.h"
#include "player.h"

// takes an item id and returns it's name
char *get_item_name(item_ids_t item)
{
	char *name = calloc(32, sizeof(char));
	switch(item) {
		case BLANK:
			strncpy(name, BLANK_NAME, strlen(BLANK_NAME)+1);
			break;
		case TELEPORT_SCROLL:
			strncpy(name, TELEPORT_SCROLL_NAME, strlen(TELEPORT_SCROLL_NAME)+1);
			break;
		case HEALTH_POTION:
			strncpy(name, HEALTH_POTION_NAME, strlen(HEALTH_POTION_NAME)+1);
			break;
		case APPLE:
			strncpy(name, APPLE_NAME, strlen(APPLE_NAME)+1);
			break;
		case CHICKEN_DINNER:
			strncpy(name, CHICKEN_DINNER_NAME, strlen(CHICKEN_DINNER_NAME)+1);
			break;
		default:
			strncpy(name, BLANK_NAME, strlen(BLANK_NAME)+1);
			break;
	}
	return name;
}


// returns 1 on success and 0 on fail
int use_item(player_t *player)
{
	int success = 0;
	if(player->inventory[player->action_bar.inv_selector].stack > 0) {
		switch(player->inventory[player->action_bar.inv_selector].id) {
			case BLANK:
				success = 0;
			case TELEPORT_SCROLL:
				use_teleport_scroll(player);
				success = 1;
			case HEALTH_POTION:
				use_health_potion(player);
				success = 1;
			case APPLE:
				use_apple(player);
				success = 1;
			case CHICKEN_DINNER:
				use_chicken_dinner(player);
				success = 1;
			default:
				success = 0;
		}
		remove_item(player);
	}
	return success;
}


int use_teleport_scroll(player_t *player)
{
	player->x[0] = TELEPORT_SCROLL_TELEPORT_LOCATION.x;
	player->y[0] = TELEPORT_SCROLL_TELEPORT_LOCATION.x;
	player->global_x = TELEPORT_SCROLL_TELEPORT_LOCATION.global_x;
	player->global_y = TELEPORT_SCROLL_TELEPORT_LOCATION.global_y;
	return 0;
}

int use_health_potion(player_t *player)
{
	player_increase_health(player, HEALTH_POTION_HEAL_AMOUNT);
	return 1;
}

int use_apple(player_t *player)
{
	player_increase_health(player, APPLE_HEAL_AMOUNT);
	return 1;
}

int use_chicken_dinner(player_t *player)
{
	player_increase_health(player, CHICKEN_DINNER_HEAL_AMOUNT);
	return 1;
}

void remove_item(player_t *player)
{
	player->inventory[player->action_bar.inv_selector].stack -= 1;
	if(player->inventory[player->action_bar.inv_selector].stack <= 0) {
		player_organize_inv(player, player->action_bar.inv_selector);
	}
}



