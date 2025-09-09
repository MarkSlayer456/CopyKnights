#include "save.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	save_world(world, name);
	save_player(player, name);
}

void save_player(player_t *player, char *name) {
	FILE *file = fopen(get_save_path(), "wb");
	if(!file) return;
	
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
	fwrite(player->inventory, sizeof(item_t), player->inventory_count, file);
	fwrite(&player->nearby_loot_count, sizeof(int), 1, file);
	fwrite(player->nearby_loot, sizeof(item_t), player->nearby_loot_count, file);
	
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
	
	fclose(file);
}

void save_world(world_t *world, char *name) {
	
}

void save_enemies(enemy_t *enemies, char *name) {
	
}

void save_rooms(room_t *room, char *name) {
	
}

void load_player(player_t *player, char *name) {
	FILE *file = fopen(get_save_path(), "rb");
	if(!file) return;
	
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
	fread(player->inventory, sizeof(item_t), player->inventory_count, file);
	fread(&player->nearby_loot_count, sizeof(int), 1, file);
	fread(player->nearby_loot, sizeof(item_t), player->nearby_loot_count, file);
	
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
	
	fclose(file);
}
