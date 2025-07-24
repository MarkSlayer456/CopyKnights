#ifndef HUD_H_
#define HUD_H_
#include "game_manager.h"

#define Y_PLAYER_HEALTH_OFFSET  6
#define Y_RADAR_OFFSET 20

void hud_update_player_health(player_t *player);
void hud_update_nearby_enemies(world_t *world, player_t *player);

void hud_update_action_bar(player_t *player);

void hud_update_all(world_t *world, player_t *player);


void display_error_message(const char *str);

#endif
