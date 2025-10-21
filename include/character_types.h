#ifndef CHARACTER_TYPES_H_

#define CHARACTER_TYPES_H_

#include "items/item_types.h"

typedef struct {
	item_ids_t id;
	float drop_chance;
	int min_quantity;
	int max_quantity;
} item_drop_t;

typedef struct {
	item_drop_t *drops;
	int drop_count;
} drop_table_t;

typedef struct location {
	int x;
	int y;
	int global_x;
	int global_y;
} location_t;

typedef struct {
	int spells_selector; // current spell you are selected on
	
	int inv_selector; // current inventory item you are selected on in a category
	int inv_offset;
	
	int loot_selector; // current loot item you are selected on
	int loot_offset;
	inv_cat_t cat; // current inventory category you are selected on TODO add categories
} inventory_manager_t;

typedef struct class_data {
	class_type_t type;
	int base_strength;
	int base_dexterity;
	int base_intelligence;
	int base_constitution;
	int base_speed;
	float growth_strength;
	float growth_dexterity;
	float growth_intelligence;
	float growth_constitution;
	float growth_speed;
} class_data_t;

typedef struct class_type_map {
	const char *name;
	class_type_t value;
} class_type_map_t;

typedef struct lantern {
	int power;
	bool is_on;
	int turns_since_last_dim;
} lantern_t;

typedef struct equipment {
	item_ids_t armor_id, main_hand_id, off_hand_id;
	item_t *armor;
	item_t *main_hand;
	bool main_hand_two_handed; // is this weapon 2 handed
	item_t *off_hand;
} equipment_t;

typedef struct player {
	int level;
	int xp;
	int health;
	int max_health;
	float strength;
	float dexterity;
	float intelligence;
	float constitution;
	float speed;
	int x; // each knight has it's only pos
	int y; // each knight has it's only pos
	int global_x; // pos on the map
	int global_y; // pos on the map
	item_t *inventory; // list of items in players inventory
	int inventory_count; // amount of items the player has
	item_t *nearby_loot[MAX_ITEMS_NEARBY_PLAYER];
	int nearby_loot_count;
	int mana; // amount of player currently has
	int max_mana;
	int action_points;
	class_type_t player_class;
	lantern_t lantern;
	equipment_t equipment;
	int oil;
	player_state_t state;
	
	inventory_manager_t inventory_manager;
} player_t;

typedef struct enemy {
	int health;
	int level;
	int strength;
	int dexterity;
	int intelligence;
	int constitution;
	int speed;
	int defense;
	int world;
	int x; // room pos
	int y; // room pos
	int global_x; // map pos
	int global_y; // map pos
	uint8_t range;
	enemy_type_t type;
	char name[MAX_ENEMY_NAME_LENGTH]; // name will be the type of enemy
	int action_points;
	// and the first letter will be the letter to represent them
	char symbol;
	trait_t trait;
} enemy_t;

typedef struct enemy_data {
	enemy_type_t type;
	int base_strength;
	int base_dexterity;
	int base_intelligence;
	int base_constitution;
	int base_speed;
	int base_defense;
	int level;
	char symbol;
	trait_t trait;
	uint8_t range;
	bool can_spawn[NUMBER_OF_BIOMES];
	int highest_level[NUMBER_OF_BIOMES];
	int lowest_level[NUMBER_OF_BIOMES];
	drop_table_t drop_table;
} enemy_data_t;

typedef struct enemy_type_map {
	const char *name;
	enemy_type_t value;
} enemy_type_map_t;

// this will also be used to make debuffs with negative numbers
typedef struct {
	int turns_left; // how many turns until this buff wears off
	// if turns_left is -1 the effect will never end, could be useful for
	// effects that have to be cleared by magic or armor and weapon effects
	char name[BUFF_NAME_MAX_LEN];
	enum buff_type type;
	int8_t damage;
	float flat_strength;
	float flat_dexterity;
	float flat_intelligence;
	float flat_constitution;
	float flat_speed;
	float percent_strength;
	float percent_dexterity;
	float percent_intelligence;
	float percent_constitution;
	float percent_speed;
	bool applied;
	enum target_type target_type_id;
	union {
		player_t *player;
		enemy_t *enemy;
	} target;
} buff_t;

#endif
