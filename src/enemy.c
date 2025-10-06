#include "game_manager.h"
#include "enemy.h"
#include "player.h"
#include "hud.h"
#include "map_manager.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <assert.h>
#include "types.h"
#include "functions.h"
#include "items/items.h"

extern char walk_chars[WALK_CHAR_LENGTH];
enemy_type_map_t biome_type_map[] = {
    {NULL_ENEMY_NAME, ENEMY_NONE},
    {CAVE_NAME, CAVE},
    {CATACOMBS_NAME, CATACOMBS},
    {BOG_NAME, BOG},
    {ANCIENT_CITY_NAME, ANCIENT_CITY},
    {ARCANE_LABYRINTH_NAME, ARCANE_LABYRINTH},
    {VOID_HOLLOW_NAME, VOID_HOLLOW},
};

const int biome_type_map_len = sizeof(biome_type_map) / sizeof(biome_type_map[0]);

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

enemy_t *enemy_spawn(enemy_type_t type, const enemy_data_t *enemy_data, int x, int y, biome_t biome)
{
    if(enemy_data == NULL) return NULL;
    DEBUG_LOG("%s", "spawning enemy...");
    int i = 0;
    while(i < MAX_ENEMIES_PER_LEVEL) {
        if(enemy_data[i].type != type) {
            i++;
            continue;
        }
        enemy_t *e = calloc(1, sizeof(enemy_t));
        e->type = enemy_data[i].type;
        DEBUG_LOG("Type: %s", enemy_get_name(e->type));
        
        unsigned int seed = cantor_pair(x, y);
        int range = enemy_data[i].highest_level[biome] - enemy_data[i].lowest_level[biome] + 1;
        int level = (rand_r_portable(&seed) % range) +  enemy_data[i].lowest_level[biome];
        
        e->level = level;
        DEBUG_LOG("Level: %d", e->level);
        e->strength = (int)enemy_data[i].base_strength + ((e->level-1) * ENEMY_GROWTH_MODIFER);
        DEBUG_LOG("str: %d", e->strength);
        e->dexterity = (int)enemy_data[i].base_dexterity + ((e->level-1) * ENEMY_GROWTH_MODIFER);
        DEBUG_LOG("dex: %d", e->dexterity);
        e->intelligence = (int)enemy_data[i].base_intelligence + ((e->level-1) * ENEMY_GROWTH_MODIFER);
        DEBUG_LOG("int: %d", e->intelligence);
        e->constitution = (int)enemy_data[i].base_constitution + ((e->level-1) * ENEMY_GROWTH_MODIFER);
        DEBUG_LOG("con: %d", e->constitution);
        e->health = (int)(enemy_data[i].base_constitution + ((e->level-1) * ENEMY_GROWTH_MODIFER)) * 10;
        e->speed = (int)enemy_data[i].base_speed + ((e->level-1) * ENEMY_GROWTH_MODIFER);
        DEBUG_LOG("speed: %d", e->speed);
        e->defense = (int)enemy_data[i].base_defense + ((e->level-1) * ENEMY_GROWTH_MODIFER);
        DEBUG_LOG("defense: %d", e->defense);
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

void load_biome_data(enemy_data_t *enemy_data) {
    FILE *fp = fopen("./data/biome_spawns.csv", "r");
    if(!fp) {
        perror("File open failed");
        return;
    }
    
    char line[2048];
    
    if(fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return;
    }
    
    int row = 0;
    while(fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        char *biome_str = strtok(line, ",");
        char *enemy_str = strtok(NULL, ",");
        char *min_str = strtok(NULL, ",");
        char *max_str = strtok(NULL, ",");
        
        if(!biome_str || !enemy_str || !min_str || !max_str) {
            continue;
        }
        
        biome_t biome = get_biome(biome_str);
        if(biome == BIOME_NULL) {
            continue;
        }
        
        enemy_type_t enemy_type = enemy_get_type(enemy_str);
        int min_level = atoi(min_str);
        int max_level = atoi(max_str);
        
        int i = 0;
        while(i < MAX_ENEMIES) {
            if(enemy_data[i].type != enemy_type) {
                i++;
                continue;
            }
            enemy_data[i].can_spawn[biome] = true;
            enemy_data[i].highest_level[biome] = max_level;
            enemy_data[i].lowest_level[biome] = min_level;
            break;
        }
        
        row++;
    }
}

biome_t get_biome(const char *name) {
    for(int i = 0; i < biome_type_map_len; i++) {
        if(strcasecmp(name, biome_type_map[i].name) == 0) {
            return biome_type_map[i].value;
        }
    }
    return BIOME_NULL; // or an INVALID_TRAIT enum
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

enemy_type_t enemy_generate_type(unsigned int *seed, enemy_data_t *enemy_data, biome_t biome)
{
    enemy_type_t enemies[MAX_ENEMIES];
    int enemies_size = 0;
    for(int i = 0; i < MAX_ENEMIES; i++) {
        if(enemy_data[i].can_spawn[biome]) {
            enemies[enemies_size++] = enemy_data[i].type;
        }
    }
    assert(enemies_size > 0);
    int random_number = (rand_r_portable(seed) % enemies_size);
    return enemies[random_number];
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
            enemy_data[row].drop_table.drops = NULL;
            switch(col) {
                case 0:
                    enemy_data[row].type = enemy_get_type(token);
                    break;
                case 1:
                    enemy_data[row].base_strength = atoi(token);
                    break;
                case 2:
                    enemy_data[row].base_dexterity = atoi(token);
                    break;
                case 3:
                    enemy_data[row].base_intelligence = atoi(token);
                    break;
                case 4:
                    enemy_data[row].base_constitution = atoi(token);
                    break;
                case 5:
                    enemy_data[row].base_speed = atoi(token);
                    break;
                case 6:
                    enemy_data[row].base_defense = atoi(token);
                    break;
                case 7:
                    // TODO
                    // enemy_data[row].trait = atoi(token);
                    break;
                case 8:
                    enemy_data[row].symbol = token[0];
                    break;
                    
            }
            token = strtok(NULL, ",");
            col++;
        }
        DEBUG_LOG("Loaded Enemy Data: %d, %d, %d, %d, %d, %d, %d, %c", enemy_data[row].type, 
                  enemy_data[row].base_strength, enemy_data[row].base_dexterity, enemy_data[row].base_intelligence,
                  enemy_data[row].base_constitution, enemy_data[row].base_speed, enemy_data[row].base_defense, enemy_data[row].symbol);
        col = 0;
        row++;
    }
    
    load_biome_data(enemy_data);
    
}

void load_enemy_drop_data(enemy_data_t *enemy_data) {
    FILE *fp = fopen("./data/enemy_drops.csv", "r");
    if(!fp) {
        perror("enemy_drops.csv File open failed");
        return;
    }
    
    char line[2048];
    
    if(fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return;
    }
    
    while(fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        char *enemy_name = strtok(line, ",");
        char *item_name = strtok(NULL, ",");
        char *drop_chance_str = strtok(NULL, ",");
        char *min_quantity_str = strtok(NULL, ",");
        char *max_quantity_str = strtok(NULL, ",");
        
        if(!enemy_name || !item_name || !drop_chance_str || !min_quantity_str || !max_quantity_str) {
            continue;
        }
        
        DEBUG_LOG("drop for: %s, %s", enemy_name, item_name);
        
        enemy_type_t enemy_type = enemy_get_type(enemy_name);
        item_ids_t item_id = item_get_id(item_name);
        float drop_chance = atof(drop_chance_str);
        int min_quantity = atoi(min_quantity_str);
        int max_quantity = atoi(max_quantity_str);
        
        DEBUG_LOG("DROPS DATA: %d %d", enemy_type, item_id);
        
        int i = 0;
        while(i < MAX_ENEMIES) {
            if(enemy_type != enemy_data[i].type) {
                i++;
                continue;
            }
            
            int drop_count = enemy_data[i].drop_table.drop_count;
            enemy_data[i].drop_table.drops = realloc(enemy_data[i].drop_table.drops, sizeof(item_drop_t) * (drop_count+1));
            enemy_data[i].drop_table.drops[drop_count].id = item_id;
            enemy_data[i].drop_table.drops[drop_count].drop_chance = drop_chance;
            enemy_data[i].drop_table.drops[drop_count].min_quantity = min_quantity;
            enemy_data[i].drop_table.drops[drop_count].max_quantity = max_quantity;
            enemy_data[i].drop_table.drop_count++;
            break;
        }
    }
}

void enemy_kill(enemy_t *enemy, world_t *world, const player_t *player) 
{
    room_t *room = world->room[player->global_x][player->global_y];
    enemy_handle_death_drops(enemy, world->enemy_data, world->item_data, room->tiles[enemy->y][enemy->x]);
	for(int i = 0; i < MAX_ENEMIES_PER_LEVEL; i++) {
		if(enemy && enemy == room->enemies[i]) {
            enemy_t *tmp = room->enemies[i];
            for(int j = i; j < room->current_enemy_count-1; j++) {
                room->enemies[j] = room->enemies[j+1];
            }
            free(tmp);
            room->enemies[room->current_enemy_count-1] = NULL;
            room->current_enemy_count--;
            break;
		}
	}
}

void enemy_handle_death_drops(enemy_t *enemy, enemy_data_t *enemy_data, item_data_t *item_data, tile_t *tile) {
    for(int i = 0; i < MAX_ENEMIES; i++) {
        if(enemy_data[i].type == enemy->type) {
            int drop_count = enemy_data[i].drop_table.drop_count;
            if(drop_count == 0) return;
            int index = rand() % drop_count;
            item_drop_t *drop = enemy_data[i].drop_table.drops;
            int quantity = (rand() % (drop[index].max_quantity - drop[index].min_quantity + 1)) + drop[index].min_quantity;
            drop_item(tile, item_data, drop[index].id, quantity);
            break;
        }
    }
}

/*
 * returns true on kill returns false otherwise
 */
bool enemy_decrease_health(enemy_t *enemy, world_t *world, const player_t *player, int amount)
{
    if(!enemy) return false;
    enemy->health -= amount;
	if(enemy->health <= 0) {
		enemy_kill(enemy, world, player);
        return true;
	}
	return false;
}

void enemy_attack(enemy_t *enemy, player_t *player, world_t *world)
{
    char message[MAX_MESSAGE_LENGTH_WITHOUT_PREFIX];
    snprintf(message, MAX_MESSAGE_LENGTH_WITHOUT_PREFIX, "[%c] attacked for %d", enemy->symbol, enemy->strength);
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
                    break;
                } else if(enemy->x-1 == player->x && enemy->y == player->y) {
                    enemy_attack(enemy, player, world);
                    break;
                }
			} 
			if(player->x > enemy->x) {
                if(enemy_can_move_dir(enemy, world, player, RIGHT) && (enemy->x+1 != player->x || enemy->y != player->y)) {
                    enemy->x+=1;
                    break;
                } else if(enemy->x+1 == player->x && enemy->y == player->y) {
                    enemy_attack(enemy, player, world);
                    break;
                }
			} 
			if(player->y < enemy->y) {
                if(enemy_can_move_dir(enemy, world, player, UP) && (enemy->y-1 != player->y || enemy->x != player->x)) {
                    enemy->y-=1;
                    break;
                } else if(enemy->y-1 == player->y && enemy->x == player->x) {
                    enemy_attack(enemy, player, world);
                    break;
                }
			} 
			if(player->y > enemy->y) {
                if(enemy_can_move_dir(enemy, world, player, DOWN) && (enemy->y+1 != player->y || enemy->x != player->x)) {
                    enemy->y+=1;
                    break;
                } else if(enemy->y+1 == player->y && enemy->x == player->x) {
                    enemy_attack(enemy, player, world);
                    break;
                }
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
    room_t *room = world->room[player->global_x][player->global_y];
    for(int i = 0; i < room->current_enemy_count; i++) {
        enemy_t *tmp = room->enemies[i];
        if(tmp->x == x && tmp->y == y) {
            return tmp->symbol;
        }
    }
    if(x > 0 && y > 0) {
        return room->tiles[y][x]->floor;
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
