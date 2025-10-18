#ifndef TRAPS_H_
#define TRAPS_H_
#include "types.h"

enum trap_id get_trap_id(const char *id);

void traps_triggered_check(world_t *world, player_t *player);

void load_trap_effects(world_t *world);

void load_trap_data(world_t *world);

#endif
