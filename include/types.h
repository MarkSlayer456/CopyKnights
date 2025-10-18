#ifndef TYPES_H_

#define TYPES_H_
#include <stdbool.h>
#include <ncurses.h>
#include "game_constants.h"
#include "items/item_types.h"
#include "character_types.h"
#include "map_types.h"

#define DEBUG_LOG(fmt, ...) \
do { \
	FILE *f = fopen("debug.log", "a"); \
	if (f) { \
		fprintf(f, fmt "\n", ##__VA_ARGS__); \
		fclose(f); \
	} \
} while (0)

/*
 * holds all world information, aside from the player
 */
typedef struct world {
	WINDOW *win;
	room_t *room[WORLD_HEIGHT][WORLD_WIDTH];
	unsigned int seed;
	enemy_data_t *enemy_data;
	class_data_t *class_data;
	item_data_t *item_data;
	int item_data_count;
	int trap_data_count;
	trap_data_t *trap_data;
	int messages_size;
	char **messages;
	int max_message_storage;
	int turn_order_size;
	int *turn_order; // -1 for player, index for enemies
	int room_template_count;
	room_template_t room_templates[128];
	bool is_player_turn; // this is used to save the player's turn, mostly for the save funcationality to work
	uint8_t buff_size;
	uint8_t buff_count;
	buff_t *buffs;
} world_t;

//TODO I want to redo this
typedef struct {
	enum menu current_menu;
	int cursor_pos;
	enum menu dests[MAX_MENUS]; // where each button takes the user
	int dests_count;
} menu_manager_t;

typedef struct {
	WINDOW *win;
	char (*filename)[SAVE_FILE_MAX_LEN];
	uint8_t filename_count; // how many save files
	uint8_t filename_size; // current size of filename
	uint8_t cursor_offset;
	uint8_t cursor_pos;
} load_menu_t;
#endif
