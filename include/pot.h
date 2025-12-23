#ifndef POT_H_
#define POT_H_
#include "types.h"
#include <stdbool.h>

void pot_break(world_t *world, room_t *room, pot_t *pot);

void pot_handle_break_drops(world_t *world, room_t *room, int y, int x);

void pot_spawn(room_t *room, int y, int x);

void load_pot_data(world_t *world);

#endif
