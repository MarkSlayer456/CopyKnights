#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include "character_types.h"
#include "buff.h"
#include "player.h"
#include "enemy.h"
#include "items/item_types.h"
#include "types.h"

type_map_t buff_type_map[] = {
	{BUFF_NULL_NAME, BUFF_NULL},
	{BUFF_BLEED_NAME, BUFF_BLEED},
	{BUFF_BURN_NAME, BUFF_BURN},
	{BUFF_FROST_NAME, BUFF_FROST},
	{BUFF_POISON_NAME, BUFF_POISON},
	{BUFF_BLACKENING_NAME, BUFF_BLACKENING},
	{BUFF_HEAL_NAME, BUFF_REGEN},
};

const int buff_type_map_len = sizeof(buff_type_map) / sizeof(buff_type_map[0]);

const char *buff_get_name(int16_t value) {
	for(int i = 0; i < buff_type_map_len; i++) {
		if(value == buff_type_map[i].value) {
			return buff_type_map[i].name;
		}
	}
	return BUFF_NULL_NAME;
}

enum buff_type buff_get_type(const char *name) {
	for(int i = 0; i < buff_type_map_len; i++) {
		if(strcasecmp(name, buff_type_map[i].name) == 0) {
			return buff_type_map[i].value;
		}
	}
	return BUFF_NULL;
}

void buff_remove_effects(buff_t *buff_array, uint8_t index) {
	buff_t buff = buff_array[index];
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

void buff_apply(buff_t *buff_array, uint8_t *buff_count, world_t *world) {
	for(int i = 0; i < (*buff_count); i++) {
		buff_t *buff = &buff_array[i];
		if(buff->turns_left == 0) {
			buff_remove_effects(buff_array, i);
			buff_remove_from_list(i, buff_array, buff_count);
			return;
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
		//TODO probably need to calculate and account for armor that resists these effects
		if(buff->target_type_id == TARGET_PLAYER) {
			player_damage(buff->target.player, world, buff->damage);
		} else if(buff->target_type_id == TARGET_ENEMY) {
			enemy_damage(buff->target.enemy, world, buff->damage);
		}
		buff->turns_left--;
	} 
}

void buff_remove_from_list(uint8_t index, buff_t *buff_array, uint8_t *buff_count) {
	if(index < (*buff_count)) {
		for(uint8_t i = index; i < (*buff_count)-1; i++) {
			buff_array[i] = buff_array[i+1];
		}
		(*buff_count)--;
	}
	memset(&buff_array[*buff_count], 0, sizeof(buff_t));
}

buff_t *buff_add_to_list(buff_t buff, buff_t *buff_array, uint8_t *buff_count, uint8_t *buff_size) {
	if((*buff_count)+1 >= (*buff_size)) {
		if(*buff_size == 0) {
			*buff_size += 2;
		} else {
			*buff_size *= 2;
		}
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
	if(buff1->type == buff2->type) {
		bool match = false;
		if(buff1->target_type_id == buff2->target_type_id) {
			if(buff1->target_type_id == TARGET_PLAYER) {
				// should only be one player but just in case
				if(buff1->target.player == buff2->target.player) {
					match = true;
				} 
			}
		} else if(buff1->target_type_id == TARGET_ENEMY) {
			if(buff1->target.enemy == buff2->target.enemy) {
				match = true;
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

void buff_set_type(buff_t *buff, enum buff_type type) {
	buff->type = type;
	strcpy(buff->name, buff_get_name(type));
}

buff_t buff_create() {
	buff_t buff = (buff_t){0};
	return buff;
}
