#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "map_manager.h"
#include "game_manager.h"
#include "player.h"
#include "items/items.h"
#include "enemy.h"
#include "hud.h"
#include "types.h"
#include "lantern.h"
#include "items/armor.h"
#include "items/weapons.h"

/*
	Top down dungeon crawler
	oil is the currency here
	your lamp is powered by oil
	oil is your friend without you won't last long
*/

WINDOW *hud; // gives player useful information
WINDOW *action_bar; // OLG player's inventory/spells menu, maybe a help menu in the future
WINDOW *error; // USED FOR ERROR CHECKING ONLY
WINDOW *inventory_hud; // player's inventory
WINDOW *inventory_desc_hud; // player's description in the inventory
char walk_chars[WALK_CHAR_LENGTH] = {EMPTY, 0, DOOR, MUD, CHEST}; // characters entites can walk on

int main(int argc, char *argv[]) {
    WINDOW *win;
	
    initscr();
    curs_set(0);
	win = newwin(20, 20, 0, 0);
	box(win,0,0);
    hud = newwin(HUD_HEIGHT, HUD_WIDTH, 0, 21);
	action_bar = newwin(5, 21, 21, 0);
	inventory_hud = newwin(INVENTORY_HEIGHT, INVENTORY_WIDTH, 0, 0);
	inventory_desc_hud = newwin(INVENTORY_HEIGHT, INVENTORY_WIDTH, INVENTORY_HEIGHT, 0);
    error = newwin(25, 25, 51, 30);
    
    refresh();
    wrefresh(hud);
    wrefresh(error);
    
	noecho();
	nodelay(stdscr, FALSE);
	keypad(stdscr, FALSE);
	scrollok(win, FALSE);
	raw();
	
	if(argc != 1) {
		fprintf(stderr, "No arguments required!\n");
	}

	world_t *world = malloc(sizeof(world_t));
	world->isPlayerTurn = 1;
	
	world->enemy_data = calloc(MAX_ENEMIES, sizeof(enemy_data_t));
	for(int row = 0; row < MAX_ENEMIES; row++) {
		for(int i = 0; i < NUMBER_OF_BIOMES; i++) {
			world->enemy_data[row].can_spawn[i] = false;
			world->enemy_data[row].lowest_level[i] = -1;
			world->enemy_data[row].highest_level[i]= -1;
		}
	}
	
	world->class_data = calloc(MAX_CLASSES, sizeof(class_data_t));
	world->item_data = calloc(MAX_ITEMS, sizeof(item_data_t));
	for(int i = 0; i < MAX_ITEMS; i++) {
		world->item_data[i] = (item_data_t){0};
	}
	
	world->max_message_storage = DEFAULT_MAX_MESSAGE_STORAGE;
	world->messages_size = 0;
	world->messages = calloc(DEFAULT_MAX_MESSAGE_STORAGE, sizeof(char *));
	for(int i = 0; i < DEFAULT_MAX_MESSAGE_STORAGE; i++) {
		world->messages[i] = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
	}
	
	load_enemy_data(world->enemy_data);
	load_class_data(world->class_data);
	load_armor_data(world);
	load_weapon_data(world);
	
	player_t *player = malloc(sizeof(player_t));
	
	action_bar_t action = {
		.selector = NOT_OPEN,
		.spells_selector = 0,
		.inv_selector = 0,
		.loot_selector = 0,
		.loot_offset = 0,
		.cat = ITEM
	};
	
	player->action_bar = action;
	player->x = 1;
	player->y = 10;
	player->player_class = SWORDSMAN;
	for(int i = 0; i < MAX_CLASSES; i++) {
		if(world->class_data[i].type == player->player_class) {
			int base_strength = world->class_data[i].base_strength;
			int base_dexterity = world->class_data[i].base_dexterity;
			int base_intelligence = world->class_data[i].base_intelligence;
			int base_constitution = world->class_data[i].base_constitution;
			int base_speed = world->class_data[i].base_speed;
			
			player->strength = base_strength;
			player->dexterity = base_dexterity;
			player->intelligence = base_intelligence;
			player->constitution = base_constitution;
			player->speed = base_speed;
			break;
		}
	}
	player->health = player->constitution * 100;
	player->max_health = player->constitution * 100;
	player->global_x = 0;
	player->global_y = 0;
	player->action_points = 0;
	player->level = 1;
	player->xp = 0;
	player->inv_offset = 0;
	player->oil = STARTING_OIL;
	
	player->inventory = malloc(INV_SIZE * sizeof(item_t));
	player->inventory_count = 0;
	
	item_t blank = {BLANK_NAME, "does nothing", BLANK, 0};
	item_t test_item1 = {HEALTH_POTION_NAME, "heals player", HEALTH_POTION, 5};
	item_t test_item2 = {APPLE_NAME, "heals player", APPLE, 3};
	item_t test_item3 = {TELEPORT_SCROLL_NAME, "teleports player", TELEPORT_SCROLL, 1};
	
	for(int i = 0; i < INV_SIZE; i++) {
		player->inventory[i] = blank;
	}
	player_add_to_inv(player, test_item1);
	player_add_to_inv(player, test_item2);
	player_add_to_inv(player, test_item3);
	
	player->lantern.power = 5;
	player->lantern.is_on = true;
	player->lantern.turns_since_last_dim = 0;

	world->isPlayerTurn = 1;

	world->turn_order = calloc(MAX_ENEMIES_PER_LEVEL+1, sizeof(int));
	world->turn_order_size = 0;
	for(int y = 0; y < WORLD_HEIGHT; y++) {
		for(int x = 0; x < WORLD_WIDTH; x++) {
			world->room[y][x] = calloc(1, sizeof(room_t));
		}
	}
	world->seed = TEST_SEED;
	room_t *first = setup_first_room(&world->seed, 0, 0, world->enemy_data, world->item_data);
	first->enemies[0] = enemy_spawn(BAT, world->enemy_data, 1, 1);
	
	first->current_enemy_count++;
	
	first->tiles[1][1]->items[0] = calloc(1, sizeof(item_t));
	first->tiles[1][1]->items[1] = calloc(1, sizeof(item_t));
	strcpy(first->tiles[1][1]->items[0]->name, APPLE_NAME);
	first->tiles[1][1]->items[0]->stack = 1;
	first->tiles[1][1]->items[0]->id = APPLE;
	strcpy(first->tiles[1][1]->items[1]->name, APPLE_NAME);
	first->tiles[1][1]->items[1]->stack = 1;
	first->tiles[1][1]->items[1]->id = APPLE;
	
	first->tiles[10][1]->items[0] = calloc(1, sizeof(item_t));
	item_t *item = first->tiles[10][1]->items[0];
	strcpy(item->name, world->item_data[0].name);
	item->stack = 1;
	item->id = world->item_data[0].id;
	strcpy(item->desc, "+4 armor");
	item->value_type = world->item_data[0].value_type;
	item->stat_type.armor.type = world->item_data[0].stat_type.armor.type;
	item->stat_type.armor.defense = world->item_data[0].stat_type.armor.defense;
	
	world->room[0][0] = first;
	
	world->win = win;
    world->turn_order_size = 0;
	for(;;) {
		calculate_light(world, player);
		generate_turn_order_display(world, player);
    	draw(world, player);
		player_get_nearby_loot(world->room[player->global_x][player->global_y], player);
		
		int actor = pick_next_actor(world, player);
		assert(actor != INVALID_ACTOR_INDEX);
		if(actor == PLAYER_TURN_ORDER_INDEX) {
			lantern_update_dimming(&player->lantern);
			bool run = false;
			char c;
			while(run == false) {
				c = getch();
				run = manage_input(c, world, player);
				draw(world, player);
			}
		} else if(actor >= 0) {
			enemy_t *enemy = world->room[player->global_x][player->global_y]->enemies[actor];
			if(enemy != NULL) {
				enemy_decide_move(enemy, world, player);
			}
		}
    }
    endwin();
    exit(0);
    return 0;
}
