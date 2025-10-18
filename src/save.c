#include "save.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "map_manager.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

const char *get_save_path() {
	//TODO save file loaction nedds changed
	#ifdef _WIN32
	static char path[512];
	snprintf(path, sizeof(path), "%s\\CopyKnights\\save.bin", getenv("APPDATA"));
	return path;
	#else
	static char path[512];
	snprintf(path, sizeof(path), "%s/.CopyKnights/", getenv("HOME"));
	return path;
	#endif
}

void ensure_save_folder() {
	#ifdef _WIN32
	char folder[512];
	strcpy(folder, get_save_path());
	// Remove the filename
	char *last_slash = strrchr(folder, '\\');
	if (last_slash) *last_slash = '\0';
	CreateDirectory(folder, NULL);
	#else
	char folder[512];
	strcpy(folder, get_save_path());
	// Remove the filename
	char *last_slash = strrchr(folder, '/');
	if (last_slash) *last_slash = '\0';
	mkdir(folder, 0755);
	#endif
}

void save_game(world_t *world, player_t *player, char *name) {
	char path[512];
	snprintf(path, sizeof(path), "%s%s", get_save_path(), name);
	FILE *file = fopen(path, "wb");
	if(!file) {
		DEBUG_LOG("saving error: failed reading file: %s", path);
	}
	save_world(world, file);
	save_player(player, file);
	fclose(file);
	DEBUG_LOG("game saved: %s", path);
}

void load_game(world_t *world, player_t *player, char *name) {
	char path[512];
	snprintf(path, sizeof(path), "%s%s", get_save_path(), name);
	FILE *file = fopen(path, "rb");
	if(!file) {
		DEBUG_LOG("loading error: failed reading file: %s", path);
	}
	load_world(world, player, file);
	load_player(player, file, world->item_data);
	fclose(file);
	DEBUG_LOG("game loaded: %s", path);
}

void save_player(player_t *player, FILE *file) {
	DEBUG_LOG("%s", "saving player");
	if(!player) {
		DEBUG_LOG("%s", "player null");
	}
	fwrite(&player->level, sizeof(int), 1, file);
	fwrite(&player->xp, sizeof(int), 1, file);
	fwrite(&player->health, sizeof(int), 1, file);
	fwrite(&player->max_health, sizeof(int), 1, file);
	fwrite(&player->strength, sizeof(float), 1, file);
	fwrite(&player->dexterity, sizeof(float), 1, file);
	fwrite(&player->intelligence, sizeof(float), 1, file);
	fwrite(&player->constitution, sizeof(float), 1, file);
	fwrite(&player->speed, sizeof(float), 1, file);
	fwrite(&player->x, sizeof(int), 1, file);
	fwrite(&player->y, sizeof(int), 1, file);
	fwrite(&player->global_x, sizeof(int), 1, file);
	fwrite(&player->global_y, sizeof(int), 1, file);
	
	fwrite(&player->inventory_count, sizeof(int), 1, file);
	for(int i = 0; i < player->inventory_count; i++) {
		fwrite(&player->inventory[i].id, sizeof(item_ids_t), 1, file);
		fwrite(&player->inventory[i].stack, sizeof(int), 1, file);
		if(player->inventory[i].value_type == VALUE_TYPE_WEAPON) {
			fwrite(&player->inventory[i].stat_type.weapon.equipped, sizeof(bool), 1, file);
		} else if(player->inventory[i].value_type == VALUE_TYPE_ARMOR) {
			fwrite(&player->inventory[i].stat_type.armor.equipped, sizeof(bool), 1, file);
		}
	}
	fwrite(&player->nearby_loot_count, sizeof(int), 1, file);
	for(int i = 0; i < player->nearby_loot_count; i++) {
		fwrite(&player->nearby_loot[i]->id, sizeof(item_ids_t), 1, file);
		fwrite(&player->nearby_loot[i]->stack, sizeof(int), 1, file);
	}
	
	fwrite(&player->mana, sizeof(int), 1, file);
	fwrite(&player->max_mana, sizeof(int), 1, file);
	fwrite(&player->action_points, sizeof(int), 1, file);
	fwrite(&player->player_class, sizeof(class_type_t), 1, file);
	fwrite(&player->lantern, sizeof(lantern_t), 1, file);
	
	fwrite(&player->equipment.armor_id, sizeof(item_ids_t), 1, file);
	fwrite(&player->equipment.main_hand_id, sizeof(item_ids_t), 1, file);
	fwrite(&player->equipment.off_hand_id, sizeof(item_ids_t), 1, file);
	fwrite(&player->equipment.main_hand_two_handed, sizeof(bool), 1, file);
	
	fwrite(&player->oil, sizeof(int), 1, file);
	fwrite(&player->state, sizeof(player_state_t), 1, file);
	fwrite(&player->inventory_manager, sizeof(inventory_manager_t), 1, file);
	
}

void save_world(world_t *world, FILE *file) {
	for(int x = 0; x < WORLD_WIDTH; x++) {
		for(int y = 0; y < WORLD_HEIGHT; y++) {
			if(!world->room[x][y]->is_created) {
				int invalid = -1;
				fwrite(&invalid, sizeof(int), 1, file);
				fwrite(&invalid, sizeof(int), 1, file);
				continue;
			}
			fwrite(&x, sizeof(int), 1, file);
			fwrite(&y, sizeof(int), 1, file);
			save_room(world->room[x][y], file);
		}
	}
	fwrite(&world->seed, sizeof(int), 1, file);
	
	fwrite(&world->messages_size, sizeof(int), 1, file);
	fwrite(&world->max_message_storage, sizeof(int), 1, file);
	
	for(int i = 0; i < world->messages_size; i++) {
		int len = strlen(world->messages[i]) + 1;
		fwrite(&len, sizeof(int), 1, file);
		fwrite(world->messages[i], sizeof(char), len, file);
	}
	
	fwrite(&world->turn_order_size, sizeof(int), 1, file);
	fwrite(world->turn_order, sizeof(int), world->turn_order_size, file);
	fwrite(&world->room_template_count, sizeof(int), 1, file);
	fwrite(&world->room_templates, sizeof(room_template_t), world->room_template_count, file);
	fwrite(&world->buff_size, sizeof(uint8_t), 1, file);
	fwrite(&world->buff_count, sizeof(uint8_t), 1, file);
	for(int i = 0; i < world->buff_count; i++) {
		save_buff(&world->buffs[i], world, file);
	}
}

void save_room(room_t *room, FILE *file) {
	fwrite(room->room_file_name, sizeof(char), ROOM_FILE_NAME_MAX_SIZE, file);
	for(int x = 0; x < ROOM_WIDTH; x++) {
		for(int y = 0; y < ROOM_HEIGHT; y++) {
			// fwrite(&room->tiles[y][x]->floor, sizeof(char), 1, file);
			fwrite(&room->tiles[y][x]->item_count, sizeof(int8_t), 1, file);
			for(int i = 0; i < room->tiles[y][x]->item_count; i++) {
				fwrite(&room->tiles[y][x]->items[i]->id, sizeof(item_ids_t), 1, file);
				fwrite(&room->tiles[y][x]->items[i]->stack, sizeof(int), 1, file);
			}
			fwrite(&room->tiles[y][x]->deleted_trap_count, sizeof(uint8_t), 1, file);
			for(int i = 0; i < room->tiles[y][x]->deleted_trap_count; i++) {
				fwrite(&room->tiles[y][x]->deleted_trap_x[i], sizeof(uint8_t), 1, file);
				fwrite(&room->tiles[y][x]->deleted_trap_y[i], sizeof(uint8_t), 1, file);
			}
		}
	}
	fwrite(&room->current_enemy_count, sizeof(int8_t), 1, file);
	for (int i = 0; i < room->current_enemy_count; i++) {
		fwrite(room->enemies[i], sizeof(enemy_t), 1, file);
	}
	fwrite(&room->is_created, sizeof(bool), 1, file);
	fwrite(&room->global_time, sizeof(int), 1, file);
	fwrite(&room->biome, sizeof(biome_t), 1, file);
	fwrite(&room->is_main_path, sizeof(bool), 1, file);
	fwrite(&room->door_mask, sizeof(uint8_t), 1, file);
}

void save_buff(buff_t *buff, world_t *world, FILE *file) {
	fwrite(&buff->turns_left, sizeof(int), 1, file);
	fwrite(&buff->name, sizeof(char), BUFF_NAME_MAX_LEN, file);
	fwrite(&buff->type, sizeof(enum buff_type), 1, file);
	fwrite(&buff->flat_strength, sizeof(float), 1, file);
	fwrite(&buff->flat_dexterity, sizeof(float), 1, file);
	fwrite(&buff->flat_intelligence, sizeof(float), 1, file);
	fwrite(&buff->flat_constitution, sizeof(float), 1, file);
	fwrite(&buff->flat_speed, sizeof(float), 1, file);
	fwrite(&buff->percent_strength, sizeof(float), 1, file);
	fwrite(&buff->percent_dexterity, sizeof(float), 1, file);
	fwrite(&buff->percent_intelligence, sizeof(float), 1, file);
	fwrite(&buff->percent_constitution, sizeof(float), 1, file);
	fwrite(&buff->percent_speed, sizeof(float), 1, file);
	fwrite(&buff->applied, sizeof(bool), 1, file);
	fwrite(&buff->target_type_id, sizeof(enum target_type), 1, file);
	if(buff->target_type_id == TARGET_ENEMY) {
		fwrite(&buff->target.enemy->global_x, sizeof(int), 1, file);
		fwrite(&buff->target.enemy->global_y, sizeof(int), 1, file);
		fwrite(&buff->target.enemy->x, sizeof(int), 1, file);
		fwrite(&buff->target.enemy->y, sizeof(int), 1, file);
	}
}

void load_buff(buff_t *buff, world_t *world, player_t *player, FILE *file) {
	fread(&buff->turns_left, sizeof(int), 1, file);
	fread(&buff->name, sizeof(char), BUFF_NAME_MAX_LEN, file);
	fread(&buff->type, sizeof(enum buff_type), 1, file);
	fread(&buff->flat_strength, sizeof(float), 1, file);
	fread(&buff->flat_dexterity, sizeof(float), 1, file);
	fread(&buff->flat_intelligence, sizeof(float), 1, file);
	fread(&buff->flat_constitution, sizeof(float), 1, file);
	fread(&buff->flat_speed, sizeof(float), 1, file);
	fread(&buff->percent_strength, sizeof(float), 1, file);
	fread(&buff->percent_dexterity, sizeof(float), 1, file);
	fread(&buff->percent_intelligence, sizeof(float), 1, file);
	fread(&buff->percent_constitution, sizeof(float), 1, file);
	fread(&buff->percent_speed, sizeof(float), 1, file);
	fread(&buff->applied, sizeof(bool), 1, file);
	fread(&buff->target_type_id, sizeof(enum target_type), 1, file);
	if(buff->target_type_id == TARGET_ENEMY) {
		int target_gx, target_gy, target_x, target_y;
		fread(&target_gx, sizeof(int), 1, file);
		fread(&target_gy, sizeof(int), 1, file);
		fread(&target_x, sizeof(int), 1, file);
		fread(&target_y, sizeof(int), 1, file);
		//TODO this is insane and probably won't even work... this is like 5 or 6 nested loops
		for(int gx = 0; gx < WORLD_WIDTH; gx++) {
			for(int gy = 0; gy < WORLD_HEIGHT; gy++) {
				room_t *room = world->room[gx][gy];
				if(!room->is_created) continue;
				for(int x = 0; x < ROOM_WIDTH; x++) {
					for(int y = 0; y < ROOM_HEIGHT; y++) {
						for(int i = 0; i < room->current_enemy_count; i++) {
							enemy_t *enemy = room->enemies[i];
							if(enemy->x == target_x && enemy->y == target_y
							&& enemy->global_x == target_gx && enemy->global_y == target_gy) {
								buff->target.enemy = enemy;
							}
						}
					}
				}
			}
		}
	} else {
		buff->target.player = player;
	}
}

void load_player(player_t *player, FILE *file, item_data_t *item_data) {
	fread(&player->level, sizeof(int), 1, file);
	fread(&player->xp, sizeof(int), 1, file);
	fread(&player->health, sizeof(int), 1, file);
	fread(&player->max_health, sizeof(int), 1, file);
	fread(&player->strength, sizeof(float), 1, file);
	fread(&player->dexterity, sizeof(float), 1, file);
	fread(&player->constitution, sizeof(float), 1, file);
	fread(&player->intelligence, sizeof(float), 1, file);
	fread(&player->speed, sizeof(float), 1, file);
	fread(&player->x, sizeof(int), 1, file);
	fread(&player->y, sizeof(int), 1, file);
	fread(&player->global_x, sizeof(int), 1, file);
	fread(&player->global_y, sizeof(int), 1, file);
	
	fread(&player->inventory_count, sizeof(int), 1, file);
	for(int i = 0; i < player->inventory_count; i++) {
		fread(&player->inventory[i].id, sizeof(item_ids_t), 1, file);
		fread(&player->inventory[i].stack, sizeof(int), 1, file);
		load_item_from_data(&player->inventory[i], item_data);
		if(player->inventory[i].value_type == VALUE_TYPE_WEAPON) {
			fread(&player->inventory[i].stat_type.weapon.equipped, sizeof(bool), 1, file);
			DEBUG_LOG("main_hand: %d, Equipped: %d", player->inventory[i].stat_type.weapon.main_hand, player->inventory[i].stat_type.weapon.equipped);
			if(player->inventory[i].stat_type.weapon.equipped) {
				if(player->inventory[i].stat_type.weapon.main_hand) {
					player->equipment.main_hand = &player->inventory[i];
				} else {
					player->equipment.off_hand = &player->inventory[i];
				}
			}
			
		} else if(player->inventory[i].value_type == VALUE_TYPE_ARMOR) {
			fread(&player->inventory[i].stat_type.armor.equipped, sizeof(bool), 1, file);
			if(player->inventory[i].stat_type.armor.equipped) {
				player->equipment.armor = &player->inventory[i];
			}
		}
	}
	fread(&player->nearby_loot_count, sizeof(int), 1, file);
	for(int i = 0; i < player->nearby_loot_count; i++) {
		fread(&player->nearby_loot[i]->id, sizeof(item_ids_t), 1, file);
		fread(&player->nearby_loot[i]->stack, sizeof(int), 1, file);
	}
	
	fread(&player->mana, sizeof(int), 1, file);
	fread(&player->max_mana, sizeof(int), 1, file);
	fread(&player->action_points, sizeof(int), 1, file);
	fread(&player->player_class, sizeof(class_type_t), 1, file);
	fread(&player->lantern, sizeof(lantern_t), 1, file);
	
	fread(&player->equipment.armor_id, sizeof(item_ids_t), 1, file);
	fread(&player->equipment.main_hand_id, sizeof(item_ids_t), 1, file);
	fread(&player->equipment.off_hand_id, sizeof(item_ids_t), 1, file);
	fread(&player->equipment.main_hand_two_handed, sizeof(bool), 1, file);
	
	fread(&player->oil, sizeof(int), 1, file);
	fread(&player->state, sizeof(player_state_t), 1, file);
	fread(&player->inventory_manager, sizeof(inventory_manager_t), 1, file);
}

void load_world(world_t *world, player_t *player, FILE *file) {
	for(int x = 0; x < WORLD_WIDTH; x++) {
		for(int y = 0; y < WORLD_HEIGHT; y++) {
			int next_room_x = 0; 
			int next_room_y = 0;
			fread(&next_room_x, sizeof(int), 1, file);
			fread(&next_room_y, sizeof(int), 1, file);
			if(y == next_room_y && x == next_room_x) {
				load_room_save(world->room[x][y], file, world->item_data);
			}
		}
	}
	fread(&world->seed, sizeof(int), 1, file);
	
	fread(&world->messages_size, sizeof(int), 1, file);
	fread(&world->max_message_storage, sizeof(int), 1, file);
	
	world->messages = calloc(world->max_message_storage, sizeof(char *));
	for(int i = 0; i < world->max_message_storage; i++) {
		world->messages[i] = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
	}
	
	for(int i = 0; i < world->messages_size; i++) {
		int len;
		fread(&len, sizeof(int), 1, file);
		DEBUG_LOG("i = %d", i);
		fread(world->messages[i], sizeof(char), len, file);
	}
	
	fread(&world->turn_order_size, sizeof(int), 1, file);
	fread(world->turn_order, sizeof(int), world->turn_order_size, file);
	fread(&world->room_template_count, sizeof(int), 1, file);
	fread(&world->room_templates, sizeof(room_template_t), world->room_template_count, file);
	fread(&world->buff_size, sizeof(uint8_t), 1, file);
	fread(&world->buff_count, sizeof(uint8_t), 1, file);
	for(int i = 0; i < world->buff_count; i++) {
		load_buff(&world->buffs[i], world, player, file);
	}
}

void load_room_save(room_t *room, FILE *file, item_data_t *item_data) {
	fread(room->room_file_name, sizeof(char), ROOM_FILE_NAME_MAX_SIZE, file);
	for(int x = 0; x < ROOM_WIDTH; x++) {
		for(int y = 0; y < ROOM_HEIGHT; y++) {
			// fwrite(&room->tiles[y][x]->floor, sizeof(char), 1, file);
			fread(&room->tiles[y][x]->item_count, sizeof(int8_t), 1, file);
			for(int i = 0; i < room->tiles[y][x]->item_count; i++) {
				room->tiles[y][x]->items[i] = calloc(1, sizeof(item_t));
				fread(&room->tiles[y][x]->items[i]->id, sizeof(item_ids_t), 1, file);
				fread(&room->tiles[y][x]->items[i]->stack, sizeof(int), 1, file);
				load_item_from_data(room->tiles[y][x]->items[i], item_data);
			}
			fread(&room->tiles[y][x]->deleted_trap_count, sizeof(uint8_t), 1, file);
			for(int i = 0; i < room->tiles[y][x]->deleted_trap_count; i++) {
				fread(&room->tiles[y][x]->deleted_trap_x[i], sizeof(uint8_t), 1, file);
				fread(&room->tiles[y][x]->deleted_trap_y[i], sizeof(uint8_t), 1, file);
			}
		}
	}
	fread(&room->current_enemy_count, sizeof(int8_t), 1, file);
	for (int i = 0; i < room->current_enemy_count; i++) {
		fread(room->enemies[i], sizeof(enemy_t), 1, file);
	}
	fread(&room->is_created, sizeof(bool), 1, file);
	fread(&room->global_time, sizeof(int), 1, file);
	fread(&room->biome, sizeof(biome_t), 1, file);
	fread(&room->is_main_path, sizeof(bool), 1, file);
	fread(&room->door_mask, sizeof(uint8_t), 1, file);
	load_room_floor_tiles(room);
}
