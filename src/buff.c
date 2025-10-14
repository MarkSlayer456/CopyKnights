#include "buff.h"

void buff_remove_effects(buff_t *buff_array, int buff_count) {
	for(int i = 0; i < buff_count; i++) {
		buff_t buff = buff_array[i];
		if(buff.target_type_id == TARGET_PLAYER) {
			player_t *player = buff.target.player;
			player->strength -= buff.flat_strength; 
			player->dexterity -= buff.flat_dexterity; 
			player->intelligence -= buff.flat_intelligence; 
			player->constitution -= buff.flat_constitution; 
			player->speed -= buff.flat_speed; 
			player->strength /= (1 + buff.percent_strength); 
			player->dexterity /= (1 + buff.percent_dexterity); 
			player->intelligence /= (1 + buff.percent_intelligence); 
			player->constitution /= (1 + buff.percent_constitution); 
			player->speed /= (1 + buff.percent_speed); 
		} else if(buff.target_type_id == TARGET_ENEMY) {
			enemy_t *enemy = buff.target.enemy;
			enemy->strength -= buff.flat_strength; 
			enemy->dexterity -= buff.flat_dexterity; 
			enemy->intelligence -= buff.flat_intelligence; 
			enemy->constitution -= buff.flat_constitution; 
			enemy->speed -= buff.flat_speed; 
			enemy->strength /= (1 + buff.percent_strength); 
			enemy->dexterity /= (1 + buff.percent_dexterity); 
			enemy->intelligence /= (1 + buff.percent_intelligence); 
			enemy->constitution /= (1 + buff.percent_constitution); 
			enemy->speed /= (1 + buff.percent_speed); 
		}
	}
}

void buff_apply(buff_t *buff_array, int *buff_count) {
	for(int i = 0; i < (*buff_count); i++) {
		buff_t *buff = &buff_array[i];
		if(buff->turns_left == 0) {
			buff_remove_effects(buff_array,(*buff_count));
			buff_remove_from_list(i, buff_array, buff_count);
		} else if(!buff->applied) {
			if(buff->target_type_id == TARGET_PLAYER) {
				player_t *player = buff->target.player;
				player->strength += buff->flat_strength; 
				player->dexterity += buff->flat_dexterity; 
				player->intelligence += buff->flat_intelligence; 
				player->constitution += buff->flat_constitution; 
				player->speed += buff->flat_speed; 
				player->strength *= (1 + buff->percent_strength); 
				player->dexterity *= (1 + buff->percent_dexterity); 
				player->intelligence *= (1 + buff->percent_intelligence); 
				player->constitution *= (1 + buff->percent_constitution); 
				player->speed *= (1 + buff->percent_speed); 
			} else if(buff->target_type_id == TARGET_ENEMY) {
				enemy_t *enemy = buff->target.enemy;
				enemy->strength += buff->flat_strength; 
				enemy->dexterity += buff->flat_dexterity; 
				enemy->intelligence += buff->flat_intelligence; 
				enemy->constitution += buff->flat_constitution; 
				enemy->speed += buff->flat_speed; 
				enemy->strength *= (1 + buff->percent_strength); 
				enemy->dexterity *= (1 + buff->percent_dexterity); 
				enemy->intelligence *= (1 + buff->percent_intelligence); 
				enemy->constitution *= (1 + buff->percent_constitution); 
				enemy->speed *= (1 + buff->percent_speed); 
			}
			buff->applied = true;
		}
		buff->turns_left--;
	} 
}

void buff_remove_from_list(uint8_t index, buff_t *buff_array, int *buff_count) {
	if(index < (*buff_count)) {
		for(int i = index; i < (*buff_count)-1; i++) {
			buff_array[i] = buff_array[i+1];
		}
		(*buff_count)--;
		memset(&buff_array[*buff_count], 0, sizeof(buff_t));
	}
}

buff_t *buff_add_to_list(buff_t buff, buff_t *buff_array, int *buff_count, uint8_t *buff_size) {
	if((*buff_count)+1 >= (*buff_size)) {
		*buff_size *= 2;
		buff_t *tmp = realloc(buff_array, sizeof(buff_t)  * (*buff_size));
		if(!tmp) {
			return buff_array;
		}
		buff_array = tmp;
	}
	buff_array[(*buff_count)] = buff;
	(*buff_count)++; 
	return buff_array;
}

void buff_combine(buff_t *buff1, buff_t *buff2) {
	if(strcasecmp(buff1->name, buff2->name) == 0) {
		if(buff1->target_type_id == buff2->target_type_id) {
			bool match = false;
			if(buff1->target_type_id == TARGET_PLAYER) {
				// should only be one player but just in case
				if(buff1->target.player == buff2->target.player) {
					match = true;
				} 
			}
		} else if(buff1->target_type_id == TARGET_ENEMY) {
			if(buff1->target_type_id == ENEMY) {
				if(buff1->target.enemy == buff2->target.enemy) {
					match = true;
				}
			}
		}
		if(match) {
			buff1->flat_strength += buff2->flat_strength;
			buff1->flat_dexterity += buff2->flat_dexterity;
			buff1->flat_intelligence += buff2->flat_intelligence;
			buff1->flat_constitution += buff2->flat_constitution;
			buff1->flat_speed += buff2->flat_speed;
			buff1->percent_strength += buff2->percent_strength;
			buff1->percent_dexterity += buff2->percent_dexterity;
			buff1->percent_intelligence += buff2->percent_intelligence;
			buff1->percent_constitution += buff2->percent_constitution;
			buff1->percent_speed += buff2->percent_speed;
		}
	}
}

buff_t buff_create() {
	buff_t buff = (buff_t){0};
	return buff;
}
