#ifndef GAME_MANAGER_H_
#define GAME_MANAGER_H_
#include "map_manager.h"
#include "enemy.h"
#include "player.h"
#include <ncurses.h>

#define MESSAGE_IS_PLAYERS_TURN					"It is your turn!"

#define WALL 				'#' // a wall
#define BOG_WALL			'%'
#define MUD					'~'
#define EMPTY				'.'
#define DOOR				'/'
#define CHEST				'$' // can walk through
#define PLAYER				'@'

#define PLAYER_TURN_ORDER_INDEX					-1
#define INVALID_ACTOR_INDEX						999999999

#define DOOR_BLOCKED_MESSAGE					"This door won't seem to budge!"

#define ARROW_KEY_MOD							1000
#define DOWN_ARROW								ARROW_KEY_MOD + 66
#define UP_ARROW								ARROW_KEY_MOD + 65
#define RIGHT_ARROW								ARROW_KEY_MOD + 67
#define LEFT_ARROW								ARROW_KEY_MOD + 68
#define KEY_I									105
#define ENTER_KEY								10
#define ESC_KEY									27

#define TEST_SEED								596231
#define ENEMY_X_PRIME							19349663u
#define ENEMY_Y_PRIME							73856093u

#define KEY_A      								97
#define KEY_B									98
#define KEY_S       							115

#define CTRL_Q									17

//TODO add more ...

#define WALK_CHAR_LENGTH						16

#define ENEMY_CHAR      'E' // used when finding enemies on the map

#define ACTION_BAR_SELECTOR_COUNT				2

#define TIME_TO_ACT								50


#define MAX_ITEM_NAME_LENGTH					32
#define DEFAULT_MAX_MESSAGE_STORAGE				1000
#define MAX_MESSAGE_LENGTH						59
#define MAX_MESSAGE_LENGTH_WITHOUT_PREFIX		47
//TODO maybe add max prefix length then add prefixes to messages

#define DEBUG_LOG(fmt, ...) \
do { \
	FILE *f = fopen("debug.log", "a"); \
	if (f) { \
		fprintf(f, fmt "\n", ##__VA_ARGS__); \
		fclose(f); \
	} \
} while (0)
// EXAMPLE: DEBUG_LOG("Constitution: %d", enemy_data[row].constitution);

typedef struct class_data class_data_t;
typedef enum class_type class_type_t;

typedef enum direction {
	LEFT,
	RIGHT,
	UP,
	DOWN
} direction_t;

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
	class_data_t *class_data;
	char **messages;
	int max_message_storage;
	int messages_size;
	int *turn_order; // -1 for player, index for enemies
	int turn_order_size;
} world_t;

void draw(world_t *world, player_t *player);
void manage_input(char c, world_t *world, player_t *player);

void display_world_message(world_t *world, player_t *player, const char *str);

void display_combat_message(world_t *world, player_t *player, const char *str);

bool is_opaque(room_t *room, int x, int y);

void mark_has_light(room_t *room, int x, int y);

void cast_light_check(world_t *world, player_t *player, int x0, int y0, float angle);

void calculate_light(world_t *world, player_t *player);

int pick_next_actor(world_t *world, player_t *player);

void turn_order_enter_new_room(world_t *world, player_t *player);

void end_game(world_t *world, player_t *player);

void shutdown(world_t *world);

#endif
