#ifndef PLAYER_H_
#define PLAYER_H_

#define NULL_CLASS_NAME					"NULL"
#define SWORDSMAN_CLASS_NAME			"Swordsman"
#define BARBARIAN_CLASS_NAME			"Barbarian"
#define SPEARMAN_CLASS_NAME				"Spearman"
#define ARCHER_CLASS_NAME				"Archer"
#define WIZARD_CLASS_NAME				"Wizard"
#define MERCHANT_CLASS_NAME				"Merchant"

#define BASE_STRENGTH					10
#define BASE_DEXTERITY					10
#define BASE_INTELLIGENCE				10
#define BASE_CONSTITUTION				10
#define BASE_SPEED						10

#define MAX_CLASSES						32

typedef enum class_type {
	CLASS_NONE = -1,
	SWORDSMAN,
	BARBARIAN,
	SPEARMAN,
	ARCHER,
	WIZARD,
	MERCHANT,
} class_type_t;

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

#include "game_manager.h"

class_type_t class_get_type(const char *name);

void load_class_data(class_data_t *class_data);

void player_wait(player_t *player, world_t *world);

void player_decrease_health(player_t *player, world_t *world, int attack);
void player_increase_health(player_t *player, int amount);

void player_move_left(player_t *player, world_t *world);
void player_move_right(player_t *player, world_t *world);
void player_move_down(player_t *player, world_t *world);
void player_move_up(player_t *player, world_t *world);

char player_get_current_pos(player_t *player, world_t *world);

void player_add_knight(player_t *player);

// used by can_move function probably shouldn't call for other reasons
char player_check_dir(player_t *player, world_t *world, direction_t dir);

// these return 1 if the player can move to the space
// and 0 if the player can't move there (ie there is a wall or enemy)
int player_can_move_dir(player_t *player, world_t *world, direction_t dir);

// this will return NULL if no enemy is next to the player
enemy_t *player_get_dir_enemy(player_t *player, world_t *world, direction_t dir);

void player_attack(player_t *player, world_t *world);

int xp_to_level_up(int level);

void player_add_xp(player_t *player, int amount, const class_data_t *class_data);

void player_check_level_up(player_t *player, const class_data_t *class_data);

void player_open_action_bar(player_t *player);
void player_close_action_bar(player_t *player);
void player_cycle_action_bar_up(player_t *player);
void player_cycle_action_bar_down(player_t *player);
void player_cycle_inv_selector_up(player_t *player);
void player_cycle_inv_selector_down(player_t *player);

void player_open_inventory(player_t *player);
void player_close_inventory(player_t *player);
void player_open_spells(player_t *player);
void player_close_spells(player_t *player);

// removes an item from the inventory list, not used to decrease item count
void player_organize_inv(player_t *player, int loc);

#endif


