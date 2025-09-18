#ifndef MAP_TYPES_H_
#define MAP_TYPES_H_

typedef struct room_template {
	char filename[128];
	unsigned int mask;
	biome_t biome;
} room_template_t;

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
	bool is_main_path; // is main path
	unsigned int door_mask;
} room_t;

#endif
