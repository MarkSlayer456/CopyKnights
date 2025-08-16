#include <stdlib.h>
#include <string.h>
#include "items.h"
#include "player.h"
#include "types.h"
#include "game_constants.h"

stats_map_t stats_map[] = {
    {STRENGTH_NAME, STRENGTH},
    {DEXTERITY_NAME, DEXTERITY},
    {INTELLIGENCE_NAME, INTELLIGENCE},
    {CONSTITUTION_NAME, CONSTITUTION},
    {SPEED_NAME, SPEED},
};

const int stats_map_len = sizeof(stats_map) / sizeof(stats_map[0]);

armor_type_map_t armor_type_map[] = {
    {ARMOR_TYPE_LIGHT_NAME, LIGHT},
    {ARMOR_TYPE_MEDIUM_NAME, MEDIUM},
    {ARMOR_TYPE_HEAVY_NAME, HEAVY},
};

const int armor_type_map_len = sizeof(armor_type_map) / sizeof(armor_type_map[0]);

item_type_map_t item_type_map[] = {
    {BLANK_NAME, BLANK},
    {TELEPORT_SCROLL_NAME, TELEPORT_SCROLL},
    {HEALTH_POTION_NAME, HEALTH_POTION},
    {APPLE_NAME, APPLE},
    {CHICKEN_DINNER_NAME, CHICKEN_DINNER},
    {BLACKSTONE_ARMOR_NAME, BLACKSTONE_ARMOR},
    {BRONZE_ARMOR_NAME, BRONZE_ARMOR},
    {IRON_ARMOR_NAME, IRON_ARMOR},
    {STEEL_ARMOR_NAME, STEEL_ARMOR},
    {MITHRIL_ARMOR_NAME, MITHRIL_ARMOR},
    {SOULCRUST_ARMOR_NAME, SOULCRUST_ARMOR},
    {TERRACITE_ARMOR_NAME, TERRACITE_ARMOR},
    {GRAVEMARROW_ARMOR_NAME, GRAVEMARROW_ARMOR},
    {VOIDMARROW_ARMOR_NAME, VOIDMARROW_ARMOR},
    {RAT_HIDE_ARMOR_NAME, RAT_HIDE_ARMOR},
    {SLIME_ARMOR_NAME, SLIME_ARMOR},
    {BOG_IRON_ARMOR_NAME, BOG_IRON_ARMOR},
    {BONE_ARMOR_NAME, BONE_ARMOR},
    {SUNSTEEL_ARMOR_NAME, SUNSTEEL_ARMOR},
    {FALSE_IRON_ARMOR_NAME, FALSE_IRON_ARMOR},
    {BARKMAIL_ARMOR_NAME, BARKMAIL_ARMOR},
    {DRAGON_BONE_ARMOR_NAME, DRAGON_BONE_ARMOR},
    {CLOAK_NAME, CLOAK},
    {LEATHER_CLOAK_NAME, LEATHER_CLOAK},
    {BOGSTICK_VEST_NAME, BOGSTICK_VEST},
    {VOIDLACE_CLOAK_NAME, VOIDLACE_CLOACK},
};

const int item_type_map_len = sizeof(item_type_map) / sizeof(item_type_map[0]);

item_ids_t item_get_id(const char *name) {
	for(int i = 0; i < item_type_map_len; i++) {
		if(strcasecmp(name, item_type_map[i].name) == 0) {
			return item_type_map[i].value;
		}
	}
	return BLANK;
}

armor_type_t armor_get_type(const char *name) {
    for(int i = 0; i < armor_type_map_len; i++) {
        if(strcasecmp(name, armor_type_map[i].name) == 0) {
            return armor_type_map[i].value;
        }
    }
    return NULL_ARMOR_TYPE;
}

stats_t get_stat(const char *name) {
    for(int i = 0; i < stats_map_len; i++) {
        if(strcasecmp(name, stats_map[i].name) == 0) {
            return stats_map[i].value;
        }
    }
    return NULL_STAT;
}


// returns 1 on success and 0 on fail
int use_item(player_t *player)
{
	int success = 0;
	if(player->inventory[player->action_bar.inv_selector].stack > 0) {
		switch(player->inventory[player->action_bar.inv_selector].id) {
			case BLANK:
				success = 0;
			case TELEPORT_SCROLL:
				use_teleport_scroll(player);
				success = 1;
			case HEALTH_POTION:
				use_health_potion(player);
				success = 1;
			case APPLE:
				use_apple(player);
				success = 1;
			case CHICKEN_DINNER:
				use_chicken_dinner(player);
				success = 1;
			default:
				success = 0;
		}
		remove_item(player);
		player_close_inventory(player);
		player_close_action_bar(player);
	}
	return success;
}


int use_teleport_scroll(player_t *player)
{
	player->x = TELEPORT_SCROLL_TELEPORT_LOCATION.x;
	player->y = TELEPORT_SCROLL_TELEPORT_LOCATION.x;
	player->global_x = TELEPORT_SCROLL_TELEPORT_LOCATION.global_x;
	player->global_y = TELEPORT_SCROLL_TELEPORT_LOCATION.global_y;
	return 0;
}

int use_health_potion(player_t *player)
{
	player_increase_health(player, HEALTH_POTION_HEAL_AMOUNT);
	return 1;
}

int use_apple(player_t *player)
{
	player_increase_health(player, APPLE_HEAL_AMOUNT);
	return 1;
}

int use_chicken_dinner(player_t *player)
{
	player_increase_health(player, CHICKEN_DINNER_HEAL_AMOUNT);
	return 1;
}

void remove_item(player_t *player)
{
	player->inventory[player->action_bar.inv_selector].stack -= 1;
	if(player->inventory[player->action_bar.inv_selector].stack <= 0) {
		player_organize_inv(player, player->action_bar.inv_selector);
	}
}

void load_armor_effects(world_t *world) {
    item_data_t *item_data = world->item_data;
    FILE *fp = fopen("./data/armor_effects.csv", "r");
    if(!fp) {
        perror("File open failed");
        return;
    }
    
    char line[2048];
    
    if(fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return;
    }
    
    int row = 0;
    while(fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        char *armor_name = strtok(line, ",");
        char *target_stat = strtok(NULL, ",");
        char *value = strtok(NULL, ",");
        
        if(!armor_name || !target_stat || !value) {
            continue;
        }
        
        armor_type_t armor_type = armor_get_type(armor_name);
        if(armor_type == NULL_ARMOR_TYPE) {
            continue;
        }
        
        int i = 0;
        while(i < MAX_ITEMS) {
            if(item_data[i].value_type == VALUE_TYPE_ARMOR && item_data[i].stat_type.armor.type != armor_type) {
                i++;
                continue;
            }
            int idx = item_data[i].stat_type.armor.modifier_count;
            
            item_data[i].stat_type.armor.modifier_stats[idx].modifier = atoi(value);
            item_data[i].stat_type.armor.modifier_stats[idx].stat = get_stat(target_stat);
            item_data[i].stat_type.armor.modifier_count++;
            break;
        }
        row++;
    }
}

void load_armor_data(world_t *world) {
	item_data_t *item_data = world->item_data;
    FILE *fp = fopen("./data/armors.csv", "r");
    if(!fp) {
        perror("File open failed");
        return;
    }
    
    char line[2048];
    
    if(fgets(line, sizeof(line), fp) == NULL) {
        fprintf(stderr, "File is empty\n");
        fclose(fp);
        return;
    }
    
    int row = 0;
    while(fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        int col = 0;
        char *token = strtok(line, ",");
        while(token) {
            switch(col) {
                case 0:
					snprintf(item_data[row].name, sizeof(item_data[row].name), "%s", token);
                    item_data[row].id = item_get_id(token);
                    break;
                case 1:
                    item_data[row].value_type = VALUE_TYPE_ARMOR;
                    item_data[row].stat_type.armor.type = armor_get_type(token);
                    break;
                case 2:
                    item_data[row].stat_type.armor.defense = atoi(token);
                    break;
            }
            token = strtok(NULL, ",");
            col++;
        }
        world->item_data_count++;
		DEBUG_LOG("Loaded Armor Data: %d, %s, %d, %d", 
				  item_data[row].id, item_data[row].name, 
            item_data[row].stat_type.armor.defense, item_data[row].stat_type.armor.type);
        col = 0;
        row++;
    }
    load_armor_effects(world);
}


