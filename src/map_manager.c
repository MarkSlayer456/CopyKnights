#include "map_manager.h"
#include "enemy.h"
#include "game_manager.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <assert.h>
#include "types.h"
#include "items/items.h"
#include "functions.h"

room_t *setup_first_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data, item_data_t *item_data) {
	return load_room(seed, x, y, enemy_data, item_data);
}

room_t *generate_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data, item_data_t *item_data) {
	return load_room(seed, x, y, enemy_data, item_data);
}
   
// name is a file name
room_t *load_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data, item_data_t *item_data)
{
	int room_num = 0; 
	unsigned int cantor = cantor_pair(x, y);
	unsigned int map_seed = cantor ^ (*seed);
	map_seed *= SEED_PRIMER;
	map_seed ^= (*seed) >> 15;
	DEBUG_LOG("room_num: %d", room_num);
	DEBUG_LOG("(x,y): (%d,%d)", x, y);
	room_t *room = calloc(1, sizeof(room_t));
	room->is_created = true;
	for(int y = 0; y < ROOM_HEIGHT; y++) {
		for(int x = 0; x < ROOM_WIDTH; x++) {
			room->tiles[y][x] = calloc(1, sizeof(tile_t));
			for(int i = 0; i < MAX_ITEMS_PER_TILE; i++) {
				room->tiles[y][x]->items[i] = calloc(1, sizeof(item_t));
			}
		}
	}
	room->current_enemy_count = 0;
	int depth = (int) sqrt(x*x + y*y);
	char folder[32] = "";
	char *file = calloc(128, sizeof(char));
	strcat(file, "./data/rooms/");
	if(depth <= 10) {
		room->biome = CAVE;
		strcat(folder, "cave");
		room_num = (rand_r(&map_seed) % CAVE_ROOM_COUNT)+1;
	} else if(depth <= 20) {
		room->biome = BOG;
		strcat(folder, "bog");
		room_num = (rand_r(&map_seed) % BOG_ROOM_COUNT)+1;
	} else if(depth <= 30) {
		room->biome = CATACOMBS;
		strcat(folder, "catacombs");
		room_num = (rand_r(&map_seed) % CATACOMBS_ROOM_COUNT)+1;
	} else if(depth <= 40) {
		room->biome = ANCIENT_CITY;
		strcat(folder, "ancient_city");
		room_num = (rand_r(&map_seed) % ANCIENT_CITY_ROOM_COUNT)+1;
	} else if(depth <= 50) {
		room->biome = ARCANE_LABYRINTH;
		strcat(folder, "arcane_labyrinth");
		room_num = (rand_r(&map_seed) % ARCANE_LABYRINTH_ROOM_COUNT)+1;
	} else if(depth <= 60) {
		room->biome = VOID_HOLLOW;
		strcat(folder, "void_hollow");
		room_num = (rand_r(&map_seed) % VOID_HOLLOW_ROOM_COUNT)+1;
	}
	assert(room_num > 0);

	if(x == 0 && y == 0) {
		strcat(file, "startingroom.ck");
	} else {
		snprintf(file, 64, "./data/rooms/%s/room%d.ck", folder, room_num);
	}

	int fd = open(file, O_RDONLY);
	if(fd < 0) {
		DEBUG_LOG("an error occured loading file %s\n", file);
		exit(0);
	}
	char *buf = calloc(512, sizeof(char));
	read(fd, buf, 512);
	int i = 0;

	char *tok = strtok(buf, "\n");
	while(tok != NULL) {
		DEBUG_LOG("tok = %s | i = %d", tok, i);
		for(int j = 0; j < strlen(tok); j++) {
			switch(tok[j]) {
				case POTENTIAL_ENEMY_SPAWN_CHAR:
					room->enemies[room->current_enemy_count] = enemy_spawn(enemy_generate_type(&map_seed, enemy_data, room->biome), enemy_data, j, i);
					room->tiles[i][j]->floor = EMPTY;
					room->current_enemy_count++;
					break;
				case POTENTIAL_CHEST_SPAWN_CHAR:
					break;
				case POTENTIAL_TRAP_SPAWN_CHAR:
					break;
				case POTENTIAL_ITEM_SPAWN_CHAR:
					item_spawn(item_generate_type(&map_seed, item_data, room->biome), room->biome, room->tiles[i][j], item_data);
					room->tiles[i][j]->floor = EMPTY;
					break;
				default:
					room->tiles[i][j]->floor = tok[j];
					break;
			}
		}
		tok = strtok(NULL, "\n");
		i++;
	}
	close(fd);

	return room;
}
