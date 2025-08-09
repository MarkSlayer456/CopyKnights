#ifndef PLAYER_H_
#define PLAYER_H_

#include "types.h"

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


