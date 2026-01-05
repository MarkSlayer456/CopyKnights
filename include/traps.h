#ifndef TRAPS_H_
#define TRAPS_H_
#include "types.h"

enum trap_id get_trap_id(const char *id);

void traps_triggered_check_player(world_t *world, player_t *player);
void traps_triggered_check_enemies(world_t *world, room_t *room);

void remove_trap(room_t *room, int x, int y);

void load_trap_effects(world_t *world);

void load_trap_data(world_t *world);

#endif
