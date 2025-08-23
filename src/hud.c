#include "hud.h"
#include "game_manager.h"
#include "map_manager.h"
#include "items/items.h"
#include "player.h"
#include <stdlib.h>
#include <string.h>
#include "types.h"

extern WINDOW *hud, *action_bar, *inventory_hud, *inventory_desc_hud;

void hud_update_player_health(player_t *player) {
	wmove(hud, 0, 0);
	wprintw(hud, "Player Status");
	int y = 1;
	int x = 0;
	wmove(hud, y, x);
	waddch(hud, PLAYER);
	waddch(hud, ':');
	waddch(hud, ' ');
	//TODO rename all strings in this function
	char buf[64];
	snprintf(buf, sizeof(buf), "%d", player->health);
	waddstr(hud, buf);
	
	waddch(hud, ' ');
	waddch(hud, '/');
	waddch(hud, ' ');
	
	snprintf(buf, sizeof(buf), "%d", player->max_health);
	waddstr(hud, buf);
	
	getyx(hud, y, x);
	wmove(hud, y+1, 0);
	snprintf(buf, sizeof(buf), "Level: %d | XP: %d / %d", player->level, player->xp, xp_to_level_up(player->level));
	waddstr(hud, buf);
	
	// this will break if any stats are over 3 digits
	getyx(hud, y, x);
	wmove(hud, y+1, 0);
	snprintf(buf, sizeof(buf), "Str: %d | Dex: %d | Int: %d", player->strength, player->dexterity, player->intelligence);
	waddstr(hud, buf);
	
	getyx(hud, y, x);
	wmove(hud, y+1, 0);
	snprintf(buf, sizeof(buf), "Constitution: %d | Speed: %d", player->constitution, player->speed);
	waddstr(hud, buf);
}

void hud_update_nearby_enemies(world_t *world, player_t *player) {
	int detect_radius = player->lantern.power;
	room_t *room = world->room[player->global_x][player->global_y];
	
	wmove(hud, PLAYER_STATS_HUD_SPACE, 0);
	char str6[128];
	int pos = snprintf(str6, 128, "Turn Order: ");
	for(int i = 0; i < world->turn_order_size; i++) {
		if(world->turn_order[i] == PLAYER_TURN_ORDER_INDEX) {
			pos += snprintf(str6 + pos, sizeof(str6)-pos, "[%c] ", PLAYER);
			continue;
		}
		enemy_t *enemy = room->enemies[world->turn_order[i]];
		if(enemy != NULL) {
			if(room->tiles[enemy->y][enemy->x]->has_light) {
				pos += snprintf(str6 + pos, sizeof(str6)-pos, "[%c] ", enemy->symbol);
			} else {
				pos += snprintf(str6 + pos, sizeof(str6)-pos, "[?] ");
			}
		}	
	}
	waddstr(hud, str6);
	
	for(int i = 0; i < MAX_ENEMIES_PER_LEVEL; i++) {
		if(!room->enemies[i]) continue;
		if(room->enemies[i]->x > player->x-detect_radius && room->enemies[i]->x < player->x+detect_radius &&
			room->enemies[i]->y > player->y-(detect_radius) && room->enemies[i]->y < player->y+(detect_radius)) {
			wmove(hud, PLAYER_STATS_HUD_SPACE+i+1, 0);
			char name[32];
			snprintf(name, sizeof(name), "%s", room->enemies[i]->name);
			waddstr(hud, name);
			mvwaddch(hud, PLAYER_STATS_HUD_SPACE+i+1, strlen(name), ':');
			char str[8];
			wmove(hud, PLAYER_STATS_HUD_SPACE+i+1, strlen(name)+1);
			snprintf(str, sizeof(str), "%d", room->enemies[i]->health);
			waddstr(hud, str);
		}
	}
}

void hud_update_action_bar(player_t *player, room_t *room) {
	wmove(action_bar, 0, 0);
	if(player->action_bar.selector == INVENTORY) {
		int add_size = 8;
		char item[MAX_ITEM_NAME_LENGTH+add_size];
		for(int i = 0; i < INV_SIZE; i++) {
			if(player->inventory[i].stack == 0) continue;
			if(i == player->action_bar.inv_selector) {
				snprintf(item, MAX_ITEM_NAME_LENGTH+add_size, ">>%s x%d", player->inventory[i].name, player->inventory[i].stack);
			} else {
				snprintf(item, MAX_ITEM_NAME_LENGTH+add_size, "%s x%d", player->inventory[i].name, player->inventory[i].stack);
			}
			mvwprintw(action_bar, i, 0, item);
		}
		// TODO display player's inventory
	} else if(player->action_bar.selector == SPELLS) {
		// TODO display player's spells
	} else if(player->action_bar.selector == LOOT) { 
		for(int i = 0; i < MAX_ITEMS_PER_TILE; i++) {
			item_t *item = room->tiles[player->y][player->x]->items[i];
		}
		// TODO
	} else {
		char inv[12] = "inventory";
		char spells[9] = "spells";
		switch(player->action_bar.selector) {
			case INVENTORY:
				snprintf(inv, 12, "%s", ">>inventory");
				break;
			case SPELLS:
				snprintf(spells, 9, "%s", ">>spells");
				break;
			case NOT_OPEN:
				break;
		}
		wprintw(action_bar, inv);
		mvwprintw(action_bar, 1, 0, spells);
	}
	
}

void hud_update_messages(world_t *world, player_t *player) {
	int start_y = PLAYER_STATS_HUD_SPACE+ENEMY_STATS_HUD_SPACE+MESSAGE_HUD_SPACE-1;
	wmove(hud, start_y, 0);
	int printed_messages = 0;
	for(int i = world->messages_size-1; i > -1; i--) {
		if(printed_messages == MESSAGE_HUD_SPACE) return;
		if(world->messages[i] != NULL) {
			int y, x;
			getyx(hud, y, x);
			wmove(hud, y-1, 0);
			char *clear = calloc(HUD_LENGTH, sizeof(char));
			wprintw(hud, clear);
			wmove(hud, y-1, 0);
			wprintw(hud, world->messages[i]);
		} else {
			DEBUG_LOG("hud.c: hud_update_messages: world->messages[i] is null!");
		}
		printed_messages++;
	}
}

void hud_update_all(world_t *world, player_t *player) {
	// TODO
}

//TODO this is a testing function not intended for anything else!
void display_error_message(const char *str) {
	wmove(hud, PLAYER_STATS_HUD_SPACE, 0);
	char *message = malloc(128);
	snprintf(message, 128, "%s", str);
	wprintw(hud, message);
}

void display_inventory_hud(world_t *world, player_t *player) {
	room_t *room = world->room[player->global_x][player->global_y];
	box(inventory_hud, 0, 0);
	char inv_str[32] = "Inventory";
	int middle = (SCREEN_WIDTH/2)-1;
	mvwprintw(inventory_hud, 0, (middle-strlen(inv_str))/2, inv_str);
	char loot_str[32] = "Nearby Loot";
	mvwprintw(inventory_hud, 0, INVENTORY_WIDTH-(INVENTORY_WIDTH/4)-(strlen(loot_str))/2, loot_str);
	for (int y = 0; y < INVENTORY_HEIGHT; y++) {
		mvwaddch(inventory_hud, y, middle, '|');  // vertical divider at col 39
	}
	if(player->action_bar.selector == INVENTORY || player->action_bar.selector == LOOT) {
		int add_size = 8;
		char item_name[MAX_ITEM_NAME_LENGTH+add_size];
		int visible_item_count = INVENTORY_HEIGHT-2;
		int pos = 1;
		int loot_pos = 1;

		int start = player->action_bar.loot_offset;
		int end = visible_item_count+player->action_bar.loot_offset;
		int max = player->nearby_loot_count;
		for(int i = start; i < end && i < max; i++) {
			item_t *item = player->nearby_loot[i];
			if(item == NULL || item->stack == 0) continue;
			if(i == player->action_bar.loot_selector) {
				if(player->action_bar.selector == LOOT) {
					snprintf(item_name, MAX_ITEM_NAME_LENGTH+add_size, ">>%s x%d", item->name, item->stack);
				} else {
					snprintf(item_name, MAX_ITEM_NAME_LENGTH+add_size, ">%s x%d", item->name, item->stack);
				}
			} else {
				snprintf(item_name, MAX_ITEM_NAME_LENGTH+add_size, "%s x%d", item->name, item->stack);
			}
			mvwprintw(inventory_hud, loot_pos++, INVENTORY_WIDTH/2, item_name);
		}
		
		for(int i = player->inv_offset; i < visible_item_count+player->inv_offset; i++) {
			if(player->inventory[i].stack == 0) continue;
			if(i == player->action_bar.inv_selector) {
				if(player->action_bar.selector == INVENTORY) {
					snprintf(item_name, MAX_ITEM_NAME_LENGTH+add_size, ">>%s x%d", player->inventory[i].name, player->inventory[i].stack);
				} else {
					snprintf(item_name, MAX_ITEM_NAME_LENGTH+add_size, ">%s x%d", player->inventory[i].name, player->inventory[i].stack);
				}
			} else {
				snprintf(item_name, MAX_ITEM_NAME_LENGTH+add_size, "%s x%d", player->inventory[i].name, player->inventory[i].stack);
			}
			mvwprintw(inventory_hud, pos++, 1, item_name);
		}
		// TODO display player's inventory
	} else if(player->action_bar.selector == SPELLS) {
		// TODO display player's spells
	} else {
		werase(inventory_hud); // TODO this is unoptimal we should check for this case first and not draw if we don't need to
		char inv[12] = "inventory";
		char spells[9] = "spells";
		switch(player->action_bar.selector) {
			case INVENTORY:
				snprintf(inv, 12, "%s", ">>inventory");
				break;
			case SPELLS:
				snprintf(spells, 9, "%s", ">>spells");
				break;
			case NOT_OPEN:
				break;
		}
		mvwprintw(inventory_hud, 1, 1, "%s", inv);
		mvwprintw(inventory_hud, 2, 1, "%s", spells);
	}
}
void display_inventory_desc_hud(world_t *world, player_t *player) {
	if(player->action_bar.selector == NOT_OPEN || player->action_bar.selector == SPELLS) {
		werase(inventory_desc_hud);
		return;
	}
	box(inventory_desc_hud, 0, 0);
	int visible_item_count = INVENTORY_HEIGHT-2;
	char desc_title_inv[32] = "Description";
	mvwprintw(inventory_desc_hud, 0, (39-strlen(desc_title_inv))/2, desc_title_inv);
	mvwprintw(inventory_desc_hud, 0, INVENTORY_WIDTH-(INVENTORY_WIDTH/4)-(strlen(desc_title_inv))/2, desc_title_inv);
	for (int y = 0; y < INVENTORY_HEIGHT; y++) {
		mvwaddch(inventory_desc_hud, y, 39, '|');
	}
	for(int i = player->inv_offset; i < visible_item_count+player->inv_offset; i++) {
		if(i == player->action_bar.inv_selector) {
			mvwprintw(inventory_desc_hud, 1, 1,  "%s", player->inventory[i].desc);
		}
	}
	
	room_t *room = world->room[player->global_x][player->global_y];
	int start = player->action_bar.loot_offset;
	int end = visible_item_count+player->action_bar.loot_offset;
	int max = player->nearby_loot_count;
	for(int i = start; i < end && i < max; i++) {
		item_t *item = player->nearby_loot[i];
		if(item == NULL || item->stack == 0) continue;
		if(i == player->action_bar.loot_selector) {
			mvwprintw(inventory_desc_hud, 1, INVENTORY_WIDTH/2, "%s", item->desc);
		}
	}
	
	for(int i = player->inv_offset; i < visible_item_count+player->inv_offset; i++) {
		if(i == player->action_bar.inv_selector) {
			mvwprintw(inventory_desc_hud, 1, 1,  "%s", player->inventory[i].desc);
		}
	}
}
