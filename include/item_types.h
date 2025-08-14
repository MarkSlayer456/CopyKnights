#ifndef ITEM_TYPES_H_
#define ITEM_TYPES_H_

// names should be limited to 32 characters
typedef enum item_ids {
	// items
	BLANK,
	TELEPORT_SCROLL,
	HEALTH_POTION,
	APPLE,
	CHICKEN_DINNER,
	
	// heavy armors
	BLACKSTONE_ARMOR,
	BRONZE_ARMOR,
	IRON_ARMOR,
	STEEL_ARMOR,
	MITHRIL_ARMOR,
	SOULCRUST_ARMOR,
	TERRACITE_ARMOR,
	GRAVEMARROW_ARMOR,
	VOIDMARROW_ARMOR,
	
	// medium armors
	RAT_HIDE_ARMOR,
	SLIME_ARMOR,
	BOG_IRON_ARMOR,
	BONE_ARMOR,
	SUNSTEEL_ARMOR,
	FALSE_IRON_ARMOR,
	BARKMAIL_ARMOR,
	DRAGON_BONE_ARMOR,
	
	// light armors
	CLOAK,
	LEATHER_CLOAK,
	BOGSTICK_VEST,
	VOIDLACE_CLOACK
} item_ids_t;

typedef struct item_type_map {
	const char *name;
	item_ids_t value;
} item_type_map_t;

typedef enum armor_type {
	LIGHT,
	MEDIUM,
	HEAVY
} armor_type_t;

typedef enum stats {
	STRENGTH,
	DEXTERITY,
	INTELLIGENCE,
	CONSTITUTION,
	SPEED
} stats_t;

typedef enum value_type {
	VALUE_TYPE_NONE,
	VALUE_TYPE_ARMOR,
	VALUE_TYPE_WEAPON,
	VALUE_TYPE_FOOD,
	VALUE_TYPE_SCROLL
} value_type_t;

typedef struct armor_stats {
	armor_type_t type;
	int defense;
	stats_t stat_one;
	int modifier_one;
	stats_t stat_two;
	int modifier_two;
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
	union {
		armor_stats_t armor;
		weapon_stats_t weapon;
		food_stats_t food;
		scroll_stats_t scroll;
	} stat_type;
	int spawn_rate;
} item_data_t;

#endif
