#ifndef BUFFS_H_
#define BUFFS_H_

void buff_apply(buff_t *buff_array, int *buff_count);

void buff_remove_from_list(uint8_t index, buff_t *buff_array, int *buff_count) {

void buff_add_to_list(buff_t buff, buff_t *buff_array, uint8_t *buff_count, uint8_t *buff_size);

void buff_combine(buff_t *buff1, buff_t *buff2);

buff_t *buff_create();


#endif
