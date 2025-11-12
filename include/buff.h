#ifndef BUFFS_H_
#define BUFFS_H_
#include "types.h"
#include <stdint.h>

const char *buff_get_name(int16_t value);

enum buff_type buff_get_type(const char *name);

void buff_apply(buff_t *buff_array, uint8_t *buff_count, world_t *world);

void buff_remove_from_list(uint8_t index, buff_t *buff_array, uint8_t *buff_count);

buff_t *buff_add_to_list(buff_t buff, buff_t *buff_array, uint8_t *buff_count, uint8_t *buff_size);

void buff_combine(buff_t *buff1, buff_t *buff2);

void buff_set_type();

buff_t buff_create();


#endif
