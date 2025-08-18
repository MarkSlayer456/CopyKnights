#ifndef ITEM_TYPES_H_
#define ITEM_TYPES_H_

#include "item_enums.h"
#include "enums.h"

typedef struct type_map {
	const char *name;
	item_ids_t value;
} type_map_t;

typedef struct stat_modifier {
	int modifier;
	stats_t stat;
} stat_modifier_t;

typedef struct armor_stats {
	armor_type_t type;
	int defense;
	stat_modifier_t modifier_stats[MAX_ARMOR_MODIFIERS];
	int modifier_count;
} armor_stats_t;

typedef struct weapon_stats {
	int attack;
	int strength_requirement;
	int dexterity_requirement;
	int intelligence_requirement;
} weapon_stats_t;

typedef struct food_stats {
	int heal_amount;
} food_stats_t;

typedef struct scroll_stats {
	// TODO 
} scroll_stats_t;

typedef struct item {
	char name[MAX_ITEM_NAME_LENGTH];
	item_ids_t id;
	int stack;
	value_type_t value_type;
	union {
		armor_stats_t armor;
		weapon_stats_t weapon;
		food_stats_t food;
		scroll_stats_t scroll;
	} stat_type;
} item_t;

typedef struct item_data {
	char name[MAX_ITEM_NAME_LENGTH];
	item_ids_t id;
	value_type_t value_type;
	rarity_t rarity;
	biome_t spawn_biomes[NUMBER_OF_BIOMES];
	int spawn_biome_count;
	union {
		armor_stats_t armor;
		weapon_stats_t weapon;
		food_stats_t food;
		scroll_stats_t scroll;
	} stat_type;
} item_data_t;

#endif
