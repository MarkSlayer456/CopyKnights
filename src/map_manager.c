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
	int room_num = (rand_r(seed) % ROOM_COUNT)+1;
	unsigned int enemy_seed = (x*ENEMY_X_PRIME) + (y*ENEMY_Y_PRIME);
	DEBUG_LOG("room_num: %d", room_num);
	DEBUG_LOG("(x,y): (%d,%d)", x, y);
	room_t room;
	room.is_created = true;
	room.layout = calloc(20, sizeof(char *));
	for(int i = 0; i < 20; i++) {
		room.layout[i] = calloc(21, sizeof(char));
	}
	room.enemies = calloc(ROOM_ENEMY_MAX, sizeof(enemy_t *));
	room.current_enemy_count = 0;

	char *file = calloc(32, sizeof(char));
	strcat(file, "./rooms/");
	if(x == 0 && y == 0) {
		strcat(file, "startingroom.ck");
	} else {
		snprintf(file, 32, "./rooms/room%d.ck", room_num);
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
		strcpy(room.layout[i], tok);
		for(int j = 0; j < strlen(room.layout[i]); j++) {
			//TODO make this a chance not a 100%
			switch(room.layout[i][j]) {
				case POTENTIAL_ENEMY_SPAWN_CHAR:
					room.enemies[room.current_enemy_count] = enemy_spawn(enemy_generate_type(&enemy_seed), enemy_data, j, i);
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

	return room;
}
