#include "traps.h"
#include "items/items.h"
#include <string.h>
#include <strings.h>
#include <stdlib.h>


type_map_t trap_id_map[] = {
	{TRAP_STALAGMITE_NAME, TRAP_STALAGMITE},
	{TRAP_HOLE_NAME, TRAP_HOLE},
	{TRAP_MUD_NAME, TRAP_MUD},
	{TRAP_TWISTED_ROOT_NAME, TRAP_TWISTED_ROOT},
};

const int trap_id_map_len = sizeof(trap_id_map) / sizeof(trap_id_map[0]);

enum trap_id get_trap_id(const char *id) {
	for(int i = 0; i < trap_id_map_len; i++) {
		if(strcasecmp(id, trap_id_map[i].name) == 0) {
			return trap_id_map[i].value;
		}
	}
	return NULL_TRAP;
}

void traps_triggered_check(room_t *room, player_t *player) {
	
}

void traps_turn_pass(room_t *room, player_t *player) {
	
}

void load_traps_effects(world_t *world) {
	trap_data_t *trap_data = world->trap_data;
	FILE *fp = fopen("./data/traps/trap_effects.csv", "r");
	if(!fp) {
		perror("trap_effects.csv File open failed");
		return;
	}
	
	char line[2048];
	
	if(fgets(line, sizeof(line), fp) == NULL) {
		fclose(fp);
		return;
	}
	
	int row = 0;
	while(fgets(line, sizeof(line), fp)) {
		line[strcspn(line, "\n")] = '\0';
		char *trap_name = strtok(line, ",");
		char *target_stat = strtok(NULL, ",");
		char *value = strtok(NULL, ",");
		
		if(!trap_name || !target_stat || !value) {
			continue;
		}
		
		int i = 0;
		while(i < MAX_ITEMS) {
			if(strcmp(trap_name, trap_data[i].name) != 0) {
				i++;
				continue;
			}
			int idx = trap_data[i].modifier_count;
			
			trap_data[i].modifier_stats[idx].modifier = (int16_t)atoi(value);
			trap_data[i].modifier_stats[idx].stat = get_stat(target_stat);
			trap_data[i].modifier_count++;
			break;
		}
		row++;
	}
}

void load_trap_data(world_t *world) {
	trap_data_t *trap_data = world->trap_data;
	FILE *fp = fopen("./data/traps/traps.csv", "r");
	if(!fp) {
		perror("File open failed");
		return;
	}
	
	char line[2048];
	
	if(fgets(line, sizeof(line), fp) == NULL) {
		fprintf(stderr, "File is empty\n");
		fclose(fp);
		return;
	}
	while(fgets(line, sizeof(line), fp)) {
		line[strcspn(line, "\n")] = '\0';
		int col = 0;
		char *token = strtok(line, ",");
		while(token) {
			switch(col) {
				case 0:
					snprintf(trap_data[world->trap_data_count].name, sizeof(trap_data[world->trap_data_count].name), "%s", token);
					trap_data[world->trap_data_count].id = get_trap_id(token);
					break;
				case 1:
					trap_data[world->trap_data_count].effect_duration = (int16_t)atoi(token);
					break;
				case 2:
					trap_data[world->trap_data_count].damage = (int16_t)atoi(token);
					break;
				case 3:
					trap_data[world->trap_data_count].damage_each_turn = get_stat(token);
					break;
				case 4:
					trap_data[world->trap_data_count].break_on_trigger = atoi(token) ? true : false;
					break;
			}
			token = strtok(NULL, ",");
			col++;
		}
		world->trap_data_count++;
		col = 0;
	}
	load_traps_effects(world);
}
