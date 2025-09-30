#include "save.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	snprintf(path, sizeof(path), "%s/.CopyKnights/save.bin", getenv("HOME"));
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
	FILE *file = fopen(get_save_path(), "wb");
	if(!file) return;
	save_world(world, file);
	save_player(player, file);
	fclose(file);
	DEBUG_LOG("%s", "game saved");
}

void load_game(world_t *world, player_t *player, char *name) {
	FILE *file = fopen(get_save_path(), "rb");
	if(!file) return;
	load_world(world, file);
	load_player(player, file, world->item_data);
	fclose(file);
	DEBUG_LOG("%s", "game loaded");
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
	fwrite(&player->strength, sizeof(int), 1, file);
	fwrite(&player->dexterity, sizeof(int), 1, file);
	fwrite(&player->intelligence, sizeof(int), 1, file);
	fwrite(&player->constitution, sizeof(int), 1, file);
	fwrite(&player->speed, sizeof(int), 1, file);
	fwrite(&player->x, sizeof(int), 1, file);
	fwrite(&player->y, sizeof(int), 1, file);
	fwrite(&player->global_x, sizeof(int), 1, file);
	fwrite(&player->global_y, sizeof(int), 1, file);
	
	fwrite(&player->inventory_count, sizeof(int), 1, file);
	for(int i = 0; i < player->inventory_count; i++) {
		fwrite(&player->inventory[i].id, sizeof(item_ids_t), 1, file);
		fwrite(&player->inventory[i].stack, sizeof(int), 1, file);
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
	
	for(int i = 0; i < world->messages_size; i++) {
		int len = strlen(world->messages[i]) + 1;
		fwrite(&len, sizeof(int), 1, file);
		fwrite(world->messages[i], sizeof(char), len, file);
	}
	
	fwrite(&world->max_message_storage, sizeof(int), 1, file);
	fwrite(&world->turn_order_size, sizeof(int), 1, file);
	fwrite(world->turn_order, sizeof(int), world->turn_order_size, file);
	fwrite(&world->room_template_count, sizeof(int), 1, file);
	fwrite(&world->room_templates, sizeof(room_template_t), world->room_template_count, file);
}

void save_room(room_t *room, FILE *file) {
	fwrite(room->room_file_name, sizeof(char), ROOM_FILE_NAME_MAX_SIZE, file);
	for(int x = 0; x < ROOM_WIDTH; x++) {
		for(int y = 0; y < ROOM_HEIGHT; y++) {
			// fwrite(&room->tiles[y][x]->floor, sizeof(char), 1, file);
			fwrite(&room->tiles[y][x]->item_count, sizeof(int), 1, file);
			for(int i = 0; i < room->tiles[y][x]->item_count; i++) {
				fwrite(&room->tiles[y][x]->items[i]->id, sizeof(item_ids_t), 1, file);
				fwrite(&room->tiles[y][x]->items[i]->stack, sizeof(int), 1, file);
			}
		}
	}
	fwrite(&room->current_enemy_count, sizeof(int), 1, file);
	for (int i = 0; i < room->current_enemy_count; i++) {
		fwrite(room->enemies[i], sizeof(enemy_t), 1, file);
	}
	fwrite(&room->is_created, sizeof(bool), 1, file);
	fwrite(&room->global_time, sizeof(int), 1, file);
	fwrite(&room->biome, sizeof(biome_t), 1, file);
	fwrite(&room->is_main_path, sizeof(bool), 1, file);
	fwrite(&room->door_mask, sizeof(unsigned int), 1, file);
}

void load_player(player_t *player, FILE *file, item_data_t *item_data) {
	fread(&player->level, sizeof(int), 1, file);
	fread(&player->xp, sizeof(int), 1, file);
	fread(&player->health, sizeof(int), 1, file);
	fread(&player->max_health, sizeof(int), 1, file);
	fread(&player->strength, sizeof(int), 1, file);
	fread(&player->dexterity, sizeof(int), 1, file);
	fread(&player->constitution, sizeof(int), 1, file);
	fread(&player->intelligence, sizeof(int), 1, file);
	fread(&player->speed, sizeof(int), 1, file);
	fread(&player->x, sizeof(int), 1, file);
	fread(&player->y, sizeof(int), 1, file);
	fread(&player->global_x, sizeof(int), 1, file);
	fread(&player->global_y, sizeof(int), 1, file);
	
	fread(&player->inventory_count, sizeof(int), 1, file);
	for(int i = 0; i < player->inventory_count; i++) {
		fread(&player->inventory[i].id, sizeof(item_ids_t), 1, file);
		fread(&player->inventory[i].stack, sizeof(int), 1, file);
		load_item_from_data(&player->inventory[i], item_data);
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

void load_world(world_t *world, FILE *file) {
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
	
	for(int i = 0; i < world->messages_size; i++) {
		int len;
		fread(&len, sizeof(int), 1, file);
		DEBUG_LOG("i = %d", i);
		fread(world->messages[i], sizeof(char), len, file);
	}
	
	fread(&world->max_message_storage, sizeof(int), 1, file);
	fread(&world->turn_order_size, sizeof(int), 1, file);
	fread(world->turn_order, sizeof(int), world->turn_order_size, file);
	fread(&world->room_template_count, sizeof(int), 1, file);
	fread(&world->room_templates, sizeof(room_template_t), world->room_template_count, file);
}

void load_room_save(room_t *room, FILE *file, item_data_t *item_data) {
	fread(room->room_file_name, sizeof(char), ROOM_FILE_NAME_MAX_SIZE, file);
	for(int x = 0; x < ROOM_WIDTH; x++) {
		for(int y = 0; y < ROOM_HEIGHT; y++) {
			// fwrite(&room->tiles[y][x]->floor, sizeof(char), 1, file);
			fread(&room->tiles[y][x]->item_count, sizeof(int), 1, file);
			for(int i = 0; i < room->tiles[y][x]->item_count; i++) {
				room->tiles[y][x]->items[i] = calloc(1, sizeof(item_t));
				fread(&room->tiles[y][x]->items[i]->id, sizeof(item_ids_t), 1, file);
				fread(&room->tiles[y][x]->items[i]->stack, sizeof(int), 1, file);
				load_item_from_data(room->tiles[y][x]->items[i], item_data);
			}
		}
	}
	fread(&room->current_enemy_count, sizeof(int), 1, file);
	for (int i = 0; i < room->current_enemy_count; i++) {
		fread(room->enemies[i], sizeof(enemy_t), 1, file);
	}
	fread(&room->is_created, sizeof(bool), 1, file);
	fread(&room->global_time, sizeof(int), 1, file);
	fread(&room->biome, sizeof(biome_t), 1, file);
	fread(&room->is_main_path, sizeof(bool), 1, file);
	fread(&room->door_mask, sizeof(unsigned int), 1, file);
	load_room_floor_tiles(room);
}
