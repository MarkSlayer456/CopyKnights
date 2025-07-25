#include "player.h"
#include "game_manager.h"
#include "functions.h"
#include "enemy.h"
#include "hud.h"
#include "items.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MIN(a,b)       (a < b) ? a : b

extern char walk_chars[WALK_CHAR_LENGTH];

char player_get_current_pos(player_t *player, world_t *world)
{
	for(int i = 0; i < MAX_KNIGHTS; i++) {
		return world->room[player->global_x][player->global_y].layout[player->y[i]][player->x[i]];
	}
}

void player_move_left(player_t *player, world_t *world)
{
	for(int i = 0; i < MAX_KNIGHTS; i++) {
		if(player_can_move_left(player, world, i)) {
			player->x[i] -= 1;
			if(player_get_current_pos(player, world) == DOOR) {
				if(player->global_x - 1 < 0) {
					player->x[i] += 1;
					// TODO display message to player
					return;
				}
				player->global_x--;
				if(!world->room[player->global_x][player->global_y].is_created) {
					world->room[player->global_x][player->global_y] = generate_room(&world->seed, player->global_x, player->global_y, world->enemy_data);
				}
				player->x[0] = ROOM_WIDTH-1;
			}
		}
	}
	world->isPlayerTurn = 0;
}

void player_move_right(player_t *player, world_t *world)
{
	for(int i = 0; i < MAX_KNIGHTS; i++) {
		if(player_can_move_right(player, world, i)) {
			player->x[i] += 1;
			if(player_get_current_pos(player, world) == DOOR) {
				player->global_x++;
				if(!world->room[player->global_x][player->global_y].is_created) {
					world->room[player->global_x][player->global_y] = generate_room(&world->seed, player->global_x, player->global_y, world->enemy_data);
				}
				player->x[0] = 0;
			}
		}
	}	
	world->isPlayerTurn = 0;
}

void player_move_down(player_t *player, world_t *world)
{
	for(int i = 0; i < MAX_KNIGHTS; i++) {
		if(player_can_move_down(player, world, i)) {
			player->y[i] += 1;
			if(player_get_current_pos(player, world) == DOOR) {
				player->global_y++;
				if(!world->room[player->global_x][player->global_y].is_created) {
					world->room[player->global_x][player->global_y] = generate_room(&world->seed, player->global_x, player->global_y, world->enemy_data);
				}
				player->y[0] = 0;
			}
		}
	}	
	world->isPlayerTurn = 0;
}

void player_move_up(player_t *player, world_t *world)
{
	for(int i = 0; i < MAX_KNIGHTS; i++) {
		if(player_can_move_up(player, world, i)) {
			player->y[i] -= 1;
			if(player_get_current_pos(player, world) == DOOR) {
				if(player->global_y - 1 < 0) return;
				player->global_y--;
				if(!world->room[player->global_x][player->global_y].is_created) {
					world->room[player->global_x][player->global_y] = generate_room(&world->seed, player->global_x, player->global_y, world->enemy_data);
				}
				player->y[0] = ROOM_HEIGHT-1;
			}
		}
	}
	world->isPlayerTurn = 0;
}

char player_check_left(player_t *player, world_t *world, int knightNum)
{
	room_t *room = &world->room[player->global_x][player->global_y];
    for(int i = 0; i < room->current_enemy_count; i++) {
		if(player->x[knightNum] - 1 == room->enemies[i]->x && player->y[knightNum] == room->enemies[i]->y) {
            return ENEMY_CHAR;
        }  
    }
	if(player->x[knightNum] - 1 >= 0) {
		return room->layout[player->y[knightNum]][player->x[knightNum]-1];
	} else {
		return ' ';
	}
}

char player_check_right(player_t *player, world_t *world, int knightNum)
{
	room_t *room = &world->room[player->global_x][player->global_y];
    for(int i = 0; i < room->current_enemy_count; i++) {
        if(player->x[knightNum] + 1 == room->enemies[i]->x && player->y[knightNum] == room->enemies[i]->y) {
            return ENEMY_CHAR;
        }  
    }
	if(player->x[knightNum] + 1 <= ROOM_WIDTH) {
		return room->layout[player->y[knightNum]][player->x[knightNum]+1];
	} else {
		return ' ';
	}
}

char player_check_down(player_t *player, world_t *world, int knightNum)
{
	room_t *room = &world->room[player->global_x][player->global_y];
    for(int i = 0; i < room->current_enemy_count; i++) {
        if(player->y[knightNum] + 1 == room->enemies[i]->y && player->x[knightNum] == room->enemies[i]->x) {
            return ENEMY_CHAR;
        }  
    }
	if(player->y[knightNum] + 1 <= ROOM_HEIGHT) {
		return room->layout[player->y[knightNum] + 1][player->x[knightNum]];
	} else {
		return ' ';
	}
}

char player_check_up(player_t *player, world_t *world, int knightNum)
{
	room_t *room = &world->room[player->global_x][player->global_y];
    for(int i = 0; i < room->current_enemy_count; i++) {
		if(player->y[knightNum] - 1 == room->enemies[i]->y && player->x[knightNum] == room->enemies[i]->x) {
            return ENEMY_CHAR;
        }  
    }
	if(player->y[knightNum] - 1 >= 0) {
		return room->layout[player->y[knightNum]-1][player->x[knightNum]];
	} else {
		return ' ';
	}
}

int player_can_move_left(player_t *player, world_t *world, int knightNum)
{
	
	char a = player_check_left(player, world, knightNum);
	for(int i = 0; i < WALK_CHAR_LENGTH; i++) {
		if(a == walk_chars[i]) { // the space is open
			return 1;
		}
	}
	return 0;
}

int player_can_move_right(player_t *player, world_t *world, int knightNum)
{
	
	char a = player_check_right(player, world, knightNum);
	for(int i = 0; i < WALK_CHAR_LENGTH; i++) {
		if(a == walk_chars[i]) { // the space is open
			return 1;
		}
	}
	return 0;
}

int player_can_move_down(player_t *player, world_t *world, int knightNum)
{
	
	char a = player_check_down(player, world, knightNum);
	for(int i = 0; i < WALK_CHAR_LENGTH; i++) {
		if(a == walk_chars[i]) { // the space is open
			return 1;
		}
	}
	return 0;
}

int player_can_move_up(player_t *player, world_t *world, int knightNum)
{
	
	char a = player_check_up(player, world, knightNum);
	for(int i = 0; i < WALK_CHAR_LENGTH; i++) {
		if(a == walk_chars[i]) { // the space is open
			return 1;
		}
	}
	return 0;
}

void player_wait(player_t *player, world_t *world)
{
	display_combat_message(world, player, "You stand still!");
    world->isPlayerTurn = 0;
}

void player_decrease_health(player_t *player, world_t *world, int attack, int knightNum)
{
	display_combat_message(world, player, "The player was hit!");
    player->health[knightNum] -= attack;
    if(player->health[knightNum] <= 0) {
        end_game(world, player);
    }
}

void player_increase_health(player_t *player, int amount)
{
	for(int i = 0; i < MAX_KNIGHTS; i++) {
		if(player->health[i] + amount > player->max_health[i]) {
			player->health[i] = player->max_health[i];
		} else {
			player->health[i] += amount;
		}
	}
}


enemy_t *player_get_left_enemy(player_t *player, world_t *world, int knightNum) {
	room_t *room = &world->room[player->global_x][player->global_y];
    for(int i = 0; i < room->current_enemy_count; i++) {
		if(player->x[knightNum] - 1 == room->enemies[i]->x && player->y[knightNum] == room->enemies[i]->y) {
            return room->enemies[i];
        }  
    }
    return NULL;
}

enemy_t *player_get_right_enemy(player_t *player, world_t *world, int knightNum) {
	room_t *room = &world->room[player->global_x][player->global_y];
    for(int i = 0; i < room->current_enemy_count; i++) {
		if(player->x[knightNum] + 1 == room->enemies[i]->x && player->y[knightNum] == room->enemies[i]->y) {
            return room->enemies[i];
        }  
    }
    return NULL;
}

enemy_t *player_get_down_enemy(player_t *player, world_t *world, int knightNum) {
	room_t *room = &world->room[player->global_x][player->global_y];
    for(int i = 0; i < room->current_enemy_count; i++) {
		if(player->y[knightNum] + 1 == room->enemies[i]->y && player->x[knightNum] == room->enemies[i]->x) {
            return room->enemies[i];
        }  
    }
    return NULL;
}

enemy_t *player_get_up_enemy(player_t *player, world_t *world, int knightNum) {
	room_t *room = &world->room[player->global_x][player->global_y];
    for(int i = 0; i < room->current_enemy_count; i++) {
		if(player->y[knightNum] - 1 == room->enemies[i]->y && player->x[knightNum] == room->enemies[i]->x) {
            return room->enemies[i];
        }  
    }
    return NULL;
}

/**
 * This function looks at all knights and will pick the weakest enemy for the knight to attack
 * then call the given attack function
 */
void player_attack(player_t *player, world_t *world) {
    for(int i = 0; i < MAX_KNIGHTS; i++) {
		enemy_t *enL = player_get_left_enemy(player, world, i);
		enemy_t *enR = player_get_right_enemy(player, world, i);
		enemy_t *enD = player_get_down_enemy(player, world, i);
		enemy_t *enU = player_get_up_enemy(player, world, i);
		if(!enL) enL = enemy_create_temp(world);
		if(!enR) enR = enemy_create_temp(world);
		if(!enD) enD = enemy_create_temp(world);
		if(!enU) enU = enemy_create_temp(world);
		if(enL->health == 1000000 && enR->health == 1000000 &&
			enD->health == 1000000 && enU->health == 1000000) {
			continue;
		}
		int res = compare4(enL->health, enR->health, enD->health, enU->health);
        if(res == enL->health) {
            enemy_decrease_health(enL, world, player, i);
        } else if(res == enR->health) {
			enemy_decrease_health(enR, world, player, i);
        } else if(res == enD->health) {
			enemy_decrease_health(enD, world, player, i);
        } else if(res == enU->health) {
			enemy_decrease_health(enU, world, player, i);
        }
    }
    world->isPlayerTurn = 0;
}

void player_open_action_bar(player_t *player)
{
	player->action_bar.selector = INVENTORY;
}

void player_close_action_bar(player_t *player)
{
	player->action_bar.selector = NOT_OPEN;
}

void player_cycle_action_bar_down(player_t *player)
{
	if(player->action_bar.selector+1 >= ACTION_BAR_SELECTOR_COUNT) {
		player->action_bar.selector = 0;
	} else {
		player->action_bar.selector += 1;
	}
}

void player_cycle_action_bar_up(player_t *player)
{
	if(player->action_bar.selector <= 0) {
		player->action_bar.selector = ACTION_BAR_SELECTOR_COUNT-1;
	} else {
		player->action_bar.selector -= 1;
	}
}
void player_cycle_inv_selector_up(player_t *player)
{
	if(player->action_bar.inv_selector > 0) {
		player->action_bar.inv_selector--;
	}
		
}

void player_cycle_inv_selector_down(player_t *player)
{
	if(player->inventory[player->action_bar.inv_selector+1].id != BLANK) {
		player->action_bar.inv_selector++;
	}
}

void player_open_inventory(player_t *player)
{
	player->action_bar.inv_open = 1;
	hud_update_action_bar(player);
}

void player_close_inventory(player_t *player)
{
	player->action_bar.inv_open = 0;
}

void player_open_spells(player_t *player)
{
	player->action_bar.spells_open = 1;
}

void player_close_spells(player_t *player)
{
	player->action_bar.spells_open = 0;
}

// removes an item from the inventory list and reorganizes, not used to decrease item count
void player_organize_inv(player_t *player, int loc)
{
	for(int i = loc; i < INV_SIZE-1; i++) {
		player->inventory[i] = player->inventory[i + 1];
	}
	item_t blank = {BLANK_NAME, BLANK, 0};
	player->inventory[INV_SIZE-1] = blank;
	while(player->inventory[player->action_bar.inv_selector].id == BLANK && player->action_bar.inv_selector > 0) {
		player_cycle_inv_selector_up(player);
	}
}





