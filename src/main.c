#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include "map_manager.h"
#include "game_manager.h"
#include "player.h"
#include "items.h"
#include "enemy.h"

/*
	
	
	top down single player game where you start
	as a lonely knight and you slowly gain more
	knights that copy all your actions, probably
	have a max of like 16 or 8 (feel like 8 is to small)
	
	kinda turnbased, each time player moves the next "turn"
	happens so the ai makes a move after the player "reacts"
	
	
*/

WINDOW *hud; // gives player useful information
WINDOW *action_bar; // player's inventory/spells menu, maybe a help menu in the furture
WINDOW *error; // USED FOR ERROR CHECKING ONLY
char walk_chars[WALK_CHAR_LENGTH] = {EMPTY, 0, DOOR}; // characters entites can walk on


int main(int argc, char *argv[]) {
    WINDOW *win;

    initscr();
    win = newwin(25, 25, 0, 0);
    hud = newwin(50, 50, 0, 30);
	action_bar = newwin(5, 50, 30, 0);
    error = newwin(25, 25, 51, 30);
    
    refresh();
    //wmove(hud, 0, 0);
    wrefresh(hud);
    wrefresh(error);
    
    noecho();
    nodelay(stdscr, FALSE);
    keypad(stdscr, FALSE);
    scrollok(win, FALSE);
    raw();
	if(argc == 1) {
		//game starts
	} else {
		fprintf(stderr, "No arguments required!\n");
	}


	world_t *world = malloc(sizeof(world_t));
	world->isPlayerTurn = 1;
	world->enemy_data = calloc(MAX_ENEMIES, sizeof(enemy_data_t *));
	load_enemy_data(world->enemy_data);
	
	player_t *player = malloc(sizeof(player_t));
	player->x = malloc(MAX_KNIGHTS * sizeof(int));
	player->y = malloc(MAX_KNIGHTS * sizeof(int));
	player->health = malloc(MAX_KNIGHTS * sizeof(int));
	player->max_health = malloc(MAX_KNIGHTS * sizeof(int));
	player->attack = malloc(MAX_KNIGHTS * sizeof(int));
	player->defense = malloc(MAX_KNIGHTS * sizeof(int));
	player->global_x = 0;
	player->global_y = 0;
	
	player->inventory = malloc(INV_SIZE * sizeof(item_t));
	
	item_t blank = {BLANK_NAME, BLANK, 0};
	
	for(int i = 0; i < INV_SIZE; i++) {
		player->inventory[i] = blank;
	}
	
	item_t test_item1 = {HEALTH_POTION_NAME, HEALTH_POTION, 16};
	item_t test_item2 = {APPLE_NAME, APPLE, 3};
	item_t test_item3 = {TELEPORT_SCROLL_NAME, TELEPORT_SCROLL, 1};
	
	player->inventory[0] = test_item1;
	
	player->inventory[1] = test_item2;
	
	player->inventory[2] = test_item3;
	
	//TODO should set the others to a negative number
	for(int i = 0; i < MAX_KNIGHTS; i++) {
		player->x[i] = -1000;
		player->y[i] = -1000;
		player->health[i] = 0;
		player->max_health[i] = 0;
		player->attack[i] = 0;
		player->defense[i] = 0;
		player->knights++;
	}
	action_bar_t action = {0, 0, NOT_OPEN, 0, 0, ITEM};
	player->action_bar = action;
	player->health[0] = 1000;
	player->max_health[0] = 1000;
	player->x[0] = 1;
	player->y[0] = 10;
	player->global_x = 0;
	player->global_y = 0;
	player->attack[0] = 25;
	player->defense[0] = 25;

	world->isPlayerTurn = 1;

	world->room = calloc(100, sizeof(room_t *));
	for(int i = 0; i < 100; i++) {
		world->room[i] = calloc(100, sizeof(room_t));
		for(int j = 0; j < 100; j++) {
			world->room[i][j].is_created = false;
		}
	}
	world->seed = TEST_SEED;
	room_t first = setup_first_room(&world->seed, 0, 0, world->enemy_data);
	first.enemies = calloc(MAX_ENEMIES_PER_LEVEL, sizeof(enemy_t *));
	first.enemies[0] = enemy_spawn(SLIME, world->enemy_data, 1, 1);
	first.enemies[0]->global_x = 0;
	first.enemies[0]->global_y = 0;
	
	first.current_enemy_count++;
	
	world->room[0][0] = first;
	world->win = win;
    for(;;) {
    	draw(world, player);
    	if(world->isPlayerTurn) {
			char c = getch();
        	manage_input(c, world, player);
        } else {
        	//TODO remove this and actually do something
			for(int i = 0; i < world->room[player->global_x][player->global_y].current_enemy_count; i++) {
				enemy_decide_move(world->room[player->global_x][player->global_y].enemies[i], world, player);
			}
        	world->isPlayerTurn = 1;
        }
    }
    endwin();
    exit(0);
    return 0;
}
