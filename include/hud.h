#ifndef HUD_H_
#define HUD_H_
#include "game_manager.h"

#define PLAYER_STATS_HUD_SPACE		10
#define ENEMY_STATS_HUD_SPACE		10
#define MESSAGE_HUD_SPACE			20
#define HUD_LENGTH					50


void hud_update_player_health(player_t *player);
void hud_update_nearby_enemies(world_t *world, player_t *player);

void hud_update_action_bar(player_t *player);

void hud_update_messages(world_t *world, player_t *player);

void hud_update_all(world_t *world, player_t *player);

void display_error_message(const char *str);

#endif
