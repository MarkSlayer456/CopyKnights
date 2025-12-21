#include "pot.h"
#include "types.h"
#include <stdbool.h>

void pot_break(world_t *world, pot_t *pot) {
	pot->broken = true;
	pot_handle_break_drops(world, pot->x, pot->y);
}

void pot_handle_break_drops(world_t *world, int x, int y) {
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
