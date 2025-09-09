#include <string.h>
#include <stdlib.h>
#include "items/foods.h"
#include "items/item_types.h"
#include "items/items.h"
#include "enemy.h"

void load_foods_spawn_data(world_t *world) {
	item_data_t *item_data = world->item_data;
	FILE *fp = fopen("./data/foods/foods_spawns.csv", "r");
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
		char *food_name = strtok(line, ",");
		char *biome_name = strtok(NULL, ",");
		char *spawn_rarity = strtok(NULL, ",");
		
		if(!food_name || !biome_name || !spawn_rarity) {
			continue;
		}
		
		biome_t biome = get_biome(biome_name);
		if(biome == BIOME_NULL) {
			continue;
		}
		
		int i = 0;
		while(i < MAX_ITEMS) {
			if(strcmp(food_name, item_data[i].name) != 0) {
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

void load_foods_effects(world_t *world) {
	item_data_t *item_data = world->item_data;
	FILE *fp = fopen("./data/foods/foods_effects.csv", "r");
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
		char *food_name = strtok(line, ",");
		char *target_stat = strtok(NULL, ",");
		char *value = strtok(NULL, ",");
		
		if(!food_name || !target_stat || !value) {
			continue;
		}
		
		int i = 0;
		while(i < MAX_ITEMS) {
			if(strcmp(food_name, item_data[i].name) != 0) {
				i++;
				continue;
			}
			int idx = item_data[i].stat_type.food.modifier_count;
			
			item_data[i].stat_type.food.modifier_stats[idx].modifier = atoi(value);
			item_data[i].stat_type.food.modifier_stats[idx].stat = get_stat(target_stat);
			item_data[i].stat_type.food.modifier_count++;
			break;
		}
		row++;
	}
	load_foods_spawn_data(world);
}

void load_foods_data(world_t *world) {
	item_data_t *item_data = world->item_data;
	FILE *fp = fopen("./data/foods/foods.csv", "r");
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
					snprintf(item_data[world->item_data_count].name, sizeof(item_data[world->item_data_count].name), "%s", token);
					item_data[world->item_data_count].id = item_get_id(token);
					break;
				case 1:
					item_data[world->item_data_count].value_type = VALUE_TYPE_FOOD;
					item_data[world->item_data_count].stat_type.food.heal_amount = atoi(token);
					break;
				case 2:
					item_data[world->item_data_count].stat_type.food.mana_heal_amount = atoi(token);
					break;
			}
			token = strtok(NULL, ",");
			col++;
		}
		snprintf(item_data[world->item_data_count].desc, MAX_ITEM_DESC_LEN, "+%d health, +%d mana", item_data[world->item_data_count].stat_type.food.heal_amount, item_data[world->item_data_count].stat_type.food.mana_heal_amount);
		DEBUG_LOG("Loaded Food Data: %d, %s, %d, %d, %s", 
				  item_data[world->item_data_count].id, 
			item_data[world->item_data_count].name, 
			item_data[world->item_data_count].stat_type.food.heal_amount, 
			item_data[world->item_data_count].stat_type.food.mana_heal_amount,
			item_data[world->item_data_count].desc);
		world->item_data_count++;
		col = 0;
	}
	load_foods_effects(world);
}
