#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "items/items.h"
#include "player.h"
#include "enemy.h"
#include "types.h"
#include "game_constants.h"
#include "map_manager.h"
#include "functions.h"

type_map_t rarity_map[] = {
    {RARITY_COMMON_NAME, COMMON},
    {RARITY_UNCOMMOM_NAME, UNCOMMON},
    {RARITY_RARE_NAME, RARE},
    {RARITY_LEGENDARY_NAME, LEGENDARY},
};

const int rarity_map_len = sizeof(rarity_map) / sizeof(rarity_map[0]);

type_map_t item_biome_map[] = {
    {CAVE_NAME, CAVE},
    {BOG_NAME, BOG},
    {CATACOMBS_NAME, CATACOMBS},
    {ANCIENT_CITY_NAME, ANCIENT_CITY},
    {ARCANE_LABYRINTH_NAME, ARCANE_LABYRINTH},
    {VOID_HOLLOW_NAME, VOID_HOLLOW},
};

const int item_biome_map_len = sizeof(item_biome_map) / sizeof(item_biome_map[0]);

type_map_t stats_map[] = {
    {STRENGTH_NAME, STRENGTH},
    {DEXTERITY_NAME, DEXTERITY},
    {INTELLIGENCE_NAME, INTELLIGENCE},
    {CONSTITUTION_NAME, CONSTITUTION},
    {SPEED_NAME, SPEED},
};

const int stats_map_len = sizeof(stats_map) / sizeof(stats_map[0]);

type_map_t item_type_map[] = {
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
    
    {BLACK_STONE_SWORD_NAME, BLACK_STONE_SWORD},
    {BRONZE_SWORD_NAME, BRONZE_SWORD},
    {IRON_SWORD_NAME, IRON_SWORD},
    {STEEL_SWORD_NAME, STEEL_SWORD},
    {MITHRIL_SWORD_NAME, MITHRIL_SWORD},
    {SOULCRUST_SWORD_NAME, SOULCRUST_SWORD},
    {TERRACTIE_SWORD_NAME, TERRACTIE_SWORD},
    {GRAVEMARROW_SWORD_NAME, GRAVEMARROW_SWORD},
    {VOIDMARROW_SWORD_NAME, VOIDMARROW_SWORD},
    
    {BLACK_STONE_SPEAR_NAME, BLACK_STONE_SPEAR},
    {BRONZE_SPEAR_NAME, BRONZE_SPEAR},
    {IRON_SPEAR_NAME, IRON_SPEAR},
    {STEEL_SPEAR_NAME, STEEL_SPEAR},
    {MITHRIL_SPEAR_NAME, MITHRIL_SPEAR},
    {SOULCRUST_SPEAR_NAME, SOULCRUST_SPEAR},
    {TERRACTIE_SPEAR_NAME, TERRACTIE_SPEAR},
    {GRAVEMARROW_SPEAR_NAME, GRAVEMARROW_SPEAR},
    {VOIDMARROW_SPEAR_NAME, VOIDMARROW_SPEAR},
    
    {BLACK_STONE_MACE_NAME, BLACK_STONE_MACE},
    {BRONZE_MACE_NAME, BRONZE_MACE},
    {IRON_MACE_NAME, IRON_MACE},
    {STEEL_MACE_NAME, STEEL_MACE},
    {MITHRIL_MACE_NAME, MITHRIL_MACE},
    {SOULCRUST_MACE_NAME, SOULCRUST_MACE},
    {TERRACTIE_MACE_NAME, TERRACTIE_MACE},
    {GRAVEMARROW_MACE_NAME, GRAVEMARROW_MACE},
    {VOIDMARROW_MACE_NAME, VOIDMARROW_MACE},
    
    {BLACK_STONE_AXE_NAME, BLACK_STONE_AXE},
    {BRONZE_AXE_NAME, BRONZE_AXE},
    {IRON_AXE_NAME, IRON_AXE},
    {STEEL_AXE_NAME, STEEL_AXE},
    {MITHRIL_AXE_NAME, MITHRIL_AXE},
    {SOULCRUST_AXE_NAME, SOULCRUST_AXE},
    {TERRACTIE_AXE_NAME, TERRACTIE_AXE},
    {GRAVEMARROW_AXE_NAME, GRAVEMARROW_AXE},
    {VOIDMARROW_AXE_NAME, VOIDMARROW_AXE},
    
    {BLACK_STONE_PICKAXE_NAME, BLACK_STONE_PICKAXE},
    {BRONZE_PICKAXE_NAME, BRONZE_PICKAXE},
    {IRON_PICKAXE_NAME, IRON_PICKAXE},
    {STEEL_PICKAXE_NAME, STEEL_PICKAXE},
    {MITHRIL_PICKAXE_NAME, MITHRIL_PICKAXE},
    {SOULCRUST_PICKAXE_NAME, SOULCRUST_PICKAXE},
    {TERRACTIE_PICKAXE_NAME, TERRACTIE_PICKAXE},
    {GRAVEMARROW_PICKAXE_NAME, GRAVEMARROW_PICKAXE},
    {VOIDMARROW_PICKAXE_NAME, VOIDMARROW_PICKAXE},
    
    {BLACK_STONE_BOW_NAME, BLACK_STONE_BOW},
    {BRONZE_BOW_NAME, BRONZE_BOW},
    {IRON_BOW_NAME, IRON_BOW},
    {STEEL_BOW_NAME, STEEL_BOW},
    {MITHRIL_BOW_NAME, MITHRIL_BOW},
    {SOULCRUST_BOW_NAME, SOULCRUST_BOW},
    {TERRACTIE_BOW_NAME, TERRACTIE_BOW},
    {GRAVEMARROW_BOW_NAME, GRAVEMARROW_BOW},
    {VOIDMARROW_BOW_NAME, VOIDMARROW_BOW},
    
    {BLACK_STONE_STAFF_NAME, BLACK_STONE_STAFF},
    {BRONZE_STAFF_NAME, BRONZE_STAFF},
    {IRON_STAFF_NAME, IRON_STAFF},
    {STEEL_STAFF_NAME, STEEL_STAFF},
    {MITHRIL_STAFF_NAME, MITHRIL_STAFF},
    {SOULCRUST_STAFF_NAME, SOULCRUST_STAFF},
    {TERRACTIE_STAFF_NAME, TERRACTIE_STAFF},
    {GRAVEMARROW_STAFF_NAME, GRAVEMARROW_STAFF},
    {VOIDMARROW_STAFF_NAME, VOIDMARROW_STAFF},
};

const int item_type_map_len = sizeof(item_type_map) / sizeof(item_type_map[0]);

item_ids_t item_get_id(const char *name) {
	for(int i = 0; i < item_type_map_len; i++) {
		if(strcmp(name, item_type_map[i].name) == 0) {
			return item_type_map[i].value;
		}
	}
	return BLANK;
}

stats_t get_stat(const char *name) {
    for(int i = 0; i < stats_map_len; i++) {
        if(strcmp(name, stats_map[i].name) == 0) {
            return stats_map[i].value;
        }
    }
    return NULL_STAT;
}

rarity_t get_rarity(const char *name) {
    for(int i = 0; i < rarity_map_len; i++) {
        if(strcmp(name, rarity_map[i].name) == 0) {
            return rarity_map[i].value;
        }
    }
    return NULL_RARITY;
}

// returns 1 on success and 0 on fail
int use_item(player_t *player)
{
	int success = 0;
	if(player->inventory[player->inventory_manager.inv_selector].stack > 0) {
        if(player->inventory[player->inventory_manager.inv_selector].value_type == VALUE_TYPE_ARMOR) {
            player->equipment.armor = &player->inventory[player->inventory_manager.inv_selector];
        } else if(player->inventory[player->inventory_manager.inv_selector].value_type == VALUE_TYPE_WEAPON) {
            player->equipment.main_hand = &player->inventory[player->inventory_manager.inv_selector]; 
            //TODO off_hand and two handed weapons
        } else if(player->inventory[player->inventory_manager.inv_selector].value_type == VALUE_TYPE_FOOD) {
            //TODO effects with durations
            player_increase_health(player, player->inventory[player->inventory_manager.inv_selector].stat_type.food.heal_amount);
            player_increase_mana(player, player->inventory[player->inventory_manager.inv_selector].stat_type.food.mana_heal_amount);
        } else {
            switch(player->inventory[player->inventory_manager.inv_selector].id) {
                case BLANK:
                    success = 0;
                case TELEPORT_SCROLL:
                    use_teleport_scroll(player);
                    success = 1;
                default:
                    success = 0;
            }
            remove_item(player);
        }
		player_close_inventory(player);
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
	player->inventory[player->inventory_manager.inv_selector].stack -= 1;
	if(player->inventory[player->inventory_manager.inv_selector].stack <= 0) {
		player_organize_inv(player, player->inventory_manager.inv_selector);
	}
}
void item_spawn(item_ids_t id, biome_t biome, tile_t *tile, item_data_t *item_data) {
    item_t *item = tile->items[tile->item_count];
    for(int i = 0; i < MAX_ITEMS; i++) {
        if(!item) continue;
        if(item_data[i].id == BLANK) continue;
        if(item_data[i].id == id) {
            strcpy(item->name, item_data[i].name);
            strcpy(item->desc, item_data[i].desc);
            item->id = id;
            item->stack = 1;
            item->value_type = item_data[i].value_type;
            switch(item_data[i].value_type) {
                case VALUE_TYPE_NONE:
                    break;
                case VALUE_TYPE_ARMOR:
                    item->stat_type.armor = item_data[i].stat_type.armor;
                    break;
                case VALUE_TYPE_WEAPON:
                    item->stat_type.weapon = item_data[i].stat_type.weapon;
                    break;
                case VALUE_TYPE_FOOD:
                    item->stat_type.food = item_data[i].stat_type.food;
                    break;
                case VALUE_TYPE_SCROLL:
                    item->stat_type.scroll = item_data[i].stat_type.scroll;
                    break;
            }
        }
    }
}

item_ids_t item_generate_type(unsigned int *seed, item_data_t *item_data, biome_t biome) {
    item_ids_t items[MAX_ITEMS];
    int items_size = 0;
    for(int i = 0; i < MAX_ITEMS; i++) {
        if(item_data[i].spawn_biomes[biome]) {
            items[items_size++] = item_data[i].id;
        }
    }
    assert(items_size > 0);
    int random_number = (rand_r_portable(seed) % items_size);
    return items[random_number];
}

