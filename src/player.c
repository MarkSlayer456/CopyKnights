#include "player.h"
#include "game_manager.h"
#include "map_manager.h"
#include "functions.h"
#include "enemy.h"
#include "hud.h"
#include "items/items.h"
#include "math.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include "types.h"
#include "entrances.h"

#define MIN(a,b)       (a < b) ? a : b
#define MAX(a,b)       (a > b) ? a : b

extern char walk_chars[WALK_CHAR_LENGTH];

class_type_map_t class_type_map[] = {
	{NULL_CLASS_NAME, CLASS_NONE},
	{SWORDSMAN_CLASS_NAME, SWORDSMAN},
	{BARBARIAN_CLASS_NAME, BARBARIAN},
	{SPEARMAN_CLASS_NAME, SPEARMAN},
	{ARCHER_CLASS_NAME, ARCHER},
	{WIZARD_CLASS_NAME, WIZARD},
	{MERCHANT_CLASS_NAME, MERCHANT},
	{ROGUE_CLASS_NAME, ROGUE},
	{MONK_CLASS_NAME, MONK},
	{PALADIN_CLASS_NAME, PALADIN},
	{BRAWLER_CLASS_NAME, BRAWLER},
	{VOID_EMBRACE_CLASS_NAME, VOID_EMBRACE}
};

const int class_type_map_len = sizeof(class_type_map) / sizeof(class_type_map[0]);

class_type_t class_get_type(const char *name) {
	for(int i = 0; i < class_type_map_len; i++) {
		if(strcasecmp(name, class_type_map[i].name) == 0) {
			return class_type_map[i].value;
		}
	}
	return CLASS_NONE; // or an INVALID_TRAIT enum
}

void load_class_data(class_data_t *class_data) {
	FILE *fp = fopen("./data/classes.csv", "r");
	if(!fp) {
		perror("File open failed");
		return;
	}
	
	char line[2048];
	
	if(fgets(line, sizeof(line), fp) == NULL) {
		fprintf(stderr, "File is empty\n");
		fclose(fp);
		return;
	}
	
	int row = 0;
	while(fgets(line, sizeof(line), fp)) {
		line[strcspn(line, "\n")] = '\0';
		int col = 0;
		char *token = strtok(line, ",");
		while(token) {
			switch(col) {
				case 0:
					class_data[row].type = class_get_type(token);
					break;
				case 1:
					class_data[row].base_strength = atoi(token);
					break;
				case 2:
					class_data[row].base_dexterity = atoi(token);
					break;
				case 3:
					class_data[row].base_intelligence = atoi(token);
					break;
				case 4:
					class_data[row].base_constitution = atoi(token);
					break;
				case 5:
					class_data[row].base_speed = atoi(token);
					break;
				case 6:
					class_data[row].growth_strength = atof(token);
					break;
				case 7:
					class_data[row].growth_dexterity = atof(token);
					break;
				case 8:
					class_data[row].growth_intelligence = atof(token);
					break;
				case 9:
					class_data[row].growth_constitution = atof(token);
					break;
				case 10:
					class_data[row].growth_speed = atof(token);
					break;
			}
			token = strtok(NULL, ",");
			col++;
		}
		DEBUG_LOG("Loaded Class Data: %d, %d, %d, %d, %d, %d, %f, %f, %f, %f, %f", class_data[row].type, 
			class_data[row].base_strength, class_data[row].base_dexterity, class_data[row].base_intelligence,
			class_data[row].base_constitution, class_data[row].base_speed, class_data[row].growth_strength, 
			class_data[row].growth_dexterity, class_data[row].growth_intelligence,
			class_data[row].growth_constitution, class_data[row].growth_speed);
		col = 0;
		row++;
	}
	fclose(fp);
}

char player_get_current_pos(player_t *player, world_t *world)
{
	return world->room[player->global_x][player->global_y]->tiles[player->y][player->x]->floor;
}

void player_move_dir(player_t *player, world_t *world, direction_t dir) {
	if(dir == LEFT && player_can_move_dir(player, world, LEFT)) player->x--;
	if(dir == RIGHT && player_can_move_dir(player, world, RIGHT)) player->x++;
	if(dir == DOWN && player_can_move_dir(player, world, DOWN)) player->y++;
	if(dir == UP && player_can_move_dir(player, world, UP)) player->y--;
	
	if(player_get_current_pos(player, world) == DOOR) {
		if(dir == LEFT) player->global_x--;
		if(dir == RIGHT) player->global_x++;
		if(dir == DOWN) player->global_y++;
		if(dir == UP) player->global_y--;
		if(player->global_y < 0) {
			player->y += 1;
			display_world_message(world, DOOR_BLOCKED_MESSAGE);
			return;
		}
		if(player->global_x < 0) {
			player->x += 1;
			display_world_message(world, DOOR_BLOCKED_MESSAGE);
			return;
		}
		
		if(!world->room[player->global_x][player->global_y]->is_created) {
			player_enter_new_room(player, world);
		}
		
		turn_order_enter_new_room(world, player);
		if(dir == LEFT) player->x = ROOM_WIDTH-2;
		if(dir == RIGHT) player->x = 1;
		if(dir == DOWN) player->y = 1;
		if(dir == UP) player->y = ROOM_HEIGHT-2;
	}
}

void player_enter_new_room(player_t *player, world_t *world) {
	float chance = (float)rand() / (float)RAND_MAX;
	if(chance <= ENTRANCE_MESSAGE_CHANCE) {
		int message = rand() % cave_entrance_messages_count;
		display_combat_message(world, cave_entrance_messages[message]);
	}
	world->room[player->global_x][player->global_y] = generate_room(&world->seed, player->global_x, player->global_y, world->enemy_data, world->item_data, world);
}

char player_check_dir(player_t *player, world_t *world, direction_t dir) {
	int x = player->x;
	int y = player->y;
	if(dir == LEFT) x--;
	if(dir == RIGHT) x++;
	if(dir == DOWN) y++;
	if(dir == UP) y--;
	
	room_t *room = world->room[player->global_x][player->global_y];
	for(int i = 0; i < room->current_enemy_count; i++) {
		if(room->enemies[i] == NULL) continue;
		if(y == room->enemies[i]->y && x == room->enemies[i]->x) {
			return ENEMY_CHAR;
		}  
	}
	if(y <= ROOM_HEIGHT && x <= ROOM_WIDTH) {
		return room->tiles[y][x]->floor;
	} else {
		return ' ';
	}
}

int player_can_move_dir(player_t *player, world_t *world, direction_t dir) {
	char a = player_check_dir(player, world, dir);
	for(int i = 0; i < WALK_CHAR_LENGTH; i++) {
		if(a == walk_chars[i]) { // the space is open
			return 1;
		}
	}
	return 0;
}

void player_wait(player_t *player, world_t *world)
{
	display_world_message(world, "You stand still!");
}

void player_damage(player_t *player, world_t *world, int attack) {
	player->health -= attack;
	if(player->health <= 0) {
		end_game(world, player);
	}
}

void player_damage_ignore_armor(player_t *player, world_t *world, int attack) {
	player->health -= attack;
	if(player->health <= 0) {
		end_game(world, player);
	}
}

void player_increase_health(player_t *player, int amount) {
	if(player->health + amount > player->max_health) {
		player->health = player->max_health;
	} else {
		player->health += amount;
	}
}

void player_increase_mana(player_t *player, int amount) {
	if(player->mana + amount > player->max_mana) {
		player->mana = player->max_mana;
	} else {
		player->mana += amount;
	}
}


enemy_t *player_get_dir_enemy(player_t *player, world_t *world, direction_t dir, uint8_t range) {
	int x = player->x;
	int y = player->y;
	for(int i = 0; i < range; i++) {
		if(dir == LEFT) x--;
		if(dir == RIGHT) x++;
		if(dir == DOWN) y++;
		if(dir == UP) y--;
		room_t *room = world->room[player->global_x][player->global_y];

		bool found = false;
		char test = room->tiles[y][x]->floor;
		for(int i = 0; i < WALK_CHAR_LENGTH; i++) {
			if(test == walk_chars[i]) { // the space is open
				found = true;
				break;
			}
		}
		if(!found) return NULL;

		if(room->tiles[y][x]->floor )
		for(int i = 0; i < room->current_enemy_count; i++) {
			if(room->enemies[i] == NULL) continue;
			if(x == room->enemies[i]->x && y == room->enemies[i]->y) {
				return room->enemies[i];
			}
		}
	}
    return NULL;
}

double get_weapon_stat_scaling_factor(player_t *player, stats_t stat, double required_stat) {
	double scaling_factor = 0.0f;
	switch(stat) {
		case STRENGTH:
			scaling_factor = player->strength / required_stat;
			break;
		case DEXTERITY:
			scaling_factor = player->dexterity / required_stat;
			break;
		case INTELLIGENCE:
			scaling_factor = player->intelligence / required_stat;
			break;
		case CONSTITUTION:
			scaling_factor = player->constitution / required_stat;
			break;
		case SPEED:
			scaling_factor = player->speed / required_stat;
			break;
		case NULL_STAT:
			break;
	}
	return scaling_factor;
}

/**
 * attacks in a given direction doesn't matter if there is an enemy there or not
 */
void player_attack(player_t *player, world_t *world, direction_t dir) {
	player_exit_attack_state(player, world);

	item_t *main_hand = player->equipment.main_hand;
	if(!main_hand) {
		enemy_t *enemy = player_get_dir_enemy(player, world, dir, 1);
		if(!enemy) return;
		int xp = (enemy->health + enemy->strength) * 5; // TODO this needs changed
		int unarmed_damage = 1;
		char message[MAX_MESSAGE_LENGTH_WITHOUT_PREFIX];
		snprintf(message, MAX_MESSAGE_LENGTH_WITHOUT_PREFIX, "You attacked for %d", unarmed_damage);
		display_combat_message(world, message);
		if(enemy_damage(enemy, world, unarmed_damage)) {
			player_add_xp(player, xp, world->class_data);
		}
		return;
	}
	int raw_damage = 0;
	weapon_stats_t *weapon = &main_hand->stat_type.weapon;
	double required_stat = weapon->min_attack + weapon->max_attack;

	enemy_t *enemy = player_get_dir_enemy(player, world, dir, weapon->range);
	if(!enemy) return;
	int xp = (enemy->health + enemy->strength) * 5; // TODO this needs changed
	
	double scaling_factor_stat1 = get_weapon_stat_scaling_factor(player, weapon->scaling_stat1, required_stat);
	if(scaling_factor_stat1 > 1) scaling_factor_stat1 = 1;
	
	if(weapon->scaling_stat2 != NULL_STAT) {
		double scaling_factor_stat2 = get_weapon_stat_scaling_factor(player, weapon->scaling_stat2, required_stat);
		if(scaling_factor_stat2 > 1) scaling_factor_stat2 = 1;
		double stat1_weight = scaling_factor_stat1 * get_percent_from_grade(weapon->stat1_grade);
		double stat2_weight = scaling_factor_stat2 * get_percent_from_grade(weapon->stat2_grade);
		double rand_weight = (((double)rand() / RAND_MAX) * (1-(get_percent_from_grade(weapon->stat1_grade)+get_percent_from_grade(weapon->stat2_grade))));
		raw_damage = ceil(weapon->max_attack * (stat1_weight + stat2_weight + rand_weight));
	} else {
		double stat_weight = scaling_factor_stat1 * get_percent_from_grade(weapon->stat1_grade);
		double rand_weight = (((double)rand() / RAND_MAX) * (1-get_percent_from_grade(weapon->stat1_grade)));
		raw_damage = ceil(weapon->max_attack * (stat_weight + rand_weight));
	}
	
	raw_damage = MAX(weapon->min_attack, raw_damage);
	if(raw_damage > weapon->max_attack) raw_damage = weapon->max_attack;
	DEBUG_LOG("raw damage: %d", raw_damage);
	if(player_did_crit(player_get_total_crit_chance(weapon))) {
		raw_damage *= 2;
		display_combat_message(world, "You land a crit");
	}
	int damage = raw_damage * (DEFENSE_SCALING_CONSTANT)/(DEFENSE_SCALING_CONSTANT+enemy->defense);
	damage = MAX(1, damage);
	DEBUG_LOG("actual damage: %d", damage);
	char message[MAX_MESSAGE_LENGTH_WITHOUT_PREFIX];
	snprintf(message, MAX_MESSAGE_LENGTH_WITHOUT_PREFIX, "You attacked for %d", damage);
	display_combat_message(world, message);
	if(enemy_damage(enemy, world, damage)) {
		player_add_xp(player, xp, world->class_data);
	}
}

bool player_did_crit(double total_crit_chance) {
	double roll = (double)rand() / RAND_MAX;
	return roll < total_crit_chance;
}

double player_get_total_crit_chance(weapon_stats_t *weapon) {
	// TODO doesn't account for 2 weaopns
	return BASE_CRIT_CHANCE + weapon->crit_chance;
}

int xp_to_level_up(int level) {
	return 50 * level + (level * level * 10);
}

void player_check_level_up(player_t *player, const class_data_t *class_data) {
	int level = player->level;
	int level_xp = xp_to_level_up(level);
	if(player->xp >= level_xp) {
		player->xp -= xp_to_level_up(level);
		player->level++;
		for(int i = 0; i < MAX_CLASSES; i++) {
			if(class_data[i].type == player->player_class) {
				float growth_strength = class_data[i].growth_strength;
				float growth_dexterity = class_data[i].growth_dexterity;
				float growth_intelligence = class_data[i].growth_intelligence;
				float growth_constitution = class_data[i].growth_constitution;
				float growth_speed = class_data[i].growth_speed;
				player->strength += ((int)(growth_strength));
				player->dexterity += ((int)(growth_dexterity));
				player->intelligence += ((int)(growth_intelligence));
				player->constitution += ((int)(growth_constitution));
				player->speed += ((int)(growth_speed));
				break;
			}
		}
	}
}

void player_enter_attack_state(player_t *player, world_t *world) {
	display_combat_message(world, ATTACK_DIRECTION_MESSAGE);
	player->state = PLAYER_STATE_ATTACKING;
}

void player_exit_attack_state(player_t *player, world_t *world) {
	player->state = PLAYER_STATE_MOVING;
}

void player_add_xp(player_t *player, int amount, const class_data_t *class_data) {
	player->xp += amount;
	player_check_level_up(player, class_data);
}

void player_cycle_inv_selector_up(player_t *player)
{
	if(player->inventory_manager.inv_selector > 0) {
		if(player->inventory_manager.inv_selector - player->inventory_manager.inv_offset == 0) {
			player->inventory_manager.inv_offset--;
		}
		player->inventory_manager.inv_selector--;
	}
}

void player_cycle_inv_selector_down(player_t *player)
{
	int visible_item_count = INVENTORY_HEIGHT-2;
	if(player->inventory[player->inventory_manager.inv_selector+1].id != BLANK) {
		if(player->inventory_manager.inv_selector - player->inventory_manager.inv_offset >= visible_item_count-1) {
			player->inventory_manager.inv_offset++;
		}
		player->inventory_manager.inv_selector++;
	}
}

void player_cycle_loot_selector_up(player_t *player) {
	if(player->inventory_manager.loot_selector > 0) {
		if(player->inventory_manager.loot_selector - player->inventory_manager.loot_offset == 0) {
			player->inventory_manager.loot_offset++;
		}
		player->inventory_manager.loot_selector--;
	}
}

void player_cycle_loot_selector_down(player_t *player) {
	int visible_item_count = INVENTORY_HEIGHT-2;
	if(player->nearby_loot[player->inventory_manager.loot_selector+1] != NULL) {
		if(player->inventory_manager.loot_selector - player->inventory_manager.loot_offset >= visible_item_count-1) {
			player->inventory_manager.loot_offset++;
		}
		player->inventory_manager.loot_selector++;
	}
}

void player_open_loot(player_t *player) {
	if(player->nearby_loot_count == 0) return;
	player->state = PLAYER_STATE_LOOTING;
}

void player_close_loot(player_t *player) {
	player->state  = PLAYER_STATE_MOVING;
}


void player_open_inventory(player_t *player) {
	DEBUG_LOG("%s", "open inv");
	player->state  = PLAYER_STATE_INVENTORY;
}

void player_close_inventory(player_t *player) {
	player->state  = PLAYER_STATE_MOVING;
}

int player_inv_contains(player_t *player, item_t item) {
	for(int i = 0; i < player->inventory_count; i++) {
		if(player->inventory[i].id == item.id) {
			return i;
		}
	}
	return -1;
}

bool player_add_to_inv(player_t *player, item_t item) {
	if(item.id == OIL) {
		player->oil += item.stack;
		return true;
	}
	int index = player_inv_contains(player, item);
	if(index != -1) {
		player->inventory[index].stack += item.stack;
		return true;
	}
	if(player->inventory_count < INV_SIZE) {
		player->inventory[player->inventory_count++] = item;
		return true;
	}
	return false;
}

void player_drop_item(player_t *player, world_t *world) {
	item_t item = player->inventory[player->inventory_manager.inv_selector];
	drop_item(world->room[player->global_x][player->global_y]->tiles[player->y][player->x], world->item_data, item.id, item.stack);
	memset(player->inventory+player->inventory_manager.inv_selector, 0, sizeof(item_t));
	player_organize_inv(player, player->inventory_manager.inv_selector);
	player_get_nearby_loot(world->room[player->global_x][player->global_y], player);
}

void player_take_loot_item(room_t *room, player_t *player) {
	item_t *selected_item = player->nearby_loot[player->inventory_manager.loot_selector];
	player_add_to_inv(player, *selected_item);
	int start_y = player->y - 1;
	int start_x = player->x - 1;
	int end_y = player->y + 1;
	int end_x = player->x + 1;
	
	if(start_y < 0) start_y = 0;
	if(start_x < 0) start_x = 0;
	if(end_y >= ROOM_HEIGHT) end_y = ROOM_HEIGHT - 1;
	if(end_x >= ROOM_WIDTH) end_x = ROOM_WIDTH - 1;
	
	bool found = false;
	for(int y = start_y; y <= end_y; y++) {
		for(int x = start_x; x <= end_x; x++) {
			for(int i = 0; i < room->tiles[y][x]->item_count; i++) {
				item_t *item = room->tiles[y][x]->items[i];
				DEBUG_LOG("selected_item: %p, %d ",(void *)selected_item, selected_item->id);
				if(item == selected_item) {
					remove_item_from_tile(room->tiles[y][x], item);
					if(player->inventory_manager.loot_selector == player->nearby_loot_count-1 && player->inventory_manager.loot_selector > 0) {
						player->inventory_manager.loot_selector--;
					}
					break;
				}
			}
			if(found) break;
		}
		if(found) break;
	}
	// need to repopulate the array
	player_get_nearby_loot(room, player);
	if(player->nearby_loot_count == 0) player_open_inventory(player);
}

void player_get_nearby_loot(room_t *room, player_t *player) {
	player_clear_nearby_loot(player);
	int start_y = player->y - 1;
	int start_x = player->x - 1;
	int end_y = player->y + 1;
	int end_x = player->x + 1;
	
	if(start_y < 0) start_y = 0;
	if(start_x < 0) start_x = 0;
	if(end_y >= ROOM_HEIGHT) end_y = ROOM_HEIGHT - 1;
	if(end_x >= ROOM_WIDTH) end_x = ROOM_WIDTH - 1;
	
	for(int y = start_y; y <= end_y; y++) {
		for(int x = start_x; x <= end_x; x++) {
			for(int i = 0; i < MAX_ITEMS_PER_TILE; i++) {
				item_t *item = room->tiles[y][x]->items[i];
				if(item == NULL || item->stack == 0) continue;
				player->nearby_loot[player->nearby_loot_count++] = item;
			}
		}
	}
}

void player_clear_nearby_loot(player_t *player) {
	for(int i = 0; i < player->nearby_loot_count; i++) {
		player->nearby_loot[i] = NULL;
	}
	player->nearby_loot_count = 0;
}

// removes an item from the inventory list and reorganizes, not used to decrease item count
void player_organize_inv(player_t *player, int loc)
{
	for(int i = loc; i < INV_SIZE-1; i++) {
		player->inventory[i] = player->inventory[i + 1];
		if(player->inventory[i].value_type == VALUE_TYPE_ARMOR) {
			if(player->inventory[i].stat_type.armor.equipped == true) {
				player->equipment.armor = &player->inventory[i];
			}
		} else if(player->inventory[i].value_type == VALUE_TYPE_WEAPON) {
			if(player->inventory[i].stat_type.weapon.equipped == true) {
				if(player->inventory[i].stat_type.weapon.main_hand == true) {
						player->equipment.main_hand = &player->inventory[i];
						if(player->inventory[i].stat_type.weapon.two_handed == true) {
							player->equipment.off_hand = &player->inventory[i];
						}
				} else {
					player->equipment.off_hand = &player->inventory[i + 1];
				}
			}
		}
	}
	item_t blank = {BLANK_NAME, "does nothing", BLANK, 0};
	player->inventory[INV_SIZE-1] = blank;
	player->inventory_count--;
	while(player->inventory[player->inventory_manager.inv_selector].id == BLANK && player->inventory_manager.inv_selector > 0) {
		player_cycle_inv_selector_up(player);
	}
}

void player_setup(player_t *player, world_t *world) {
	inventory_manager_t inv_manager = {
		.spells_selector = 0,
		.inv_selector = 0,
		.inv_offset = 0,
		.loot_selector = 0,
		.loot_offset = 0,
		.cat = ITEM
	};
	player->inventory_manager = inv_manager;
	player->x = 1;
	player->y = 10;
	player->player_class = BRAWLER;
	for(int i = 0; i < MAX_CLASSES; i++) {
		if(world->class_data[i].type == player->player_class) {
			int base_strength = world->class_data[i].base_strength;
			int base_dexterity = world->class_data[i].base_dexterity;
			int base_intelligence = world->class_data[i].base_intelligence;
			int base_constitution = world->class_data[i].base_constitution;
			int base_speed = world->class_data[i].base_speed;

			player->strength = base_strength;
			player->dexterity = base_dexterity;
			player->intelligence = base_intelligence;
			player->constitution = base_constitution;
			player->speed = base_speed;
			break;
		}
	}

	player->health = player->constitution * 10;
	player->max_health = player->constitution * 10;
	player->global_x = 0;
	player->global_y = 0;
	player->action_points = 0;
	player->level = 1;
	player->xp = 0;
	player->oil = STARTING_OIL;
	player->equipment = (equipment_t) {0};
	player->state = PLAYER_STATE_MOVING;

	player->inventory = malloc(INV_SIZE * sizeof(item_t));
	player->inventory_count = 0;
	for(int i = 0; i < MAX_ITEMS_NEARBY_PLAYER; i++) {
		player->nearby_loot[i] = malloc(1 * sizeof(item_t));
	}
	player->nearby_loot_count = 0;

	item_t blank = {BLANK_NAME, "does nothing", BLANK, 0};

	for(int i = 0; i < INV_SIZE; i++) {
		player->inventory[i] = blank;
	}

	player->lantern.power = 5;
	player->lantern.is_on = true;
	player->lantern.turns_since_last_dim = 0;
	player->menu_manager = (menu_manager_t){.current_menu = MAIN_MENU, .cursor_pos = 0};
}

void player_reset_values(player_t *player, world_t *world) {
	inventory_manager_t inv_manager = {
		.spells_selector = 0,
		.inv_selector = 0,
		.inv_offset = 0,
		.loot_selector = 0,
		.loot_offset = 0,
		.cat = ITEM
	};

	player->inventory_manager = inv_manager;
	player->x = 1;
	player->y = 10;
	player->player_class = BRAWLER;
	for(int i = 0; i < MAX_CLASSES; i++) {
		if(world->class_data[i].type == player->player_class) {
			int base_strength = world->class_data[i].base_strength;
			int base_dexterity = world->class_data[i].base_dexterity;
			int base_intelligence = world->class_data[i].base_intelligence;
			int base_constitution = world->class_data[i].base_constitution;
			int base_speed = world->class_data[i].base_speed;

			player->strength = base_strength;
			player->dexterity = base_dexterity;
			player->intelligence = base_intelligence;
			player->constitution = base_constitution;
			player->speed = base_speed;
			break;
		}
	}

	player->health = player->constitution * 10;
	player->max_health = player->constitution * 10;
	player->global_x = 0;
	player->global_y = 0;
	player->action_points = 0;
	player->level = 1;
	player->xp = 0;
	player->oil = STARTING_OIL;
	player->equipment = (equipment_t) {0};
	player->state = PLAYER_STATE_MOVING;

	player->inventory_count = 0;
	player->nearby_loot_count = 0;

	item_t blank = {BLANK_NAME, "does nothing", BLANK, 0};

	for(int i = 0; i < INV_SIZE; i++) {
		player->inventory[i] = blank;
	}
	memset(player->inventory, 0, sizeof(item_t)*player->inventory_count);
	for(int i = 0; i < MAX_ITEMS_NEARBY_PLAYER; i++) {
		memset(player->nearby_loot[i], 0, sizeof(item_t));
	}

	player->lantern.power = 5;
	player->lantern.is_on = true;
	player->lantern.turns_since_last_dim = 0;
	// menu manager doesn't need to be cleared, it's always in use

}
