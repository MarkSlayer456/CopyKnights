#ifndef MAP_TYPES_H_
#define MAP_TYPES_H_

#include <stdint.h>

typedef struct room_template {
	char filename[128];
	uint8_t mask;
	biome_t biome;
} room_template_t;

typedef struct tile {
	char floor; // what the floor tile is
	bool has_light; // does this tile have light
	int8_t item_count; // number of items on this tile
	item_t *items[MAX_ITEMS_PER_TILE]; // list of items on this tile
} tile_t;

typedef struct room {
	char room_file_name[ROOM_FILE_NAME_MAX_SIZE];
	tile_t *tiles[ROOM_WIDTH][ROOM_HEIGHT];
	int8_t current_enemy_count;
	enemy_t *enemies[MAX_ENEMIES_PER_LEVEL]; 
	bool is_created;
	int global_time;
	biome_t biome;
	bool is_main_path; // is main path
	uint8_t door_mask;
} room_t;

#endif
