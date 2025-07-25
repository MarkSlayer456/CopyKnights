#include "game_manager.h"
#include "enemy.h"
#include "player.h"
#include "hud.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

extern char walk_chars[WALK_CHAR_LENGTH];

enemy_type_map_t type_map[] = {
    {"RAT", RAT},
    {"BAT", BAT},
    {"SLIME", SLIME},
    {"SKELETON", SKELETON},
    {"DRAGON", DRAGON}
};

const int type_map_len = sizeof(type_map) / sizeof(type_map[0]);

enemy_t *enemy_create_temp(world_t *world) 
{
	enemy_t *e = malloc(sizeof(enemy_t));
	e->health = 1000000;
	// e->attack = 0;
	// e->defense = 0;
	e->x = 0;
	e->y = 0;
	strcpy(e->name, "");
	//TODO trait?
	return e;
}

enemy_t *enemy_spawn(enemy_type_t type, const enemy_data_t *enemy_data, int x, int y)
{
    if(enemy_data == NULL) return NULL;
    
    int i = 0;
    while(i < MAX_ENEMIES) {
        if(enemy_data[i].type != type) {
            i++;
            continue;
        }
        enemy_t *e = malloc(sizeof(enemy_t));
        e->type = enemy_data[i].type;
        DEBUG_LOG("Type: %s", enemy_get_name(e->type));
        e->strength = enemy_data[i].strength;
        e->dexterity = enemy_data[i].dexterity;
        e->intelligence = enemy_data[i].intelligence;
        e->constitution = enemy_data[i].constitution;
        e->health = enemy_data[i].constitution; // TODO some calculation to determine health
        e->x = x;
        e->y = y;
        e->trait = PASSIVE;
        strcpy(e->name, enemy_get_name(type));
        return e;
    }
    return NULL;
}

const char *enemy_get_name(enemy_type_t type)
{
    switch(type) {
        case SLIME:
            return SLIME_ENEMY_NAME;
        case BAT:
            return BAT_ENEMY_NAME;
        case RAT:
            return RAT_ENEMY_NAME;
        case SKELETON:
            return SKELETON_ENEMY_NAME;
        case DRAGON:
            return DRAGON_ENEMY_NAME;
        default:
            return "NULL";
    }
}

enemy_type_t enemy_get_type(const char *name)
{
    for(int i = 0; i < type_map_len; i++) {
        if(strcmp(name, type_map[i].name) == 0) {
            return type_map[i].value;
        }
    }
    return -1; // or an INVALID_TRAIT enum
}

enemy_type_t enemy_generate_type(unsigned int *seed)
{
    int random_number = (rand_r(seed) % ENEMY_TYPE_COUNT);
    return random_number;
}

void load_enemy_data(enemy_data_t *enemy_data) {
    FILE *fp = fopen("./data/enemies.csv", "r");
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
                    enemy_data[row].type = enemy_get_type(token);
                    break;
                case 1:
                    enemy_data[row].strength = atoi(token);
                    break;
                case 2:
                    enemy_data[row].dexterity = atoi(token);
                    break;
                case 3:
                    enemy_data[row].intelligence = atoi(token);
                    break;
                case 4:
                    enemy_data[row].constitution = atoi(token);
                    break;
            }
            token = strtok(NULL, ",");
            col++;
        }
        col = 0;
        row++;
    }
}

void enemy_kill(enemy_t *enemy, world_t *world, player_t *player) 
{
    room_t *room = &world->room[player->global_x][player->global_y];
	int found = 0;
	for(int i = 0; i < room->current_enemy_count; i++) {
		if(enemy == room->enemies[i]) {
			room->enemies[i] = NULL;
			found = 1;
		}
		if(found == 1) {
			room->enemies[i] = room->enemies[i+1];
		}
	}
	room->current_enemy_count--;
}

void enemy_decrease_health(enemy_t *enemy, world_t *world, player_t *player, int knightNum)
{
    enemy->health -= player->attack[knightNum];
	if(enemy->health <= 0) {
		enemy_kill(enemy, world, player);
	}
}

void enemy_attack(enemy_t *enemy, player_t *player, world_t *world, int knightNum)
{
    player_decrease_health(player, world, enemy->strength, knightNum);
}

void enemy_decide_move(enemy_t *enemy, world_t *world, player_t *player) 
{
	// TODO need to check which knight is the closest
    // TODO move the player check to enemy_can_move functions; just makes more sense, also need loop for them
	switch(enemy->trait) {
		case PASSIVE:
			if(player->x[0] < enemy->x) {
                if(enemy_can_move_left(enemy, world, player) && (enemy->x-1 != player->x[0] || enemy->y != player->y[0])) {
                    enemy->x-=1;
                } else if(enemy->x-1 == player->x[0] && enemy->y == player->y[0]) {
                    enemy_attack(enemy, player, world, 0);
                }
			} else if(player->x[0] > enemy->x) {
                if(enemy_can_move_right(enemy, world, player) && (enemy->x+1 != player->x[0] || enemy->y != player->y[0])) {
                    enemy->x+=1;
                } else if(enemy->x+1 == player->x[0] && enemy->y == player->y[0]) {
                    enemy_attack(enemy, player, world, 0);
                }
			} else if(player->y[0] < enemy->y) {
                if(enemy_can_move_up(enemy, world, player) && (enemy->y-1 != player->y[0] || enemy->x != player->x[0])) {
                    enemy->y-=1;
                } else if(enemy->y-1 == player->y[0] && enemy->x == player->x[0]) {
                    enemy_attack(enemy, player, world, 0);
                }
			} else if(player->y[0] > enemy->y) {
                if(enemy_can_move_down(enemy, world, player) && (enemy->y+1 != player->y[0] || enemy->x != player->x[0])) {
                    enemy->y+=1;
                } else if(enemy->y+1 == player->y[0] && enemy->x == player->x[0]) {
                    enemy_attack(enemy, player, world, 0);
                }
			} else {
                enemy_attack(enemy, player, world, 0);
            }
			break;
		case AGRESSIVE:
			break;
		case TACTICAL:
			break;
	}
}

char enemy_check_right(enemy_t *enemy, world_t *world, player_t *player)
{
	room_t *room = &world->room[player->global_x][player->global_y];
    if(enemy->x + 1 > 0) {
      return room->layout[enemy->y][enemy->x + 1];
    }
    return ' ';
}

char enemy_check_left(enemy_t *enemy, world_t *world, player_t *player)
{
	room_t room = world->room[player->global_x][player->global_y];
    if(enemy->x - 1 > 0) {
      return room.layout[enemy->y][enemy->x - 1];
    }
    return ' ';
}

char enemy_check_up(enemy_t *enemy, world_t *world, player_t *player)
{
	room_t *room = &world->room[player->global_x][player->global_y];
    if(enemy->y - 1 > 0) {
      return room->layout[enemy->y - 1][enemy->x];
    }
    return ' ';
}

char enemy_check_down(enemy_t *enemy, world_t *world, player_t *player)
{
	room_t room = world->room[player->global_x][player->global_y];
    if(enemy->y + 1 > 0) {
      return room.layout[enemy->y + 1][enemy->x];
    }
    return ' ';
}


int enemy_can_move_right(enemy_t *enemy, world_t *world, player_t *player)
{
    char a = enemy_check_right(enemy, world, player);
    for(int i = 0; i < WALK_CHAR_LENGTH; i++) {
        if(a == walk_chars[i]) {
            return 1;
        }
    }
    return 0;
}

int enemy_can_move_left(enemy_t *enemy, world_t *world, player_t *player)
{
    char a = enemy_check_left(enemy, world, player);
    for(int i = 0; i < WALK_CHAR_LENGTH; i++) {
        if(a == walk_chars[i]) {
            return 1;
        }
    }
    return 0;
}

int enemy_can_move_up(enemy_t *enemy, world_t *world, player_t *player)
{
    char a = enemy_check_up(enemy, world, player);
    for(int i = 0; i < WALK_CHAR_LENGTH; i++) {
        if(a == walk_chars[i]) {
            return 1;
        }
    }
    return 0;
}

int enemy_can_move_down(enemy_t *enemy, world_t *world, player_t *player)
{
    char a = enemy_check_down(enemy, world, player);
    for(int i = 0; i < WALK_CHAR_LENGTH; i++) {
        if(a == walk_chars[i]) {
            return 1;
        }
    }
    return 0;
}



