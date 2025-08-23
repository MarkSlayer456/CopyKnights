#ifndef ENUMS_H_
#define ENUMS_H_

typedef enum biome {
	BIOME_NULL = -1,
	CAVE,
	BOG,
	CATACOMBS,
	ANCIENT_CITY,
	ARCANE_LABYRINTH,
	VOID_HOLLOW
} biome_t;

typedef enum stats {
	NULL_STAT,
	STRENGTH,
	DEXTERITY,
	INTELLIGENCE,
	CONSTITUTION,
	SPEED
} stats_t;

typedef enum direction {
	LEFT,
	RIGHT,
	UP,
	DOWN
} direction_t;

typedef enum action_bar_selectors { 
	NOT_OPEN = -1, 
	INVENTORY = 0, 
	SPELLS = 1,
	LOOT = 2
} action_bar_selector_t;

typedef enum inventory_category {
	ITEM = 0,
	FOOD = 1,
	POTIONS = 2,
	SCROLLS = 3
} inv_cat_t;

typedef enum class_type {
	CLASS_NONE = -1,
	SWORDSMAN,
	BARBARIAN,
	SPEARMAN,
	ARCHER,
	WIZARD,
	MERCHANT,
} class_type_t;

typedef enum trait {
	PASSIVE = 80000,
	AGRESSIVE = 80001,
	TACTICAL = 80002
} trait_t;

typedef enum enemy_type {
	ENEMY_NONE = -1,
	SLIME,
	BAT,
	SKELETON,
	RAT,
	DRAGON,
	BABY_DRAGON,
	GOBLIN,
	GHOST,
	LOOT_GOBLIN,
	MOSS_BEAST,
	JESTER,
	VOIDLING,
	MARROW_CRAWLER,
	VOID_MAW,
	MARROW_LEECH,
	MUD_CRAWLER,
	BOG_LURKER,
} enemy_type_t;

#endif
