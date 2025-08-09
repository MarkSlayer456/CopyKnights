#ifndef HUD_H_
#define HUD_H_
#include "types.h"

void hud_update_player_health(player_t *player);
void hud_update_nearby_enemies(world_t *world, player_t *player);

void hud_update_action_bar(player_t *player, room_t *room);

void hud_update_messages(world_t *world, player_t *player);

void hud_update_all(world_t *world, player_t *player);

void display_error_message(const char *str);

void display_inventory_hud(world_t *world, player_t *player);
#endif
