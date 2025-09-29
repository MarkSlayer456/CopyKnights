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

void calculate_main_path(unsigned int *seed, world_t *world) {
	int main_x = 1, main_y = 0; // current main path x and y cords
	unsigned int entrance_door = 0x0;
	unsigned int starting_entrance_door = 0x8;
	unsigned int exit_door = 0x0;
	while(main_x < 100 && main_y < 100) {
		room_t *room = world->room[main_x][main_y];
		// DEBUG_LOG("main_path: (%d, %d)", main_x, main_y);
		room->is_main_path = true;
		if(main_x == 1) {
			room->door_mask = starting_entrance_door;
		} else {
			room->door_mask = entrance_door;
		}
		// TODO fix this, looks nasty, just use a for loop and random % 2 instead of 100
		int random = rand_r_portable(seed) % 100;
		if(random <= 49) {
			if(main_x >= 99) {
				main_y++;
				entrance_door = 0x1;
				exit_door = 0x4;
			} else {
				main_x++;
				entrance_door = 0x8;
				exit_door = 0x2;
			}
		} else if(random >= 50) {
			if(main_y >= 99) {
				main_x++;
				entrance_door = 0x8;
				exit_door = 0x2;
			} else {
				main_y++;
				entrance_door = 0x1;
				exit_door = 0x4;
			}
		}
		int new_doors = rand_r_portable(seed) % 16;
		room->door_mask |= exit_door;
		room->door_mask |= new_doors;
	}
	for(int x = 0; x < WORLD_WIDTH; x++) {
		for(int y = 0; y < WORLD_HEIGHT; y++) {
			room_t *room = world->room[x][y];
			// these should always be valid, NULL would crash the program
			room_t *north = y > 0 ? world->room[x][y-1] : NULL;
			room_t *south = y < WORLD_HEIGHT-1 ? world->room[x][y+1] : NULL;
			room_t *east = x < WORLD_WIDTH-1 ? world->room[x+1][y] : NULL;
			room_t *west = x > 0 ? world->room[x-1][y] : NULL;
			if(!room->is_main_path) {
				// only the east and south haven't been checked yet, so we only create
				// doors on rooms that don't already have a mask
				// and doors to the north and west already have a mask
				// because of this loop
				if(north && (north->door_mask & 0x4)) {
					room->door_mask |= 0x1;
				}
				if(south && (south->door_mask & 0x1)) {
					room->door_mask |= 0x4;
				} else if(south && south->door_mask == 0x0) {
					int random = rand_r_portable(seed) % 2; // 50% chance
					if(random == 0) {
						room->door_mask |= 0x4;
					}
				}
				if(west && (west->door_mask & 0x2)) {
					room->door_mask |= 0x8;
				}
				if(east && (east->door_mask & 0x8)) {
					room->door_mask |= 0x2;
				} else if(east && east->door_mask == 0x0) {
					int random = rand_r_portable(seed) % 2; // 50% chance
					if(random == 0) {
						room->door_mask |= 0x2;
					}
				}
			}
		}
	}
}

void calculate_door_masks(world_t *world) {
	char *file = calloc(128, sizeof(char));
	bool door_found = false;
	const int TOTAL_ROOM_COUNT = (CAVE_ROOM_COUNT + BOG_ROOM_COUNT + CATACOMBS_ROOM_COUNT 
			+ ANCIENT_CITY_ROOM_COUNT + ARCANE_LABYRINTH_ROOM_COUNT + VOID_HOLLOW_ROOM_COUNT);
	for(int i = 1; i <= TOTAL_ROOM_COUNT; i++) {
		char folder[32] = "";
		int room_num = i;
		// TODO this is just wrong, only ever runs through the caves
		if(i <= CAVE_ROOM_COUNT) {
			strcat(folder, CAVE_NAME);
		} else if(i <= BOG_ROOM_COUNT) {
			room_num = room_num - CAVE_ROOM_COUNT;
			strcat(folder, BOG_NAME);
		} else if(i <= CATACOMBS_ROOM_COUNT) {
			room_num = room_num - CAVE_ROOM_COUNT - BOG_ROOM_COUNT;
			strcat(folder, CATACOMBS_NAME);
		} else if(i <= ANCIENT_CITY_ROOM_COUNT) {
			room_num = room_num - CAVE_ROOM_COUNT - BOG_ROOM_COUNT - CATACOMBS_ROOM_COUNT;
			strcat(folder, ANCIENT_CITY_NAME);
		} else if(i <= ARCANE_LABYRINTH_ROOM_COUNT) {
			room_num = room_num - CAVE_ROOM_COUNT - BOG_ROOM_COUNT - CATACOMBS_ROOM_COUNT
						- ANCIENT_CITY_ROOM_COUNT;
			strcat(folder, ARCANE_LABYRINTH_NAME);
		} else if(i <= VOID_HOLLOW_ROOM_COUNT) {
			room_num = room_num - CAVE_ROOM_COUNT - BOG_ROOM_COUNT - CATACOMBS_ROOM_COUNT
			- ANCIENT_CITY_ROOM_COUNT - ARCANE_LABYRINTH_ROOM_COUNT;
			strcat(folder, VOID_HOLLOW_NAME);
		}
		
		snprintf(file, 96, "./data/rooms/%s/room%d.ck.txt", folder, room_num);
		
		world->room_templates[world->room_template_count].biome = get_biome(folder);
		
		int fd = open(file, O_RDONLY);
		if(fd < 0) {
			DEBUG_LOG("failed to load file: %s", file);
			break;
		}
		
		char *buf = calloc(512, sizeof(char));
		read(fd, buf, 512);
		int y = 0;
		char *tok = strtok(buf, "\n");
		while(tok != NULL) {
			for(int x = 0; x < strlen(tok); x++) {
				switch(tok[x]) {
					case DOOR_SPAWN_CHAR:
						door_found = true;
						if(y == 0) {
							world->room_templates[world->room_template_count].mask |= 0x1;
						} else if(y >= ROOM_HEIGHT-1) {
							world->room_templates[world->room_template_count].mask |= 0x4;
						} else if(x == 0) {
							world->room_templates[world->room_template_count].mask |= 0x8;
						} else if(x >= ROOM_WIDTH-1) {
							world->room_templates[world->room_template_count].mask |= 0x2;
						} else {
							world->room_templates[world->room_template_count].mask |= 0x0;
							DEBUG_LOG("%s", "invalid door placement!");
						}
						break;
					default:
						break;
				}
			}
			
			tok = strtok(NULL, "\n");
			y++;
		}
		if(door_found) {
			door_found = false;
			strcpy(world->room_templates[world->room_template_count].filename, file);
			DEBUG_LOG("room found with mask: %d", world->room_templates[world->room_template_count].mask);
			world->room_template_count++;
		}
		close(fd);
		free(buf);
	}
	free(file);
	DEBUG_LOG("rooms found: %d", world->room_template_count);
}

room_t *setup_first_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data, item_data_t *item_data, world_t *world) {
	return load_room(seed, x, y, enemy_data, item_data, world);
}

room_t *generate_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data, item_data_t *item_data, world_t *world) {
	return load_room(seed, x, y, enemy_data, item_data, world);
}
   
// name is a file name
room_t *load_room(unsigned int *seed, int x, int y, enemy_data_t *enemy_data, item_data_t *item_data, world_t *world)
{
	room_t *room = world->room[x][y];
	// sync_doors(world, x, y);
	int room_num = 0; 
	unsigned int cantor = cantor_pair(x, y);
	unsigned int map_seed = cantor ^ (*seed);
	map_seed *= SEED_PRIMER;
	map_seed ^= (*seed) >> 15;
	DEBUG_LOG("room_num: %d", room_num);
	DEBUG_LOG("(x,y): (%d,%d)", x, y);
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
		strcat(folder, CAVE_NAME);
		room_num = (rand_r_portable(&map_seed) % CAVE_ROOM_COUNT)+1;
	} else if(depth <= 20) {
		room->biome = BOG;
		strcat(folder, BOG_NAME);
		room_num = (rand_r_portable(&map_seed) % BOG_ROOM_COUNT)+1;
	} else if(depth <= 30) {
		room->biome = CATACOMBS;
		strcat(folder, CATACOMBS_NAME);
		room_num = (rand_r_portable(&map_seed) % CATACOMBS_ROOM_COUNT)+1;
	} else if(depth <= 40) {
		room->biome = ANCIENT_CITY;
		strcat(folder, ANCIENT_CITY_NAME);
		room_num = (rand_r_portable(&map_seed) % ANCIENT_CITY_ROOM_COUNT)+1;
	} else if(depth <= 50) {
		room->biome = ARCANE_LABYRINTH;
		strcat(folder, ARCANE_LABYRINTH_NAME);
		room_num = (rand_r_portable(&map_seed) % ARCANE_LABYRINTH_ROOM_COUNT)+1;
	} else if(depth <= 60) {
		room->biome = VOID_HOLLOW;
		strcat(folder, VOID_HOLLOW_NAME);
		room_num = (rand_r_portable(&map_seed) % VOID_HOLLOW_ROOM_COUNT)+1;
	}
	
	room_template_t templates[128] = {0};
	int template_count = 0;
	if(x != 0 || y != 0) {
		for(int i = 0; i < world->room_template_count; i++) {
			if(room->door_mask == world->room_templates[i].mask) {
				strcpy(templates[template_count].filename, world->room_templates[i].filename);
				templates[template_count].mask |= world->room_templates[i].mask;
				template_count++;
			}
		}
		if(template_count > 0) {
			room_num = (rand_r_portable(&map_seed) % template_count)+1;
		} else {
			DEBUG_LOG("No matching templates found for mask: %d", room->door_mask);
			exit(0);
		}
	}
	
	assert(room_num > 0);
	if(x == 0 && y == 0) {
		strcat(file, "startingroom.ck.txt");
	} else {
		// snprintf(file, 92, "./data/rooms/%s/room%d.ck.txt", folder, room_num);
		strcpy(file, templates[room_num-1].filename);
	}

	int fd = open(file, O_RDONLY);
	if(fd < 0) {
		DEBUG_LOG("an error occured loading file %s\n", file);
		exit(0);
	}
	char *buf = calloc(512, sizeof(char));
	read(fd, buf, 512);
	int i = 0;

	strcpy(room->room_file_name, file);
	char *tok = strtok(buf, "\n");
	while(tok != NULL) {
		DEBUG_LOG("tok = %s | i = %d", tok, i);
		for(int j = 0; j < strlen(tok); j++) {
			switch(tok[j]) {
				case POTENTIAL_ENEMY_SPAWN_CHAR:
					room->enemies[room->current_enemy_count] = enemy_spawn(enemy_generate_type(&map_seed, enemy_data, room->biome), enemy_data, j, i, room->biome);
					room->tiles[i][j]->floor = EMPTY;
					room->tiles[i][j]->item_count = 0;
					room->current_enemy_count++;
					break;
				case POTENTIAL_CHEST_SPAWN_CHAR:
					room->tiles[i][j]->floor = tok[j];
					room->tiles[i][j]->item_count = 0;
					break;
				case POTENTIAL_TRAP_SPAWN_CHAR:
					room->tiles[i][j]->floor = tok[j];
					room->tiles[i][j]->item_count = 0;
					break;
				case POTENTIAL_ITEM_SPAWN_CHAR:
					item_spawn(item_generate_type(&map_seed, item_data, room->biome), room->biome, room->tiles[i][j], item_data);
					room->tiles[i][j]->item_count++;
					room->tiles[i][j]->floor = EMPTY;
					break;
				default:
					room->tiles[i][j]->floor = tok[j];
					room->tiles[i][j]->item_count = 0;
					break;
			}
		}
		tok = strtok(NULL, "\n");
		i++;
	}
	close(fd);

	return room;
}

void load_room_floor_tiles(room_t *room) {
	char *file = calloc(128, sizeof(char));
	
	strcpy(file, room->room_file_name);
	
	int fd = open(file, O_RDONLY);
	if(fd < 0) {
		DEBUG_LOG("an error occured loading file %s\n", file);
		exit(0);
	}
	char *buf = calloc(512, sizeof(char));
	read(fd, buf, 512);
	int i = 0;
	
	room->is_created = true;
	strcpy(room->room_file_name, file);
	char *tok = strtok(buf, "\n");
	while(tok != NULL) {
		DEBUG_LOG("tok = %s | i = %d", tok, i);
		for(int j = 0; j < strlen(tok); j++) {
			switch(tok[j]) {
				case POTENTIAL_ENEMY_SPAWN_CHAR:
				case POTENTIAL_CHEST_SPAWN_CHAR:
				case POTENTIAL_TRAP_SPAWN_CHAR:
				case POTENTIAL_ITEM_SPAWN_CHAR:
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
}
