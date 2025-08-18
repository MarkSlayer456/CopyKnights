#ifndef TYPES_H_

#define TYPES_H_
#include <stdbool.h>
#include <ncurses.h>
#include "game_constants.h"
#include "item_types.h"
#include "enums.h"

#define DEBUG_LOG(fmt, ...) \
do { \
	FILE *f = fopen("debug.log", "a"); \
	if (f) { \
		fprintf(f, fmt "\n", ##__VA_ARGS__); \
		fclose(f); \
	} \
} while (0)
// EXAMPLE: DEBUG_LOG("Constitution: %d", enemy_data[row].constitution);

typedef struct room room_t;

typedef struct location {
	int x;
	int y;
	int global_x;
	int global_y;
} location_t;

typedef struct action_bar {
	//TODO these 3 bools should be 1 enum, less juggling just better in every way
	bool inv_open; // is inventory open
	bool spells_open; // is options open
	bool loot_open; // is a loot tile open
	action_bar_selector_t selector; // which item is the selector on
	
	int spells_selector; // current spell you are selected on
	
	int inv_selector; // current inventory item you are selected on in a category
	inv_cat_t cat; // current inventory category you are selected on TODO add categories
} action_bar_t;

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
	item_t *armor;
	item_t *main_hand;
	item_t *off_hand;
} equipment_t;

typedef struct player {
	int level;
	int xp;
	int health;
	int max_health;
	int strength;
	int dexterity;
	int intelligence;
	int constitution;
	int speed;
	int x; // each knight has it's only pos
	int y; // each knight has it's only pos
	int global_x; // pos on the map
	int global_y; // pos on the map
	item_t *inventory; // list of items in players inventory
	int *mana; // amount of player currently has
	int *max_mana;
	int action_points;
	class_type_t player_class;
	int inv_offset;
	lantern_t lantern;
	equipment_t equipment;
	
	action_bar_t action_bar;
} player_t;

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

typedef struct enemy {
	int health;
	int level;
	int strength;
	int dexterity;
	int intelligence;
	int constitution;
	int speed;
	int world;
	int x; // room pos
	int y; // room pos
	int global_x; // map pos
	int global_y; // map pos
	enemy_type_t type;
	char name[MAX_ENEMY_NAME_LENGTH]; // name will be the type of enemy
	int action_points;
	// and the first letter will be the letter to represent them
	char symbol;
	trait_t trait;
} enemy_t;

typedef struct enemy_data {
	enemy_type_t type;
	int strength;
	int dexterity;
	int intelligence;
	int constitution;
	int speed;
	int level;
	char symbol;
	trait_t trait;
	bool can_spawn[NUMBER_OF_BIOMES];
	int highest_level[NUMBER_OF_BIOMES];
	int lowest_level[NUMBER_OF_BIOMES];
} enemy_data_t;

typedef struct enemy_type_map {
	const char *name;
	enemy_type_t value;
} enemy_type_map_t;

/* a world is really a floor
 * it has the whole room inside of it
 * each room, and each room's layout because the rooms all have a layout
 */
typedef struct world {
	WINDOW *win;
	room_t *room[WORLD_HEIGHT][WORLD_WIDTH];
	int isPlayerTurn;
	unsigned int seed;
	enemy_data_t *enemy_data;
	class_data_t *class_data;
	item_data_t *item_data;
	int item_data_count;
	char **messages;
	int max_message_storage;
	int messages_size;
	int *turn_order; // -1 for player, index for enemies
	int turn_order_size;
} world_t;

#endif
