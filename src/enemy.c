#include "game_manager.h"
#include "enemy.h"
#include "player.h"
#include "hud.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

extern char walk_chars[WALK_CHAR_LENGTH];

enemy_type_map_t type_map[] = {
    {NULL_ENEMY_NAME, ENEMY_NONE},
    {RAT_ENEMY_NAME, RAT},
    {BAT_ENEMY_NAME, BAT},
    {SLIME_ENEMY_NAME, SLIME},
    {SKELETON_ENEMY_NAME, SKELETON},
    {DRAGON_ENEMY_NAME, DRAGON},
    {BABY_DRAGON_ENEMY_NAME, BABY_DRAGON},
    {GOLBIN_ENEMY_NAME, GOBLIN},
    {GHOST_ENEMY_NAME, GHOST},
    {LOOT_GOBLIN_ENEMY_NAME, LOOT_GOBLIN},
    {MOSS_BEAST_ENEMY_NAME, MOSS_BEAST},
    {JESTER_ENEMY_NAME, JESTER},
    {VOIDLING_ENEMY_NAME, VOIDLING},
    {MARROW_CRAWLER_ENEMY_NAME, MARROW_CRAWLER},
    {VOID_MAW_ENEMY_NAME, VOID_MAW},
    {MARROW_LEECH_ENEMY_NAME, MARROW_LEECH},
    {MUD_CRAWLER_ENEMY_NAME, MUD_CRAWLER},
    {BOG_LURKER_ENEMY_NAME, BOG_LURKER},
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
    DEBUG_LOG("spawning enemy...");
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
        DEBUG_LOG("str: %d", e->strength);
        e->dexterity = enemy_data[i].dexterity;
        DEBUG_LOG("dex: %d", e->dexterity);
        e->intelligence = enemy_data[i].intelligence;
        DEBUG_LOG("int: %d", e->intelligence);
        e->constitution = enemy_data[i].constitution;
        DEBUG_LOG("con: %d", e->constitution);
        e->health = enemy_data[i].constitution * 10;
        e->speed = enemy_data[i].speed;
        DEBUG_LOG("speed: %d", e->speed);
        e->x = x;
        e->y = y;
        e->trait = PASSIVE;
        e->symbol = enemy_data[i].symbol;
        e->action_points = 0;
        strcpy(e->name, enemy_get_name(type));
        i++;
        return e;
    }
    return NULL;
}

const char *enemy_get_name(enemy_type_t type)
{
    for(int i = 0; i < type_map_len; i++) {
        if(type_map[i].value == type) {
            return type_map[i].name;
        }
    }
    return NULL;
}

enemy_type_t enemy_get_type(const char *name)
{
    for(int i = 0; i < type_map_len; i++) {
        if(strcasecmp(name, type_map[i].name) == 0) {
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
                case 5:
                    enemy_data[row].speed = atoi(token);
                    break;
                case 6:
                    // TODO
                    // enemy_data[row].trait = atoi(token);
                    break;
                case 7:
                    enemy_data[row].symbol = token[0];
                    break;
                    
            }
            token = strtok(NULL, ",");
            col++;
        }
        DEBUG_LOG("Loaded Enemy Data: %d, %d, %d, %d, %d, %d, %d", enemy_data[row].type, 
                  enemy_data[row].strength, enemy_data[row].dexterity, enemy_data[row].intelligence,
                  enemy_data[row].constitution, enemy_data[row].speed, enemy_data[row].symbol);
        col = 0;
        row++;
    }
}

void enemy_kill(enemy_t *enemy, world_t *world, player_t *player) 
{
    room_t *room = &world->room[player->global_x][player->global_y];
	// int found = 0;
	for(int i = 0; i < room->current_enemy_count; i++) {
		if(enemy == room->enemies[i]) {
			room->enemies[i] = NULL;
			// found = 1;
		}
		// if(found == 1) {
		// 	room->enemies[i] = room->enemies[i+1];
		// }
	}
	// room->current_enemy_count--;
}

enemy_type_t enemy_decrease_health(enemy_t *enemy, world_t *world, player_t *player)
{
    enemy->health -= player->strength;
	if(enemy->health <= 0) {
        enemy_type_t type = enemy_get_type(enemy->name);
		enemy_kill(enemy, world, player);
        return type;
	}
	return ENEMY_NONE;
}

void enemy_attack(enemy_t *enemy, player_t *player, world_t *world)
{
    char *message = calloc(MAX_MESSAGE_LENGTH_WITHOUT_PREFIX, sizeof(char));
    snprintf(message, MAX_MESSAGE_LENGTH_WITHOUT_PREFIX, "%s attacked for %d", enemy->name, enemy->strength);
    display_combat_message(world, player, message);
    player_decrease_health(player, world, enemy->strength);
}

void enemy_decide_move(enemy_t *enemy, world_t *world, player_t *player) 
{
    // TODO move the player check to enemy_can_move functions; just makes more sense, also need loop for them
    switch(enemy->trait) {
		case PASSIVE:
			if(player->x < enemy->x) {
                if(enemy_can_move_dir(enemy, world, player, LEFT) && (enemy->x-1 != player->x || enemy->y != player->y)) {
                    enemy->x-=1;
                } else if(enemy->x-1 == player->x && enemy->y == player->y) {
                    enemy_attack(enemy, player, world);
                }
			} else if(player->x > enemy->x) {
                if(enemy_can_move_dir(enemy, world, player, RIGHT) && (enemy->x+1 != player->x || enemy->y != player->y)) {
                    enemy->x+=1;
                } else if(enemy->x+1 == player->x && enemy->y == player->y) {
                    enemy_attack(enemy, player, world);
                }
			} else if(player->y < enemy->y) {
                if(enemy_can_move_dir(enemy, world, player, UP) && (enemy->y-1 != player->y || enemy->x != player->x)) {
                    enemy->y-=1;
                } else if(enemy->y-1 == player->y && enemy->x == player->x) {
                    enemy_attack(enemy, player, world);
                }
			} else if(player->y > enemy->y) {
                if(enemy_can_move_dir(enemy, world, player, DOWN) && (enemy->y+1 != player->y || enemy->x != player->x)) {
                    enemy->y+=1;
                } else if(enemy->y+1 == player->y && enemy->x == player->x) {
                    enemy_attack(enemy, player, world);
                }
			} else {
                enemy_attack(enemy, player, world);
            }
			break;
		case AGRESSIVE:
			break;
		case TACTICAL:
			break;
	}
}

char enemy_check_dir(enemy_t *enemy, world_t *world, player_t *player, direction_t dir) {
    int x = enemy->x;
    int y = enemy->y;
    if(dir == LEFT) x--;
    if(dir == RIGHT) x++;
    if(dir == DOWN) y++;
    if(dir == UP) y--;
    room_t *room = &world->room[player->global_x][player->global_y];
    if(x > 0 && y > 0) {
        return room->layout[y][x];
    }
    return ' ';
}

int enemy_can_move_dir(enemy_t *enemy, world_t *world, player_t *player, direction_t dir) {
    char a = enemy_check_dir(enemy, world, player, dir);
    for(int i = 0; i < WALK_CHAR_LENGTH; i++) {
        if(a == walk_chars[i]) {
            return 1;
        }
    }
    return 0;
}
