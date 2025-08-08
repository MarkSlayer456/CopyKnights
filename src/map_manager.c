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



// should just use a tile set
// the rooms are premade, but just randomly picked
// this was I can ensure the map doesn't break

room_t setup_first_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data) {
	return load_room(seed, x, y, enemy_data);
}

room_t generate_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data) {
	return load_room(seed, x, y, enemy_data);
}
// seed is the seed to use, probably will be random most of the time
// dir is the direction the player entered from 
/*map_t *generate_room(unsigned int seed, int dir)
{
   	curWall = '-'
   	srand(seed);
   	int height = (rand(25)+3)%25;
   	int width = (rand(25)+3)%25;
   	for(int i = 0; i < height; i++) {
   		for(int j = 0; j < width; j++) {
   			if(i == 0 || j == 0 || i == height-1 || i == width-1) { // an edge
   				curWall = WALL;
   			}
   		}
   	}
}*/
   
// name is a file name
room_t load_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data)
{
	int room_num = 0; 
	unsigned int enemy_seed = (x*ENEMY_X_PRIME) + (y*ENEMY_Y_PRIME);
	DEBUG_LOG("room_num: %d", room_num);
	DEBUG_LOG("(x,y): (%d,%d)", x, y);
	room_t room;
	room.is_created = true;
	room.layout = calloc(ROOM_HEIGHT, sizeof(char *));
	room.has_light = calloc(ROOM_HEIGHT, sizeof(bool *));
	for(int i = 0; i < ROOM_HEIGHT; i++) {
		room.layout[i] = calloc(ROOM_WIDTH+1, sizeof(char));
		room.has_light[i] = calloc(ROOM_WIDTH, sizeof(bool));
	}
	room.enemies = calloc(MAX_ENEMIES_PER_LEVEL, sizeof(enemy_t *));
	room.current_enemy_count = 0;
	int depth = (int) sqrt(x*x + y*y);
	char folder[32] = "";
	char *file = calloc(128, sizeof(char));
	strcat(file, "./data/rooms/");
	if(depth <= 10) {
		room.biome = CAVE;
		strcat(folder, "cave");
		room_num = (rand_r(seed) % CAVE_ROOM_COUNT)+1;
	} else if(depth <= 20) {
		room.biome = BOG;
		strcat(folder, "bog");
		room_num = (rand_r(seed) % BOG_ROOM_COUNT)+1;
	} else if(depth <= 30) {
		room.biome = CATACOMBS;
		strcat(folder, "catacombs");
		room_num = (rand_r(seed) % CATACOMBS_ROOM_COUNT)+1;
	} else if(depth <= 40) {
		room.biome = ANCIENT_CITY;
		strcat(folder, "ancient_city");
		room_num = (rand_r(seed) % ANCIENT_CITY_ROOM_COUNT)+1;
	} else if(depth <= 50) {
		room.biome = ARCANE_LABYRINTH;
		strcat(folder, "arcane_labyrinth");
		room_num = (rand_r(seed) % ARCANE_LABYRINTH_ROOM_COUNT)+1;
	} else if(depth <= 60) {
		room.biome = VOID_HOLLOW;
		strcat(folder, "void_hollow");
		room_num = (rand_r(seed) % VOID_HOLLOW_ROOM_COUNT)+1;
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
		strcpy(room.layout[i], tok);
		for(int j = 0; j < strlen(room.layout[i]); j++) {
			//TODO make this a chance not a 100%
			switch(room.layout[i][j]) {
				case POTENTIAL_ENEMY_SPAWN_CHAR:
					room.enemies[room.current_enemy_count] = enemy_spawn(enemy_generate_type(&enemy_seed, enemy_data, room.biome), enemy_data, j, i);
					room.layout[i][j] = EMPTY;
					room.current_enemy_count++;
					break;
				case POTENTIAL_CHEST_SPAWN_CHAR:
					break;
				case POTENTIAL_TRAP_SPAWN_CHAR:
					break;
			}
		}
		tok = strtok(NULL, "\n");
		i++;
	}
	close(fd);

	return room;
}
