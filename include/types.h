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
// EXAMPLE: DEBUG_LOG("Constitution: %d", enemy_data[row].constitution);

/* a world is really a floor
 * it has the whole room inside of it
 * each room, and each room's layout because the rooms all have a layout
 */
typedef struct world {
	WINDOW *win;
	room_t *room[WORLD_HEIGHT][WORLD_WIDTH];
	int isPlayerTurn;
	unsigned int seed;
	enemy_data_t *enemy_data;
	class_data_t *class_data;
	item_data_t *item_data;
	int item_data_count;
	char **messages;
	int max_message_storage;
	int messages_size;
	int *turn_order; // -1 for player, index for enemies
	int turn_order_size;
} world_t;

#endif
