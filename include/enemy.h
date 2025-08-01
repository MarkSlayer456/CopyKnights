#ifndef ENEMY_H_

#define ENEMY_H_

#define MAX_ENEMIES							100


#define ENEMY_TYPE_COUNT					17
#define MAX_ENEMY_NAME_LENGTH				32
#define NULL_ENEMY_NAME						"NULL"
#define SLIME_ENEMY_NAME 					"Slime"
#define BAT_ENEMY_NAME						"Bat"
#define RAT_ENEMY_NAME						"Rat"
#define SKELETON_ENEMY_NAME					"Skeleton"
#define DRAGON_ENEMY_NAME					"Dragon"
#define BABY_DRAGON_ENEMY_NAME				"Baby Dragon"
#define GOLBIN_ENEMY_NAME					"Goblin"
#define GHOST_ENEMY_NAME					"Ghost"
#define LOOT_GOBLIN_ENEMY_NAME				"Loot Goblin"
#define MOSS_BEAST_ENEMY_NAME				"Moss Beast"
#define JESTER_ENEMY_NAME					"Jester"
#define VOIDLING_ENEMY_NAME					"Voidling"
#define MARROW_CRAWLER_ENEMY_NAME			"Marrow Crawler"
#define VOID_MAW_ENEMY_NAME					"Void Maw"
#define MARROW_LEECH_ENEMY_NAME				"Marrow Leech"
#define MUD_CRAWLER_ENEMY_NAME				"Mud Crawler"
#define BOG_LURKER_ENEMY_NAME				"Bog Lurker"

typedef struct world world_t;
typedef struct player player_t;
typedef enum direction direction_t;

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

typedef struct enemy {
	int health;
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
	char symbol;
	trait_t trait;
} enemy_data_t;

typedef struct enemy_type_map {
	const char *name;
	enemy_type_t value;
} enemy_type_map_t;

//TODO need move functions and check functions so enemies don't leave the map
// shouldn't happen but just in case

enemy_t *enemy_spawn(enemy_type_t type, const enemy_data_t *enemy_data, int x, int y);

const char *enemy_get_name(enemy_type_t type);

enemy_type_t enemy_get_type(const char *name);

enemy_type_t enemy_generate_type(unsigned int *seed);

void load_enemy_data(enemy_data_t *enemy_data);

void enemy_kill(enemy_t *enemy, world_t *world, player_t *player);

enemy_type_t enemy_decrease_health(enemy_t *enemy, world_t *world, player_t *player);

void enemy_attack(enemy_t *enemy, player_t *player, world_t *world);

void enemy_decide_move(enemy_t *enemy, world_t *world, player_t *player);

char enemy_check_dir(enemy_t *enemy, world_t *world, player_t *player, direction_t dir);

int enemy_can_move_dir(enemy_t *enemy, world_t *world, player_t *player, direction_t dir);

enemy_t *enemy_create_temp(world_t *world);

#endif
