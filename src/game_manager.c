#include "game_manager.h"
#include "hud.h"
#include "player.h"
#include "items.h"
#include "map_manager.h"
#include "enemy.h"
#include "math.h"
#include <assert.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "lantern.h"

//wmove(win, x, y);
//waddch(win, char);
extern WINDOW *hud, *action_bar, *inventory_hud;
extern char walk_chars[WALK_CHAR_LENGTH];

void draw(world_t *world, player_t *player) {
	werase(world->win);
	werase(hud);
	werase(action_bar);
	werase(inventory_hud);
	// draw stuff...
	if((player->action_bar.inv_open || player->action_bar.spells_open || player->action_bar.loot_open) || player->action_bar.selector != NOT_OPEN) {
		display_inventory_hud(world, player);
		wnoutrefresh(inventory_hud);
	} else {
		// TODO add just a hud_update() function so you don't have to call a bunch of different functions
		room_t *room = world->room[player->global_x][player->global_y];
		hud_update_player_health(player);
		hud_update_nearby_enemies(world, player);
		hud_update_action_bar(player, room);
		hud_update_messages(world, player);
		for(int i = 0; i < ROOM_HEIGHT; i++) {
			for(int j = 0; j < ROOM_WIDTH; j++) {
				if(room->tiles[i][j]->has_light == false) {
					continue;
				}
				int enemyIsThere = 0;
				int playerIsThere = 0;
				wmove(world->win, i, j);
				for(int u = 0; u < room->current_enemy_count; u++) {
					if(room->enemies[u] == NULL) continue;
					if(room->enemies[u]->x == j && room->enemies[u]->y == i) {
						waddch(world->win, room->enemies[u]->symbol);
						enemyIsThere = 1;
						break;
					}
				}
				
				if(player->x == j && player->y == i) {
					waddch(world->win, PLAYER);
					playerIsThere = 1;
				}
				
				for(int k = 0; k < MAX_ITEMS_PER_TILE; k++) {
					if(room->tiles[i][j]->items[k]->stack > 0) {
						waddch(world->win, ITEM_SYMBOL);
						enemyIsThere = 1; //TODO
						break;
					}
				}
				
				if(!playerIsThere && !enemyIsThere) {
					waddch(world->win, room->tiles[i][j]->floor);
				}
			}
		}
		wnoutrefresh(inventory_hud); // this has to be first
		wnoutrefresh(world->win);
		wnoutrefresh(hud);
		wnoutrefresh(action_bar);
	}
	doupdate();
	// wrefresh(world->win);
	// wrefresh(hud);
	// wrefresh(action_bar);
}

bool manage_input(char c, world_t *world, player_t *player) 
{
	if (c == ERR) {
		return true;
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
			case KEY_F_MINE:
				lantern_increase_power(&player->lantern);
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
		return true;
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
			hud_update_action_bar(player, world->room[player->global_x][player->global_y]);
			return false;
		} else if(player->action_bar.spells_open) {
			return false; 
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
			return false;
		}
	}
}

void display_combat_message(world_t *world, player_t *player, const char *str) {
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

bool is_opaque(room_t *room, int x, int y) {
	char a = room->tiles[y][x]->floor;
	for(int i = 0; i < WALK_CHAR_LENGTH; i++) {
		if(a == walk_chars[i] || a == PLAYER) { // the space is open
			return false;
		}
	}
	return true;
}

void mark_has_light(room_t *room, int x, int y) {
	room->tiles[y][x]->has_light = true;
}

void cast_light_check(world_t *world, player_t *player, int x0, int y0, float angle) {
	float dx = cos(angle);
	float dy = sin(angle);
	float x = x0 + 0.3f;
	float y = y0 + 0.3f;
	int max_distance = player->lantern.power > PLAYER_MIN_VISION_RANGE ? player->lantern.power : PLAYER_MIN_VISION_RANGE;
	
	for(int i = 0; i < max_distance; i++) {
		int tile_x = (int)x;
		int tile_y = (int)y;
		
		if(tile_x > ROOM_WIDTH-1 || tile_y > ROOM_HEIGHT-1 || tile_y < 0 || tile_x < 0) {
			break;
		}
		room_t *room = world->room[player->global_x][player->global_y];
		
		mark_has_light(room, tile_x, tile_y);
		
		if(is_opaque(room, tile_x, tile_y)) {
			break;
		}
		
		x += dx;
		y += dy;
	}
}

void calculate_light(world_t *world, player_t *player) {
	for(int i = 0; i < ROOM_HEIGHT; i++) {
		for(int j = 0; j < ROOM_WIDTH; j++) {
			world->room[player->global_x][player->global_y]->tiles[i][j]->has_light = false;
		}
	}
	for(float a = 0; a < 2 * M_PI; a += 0.03f) {
		cast_light_check(world, player, player->x, player->y, a);
	}
}

int pick_next_actor(world_t *world, player_t *player) {
	int idx = INVALID_ACTOR_INDEX;
	int fastest = 0;
	while(true) {
		player->action_points += player->speed;
		if(player->action_points >= TIME_TO_ACT) {
			fastest = player->action_points;
			idx = PLAYER_TURN_ORDER_INDEX;
		}
		
		for(int i = 0; i < world->room[player->global_x][player->global_y]->current_enemy_count; i++) {
			enemy_t *enemy = world->room[player->global_x][player->global_y]->enemies[i];
			if(enemy == NULL) continue;
			enemy->action_points += enemy->speed;
			if(enemy->action_points >= TIME_TO_ACT) {
				if(fastest < enemy->action_points) {
					fastest = enemy->action_points;
					idx = i;
				}
			}
		}
		
		if(idx == INVALID_ACTOR_INDEX) continue;
		
		if(idx == PLAYER_TURN_ORDER_INDEX) {
			player->action_points -= TIME_TO_ACT;
		} else {
			enemy_t *enemy = world->room[player->global_x][player->global_y]->enemies[idx];
			enemy->action_points -= TIME_TO_ACT;
		}
		if(idx != INVALID_ACTOR_INDEX) break;
	}
	return idx;
}

void generate_turn_order_display(world_t *world, player_t *player) {
	int projected_enemy_ap[MAX_ENEMIES_PER_LEVEL];
	int player_projected_ap = player->action_points;
	world->turn_order_size = 0;
	room_t *room = world->room[player->global_x][player->global_y];
	for(int i = 0; i < room->current_enemy_count; i++) {
		enemy_t *enemy = room->enemies[i];
		if(enemy == NULL) continue;
		projected_enemy_ap[i] = enemy->action_points;
	}
	while(world->turn_order_size < MAX_ENEMIES_PER_LEVEL) {
		int largest = 0;
		int idx = INVALID_ACTOR_INDEX;
		player_projected_ap += player->speed;
		if(player_projected_ap >= TIME_TO_ACT && player_projected_ap > largest) {
			largest = player_projected_ap;
			idx = PLAYER_TURN_ORDER_INDEX;
		}
		
		room_t *room = world->room[player->global_x][player->global_y];
		for(int i = 0; i < room->current_enemy_count; i++) {
			enemy_t *enemy = room->enemies[i];
			if(enemy == NULL) continue;
			projected_enemy_ap[i] += enemy->speed;
			if(projected_enemy_ap[i] >= TIME_TO_ACT && projected_enemy_ap[i] > largest) {
				largest = projected_enemy_ap[i];
				idx = i;
			}
		}
		
		if(idx == INVALID_ACTOR_INDEX) {
			continue;
		} else if(idx == PLAYER_TURN_ORDER_INDEX) {
			player_projected_ap -= TIME_TO_ACT;
		} else {
			projected_enemy_ap[idx] -= TIME_TO_ACT;
		}
		world->turn_order[world->turn_order_size++] = idx;
	}
}

void turn_order_enter_new_room(world_t *world, player_t *player) {
	player->action_points = 0;
	assert(world->room[player->global_x][player->global_y]->is_created);
	for(int i = 0; i < world->room[player->global_x][player->global_y]->current_enemy_count; i++) {
		if(world->room[player->global_x][player->global_y]->enemies[i] == NULL) continue;
		world->room[player->global_x][player->global_y]->enemies[i]->action_points = 0;
	}
}

void end_game(world_t *world, player_t *player) {
    shutdown(world); //TODO this obvisouly isn't what should happpen
}

void shutdown(world_t *world) {
	endwin();
	for(int y = 0; y < ROOM_HEIGHT; y++) {
		for(int x = 0; x < ROOM_WIDTH; x++) {
			free(world->room[y][x]);
		}
	}
	free(world);
	exit(0);
}

