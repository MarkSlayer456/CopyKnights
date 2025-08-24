#include <string.h>
#include <stdlib.h>
#include "items/items.h"
#include "types.h"
#include "enemy.h"

void load_weapon_spawn_data(world_t *world) {
	item_data_t *item_data = world->item_data;
	FILE *fp = fopen("./data/weapons/weapon_spawns.csv", "r");
	if(!fp) {
		perror("File open failed");
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
		char *weapon_name = strtok(line, ",");
		char *biome_name = strtok(NULL, ",");
		char *spawn_rarity = strtok(NULL, ",");
		
		if(!weapon_name || !biome_name || !spawn_rarity) {
			continue;
		}
		
		biome_t biome = get_biome(biome_name);
		if(biome == BIOME_NULL) {
			continue;
		}
		
		int i = 0;
		while(i < MAX_ITEMS) {
			if(strcmp(weapon_name, item_data[i].name) != 0) {
				i++;
				continue;
			}
			
			item_data[i].spawn_biomes[biome] = true;
			item_data[i].rarity[biome] = get_rarity(spawn_rarity);
			break;
		}
		row++;
	}
}

void load_weapon_effects(world_t *world) {
	item_data_t *item_data = world->item_data;
	FILE *fp = fopen("./data/weapons/weapon_effects.csv", "r");
	if(!fp) {
		perror("File open failed");
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
		char *weapon_name = strtok(line, ",");
		char *target_stat = strtok(NULL, ",");
		char *value = strtok(NULL, ",");
		
		if(!weapon_name || !target_stat || !value) {
			continue;
		}
		
		int i = 0;
		while(i < MAX_ITEMS) {
			if(strcmp(weapon_name, item_data[i].name) != 0) {
				i++;
				continue;
			}
			int idx = item_data[i].stat_type.weapon.modifier_count;
			
			item_data[i].stat_type.weapon.modifier_stats[idx].modifier = atoi(value);
			item_data[i].stat_type.weapon.modifier_stats[idx].stat = get_stat(target_stat);
			item_data[i].stat_type.weapon.equipped = false;
			item_data[i].stat_type.weapon.modifier_count++;
			break;
		}
		row++;
	}
	load_weapon_spawn_data(world);
}

void load_weapon_data(world_t *world) {
	item_data_t *item_data = world->item_data;
	FILE *fp = fopen("./data/weapons/weapons.csv", "r");
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
	
	int row = 0;
	while(fgets(line, sizeof(line), fp)) {
		line[strcspn(line, "\n")] = '\0';
		int col = 0;
		char *token = strtok(line, ",");
		while(token) {
			switch(col) {
				case 0:
					snprintf(item_data[row].name, sizeof(item_data[row].name), "%s", token);
					item_data[row].id = item_get_id(token);
					item_data[row].value_type = VALUE_TYPE_WEAPON;
					break;
				case 1:
					item_data[row].stat_type.weapon.attack = atoi(token);
					break;
				case 2:
					item_data[row].stat_type.weapon.strength_requirement = atoi(token);
					break;
				case 3:
					item_data[row].stat_type.weapon.dexterity_requirement= atoi(token);
					break;
				case 4:
					item_data[row].stat_type.weapon.intelligence_requirement = atoi(token);
					break;
			}
			token = strtok(NULL, ",");
			col++;
		}
		world->item_data_count++;
		snprintf(item_data[row].desc, MAX_ITEM_DESC_LEN, 
				 "attack damage: %d\nstrength requirement: %d\ndexterity requirement: %d\nintelligence requirement: %d"
				 , item_data[row].stat_type.weapon.attack, item_data[row].stat_type.weapon.strength_requirement, item_data[row].stat_type.weapon.dexterity_requirement, item_data[row].stat_type.weapon.intelligence_requirement);
		DEBUG_LOG("Loaded Weapon Data: %d, %s, %d, %s",
				item_data[row].id, item_data[row].name,
				item_data[row].stat_type.weapon.attack,
				item_data[row].desc);
		
		col = 0;
		row++;
	}
	load_weapon_effects(world);
}
