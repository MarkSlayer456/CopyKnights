#ifndef ENEMY_H_

#define ENEMY_H_

#define MAX_ENEMIES					100


#define ENEMY_TYPE_COUNT			5
#define MAX_ENEMY_NAME_LENGTH		32
#define SLIME_ENEMY_NAME 			"Slime"
#define BAT_ENEMY_NAME				"Bat"
#define RAT_ENEMY_NAME				"Rat"
#define SKELETON_ENEMY_NAME			"Skeleton"
#define DRAGON_ENEMY_NAME			"Dragon"

typedef struct world world_t;
typedef struct player player_t;

typedef enum trait {
	PASSIVE = 80000,
	AGRESSIVE = 80001,
	TACTICAL = 80002
} trait_t;

typedef enum enemy_type {
	SLIME,
	BAT,
	SKELETON,
	RAT,
	DRAGON
} enemy_type_t;

typedef struct enemy {
	int health;
	int strength;
	int dexterity;
	int intelligence;
	int constitution;
	int world;
	int x; // room pos
	int y; // room pos
	int global_x; // map pos
	int global_y; // map pos
	enemy_type_t type;
	char name[MAX_ENEMY_NAME_LENGTH]; // name will be the type of enemy
	// and the first letter will be the letter to represent them
	trait_t trait;
} enemy_t;

typedef struct enemy_data {
	enemy_type_t type;
	int strength;
	int dexterity;
	int intelligence;
	int constitution;
} enemy_data_t;

typedef struct enemy_type_map {
	const char *name;
	trait_t value;
} enemy_type_map_t;

//TODO need move functions and check functions so enemies don't leave the map
// shouldn't happen but just in case

enemy_t *enemy_spawn(enemy_type_t type, const enemy_data_t *enemy_data, int x, int y);

const char *enemy_get_name(enemy_type_t type);

enemy_type_t enemy_get_type(const char *name);

enemy_type_t enemy_generate_type(unsigned int *seed);

void load_enemy_data(enemy_data_t *enemy_data);

void enemy_kill(enemy_t *enemy, world_t *world, player_t *player);

void enemy_decrease_health(enemy_t *enemy, world_t *world, player_t *player, int knightNum);

void enemy_attack(enemy_t *enemy, player_t *player, world_t *world, int knightNum);

void enemy_decide_move(enemy_t *enemy, world_t *world, player_t *player);

char enemy_check_right(enemy_t *enemy, world_t *world, player_t *player);
char enemy_check_left(enemy_t *enemy, world_t *world, player_t *player);
char enemy_check_up(enemy_t *enemy, world_t *world, player_t *player);
char enemy_check_down(enemy_t *enemy, world_t *world, player_t *player);

int enemy_can_move_right(enemy_t *enemy, world_t *world, player_t *player);
int enemy_can_move_left(enemy_t *enemy, world_t *world, player_t *player);
int enemy_can_move_up(enemy_t *enemy, world_t *world, player_t *player);
int enemy_can_move_down(enemy_t *enemy, world_t *world, player_t *player);

enemy_t *enemy_create_temp(world_t *world);

#endif
