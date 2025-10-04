#ifndef ITEM_TYPES_H_
#define ITEM_TYPES_H_

#include <stdint.h>
#include "item_enums.h"
#include "enums.h"

typedef struct type_map {
	const char *name;
	int16_t value;
} type_map_t;

typedef struct stat_modifier {
	int16_t modifier;
	stats_t stat;
} stat_modifier_t;

typedef struct armor_stats {
	armor_type_t type;
	int16_t defense;
	stat_modifier_t modifier_stats[MAX_ARMOR_MODIFIERS];
	int8_t modifier_count;
	bool equipped;
} armor_stats_t;

typedef struct weapon_stats {
	int16_t max_attack;
	int16_t min_attack;
	stats_t scaling_stat1;
	stats_t scaling_stat2;
	enum grade stat1_grade;
	enum grade stat2_grade;
	int16_t strength_requirement;
	int16_t dexterity_requirement;
	int16_t intelligence_requirement;
	stat_modifier_t modifier_stats[MAX_ARMOR_MODIFIERS];
	int8_t modifier_count;
	bool equipped;
	bool two_handed;
	bool main_hand;
} weapon_stats_t;

typedef struct food_stats {
	int16_t heal_amount;
	int16_t mana_heal_amount;
	stat_modifier_t modifier_stats[MAX_ARMOR_MODIFIERS];
	int8_t modifier_count;
	int16_t duration;
} food_stats_t;

typedef struct scroll_stats {
	int tmp; // this is to avoid errors when compiling get rid of
	// TODO 
} scroll_stats_t;

typedef struct item {
	char name[MAX_ITEM_NAME_LENGTH];
	char desc[MAX_ITEM_DESC_LEN];
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
	char desc[MAX_ITEM_DESC_LEN];
	item_ids_t id;
	value_type_t value_type;
	rarity_t rarity[NUMBER_OF_BIOMES];
	bool spawn_biomes[NUMBER_OF_BIOMES];
	union {
		armor_stats_t armor;
		weapon_stats_t weapon;
		food_stats_t food;
		scroll_stats_t scroll;
	} stat_type;
} item_data_t;

#endif
