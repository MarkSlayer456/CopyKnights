#include "types.h"
#include "lantern.h"
#include "game_constants.h"

bool lantern_increase_power(lantern_t *lantern, int *oil) {
	if(lantern->power >= LANTERN_MAX_POWER) return true;
	if(lantern->power >= LANTERN_OVER_LOAD_THRESHOLD && lantern->power <= LANTERN_OVER_LOAD_LOWER_BOUNDS) {
		if(*oil >= LANTERN_OIL_FOR_OVER_LOAD_POWER) {
			*oil -= LANTERN_OIL_FOR_OVER_LOAD_POWER;
			lantern_over_load(lantern);
			return true;
		}
	} else if(lantern->power < LANTERN_NORMAL_POWER) {
		if(*oil >= LANTERN_OIL_FOR_NORMAL_POWER) {
			*oil -= LANTERN_OIL_FOR_NORMAL_POWER;
			lantern_set_normal_power(lantern);
			return true;
		}
	}
	return false;
}

void lantern_over_load(lantern_t *lantern) {
	lantern->power = LANTERN_MAX_POWER;
}

void lantern_set_normal_power(lantern_t *lantern) {
	lantern->power = LANTERN_NORMAL_POWER;
}

void lantern_decrease_power(lantern_t *lantern, int amount) {
	if(lantern->power - amount <= 1) lantern->power = 1;
	else lantern->power -= amount;
}

void lantern_update_dimming(lantern_t *lantern) {
	if(lantern->power > LANTERN_NORMAL_POWER) {
		lantern_set_normal_power(lantern);
		lantern->turns_since_last_dim = 0;
	} else {
		if(lantern->turns_since_last_dim >= LANTERN_TURNS_TO_DIM) {
			lantern_decrease_power(lantern, 1);
			lantern->turns_since_last_dim = 0;
		} else {
			lantern->turns_since_last_dim++;
		}
	}
}
