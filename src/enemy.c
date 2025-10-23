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

type_map_t trait_map[] = {
    {ENEMY_TRAIT_PASSIVE_NAME, PASSIVE},
    {ENEMY_TRAIT_AGGRESSIVE_NAME, AGGRESSIVE},
    {ENEMY_TRAIT_LIGHT_CENTERED_NAME, LIGHT_CENTERED},
    {ENEMY_TRAIT_DARK_CENTERED_NAME, DARK_CENTERED},
    {ENEMY_TRAIT_SURVIVAL_NAME, SURVIVAL},
};

const int trait_map_len = sizeof(trait_map) / sizeof(trait_map[0]);

enum trait enemy_get_trait(const char *name) {
    for(int i = 0; i < trait_map_len; i++) {
        if(strcasecmp(name, trait_map[i].name) == 0) {
            return trait_map[i].value;
        }
    }
    return PASSIVE;
}

enemy_t *enemy_spawn(enemy_type_t type, const enemy_data_t *enemy_data, int x, int y, int global_x, int global_y, biome_t biome)
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
        e->health = enemy_calc_max_health(e);
        e->speed = (int)enemy_data[i].base_speed + ((e->level-1) * ENEMY_GROWTH_MODIFER);
        DEBUG_LOG("speed: %d", e->speed);
        e->defense = (int)enemy_data[i].base_defense + ((e->level-1) * ENEMY_GROWTH_MODIFER);
        DEBUG_LOG("defense: %d", e->defense);
        e->x = x;
        e->y = y;
        e->global_x = global_x;
        e->global_y = global_y;
        e->trait = enemy_data[i].trait;
        e->symbol = enemy_data[i].symbol;
        e->range = enemy_data[i].range;
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
                    enemy_data[row].trait = enemy_get_trait(token);
                    break;
                case 8:
                    enemy_data[row].symbol = token[0];
                    break;
                case 9:
                    enemy_data[row].range = atoi(token);
                    break;

            }
            token = strtok(NULL, ",");
            col++;
        }
        DEBUG_LOG("Loaded Enemy Data: %d, %d, %d, %d, %d, %d, %d, %c, %d, %d", enemy_data[row].type,
                  enemy_data[row].base_strength, enemy_data[row].base_dexterity, enemy_data[row].base_intelligence,
                  enemy_data[row].base_constitution, enemy_data[row].base_speed, enemy_data[row].base_defense, enemy_data[row].symbol, enemy_data[row].trait, enemy_data[row].range);
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

        DEBUG_LOG("DROPS DATA: %d %d %f", enemy_type, item_id, drop_chance);

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
            item_drop_t *drop = enemy_data[i].drop_table.drops;
            if(drop_count == 0) continue;
            // int index = rand() % drop_count;
            for(int index = 0; index < drop_count; index++) {
                float chance = (float) rand() / (float) RAND_MAX;
                if(drop[index].drop_chance >= chance) {
                    int quantity = (rand() % (drop[index].max_quantity - drop[index].min_quantity + 1)) + drop[index].min_quantity;
                    drop_item(tile, item_data, drop[index].id, quantity);
                }
            }
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
			enemy_decide_move_passive(enemy, world, player);
			break;
		case AGGRESSIVE:
            enemy_decide_move_aggressive(enemy, world, player);
			break;
		case LIGHT_CENTERED:
            enemy_decide_move_light_centered(enemy, world, player);
			break;
        case DARK_CENTERED:
            enemy_decide_move_dark_centered(enemy, world, player);
            break;
        case SURVIVAL:
            // enemy_decide_move_survival(enemy, world, player);
            enemy_create_path_lists(enemy);
            enemy_find_path_to_target(enemy, world->room[player->global_x][player->global_y], player, 10, 10);
            path_node_t *step = enemy_find_next_node(enemy);
            if(!step) return;
            enemy->x = step->x;
            enemy->y = step->y;
            DEBUG_LOG("enemy pos: %d, %d", enemy->x, enemy->y);
            enemy_free_path_lists(enemy);
            break;
	}
}

void enemy_decide_move_passive(enemy_t *enemy, world_t *world, player_t *player) {
    int max_health = (int)(enemy->constitution + ((enemy->level-1) * ENEMY_GROWTH_MODIFER)) * 10;
    if(max_health > enemy->health) {
        enemy_decide_move_aggressive(enemy, world, player);
    } else {
        float chance = (float)rand() / (float)RAND_MAX;
        if(chance <= .20) {
            if(enemy_can_move_dir(enemy, world, player, LEFT)) {
                enemy->x--;
            }
        } else if(chance <= .40) {
            if(enemy_can_move_dir(enemy, world, player, RIGHT)) {
                enemy->x++;
            }
        } else if(chance <= .60) {
            if(enemy_can_move_dir(enemy, world, player, DOWN)) {
                enemy->y++;
            }
        } else if(chance <= .80) {
            if(enemy_can_move_dir(enemy, world, player, UP)) {
                enemy->y--;
            }
        }
    }
}

void enemy_decide_move_aggressive(enemy_t *enemy, world_t *world, player_t *player) {
    if(enemy_attempt_attack(enemy, world, player)) {
        return;
    }
    enemy_move_toward_location(enemy, world, player, player->y, player->x);
}

void enemy_decide_move_dark_centered(enemy_t *enemy, world_t *world, player_t *player) {
    //TODO should consider if another enemy is already in the spot they want to move to
    int player_top = player->y - player->lantern.power;
    int player_bottom = player->y + player->lantern.power;
    int player_left = player->x - player->lantern.power;
    int player_right = player->x + player->lantern.power;

    int top_dist = abs(player_top - enemy->y) + abs(player->x - enemy->x);
    int bottom_dist = abs(player_bottom - enemy->y) + abs(player->x - enemy->x);
    int right_dist = abs(player->y - enemy->y) + abs(player_right - enemy->x);
    int left_dist = abs(player->y - enemy->y) + abs(player_left - enemy->x);

    int shortest = compare4(top_dist, bottom_dist, right_dist, left_dist);
    if(enemy_attempt_attack(enemy, world, player)) {
        return;
    }
    if(shortest == top_dist) {
        enemy_move_toward_location(enemy, world, player, player_top, player->x);
    } else if(shortest == bottom_dist) {
        enemy_move_toward_location(enemy, world, player, player_bottom, player->x);
    } else if(shortest == right_dist) {
        enemy_move_toward_location(enemy, world, player, player->y, player_right);
    } else if(shortest == left_dist) {
        enemy_move_toward_location(enemy, world, player, player->y, player_left);
    }
}

void enemy_decide_move_light_centered(enemy_t *enemy, world_t *world, player_t *player) {

}

void enemy_decide_move_survival(enemy_t *enemy, world_t *world, player_t *player) {
    int enemy_max_health = enemy_calc_max_health(enemy);
    if(enemy->health <= enemy_max_health * ENEMY_SURVIVAL_RUN_FACTOR) {
        int x = -1, y = -1;
        find_suitable_tile_away_from_player(enemy, world->room[player->global_x][player->global_y], player, ENEMY_SURVIVAL_MIN_RANGE, &y, &x);
        if(x == -1 && y == -1) {
            if(enemy_attempt_attack(enemy, world, player)) {
                return;
            }
            enemy_move_toward_location(enemy, world, player, player->y, player->x);
            return;
        }
        DEBUG_LOG("values: %d, %d", x, y);
        enemy_move_toward_location(enemy, world, player, y, x);
    } else {
        if(enemy_attempt_attack(enemy, world, player)) {
            return;
        }
        enemy_move_toward_location(enemy, world, player, player->y, player->x);
    }
}

void enemy_move_toward_location(enemy_t *enemy, world_t *world, player_t *player, int y, int x) {
    if(x < enemy->x) {
        if(enemy_can_move_dir(enemy, world, player, LEFT)) {
            enemy->x-=1;
            return;
        }
    }
    if(x > enemy->x) {
        if(enemy_can_move_dir(enemy, world, player, RIGHT)) {
            enemy->x+=1;
            return;
        }
    }
    if(y < enemy->y) {
        if(enemy_can_move_dir(enemy, world, player, UP)) {
            enemy->y-=1;
            return;
        }
    }
    if(y > enemy->y) {
        if(enemy_can_move_dir(enemy, world, player, DOWN)) {
            enemy->y+=1;
            return;
        }
    }
}

int enemy_attempt_attack(enemy_t *enemy, world_t *world, player_t *player) {
    for(int i = 1; i <= enemy->range; i++) {
        if((enemy->y-i == player->y && enemy->x == player->x) ||
        (enemy->y+i == player->y && enemy->x == player->x) ||
        (enemy->x-i == player->x && enemy->y == player->y) ||
        (enemy->x+i == player->x && enemy->y == player->y)) {
            enemy_attack(enemy, player, world);
            return 1;
        }
    }
    return 0;
}


char enemy_check_dir(enemy_t *enemy, world_t *world, player_t *player, direction_t dir) {
    int x = enemy->x;
    int y = enemy->y;
    if(dir == LEFT) x--;
    if(dir == RIGHT) x++;
    if(dir == DOWN) y++;
    if(dir == UP) y--;
    if(x == player->x && y == player->y) {
        return PLAYER;
    }
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

int enemy_calc_max_health(const enemy_t *enemy) {
    return (int)(enemy->constitution + ((enemy->level-1) * ENEMY_GROWTH_MODIFER)) * 10;
}

void find_suitable_tile_away_from_player(const enemy_t *enemy, const room_t *room, const player_t *player, int range, int *x, int *y) {
    int left_dist = player->x-range;
    int right_dist = player->x+range;
    int bottom_dist = player->y+range;
    int top_dist = player->y-range;

    int shortest = compare4(left_dist, right_dist, bottom_dist, top_dist);

    if(shortest == top_dist) {
        if(check_tile(room, player, top_dist, player->x) == EMPTY) {
            *y = top_dist;
            *x = player->x;
        }
    } else if(shortest == bottom_dist) {
        if(check_tile(room, player, bottom_dist, player->x) == EMPTY) {
            *y = bottom_dist;
            *x = player->x;
        }
    } else if(shortest == right_dist) {
        if(check_tile(room, player, right_dist, player->x) == EMPTY) {
            *y = player->y;
            *x = right_dist;
        }
    } else if(shortest == left_dist) {
        if(check_tile(room, player, left_dist, player->x) == EMPTY) {
            *y = player->y;
            *x = left_dist;
        }
    }
}

bool enemy_can_walk(char symbol) {
    for(int i = 0; i < WALK_CHAR_LENGTH; i++) {
        if(symbol == walk_chars[i]) {
            return 1;
        }
    }
    return 0;
}

void enemy_create_path_lists(enemy_t *enemy) {
    enemy->olist = calloc(ENEMY_PATH_LIST_DEFAULT_SIZE, sizeof(path_node_t));
    enemy->olist_size = ENEMY_PATH_LIST_DEFAULT_SIZE;
    for (int y = 0; y < ROOM_HEIGHT; y++) {
        for (int x = 0; x < ROOM_WIDTH; x++) {
            enemy->all_nodes[y][x].g = 10000;
        }
    }
}

void enemy_add_to_olist(enemy_t *enemy, path_node_t node) {
    if(enemy->olist_count == 0) {
        enemy->olist[0] = node;
        enemy->olist_count++;
        return;
    }
    if(enemy->olist_count+1 >= enemy->olist_size) {
        enemy->olist_size *= 2;
        enemy->olist = realloc(enemy->olist, sizeof(path_node_t)*enemy->olist_size);
        memset(enemy->olist+(enemy->olist_size/2), 0, (enemy->olist_size/2)*sizeof(path_node_t));
    }
    int index = enemy->olist_count;
    for(int i = 0; i < enemy->olist_count; i++) {
        if(enemy->olist[i].f > node.f) {
            index = i;
            break;
        }
    }
    for(int j = enemy->olist_count; j > index; j--) {
        enemy->olist[j] = enemy->olist[j-1];
    }
    enemy->olist[index] = node;
    enemy->olist_count++;
}

void enemy_remove_from_olist(enemy_t *enemy, int index) {
    for(int i = index; i < enemy->olist_count; i++) {
        enemy->olist[i] = enemy->olist[i+1];
    }
    enemy->olist_count--;
}

path_node_t enemy_pop_from_olist(enemy_t *enemy) {
    path_node_t node = {0};
    if(enemy->olist_count == 0) {
        return node;
    }
    node = enemy->olist[0];
    enemy_remove_from_olist(enemy, 0);
    return node;
}

void enemy_find_path_to_target(enemy_t *enemy, room_t *room, player_t *player, int endy, int endx) {
    enemy->start_x = enemy->x;
    enemy->start_y = enemy->y;
    enemy->end_y = endy;
    enemy->end_x = endx;
    int dist = abs(enemy->start_y  - endy) + abs(enemy->start_x - endx);
    path_node_t start_node = (path_node_t) {
        .x = enemy->start_x,
        .y = enemy->start_y,
        .h = dist,
        .g = 0,
        .px = enemy->start_x,
        .py = enemy->start_y
    };
    start_node.f = start_node.g + start_node.h;
    start_node.checked = true;
    enemy->all_nodes[start_node.y][start_node.x] = start_node;
    enemy_add_to_olist(enemy, start_node);
    while(enemy->olist_count > 0) {
        path_node_t node = enemy_pop_from_olist(enemy);
        // DEBUG_LOG("Pop: (%d,%d) g=%d h=%d f=%d\n", node.x, node.y, node.g, node.h, node.f);
        enemy->clist[node.y][node.x] = true;
        if(node.x == endx && node.y == endy) {
            // DEBUG_LOG("%s", "reached end");
            return;
        }
        path_node_t neighbors[4];
        int neighbor_count = get_path_node_neighbors(node, neighbors, enemy, room, player);
        for(int i = 0; i < neighbor_count; i++) {
            enemy_add_to_olist(enemy, neighbors[i]);
        }
    }
}

path_node_t *enemy_find_next_node(enemy_t *enemy) {
    int steps = 0;
    const int max_steps = ROOM_WIDTH * ROOM_HEIGHT;
    path_node_t *cur = &enemy->all_nodes[enemy->end_y][enemy->end_x];
    path_node_t *prev = NULL;

    while(cur->x != enemy->start_x || cur->y != enemy->start_y) {
        prev = cur;
        cur = &enemy->all_nodes[cur->py][cur->px];
        if(steps >= max_steps) {
            return NULL;
        }
        steps++;
    }

    return prev;
}

int get_path_node_neighbors(path_node_t node, path_node_t neighbors[4], enemy_t *enemy, room_t *room, player_t *player) {
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {1, 0, -1, 0};
    int count = 0;

    for(int i = 0; i < 4; i++) {
        int nx = node.x + dx[i];
        int ny = node.y + dy[i];

        if(nx < 0 || ny < 0 || nx > ROOM_WIDTH-1 || ny > ROOM_HEIGHT-1) {
            continue;
        }
        if(enemy->clist[ny][nx] == true) {
            continue;
        }
        if(!enemy_can_walk(check_tile(room, player, ny, nx))) {
            continue;
        }
        if(enemy->all_nodes[ny][nx].checked) {
            continue;
        }
        int move_cost = 1;
        int new_g = node.g + move_cost;
        int cur_g = enemy->all_nodes[ny][nx].g;
        if(new_g > cur_g) {
            continue;
        }
        neighbors[count].checked = true;
        //TODO trap tiles modifing speed
        int dist = abs(nx - enemy->end_x) + abs(ny - enemy->end_y);
        neighbors[count].x = nx;
        neighbors[count].y = ny;
        neighbors[count].g = node.g + move_cost;
        neighbors[count].h = dist;
        neighbors[count].f = neighbors[count].g + neighbors[count].h;
        neighbors[count].px = node.x;
        neighbors[count].py = node.y;
        enemy->all_nodes[ny][nx] = neighbors[count];
        // DEBUG_LOG("Neighbor (%d,%d) g=%d h=%d f=%d\n", nx, ny, neighbors[count].g, neighbors[count].h, neighbors[count].f);
        count++;
    }
    return count;
}

void enemy_free_path_lists(enemy_t *enemy) {
    free(enemy->olist);
    memset(enemy->all_nodes, 0, sizeof(enemy->all_nodes));
    for (int y = 0; y < ROOM_HEIGHT; y++) {
        for (int x = 0; x < ROOM_WIDTH; x++) {
            enemy->all_nodes[y][x].g = 10000;
        }
    }
    memset(enemy->clist, 0, sizeof(enemy->clist));
    enemy->olist_size = ENEMY_PATH_LIST_DEFAULT_SIZE;
    enemy->olist_count = 0;
    enemy->olist = NULL;
}
