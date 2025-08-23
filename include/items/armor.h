#ifndef ARMOR_H_
#define ARMOR_H_
#include "types.h"

armor_type_t armor_get_type(const char *name);

void load_armor_spawn_data(world_t *world);
void load_armor_effects(world_t *world);
void load_armor_data(world_t *world);

#endif
