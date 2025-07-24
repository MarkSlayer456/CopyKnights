#ifndef GAME_MANAGER_H_
#define GAME_MANAGER_H_
#include "map_manager.h"
#include "enemy.h"
#include <ncurses.h>

#define WALL 				'#' // a wall
#define VISUAL_OBJECT		'$' // can walk through
#define EMPTY				'.'
#define DOOR				'/'
#define CHEST				'$'
#define PLAYER				'P'
#define COPYONE				'1'
#define COPYTWO				'2'
#define COPYTHREE			'3'
#define COPYFOUR			'4'
#define COPYFIVE			'5'
#define COPYSIX				'6'
#define COPYSEVEN			'7'

#define MAX_KNIGHTS 			1
#define MAX_ENEMIES_PER_LEVEL	16

#define ARROW_KEY_MOD	1000
#define DOWN_ARROW		ARROW_KEY_MOD + 66
#define UP_ARROW		ARROW_KEY_MOD + 65
#define RIGHT_ARROW		ARROW_KEY_MOD + 67
#define LEFT_ARROW		ARROW_KEY_MOD + 68
#define KEY_I		105
#define ENTER_KEY	10
#define ESC_KEY		27

#define TEST_SEED	1510192

#define KEY_A       97
#define KEY_B		98
#define KEY_S       115

#define CTRL_Q			17

//TODO add more ...

#define WALK_CHAR_LENGTH	16

#define ENEMY_CHAR      'E' // used when finding enemies on the map

#define ACTION_BAR_SELECTOR_COUNT	2


#define MAX_ITEM_NAME_LENGTH		32

#define DEBUG_LOG(fmt, ...) \
do { \
	FILE *f = fopen("debug.log", "a"); \
	if (f) { \
		fprintf(f, fmt "\n", ##__VA_ARGS__); \
		fclose(f); \
	} \
} while (0)
// EXAMPLE: DEBUG_LOG("Constitution: %d", enemy_data[row].constitution);

// names should be limited to 32 characters
typedef enum item_ids {
	BLANK,
	TELEPORT_SCROLL,
	HEALTH_POTION,
	APPLE,
	CHICKEN_DINNER
} item_ids_t;

typedef struct item {
	char name[MAX_ITEM_NAME_LENGTH];
	item_ids_t id;
	int stack;
} item_t;

typedef struct location {
	int x;
	int y;
	int global_x;
	int global_y;
} location_t;

typedef enum action_bar_selectors { 
	NOT_OPEN = -1, 
	INVENTORY = 0, 
	SPELLS = 1	
} action_bar_selector_t;

typedef enum inventory_category {
	ITEM = 0,
	FOOD = 1,
	POTIONS = 2,
	SCROLLS = 3
} inv_cat_t;

typedef struct object {
	int x;
	int y;
	int id;
} wall_t;

typedef struct action_bar {
	int inv_open; // is inventory open
	int spells_open; // is options open
	action_bar_selector_t selector; // which item is the selector on
	
	int spells_selector; // current spell you are selected on
	
	int inv_selector; // current inventory item you are selected on in a category
	inv_cat_t cat; // current inventory category you are selected on TODO add categories
} action_bar_t;

typedef struct player {
	int knights;
	int *health;
	int *max_health;
	int *attack;
	int *defense;
	int strength;
	int dexterity;
	int intelligence;
	int constitution;
	int *x; // each knight has it's only pos
	int *y; // each knight has it's only pos
	int global_x; // pos on the map
	int global_y; // pos on the map
	item_t *inventory; // list of items in players inventory
	int *mana; // amount of player currently has
	int *max_mana;
	
	action_bar_t action_bar;
} player_t;


/* a world is really a floor
 * it has the whole room inside of it
 * each room, and each room's layout because the rooms all have a layout
 */
typedef struct world {
	WINDOW *win;
	room_t **room;
	int isPlayerTurn;
	unsigned int seed;
	enemy_data_t *enemy_data;
} world_t;

void draw(world_t *world, player_t *player);
void manage_input(char c, world_t *world, player_t *player);

void end_game(world_t *world, player_t *player);

void shutdown(world_t *world);

#endif
