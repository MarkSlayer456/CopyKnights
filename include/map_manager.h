#ifndef MAP_MANAGER_H_
#define MAP_MANAGER_H_
#include "enemy.h"
#include <stdbool.h>

#define ROOM_COUNT			3
#define ROOM_WIDTH			20
#define ROOM_HEIGHT			20
#define ROOM_ENEMY_MAX		20

#define POTENTIAL_ENEMY_SPAWN_CHAR		'*'
#define POTENTIAL_CHEST_SPAWN_CHAR		'$'
#define POTENTIAL_TRAP_SPAWN_CHAR		'X'

typedef struct room room_t;

struct room {
	enemy_t **enemies;
	int current_enemy_count;
	char **layout; // what the floor looks like
	bool is_created; // TODO what is this?
	int global_time;
	bool **has_light; // if the tile has light/can be seen
};
//TODO these should return pointers not room_t this causes lots of copying that pointers don't

room_t generate_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data);

room_t load_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data);

room_t setup_first_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data);
#endif
