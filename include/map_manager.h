#ifndef MAP_MANAGER_H_
#define MAP_MANAGER_H_

#define CAVE_ROOM_COUNT						4
#define BOG_ROOM_COUNT						1
#define CATACOMBS_ROOM_COUNT				1
#define ANCIENT_CITY_ROOM_COUNT				1
#define ARCANE_LABYRINTH_ROOM_COUNT			1
#define VOID_HOLLOW_ROOM_COUNT				1
#define ROOM_WIDTH							20
#define ROOM_HEIGHT							20
#define MAX_ENEMIES_PER_LEVEL				12
#define NUMBER_OF_BIOMES					6

#define CAVE_NAME							"Cave"
#define BOG_NAME							"Bog"
#define CATACOMBS_NAME						"Catacombs"
#define ANCIENT_CITY_NAME					"Ancient City"
#define ARCANE_LABYRINTH_NAME				"Arcane Labyrinth"
#define VOID_HOLLOW_NAME					"Void Hollow"

#define POTENTIAL_ENEMY_SPAWN_CHAR			'*'
#define POTENTIAL_CHEST_SPAWN_CHAR			'$'
#define POTENTIAL_TRAP_SPAWN_CHAR			'X'

#include "enemy.h"
#include <stdbool.h>

typedef enum biome {
	BIOME_NULL = -1,
	CAVE,
	BOG,
	CATACOMBS,
	ANCIENT_CITY,
	ARCANE_LABYRINTH,
	VOID_HOLLOW
} biome_t;

typedef struct room room_t;

struct room {
	enemy_t **enemies;
	int current_enemy_count;
	char **layout; // what the floor looks like
	bool is_created; // TODO what is this?
	int global_time;
	bool **has_light; // if the tile has light/can be seen
	biome_t biome;
};
//TODO these should return pointers not room_t this causes lots of copying that pointers don't

room_t generate_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data);

room_t load_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data);

room_t setup_first_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data);
#endif
