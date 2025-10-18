#include "game_manager.h"
#include "hud.h"
#include "player.h"
#include "items/items.h"
#include "map_manager.h"
#include "enemy.h"
#include <math.h>
#include <assert.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include "types.h"
#include "lantern.h"
#include "save.h"

//wmove(win, x, y);
//waddch(win, char);
extern WINDOW *hud, *action_bar, *inventory_hud, *inventory_desc_hud;
extern char walk_chars[WALK_CHAR_LENGTH];

void draw(world_t *world, player_t *player) {
	werase(world->win);
	werase(hud);
	werase(action_bar);
	werase(inventory_hud);
	werase(inventory_desc_hud);
	// draw stuff...
	if(player->state != PLAYER_STATE_MOVING && player->state != PLAYER_STATE_ATTACKING 
		&& player->state != PLAYER_STATE_CASTING) {
		display_inventory_hud(world, player);
		display_inventory_desc_hud(world, player);
		wnoutrefresh(inventory_hud);
		wnoutrefresh(inventory_desc_hud);
	} else {
		// TODO add just a hud_update() function so you don't have to call a bunch of different functions
		room_t *room = world->room[player->global_x][player->global_y];
		hud_update_player_health(player, world->buffs, world->buff_count);
		hud_update_nearby_enemies(world, player);
		// hud_update_menus(player, room);
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
					if(room->tiles[i][j]->items[k] != NULL && room->tiles[i][j]->items[k]->stack > 0) {
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
		wnoutrefresh(inventory_desc_hud); // this has to be first
		wnoutrefresh(world->win);
		wnoutrefresh(hud);
		wnoutrefresh(action_bar);
	}
	doupdate();
}

// TODO change this function it's not written well
bool manage_input(char c, world_t *world, player_t *player, menu_manager_t *menu_manager) {
	room_t *room = world->room[player->global_x][player->global_y];
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
	if(player->state == PLAYER_STATE_MOVING) {
		switch(x) {
			case LEFT_ARROW:
				player_move_dir(player, world, LEFT);
				break;
			case RIGHT_ARROW:
				player_move_dir(player, world, RIGHT);
				break;
			case UP_ARROW:
				player_move_dir(player, world, UP);
				break;
			case DOWN_ARROW:
				player_move_dir(player, world, DOWN);
				break;
			case KEY_A:
				player_wait(player, world);
				break;
			case KEY_S:
				player_enter_attack_state(player, world);
				return false;
			case CTRL_S:
				menu_manager->current_menu = SAVE_MENU;
				world->is_player_turn = true;
				// save_game(world, player, "save.bin");
				return true;
			case KEY_F_MINE:
				if(lantern_increase_power(&player->lantern, &player->oil) == false) {
					display_world_message(world, player, LANTERN_CAN_AFFORD_REFUEL);
				}
				break;
			case CTRL_Q:
				shutdown(world);
				break;
			case KEY_I:
				player_open_inventory(player);
				return false;
				break;
			default:
				break;
		}
		world->is_player_turn = false;
		return true;
	} else if(player->state == PLAYER_STATE_INVENTORY) {
		switch(x) {
			case UP_ARROW:
				player_cycle_inv_selector_up(player);
				break;
			case DOWN_ARROW:
				player_cycle_inv_selector_down(player);
				break;
			case RIGHT_ARROW:
				player_open_loot(player);
				break;
			case CTRL_Q:
				shutdown(world);
				break;
			case ENTER_KEY:
				use_item(player);
				return true;
			case KEY_I:
				//TODO reset defaults
				player_close_inventory(player);
				return false;
			case KEY_B:
				//TODO reset defaults
				player_close_inventory(player);
				break;
			default:
				break;
		}
		world->is_player_turn = false;
		// hud_update_action_bar(player, world->room[player->global_x][player->global_y]);
		return false;
	} else if(player->state == PLAYER_STATE_LOOTING) { 
		switch(x) {
			case UP_ARROW:
				player_cycle_loot_selector_up(player);
				break;
			case DOWN_ARROW:
				player_cycle_loot_selector_down(player);
				break;
			case LEFT_ARROW:
				player_open_inventory(player);
				break;
			case CTRL_Q:
				shutdown(world);
				break;
			case ENTER_KEY:
				player_take_loot_item(room, player);
				break;
			case KEY_I:
				//TODO reset defaults
				player_close_inventory(player);
				break;
			case KEY_B:
				//TODO reset defaults
				player_close_inventory(player);
				break;
			default:
				break;
		}
		return false;
	} else if(player->state == PLAYER_STATE_ATTACKING) {
		switch(x) {
			case UP_ARROW:
				player_attack(player, world, UP);
				return true;
			case DOWN_ARROW:
				player_attack(player, world, DOWN);
				return true;
			case RIGHT_ARROW:
				player_attack(player, world, RIGHT);
				return true;
			case LEFT_ARROW:
				player_attack(player, world, LEFT);
				return true;
			case CTRL_Q:
				shutdown(world);
				break;
			case ENTER_KEY:
				player_take_loot_item(room, player);
				break;
			// TODO need a way to cancel attacks
			default:
				break;
		}
		return false;
	}
	return false;
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
	//TODO max_message_storage
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
	//TODO max_message_storage
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

void remove_points_from_actor() {	
	
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

void draw_main_menu(WINDOW *main_menu, menu_manager_t *menu_manager) {
	werase(main_menu);
	
	menu_manager->dests_count = 0;

	char display_str[16] = "";

	int y_pos = 0;
	wmove(main_menu, y_pos++, 0);
	waddstr(main_menu, GAME_TITLE);
	wmove(main_menu, y_pos++, 0);
	waddstr(main_menu, MENU_DIVIDE_LINE);
	int dest_pos = 0;
	menu_manager->dests[dest_pos++] = GAME;
	menu_manager->dests_count++;
	char new_game[9] = "New Game";
	wmove(main_menu, y_pos++, 0);
	if(menu_manager->dests[menu_manager->cursor_pos] == GAME) {
		snprintf(display_str, sizeof(display_str), ">>%s", new_game);
		waddstr(main_menu, display_str);
	} else {
		waddstr(main_menu, new_game);
	}
	menu_manager->dests[dest_pos++] = LOAD_MENU;
	menu_manager->dests_count++;
	char load_game[11] = "Load Game";
	wmove(main_menu, y_pos++, 0);
	if(menu_manager->dests[menu_manager->cursor_pos] == LOAD_MENU) {
		snprintf(display_str, sizeof(display_str), ">>%s", load_game);
		waddstr(main_menu, display_str);
	} else {
		waddstr(main_menu, load_game);
	}
	menu_manager->dests[dest_pos++] = LOG_BOOK_MENU;
	menu_manager->dests_count++;
	char log_book[9] = "Log Book";
	wmove(main_menu, y_pos++, 0);
	if(menu_manager->dests[menu_manager->cursor_pos] == LOG_BOOK_MENU) {
		snprintf(display_str, sizeof(display_str), ">>%s", log_book);
		waddstr(main_menu, display_str);
	} else {
		waddstr(main_menu, log_book);
	}
	
	wnoutrefresh(main_menu);
	doupdate();
}

void generate_load_menu_list(load_menu_t *load_menu) {
	DIR *dir = opendir(get_save_path());
	
	if(!dir) {
		perror("opendir failed");
		return;
	}
	struct dirent *entry;
	while((entry = readdir(dir)) != NULL) {
		if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}
		if(load_menu->filename_count >= load_menu->filename_size) {
			load_menu->filename_size *= 2;
			load_menu->filename = realloc(load_menu->filename, load_menu->filename_size * sizeof(char[SAVE_FILE_MAX_LEN]));
		}
		strcpy(load_menu->filename[load_menu->filename_count], entry->d_name);
		load_menu->filename_count++;
	}
}

void draw_load_menu(const load_menu_t *load_menu) {
	werase(load_menu->win);
	int y_pos = 0;
	wmove(load_menu->win, y_pos++, 0);
	waddstr(load_menu->win, LOAD_MENU_TITLE);
	wmove(load_menu->win, y_pos++, 0);
	waddstr(load_menu->win, MENU_DIVIDE_LINE);
	char display_str[256];
	for(int i = load_menu->cursor_offset; i < (LOAD_MENU_VISIBLE_ENTRIES + load_menu->cursor_offset); i++) {
		wmove(load_menu->win, y_pos, 0);
		if(i == load_menu->cursor_pos) {
			snprintf(display_str, sizeof(display_str), ">>%s", load_menu->filename[i]);
		} else {
			snprintf(display_str, sizeof(display_str), "%s", load_menu->filename[i]);
		}
		waddstr(load_menu->win, display_str);
		y_pos++;
	}
	wnoutrefresh(load_menu->win);
	doupdate();
}

void manage_load_menu_input(char c, load_menu_t *load_menu, world_t *world, player_t *player, menu_manager_t *menu_manager) {
	if(c == ERR) {
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
	switch(x) {
		case LEFT_ARROW:
			break;
		case RIGHT_ARROW:
			break;
		case UP_ARROW:
			if(load_menu->cursor_pos > 0) {
				if(load_menu->cursor_pos - load_menu->cursor_offset == 0) {
					load_menu->cursor_offset--;
				}
				load_menu->cursor_pos--;
			}
			break;
		case DOWN_ARROW: {
				if(load_menu->cursor_pos + 1 < load_menu->filename_count) {
					if(load_menu->cursor_pos - load_menu->cursor_offset >= LOAD_MENU_VISIBLE_ENTRIES-1) {
						load_menu->cursor_offset++;
					}
					load_menu->cursor_pos++;
				}
				break;
			}
		case CTRL_Q:
			shutdown(world);
			break;
		case ENTER_KEY:
			load_game(world, player, load_menu->filename[load_menu->cursor_pos]);
			menu_manager->current_menu = GAME;
			break;
	}
}

void manage_menu_input(char c, menu_manager_t *menu_manager, world_t *world) {
	if(c == ERR) {
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
	switch(x) {
		case LEFT_ARROW:
			break;
		case RIGHT_ARROW:
			break;
		case UP_ARROW:
			if(menu_manager->cursor_pos - 1 < 0) { 
				menu_manager->cursor_pos = menu_manager->dests_count-1;
			} else {
				menu_manager->cursor_pos--;
			}
			break;
		case DOWN_ARROW:
			if(menu_manager->cursor_pos + 1 < menu_manager->dests_count) { 
				menu_manager->cursor_pos++;
			} else {
				menu_manager->cursor_pos = 0;
			}
			break;
		case CTRL_Q:
			shutdown(world);
			break;
		case ENTER_KEY:
			// TODO probablly want a switch statement here
			if(menu_manager->dests[menu_manager->cursor_pos] == GAME) {
				menu_manager->current_menu = GAME;
			} else if(menu_manager->dests[menu_manager->cursor_pos] == LOAD_MENU) {
				menu_manager->current_menu = LOAD_MENU;
			}
			break;
	}
}

void display_and_manage_save_menu(WINDOW *win, char *buf, int max_len, world_t *world, player_t *player, menu_manager_t *menu_manager) {
	werase(win);
	memset(buf, 0, max_len);
	int ch;
	int len = 0;
	int y = 1;
	int x = 0;
	waddstr(win, SAVE_MENU_PROMPT);
	wmove(win, y, x);
	touchwin(win);
	wrefresh(win);
	while((ch = wgetch(win)) != ESC_KEY) {
		if(isprint(ch) && len < max_len-1) {
			buf[len++] = ch;
			mvwaddch(win, y, x + len - 1, ch);
		} else if(ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
			len--;
			buf[len] = '\0';
			mvwaddch(win, y, x + len, ' ');
			wmove(win, y, x + len);
		} else if(ch == ENTER_KEY) {
			save_game(world, player, buf);
			menu_manager->current_menu = GAME;
			break;
		}
		wnoutrefresh(win);
		doupdate();
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

