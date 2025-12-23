#include "pot.h"
#include "types.h"
#include "functions.h"
#include "items/items.h"
#include "game_constants.h"
#include <stdbool.h>
#include <stdlib.h>

void pot_break(world_t *world, room_t *room, pot_t *pot) {
	pot->broken = true;
	pot_handle_break_drops(world, room, pot->y, pot->x);
}

void pot_handle_break_drops(world_t *world, room_t *room, int y, int x) {
	unsigned int cantor = cantor_pair(x, y);
	unsigned int map_seed = cantor ^ (world->seed);
	map_seed *= SEED_PRIMER;
	map_seed ^= (world->seed) >> 15;
	float chance = (float) rand() / (float) RAND_MAX;
	DEBUG_LOG("chance: %f", chance);
	if(chance <= POT_ITEM_SPAWN_CHANCE) {
		item_spawn(item_generate_type(&map_seed, world->item_data, room->biome), room, room->tiles[y][x], world->item_data);
	}
	return;
}

void pot_spawn(room_t *room, int x, int y) {
	pot_t pot = {.x = x, .y = y, .broken = false};
	if(room->current_pot_count >= MAX_POTS_PER_ROOM) return;
	room->pots[room->current_pot_count++] = pot;
}

void load_pot_data(world_t *world) {
	return;
}
