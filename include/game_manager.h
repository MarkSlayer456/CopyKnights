#ifndef GAME_MANAGER_H_
#define GAME_MANAGER_H_
#include <ncurses.h>
#include <stdbool.h>

#define DEBUG_LOG(fmt, ...) \
do { \
	FILE *f = fopen("debug.log", "a"); \
	if (f) { \
		fprintf(f, fmt "\n", ##__VA_ARGS__); \
		fclose(f); \
	} \
} while (0)
// EXAMPLE: DEBUG_LOG("Constitution: %d", enemy_data[row].constitution);

typedef struct class_data class_data_t;
typedef enum class_type class_type_t;
typedef struct player player_t;
typedef struct world world_t;
typedef struct room room_t;

void draw(world_t *world, player_t *player);
void manage_input(char c, world_t *world, player_t *player);

void display_world_message(world_t *world, player_t *player, const char *str);

void display_combat_message(world_t *world, player_t *player, const char *str);

bool is_opaque(room_t *room, int x, int y);

void mark_has_light(room_t *room, int x, int y);

void cast_light_check(world_t *world, player_t *player, int x0, int y0, float angle);

void calculate_light(world_t *world, player_t *player);

int pick_next_actor(world_t *world, player_t *player);

void turn_order_enter_new_room(world_t *world, player_t *player);

void end_game(world_t *world, player_t *player);

void shutdown(world_t *world);

#endif
