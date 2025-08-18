#ifndef MAP_MANAGER_H_
#define MAP_MANAGER_H_

#include "types.h"
#include <stdbool.h>

typedef struct item item_t;
typedef enum item_ids item_ids_t;

typedef struct tile {
	char floor; // what the floor tile is
	bool has_light; // does this tile have light
	item_t *items[MAX_ITEMS_PER_TILE]; // list of items on this tile
} tile_t;

typedef struct room {
	tile_t *tiles[ROOM_WIDTH][ROOM_HEIGHT];
	enemy_t *enemies[MAX_ENEMIES_PER_LEVEL]; 
	int current_enemy_count;
	bool is_created; // TODO what is this?
	int global_time;
	biome_t biome;
} room_t;
//TODO these should return pointers not room_t this causes lots of copying that pointers don't

room_t *generate_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data);

room_t *load_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data);

room_t *setup_first_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data);
#endif
