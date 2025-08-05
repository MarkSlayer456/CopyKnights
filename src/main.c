#include <stdio.h>
#include <ncurses.h>
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
#include "items.h"
#include "enemy.h"
#include "hud.h"

/*
	Top down dungeon crawler
*/

WINDOW *hud; // gives player useful information
WINDOW *action_bar; // player's inventory/spells menu, maybe a help menu in the furture
WINDOW *error; // USED FOR ERROR CHECKING ONLY
char walk_chars[WALK_CHAR_LENGTH] = {EMPTY, 0, DOOR}; // characters entites can walk on


int main(int argc, char *argv[]) {
    WINDOW *win;
	
    initscr();
    curs_set(0);
	win = newwin(20, 20, 0, 0);
    hud = newwin(HUD_HEIGHT, HUD_WIDTH, 0, 21);
	action_bar = newwin(5, 21, 21, 0);
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
	world->enemy_data = calloc(MAX_ENEMIES, sizeof(enemy_data_t *));
	world->class_data = calloc(MAX_CLASSES, sizeof(enemy_data_t *));
	
	world->max_message_storage = DEFAULT_MAX_MESSAGE_STORAGE;
	world->messages_size = 0;
	world->messages = calloc(DEFAULT_MAX_MESSAGE_STORAGE, sizeof(char *));
	for(int i = 0; i < DEFAULT_MAX_MESSAGE_STORAGE; i++) {
		world->messages[i] = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
	}
	
	load_enemy_data(world->enemy_data);
	load_class_data(world->class_data);
	
	player_t *player = malloc(sizeof(player_t));
	
	action_bar_t action = {0, 0, NOT_OPEN, 0, 0, ITEM};
	
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
	player->health = player->constitution * 10;
	player->max_health = player->constitution * 10;
	player->global_x = 0;
	player->global_y = 0;
	player->action_points = 0;
	player->level = 1;
	player->xp = 0;
	
	player->inventory = malloc(INV_SIZE * sizeof(item_t));
	
	item_t blank = {BLANK_NAME, BLANK, 0};
	item_t test_item1 = {HEALTH_POTION_NAME, HEALTH_POTION, 5};
	item_t test_item2 = {APPLE_NAME, APPLE, 3};
	item_t test_item3 = {TELEPORT_SCROLL_NAME, TELEPORT_SCROLL, 1};
	
	for(int i = 0; i < INV_SIZE; i++) {
		player->inventory[i] = blank;
	}
	
	player->inventory[0] = test_item1;
	
	player->inventory[1] = test_item2;
	
	player->inventory[2] = test_item3;

	world->isPlayerTurn = 1;

	world->turn_order = calloc(MAX_ENEMIES_PER_LEVEL+1, sizeof(int));
	world->turn_order_size = 0;
	world->room = calloc(100, sizeof(room_t *));
	for(int i = 0; i < 100; i++) {
		world->room[i] = calloc(100, sizeof(room_t));
		for(int j = 0; j < 100; j++) {
			world->room[i][j].is_created = false; // TODO what is this?
		}
	}
	world->seed = TEST_SEED;
	room_t first = setup_first_room(&world->seed, 0, 0, world->enemy_data);
	first.enemies = calloc(MAX_ENEMIES_PER_LEVEL, sizeof(enemy_t *));
	first.enemies[0] = enemy_spawn(SLIME, world->enemy_data, 1, 1);
	
	first.current_enemy_count++;
	
	world->room[0][0] = first;
	
	world->win = win;
    world->turn_order_size = 0;
	// memset(world->turn_order, 0, (MAX_ENEMIES_PER_LEVEL+1) * sizeof(int));
	for(;;) {
		calculate_light(world, player);
    	draw(world, player);
		
		assert(world->turn_order_size >= 0);
		while(world->turn_order_size < MAX_ENEMIES_PER_LEVEL) {
			int actor = pick_next_actor(world, player);
			assert(actor != INVALID_ACTOR_INDEX);
			world->turn_order[world->turn_order_size++] = actor;
		}
		
		int turn_index = world->turn_order[0];
		assert(turn_index >= PLAYER_TURN_ORDER_INDEX);
		if(turn_index == PLAYER_TURN_ORDER_INDEX) {
			display_combat_message(world, player, MESSAGE_IS_PLAYERS_TURN);
			char c = getch();
			manage_input(c, world, player);
		} else {
			enemy_t *enemy = world->room[player->global_x][player->global_y].enemies[turn_index];
			if(enemy == NULL) {
				for(int k = 0; k < world->turn_order_size-1; k++) {
					world->turn_order[k] = world->turn_order[k + 1];
				}
				world->turn_order_size--;
				continue;
			}
			enemy_decide_move(enemy, world, player);
		}
		for(int k = 0; k < world->turn_order_size-1; k++) {
			world->turn_order[k] = world->turn_order[k + 1];
		}
		world->turn_order_size--;
    }
    endwin();
    exit(0);
    return 0;
}
