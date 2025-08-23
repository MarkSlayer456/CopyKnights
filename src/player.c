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
#include "types.h"

#define MIN(a,b)       (a < b) ? a : b

extern char walk_chars[WALK_CHAR_LENGTH];

class_type_map_t class_type_map[] = {
	{NULL_CLASS_NAME, CLASS_NONE},
	{SWORDSMAN_CLASS_NAME, SWORDSMAN},
	{BARBARIAN_CLASS_NAME, BARBARIAN},
	{SPEARMAN_CLASS_NAME, SPEARMAN},
	{ARCHER_CLASS_NAME, ARCHER},
	{WIZARD_CLASS_NAME, WIZARD},
	{MERCHANT_CLASS_NAME, MERCHANT},
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
}

char player_get_current_pos(player_t *player, world_t *world)
{
	return world->room[player->global_x][player->global_y]->tiles[player->y][player->x]->floor;
}

void player_move_left(player_t *player, world_t *world)
{
	if(player_can_move_dir(player, world, LEFT)) {
		player->x -= 1;
		if(player_get_current_pos(player, world) == DOOR) {
			if(player->global_x - 1 < 0) {
				player->x += 1;
				display_world_message(world, player, DOOR_BLOCKED_MESSAGE);
				return;
			}
			player->global_x--;
			if(!world->room[player->global_x][player->global_y]->is_created) {
				world->room[player->global_x][player->global_y] = generate_room(&world->seed, player->global_x, player->global_y, world->enemy_data, world->item_data);
			}
			turn_order_enter_new_room(world, player);
			player->x = ROOM_WIDTH-2;
		}
	}
	world->isPlayerTurn = 0;
}

void player_move_right(player_t *player, world_t *world)
{
	if(player_can_move_dir(player, world, RIGHT)) {
		player->x += 1;
		if(player_get_current_pos(player, world) == DOOR) {
			player->global_x++;
			if(!world->room[player->global_x][player->global_y]->is_created) {
				world->room[player->global_x][player->global_y] = generate_room(&world->seed, player->global_x, player->global_y, world->enemy_data, world->item_data);
			}
			turn_order_enter_new_room(world, player);
			player->x = 1;
		}
	}
	world->isPlayerTurn = 0;
}

void player_move_down(player_t *player, world_t *world)
{
	if(player_can_move_dir(player, world, DOWN)) {
		player->y += 1;
		if(player_get_current_pos(player, world) == DOOR) {
			player->global_y++;
			if(!world->room[player->global_x][player->global_y]->is_created) {
				world->room[player->global_x][player->global_y] = generate_room(&world->seed, player->global_x, player->global_y, world->enemy_data, world->item_data);
			}
			turn_order_enter_new_room(world, player);
			player->y = 1;
		}
	}
	world->isPlayerTurn = 0;
}

void player_move_up(player_t *player, world_t *world)
{
	if(player_can_move_dir(player, world, UP)) {
		player->y -= 1;
		if(player_get_current_pos(player, world) == DOOR) {
			if(player->global_y - 1 < 0) {
				player->y += 1;
				display_world_message(world, player, DOOR_BLOCKED_MESSAGE);
				return;
			}
			player->global_y--;
			if(!world->room[player->global_x][player->global_y]->is_created) {
				world->room[player->global_x][player->global_y] = generate_room(&world->seed, player->global_x, player->global_y, world->enemy_data, world->item_data);
			}
			turn_order_enter_new_room(world, player);
			player->y = ROOM_HEIGHT-2;
		}
	}
	world->isPlayerTurn = 0;
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
	display_world_message(world, player, "You stand still!");
    world->isPlayerTurn = 0;
}

void player_decrease_health(player_t *player, world_t *world, int attack)
{
    player->health -= attack;
    if(player->health <= 0) {
        end_game(world, player);
    }
}

void player_increase_health(player_t *player, int amount)
{
	if(player->health + amount > player->max_health) {
		player->health = player->max_health;
	} else {
		player->health += amount;
	}
}


enemy_t *player_get_dir_enemy(player_t *player, world_t *world, direction_t dir) {
	int x = player->x;
	int y = player->y;
	if(dir == LEFT) x--;
	if(dir == RIGHT) x++;
	if(dir == DOWN) y++;
	if(dir == UP) y--;
	
	room_t *room = world->room[player->global_x][player->global_y];
    for(int i = 0; i < room->current_enemy_count; i++) {
		if(room->enemies[i] == NULL) continue;
		if(x == room->enemies[i]->x && y == room->enemies[i]->y) {
            return room->enemies[i];
        }  
    }
    return NULL;
}

/**
 * This function looks at all knights and will pick the weakest enemy for the knight to attack
 * then call the given attack function
 * TODO redo this whole function it sucks
 */
void player_attack(player_t *player, world_t *world) {
	enemy_t *enL = player_get_dir_enemy(player, world, LEFT);
	enemy_t *enR = player_get_dir_enemy(player, world, RIGHT);
	enemy_t *enD = player_get_dir_enemy(player, world, DOWN);
	enemy_t *enU = player_get_dir_enemy(player, world, UP);
	if(!enL) enL = enemy_create_temp(world);
	if(!enR) enR = enemy_create_temp(world);
	if(!enD) enD = enemy_create_temp(world);
	if(!enU) enU = enemy_create_temp(world);
	if(enL->health == 1000000 && enR->health == 1000000 &&
		enD->health == 1000000 && enU->health == 1000000) {
		return;
	}
	enemy_type_t killed = ENEMY_NONE;
	int res = compare4(enL->health, enR->health, enD->health, enU->health);
	if(res == enL->health) {
		killed = enemy_decrease_health(enL, world, player);
	} else if(res == enR->health) {
		killed = enemy_decrease_health(enR, world, player);
	} else if(res == enD->health) {
		killed = enemy_decrease_health(enD, world, player);
	} else if(res == enU->health) {
		killed = enemy_decrease_health(enU, world, player);
	}
	if(killed != ENEMY_NONE) {
		//TODO each enemy type needs different xp it gives
		player_add_xp(player, 100, world->class_data);
	}
    world->isPlayerTurn = 0;
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
				int base_strength = class_data[i].base_strength;
				int base_dexterity = class_data[i].base_dexterity;
				int base_intelligence = class_data[i].base_intelligence;
				int base_constitution = class_data[i].base_constitution;
				int base_speed = class_data[i].base_speed;
				
				float growth_strength = class_data[i].growth_strength;
				float growth_dexterity = class_data[i].growth_dexterity;
				float growth_intelligence = class_data[i].growth_intelligence;
				float growth_constitution = class_data[i].growth_constitution;
				float growth_speed = class_data[i].growth_speed;
				player->strength = base_strength + ((int)(growth_strength * level));
				player->dexterity = base_dexterity + ((int)(growth_dexterity * level));
				player->intelligence = base_intelligence + ((int)(growth_intelligence * level));
				player->constitution = base_constitution + ((int)(growth_constitution * level));
				player->speed = base_speed + ((int)(growth_speed * level));
				break;
			}
		}
	}
}

void player_add_xp(player_t *player, int amount, const class_data_t *class_data) {
	player->xp += amount;
	player_check_level_up(player, class_data);
}

void player_open_action_bar(player_t *player)
{
	player->action_bar.selector = INVENTORY;
}

void player_close_action_bar(player_t *player)
{
	player->action_bar.selector = NOT_OPEN;
}

void player_cycle_action_bar_down(player_t *player)
{
	if(player->action_bar.selector+1 >= ACTION_BAR_SELECTOR_COUNT) {
		player->action_bar.selector = 0;
	} else {
		player->action_bar.selector += 1;
	}
}

void player_cycle_action_bar_up(player_t *player)
{
	if(player->action_bar.selector <= 0) {
		player->action_bar.selector = ACTION_BAR_SELECTOR_COUNT-1;
	} else {
		player->action_bar.selector -= 1;
	}
}
void player_cycle_inv_selector_up(player_t *player)
{
	if(player->action_bar.inv_selector > 0) {
		if(player->action_bar.inv_selector - player->inv_offset == 0) {
			player->inv_offset--;
		}
		player->action_bar.inv_selector--;
	}
}

void player_cycle_inv_selector_down(player_t *player)
{
	int visible_item_count = INVENTORY_HEIGHT-2;
	if(player->inventory[player->action_bar.inv_selector+1].id != BLANK) {
		if(player->action_bar.inv_selector - player->inv_offset >= visible_item_count-1) {
			player->inv_offset++;
		}
		player->action_bar.inv_selector++;
	}
}

void player_cycle_loot_selector_up(player_t *player) {
	if(player->action_bar.loot_selector > 0) {
		if(player->action_bar.loot_selector - player->action_bar.loot_offset == 0) {
			player->action_bar.loot_offset++;
		}
		player->action_bar.loot_selector--;
	}
}

void player_cycle_loot_selector_down(player_t *player) {
	int visible_item_count = INVENTORY_HEIGHT-2;
	if(player->nearby_loot[player->action_bar.loot_selector+1] != NULL) {
		if(player->action_bar.loot_selector - player->action_bar.loot_offset >= visible_item_count-1) {
			player->action_bar.loot_offset++;
		}
		player->action_bar.loot_selector++;
	}
}

void player_open_loot(player_t *player) {
	if(player->nearby_loot_count == 0) return;
	player->action_bar.selector = LOOT;
}

void player_close_loot(player_t *player) {
	player->action_bar.selector = NOT_OPEN;
}


void player_open_inventory(player_t *player) {
	player->action_bar.selector = INVENTORY;
}

void player_close_inventory(player_t *player) {
	player->action_bar.selector = NOT_OPEN;
}

void player_open_spells(player_t *player) {
	player->action_bar.selector = SPELLS;
}

void player_close_spells(player_t *player) {
	player->action_bar.selector = NOT_OPEN;
}

bool player_add_to_inv(player_t *player, item_t item) {
	if(player->inventory_count < INV_SIZE) {
		player->inventory[player->inventory_count++] = item;
		return true;
	}
	return false;
}

void player_take_loot_item(room_t *room, player_t *player) {
	item_t *selected_item = player->nearby_loot[player->action_bar.loot_selector];
	player_add_to_inv(player, *selected_item);
	int start_y = player->y - 1;
	int start_x = player->x - 1;
	int end_y = player->y + 1;
	int end_x = player->x + 1;
	
	if (start_y < 0) start_y = 0;
	if (start_x < 0) start_x = 0;
	if (end_y >= ROOM_HEIGHT) end_y = ROOM_HEIGHT - 1;
	if (end_x >= ROOM_WIDTH) end_x = ROOM_WIDTH - 1;
	
	for(int y = start_y; y <= end_y; y++) {
		for(int x = start_x; x <= end_x; x++) {
			for(int i = 0; i < MAX_ITEMS_PER_TILE; i++) {
				item_t *item = room->tiles[y][x]->items[i];
				if(item == NULL || item->stack == 0) continue;
				if(selected_item == item) {
					item->stack = 0;
				}
			}
		}
	}
	// need to repopulate the array
	player_get_nearby_loot(room, player);
	if(player->nearby_loot_count == 0) player_open_inventory(player);
}

void player_get_nearby_loot(room_t *room, player_t *player) {
	player_clear_nearby_loot(player);
	player->nearby_loot_count = 0;
	int start_y = player->y - 1;
	int start_x = player->x - 1;
	int end_y = player->y + 1;
	int end_x = player->x + 1;
	
	if (start_y < 0) start_y = 0;
	if (start_x < 0) start_x = 0;
	if (end_y >= ROOM_HEIGHT) end_y = ROOM_HEIGHT - 1;
	if (end_x >= ROOM_WIDTH) end_x = ROOM_WIDTH - 1;
	
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
	player->nearby_loot_count = 0;
	player->nearby_loot[0] = NULL;
}

// removes an item from the inventory list and reorganizes, not used to decrease item count
void player_organize_inv(player_t *player, int loc)
{
	for(int i = loc; i < INV_SIZE-1; i++) {
		player->inventory[i] = player->inventory[i + 1];
	}
	item_t blank = {BLANK_NAME, "does nothing", BLANK, 0};
	player->inventory[INV_SIZE-1] = blank;
	while(player->inventory[player->action_bar.inv_selector].id == BLANK && player->action_bar.inv_selector > 0) {
		player_cycle_inv_selector_up(player);
	}
}





