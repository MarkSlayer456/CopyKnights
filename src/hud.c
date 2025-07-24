#include "hud.h"
#include "game_manager.h"
#include "items.h"
#include <stdlib.h>
#include <string.h>
extern WINDOW *hud;
extern WINDOW *action_bar;

void hud_update_player_health(player_t *player)
{
	wmove(hud, 0, 0);
	wprintw(hud, "Player Status");
	int y = 1;
	int x = 0;
	wmove(hud, y, x);
	waddch(hud, PLAYER);
	waddch(hud, ':');
	waddch(hud, ' ');
	
	char *str = malloc(8);
	snprintf(str, 8, "%d", player->health[0]);
	wprintw(hud, str);
	
	waddch(hud, ' ');
	waddch(hud, '/');
	waddch(hud, ' ');
	
	char *str2 = malloc(8);
	snprintf(str2, 8, "%d", player->max_health[0]);
	wprintw(hud, str2);
}

void hud_update_nearby_enemies(world_t *world, player_t *player)
{
	int detect_radius = 6;
    wmove(hud, Y_PLAYER_HEALTH_OFFSET, 0);
	room_t *room = &world->room[player->global_x][player->global_y];
	for(int i = 0; i < MAX_ENEMIES_PER_LEVEL; i++) {
		if(!room->enemies[i]) break;
		if(room->enemies[i]->x > player->x[0]-detect_radius && room->enemies[i]->x < player->x[0]+detect_radius &&
			room->enemies[i]->y > player->y[0]-(detect_radius) && room->enemies[i]->y < player->y[0]+(detect_radius)) {
			wmove(hud, Y_PLAYER_HEALTH_OFFSET+i+1, 0);
			char *name = malloc(32);
			snprintf(name, 32, "%s", room->enemies[i]->name);
			wprintw(hud, name);
			mvwaddch(hud, Y_PLAYER_HEALTH_OFFSET+i+1, strlen(name), ':');
			char *str = malloc(8);
			wmove(hud, Y_PLAYER_HEALTH_OFFSET+i+1, strlen(name)+1);
			snprintf(str, 8, "%d", room->enemies[i]->health);
			wprintw(hud, str);
		}
	}
        // this is probably gonna have to be very complex.. or not just check if diff in x and y is less than a given number?
        // create a cords system so you can give there location so players know what they're looking at
}

void hud_update_action_bar(player_t *player)
{
	wclear(action_bar);
	wmove(action_bar, 0, 0);
	if(player->action_bar.inv_open) {
		int add_size = 8;
		char item[MAX_ITEM_NAME_LENGTH+add_size];
		for(int i = 0; i < INV_SIZE; i++) {
			if(player->inventory[i].stack == 0) continue;
			if(i == player->action_bar.inv_selector && player->action_bar.inv_open) {
				snprintf(item, MAX_ITEM_NAME_LENGTH+add_size, ">>%s x%d\0", player->inventory[i].name, player->inventory[i].stack);
			} else {
				snprintf(item, MAX_ITEM_NAME_LENGTH+add_size, "%s x%d\0", player->inventory[i].name, player->inventory[i].stack);
			}
			mvwprintw(action_bar, i, 0, item);
		}
		// TODO display player's inventory
	} else if(player->action_bar.spells_open) {
		// TODO display player's spells
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

void hud_update_all(world_t *world, player_t *player)
{
	// TODO
}

//TODO this is a testing function not intended for anything else!
void display_error_message(const char *str) 
{
	wmove(hud, Y_PLAYER_HEALTH_OFFSET, 0);
	char *message = malloc(128);
	snprintf(message, 128, "%s", str);
	wprintw(hud, message);
}

