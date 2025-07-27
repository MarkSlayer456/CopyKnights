#include "game_manager.h"
#include "hud.h"
#include "player.h"
#include "items.h"
#include "map_manager.h"
#include "enemy.h"
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

//wmove(win, x, y);
//waddch(win, char);
extern WINDOW *hud;
extern WINDOW *action_bar;

void draw(world_t *world, player_t *player) 
{
	werase(world->win);
	werase(hud);
	// draw stuff...
    // TODO add just a hud_update() function so you don't have to call a bunch of different functions
	hud_update_player_health(player);
    hud_update_nearby_enemies(world, player);
	hud_update_action_bar(player);
	hud_update_messages(world, player);
	room_t *room = &world->room[player->global_x][player->global_y];
	for(int i = 0; i < ROOM_HEIGHT; i++) {
		for(int j = 0; j < ROOM_WIDTH; j++) {
			int enemyIsThere = 0;
			int playerIsThere = 0;
			wmove(world->win, i, j);
			for(int u = 0; u < room->current_enemy_count; u++) {
				if(room->enemies[u] == NULL) continue;
				if(room->enemies[u]->x == j && room->enemies[u]->y == i) {
					waddch(world->win, room->enemies[u]->name[0]);
					enemyIsThere = 1;
					break;
				}
			}
			for(int k = 0; k < MAX_KNIGHTS; k++) {
				if(player->x[k] == j && player->y[k] == i) {
					if(k == 0) {
						waddch(world->win, PLAYER);
					} else {
						waddch(world->win, k);
					}
					playerIsThere = 1;
					break;
				}
			}
			if(!playerIsThere && !enemyIsThere) {
				waddch(world->win, room->layout[i][j]);
			}
		}
	}
	wrefresh(world->win);
	wrefresh(hud);
	wrefresh(action_bar);
}

void manage_input(char c, world_t *world, player_t *player) 
{
	if (c == ERR) {
		return;
	}
	int x = c;
	if(c == 27) {
		c = getch();
		if(c == 91) { // is not a ctrl arrow key
			c = getch();	
			x = ARROW_KEY_MOD + c;
		}
	}
	if(player->action_bar.selector == NOT_OPEN) {
		switch(x) {
			case LEFT_ARROW:
				player_move_left(player, world);
				break;
			case RIGHT_ARROW:
				player_move_right(player, world);
				break;
			case UP_ARROW:
				player_move_up(player, world);
				break;
			case DOWN_ARROW:
				player_move_down(player, world);
				break;
			case KEY_A:
				player_wait(player, world);
				break;
			case KEY_S:
				player_attack(player, world);
				break;
			case CTRL_Q:
				shutdown(world);
				break;
			case KEY_I:
				player_open_action_bar(player);
				break;
			default:
				break;
		}	
	} else {
		if(player->action_bar.inv_open) {
			switch(x) {
				case UP_ARROW:
					player_cycle_inv_selector_up(player);
					break;
				case DOWN_ARROW:
					player_cycle_inv_selector_down(player);
					break;
				case CTRL_Q:
					shutdown(world);
					break;
				case ENTER_KEY:
					use_item(player);
					//TODO
// 					if(player->action_bar.selector == INVENTORY) {
// 						player_open_inventory(player);
// 					} else if(player->action_bar.selector == SPELLS) {
// 						player_open_spells(player);
// 					}
					break;
				case KEY_I:
					//TODO reset defaults
					player_close_inventory(player);
					player_close_action_bar(player);
					break;
				case KEY_B:
					//TODO reset defaults
					player_close_inventory(player);
					break;
				default:
					break;
			}
			hud_update_action_bar(player);
		} else if(player->action_bar.spells_open) {
			
		} else {
			switch(x) {
				case UP_ARROW:
					player_cycle_action_bar_up(player);
					break;
				case DOWN_ARROW:
					player_cycle_action_bar_down(player);
					break;
				case CTRL_Q:
					shutdown(world);
					break;
				case ENTER_KEY:
					if(player->action_bar.selector == INVENTORY) {
						player_open_inventory(player);
					} else if(player->action_bar.selector == SPELLS) {
						player_open_spells(player);
					}
					break;
				case KEY_I:
					player_close_action_bar(player);
				default:
					break;
			}
		}
	}
}

void display_combat_message(world_t *world, player_t *player, const char *str) {
	if(MAX_MESSAGE_LENGTH < strlen(str)) return;
	int current_size = world->messages_size;
	if(current_size >= world->max_message_storage) {
		world->max_message_storage*=2;
		char **tmp = realloc(world->messages, world->max_message_storage * sizeof(char *));
		if(tmp) {
			for(int i = world->max_message_storage/2; i < world->max_message_storage; i++) {
					tmp[i] = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
			}
			world->messages = tmp;
		}
	}
	// strncpy(world->messages[current_size], str, MAX_MESSAGE_LENGTH-1);
	snprintf(world->messages[current_size], MAX_MESSAGE_LENGTH, "[Combat] %s", str);
	world->messages_size++;
}

void display_world_message(world_t *world, player_t *player, const char *str) {
	if(MAX_MESSAGE_LENGTH_WITHOUT_PREFIX < strlen(str)) return;
	int current_size = world->messages_size;
	if(current_size >= world->max_message_storage) {
		world->max_message_storage*=2;
		char **tmp = realloc(world->messages, world->max_message_storage * sizeof(char *));
		if(tmp) {
			for(int i = world->max_message_storage/2; i < world->max_message_storage; i++) {
				tmp[i] = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
			}
			world->messages = tmp;
		}
	}
	// strncpy(world->messages[current_size], str, MAX_MESSAGE_LENGTH-1);
	snprintf(world->messages[current_size], MAX_MESSAGE_LENGTH, "[World] %s", str);
	world->messages_size++;
}


int pick_next_actor(world_t *world, player_t *player) {
	int idx = 100; // TODO need a number that won't appear???
	int fastest = 0;
	while(true) {
		player->action_points += player->speed;
		if(player->action_points >= TIME_TO_ACT) {
			fastest = player->action_points;
			idx = PLAYER_TURN_ORDER_INDEX;
		}
		
		for(int i = 0; i < world->room[player->global_x][player->global_y].current_enemy_count; i++) {
			enemy_t *enemy = world->room[player->global_x][player->global_y].enemies[i];
			if(enemy == NULL) continue;
			enemy->action_points += enemy->speed;
			if(enemy->action_points >= TIME_TO_ACT) {
				if(fastest < enemy->action_points) {
					fastest = enemy->action_points;
					idx = i;
				}
			}
		}
		
		if(idx == 100) continue;
		
		if(idx == PLAYER_TURN_ORDER_INDEX) {
			player->action_points -= TIME_TO_ACT;
		} else {
			enemy_t *enemy = world->room[player->global_x][player->global_y].enemies[idx];
			enemy->action_points -= TIME_TO_ACT;
		}
		if(idx != 100) break;
	}
	return idx;
}

void turn_order_enter_new_room(world_t *world, player_t *player) {
	memset(world->turn_order, 0, (MAX_ENEMIES_PER_LEVEL+1) * sizeof(int));
	world->turn_order_size = 0;
	player->action_points = 0;
	for(int i = 0; i < world->room[player->global_x][player->global_y].current_enemy_count; i++) {
		world->room[player->global_x][player->global_y].enemies[i]->action_points = 0;
	}
}

void end_game(world_t *world, player_t *player) {
    shutdown(world); //TODO this obvisouly isn't what should happpen
}

void shutdown(world_t *world) {
	endwin();
	for(int i = 0; i < ROOM_WIDTH; i++) {
		free(world->room[i]);
	}
	free(world->room);
	free(world);
	printf("too bad nurd, GIT GUD!\n");
	exit(0);
}

