#ifndef LANTERN_H_

#define LANTERN_H_
#include "types.h"
#include <stdbool.h>

bool lantern_increase_power(lantern_t *lantern, int *oil);

void lantern_over_load(lantern_t *lantern);

void lantern_set_normal_power(lantern_t *lantern);

void lantern_decrease_power(lantern_t *lantern, int amount);

void lantern_update_dimming(lantern_t *lantern);

#endif
