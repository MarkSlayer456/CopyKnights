#include <string.h>
#include <stdlib.h>
#include "items/armor.h"
#include "items/item_types.h"
#include "items/items.h"
#include "enemy.h"

type_map_t armor_type_map[] = {
	{ARMOR_TYPE_LIGHT_NAME, LIGHT},
	{ARMOR_TYPE_MEDIUM_NAME, MEDIUM},
	{ARMOR_TYPE_HEAVY_NAME, HEAVY},
};

const int armor_type_map_len = sizeof(armor_type_map) / sizeof(armor_type_map[0]);

armor_type_t armor_get_type(const char *name) {
	for(int i = 0; i < armor_type_map_len; i++) {
		if(strcmp(name, armor_type_map[i].name) == 0) {
			return armor_type_map[i].value;
		}
	}
	return NULL_ARMOR_TYPE;
}

void load_armor_spawn_data(world_t *world) {
	item_data_t *item_data = world->item_data;
	FILE *fp = fopen("./data/armor/armor_spawns.csv", "r");
	if(!fp) {
		perror("armor_spawns.csv File open failed");
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
		char *armor_name = strtok(line, ",");
		char *biome_name = strtok(NULL, ",");
		char *spawn_rarity = strtok(NULL, ",");
		
		if(!armor_name || !biome_name || !spawn_rarity) {
			continue;
		}
		
		biome_t biome = get_biome(biome_name);
		if(biome == BIOME_NULL) {
			continue;
		}
		
		int i = 0;
		while(i < MAX_ITEMS) {
			if(strcmp(armor_name, item_data[i].name) != 0) {
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

void load_armor_effects(world_t *world) {
	item_data_t *item_data = world->item_data;
	FILE *fp = fopen("./data/armor/armor_effects.csv", "r");
	if(!fp) {
		perror("armor_effects.csv File open failed");
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
		char *armor_name = strtok(line, ",");
		char *target_stat = strtok(NULL, ",");
		char *value = strtok(NULL, ",");
		
		if(!armor_name || !target_stat || !value) {
			continue;
		}
		
		int i = 0;
		while(i < MAX_ITEMS) {
			if(strcmp(armor_name, item_data[i].name) != 0) {
				i++;
				continue;
			}
			int idx = item_data[i].stat_type.armor.modifier_count;
			
			item_data[i].stat_type.armor.modifier_stats[idx].modifier = (int16_t)atoi(value);
			item_data[i].stat_type.armor.modifier_stats[idx].stat = get_stat(target_stat);
			item_data[i].stat_type.armor.equipped = false;
			item_data[i].stat_type.armor.modifier_count++;
			char tmp_desc[MAX_ITEM_DESC_LEN];
			snprintf(tmp_desc, MAX_ITEM_DESC_LEN, "%s: %d\n", target_stat, item_data[i].stat_type.armor.modifier_stats[idx].modifier);
			strcat(item_data[i].desc, tmp_desc);
			DEBUG_LOG("Loaded Armor Stats: %d, %d", 
					item_data[i].stat_type.armor.modifier_stats[idx].stat, 
					item_data[i].stat_type.armor.modifier_stats[idx].modifier);
			break;
		}
		row++;
	}
	
	load_armor_spawn_data(world);
}

void load_armor_data(world_t *world) {
	item_data_t *item_data = world->item_data;
	FILE *fp = fopen("./data/armor/armors.csv", "r");
	if(!fp) {
		perror("armors.csv File open failed");
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
					snprintf(item_data[world->item_data_count].name, sizeof(item_data[world->item_data_count].name), "%s", token);
					item_data[world->item_data_count].id = item_get_id(token);
					break;
				case 1:
					item_data[world->item_data_count].value_type = VALUE_TYPE_ARMOR;
					item_data[world->item_data_count].stat_type.armor.type = armor_get_type(token);
					break;
				case 2:
					item_data[world->item_data_count].stat_type.armor.defense = (int16_t)atoi(token);
					break;
			}
			token = strtok(NULL, ",");
			col++;
		}
		snprintf(item_data[world->item_data_count].desc, MAX_ITEM_DESC_LEN, 
				"defense: %d\n",  
				item_data[world->item_data_count].stat_type.armor.defense);
		DEBUG_LOG("Loaded Armor Data: %d, %s, %d, %d, %s", 
				item_data[world->item_data_count].id, 
				item_data[world->item_data_count].name, 
				item_data[world->item_data_count].stat_type.armor.defense, 
				item_data[world->item_data_count].stat_type.armor.type,
				item_data[world->item_data_count].desc);
		world->item_data_count++;
		col = 0;
	}
	load_armor_effects(world);
}
