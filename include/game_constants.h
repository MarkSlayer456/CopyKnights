#ifndef GAME_CONSTANTS_H_

#define GAME_CONSTANTS_H_
#define MESSAGE_IS_PLAYERS_TURN					"It is your turn!"

#define WALL 				'#' // a wall
#define BOG_WALL			'%'
#define MUD					'~'
#define EMPTY				'.'
#define DOOR				'/'
#define CHEST				'$' // can walk through
#define PLAYER				'@'
#define ITEM_SYMBOL			'*'

#define MAX_ITEMS								1000

#define PLAYER_TURN_ORDER_INDEX					-1
#define INVALID_ACTOR_INDEX						999999999

#define DOOR_BLOCKED_MESSAGE					"This door won't seem to budge!"

#define ARROW_KEY_MOD							1000
#define DOWN_ARROW								ARROW_KEY_MOD + 66
#define UP_ARROW								ARROW_KEY_MOD + 65
#define RIGHT_ARROW								ARROW_KEY_MOD + 67
#define LEFT_ARROW								ARROW_KEY_MOD + 68
#define KEY_I									105
#define ENTER_KEY								10
#define ESC_KEY									27

#define TEST_SEED								596231
#define ENEMY_X_PRIME							19349663u
#define ENEMY_Y_PRIME							73856093u

#define KEY_A      								97
#define KEY_B									98
#define KEY_S       							115
#define KEY_F_MINE									102

#define CTRL_Q									17

//TODO add more ...

#define WALK_CHAR_LENGTH						16

#define ENEMY_CHAR      'E' // used when finding enemies on the map

#define ACTION_BAR_SELECTOR_COUNT				2

#define TIME_TO_ACT								50


#define MAX_ITEM_NAME_LENGTH					32
#define MAX_ITEM_STACK							99
#define DEFAULT_MAX_MESSAGE_STORAGE				1000
#define MAX_MESSAGE_LENGTH						59
#define MAX_MESSAGE_LENGTH_WITHOUT_PREFIX		47
#define CAVE_ROOM_COUNT						4
#define BOG_ROOM_COUNT						2
#define CATACOMBS_ROOM_COUNT				1
#define ANCIENT_CITY_ROOM_COUNT				1
#define ARCANE_LABYRINTH_ROOM_COUNT			1
#define VOID_HOLLOW_ROOM_COUNT				1
#define ROOM_WIDTH							20
#define ROOM_HEIGHT							20
#define MAX_ENEMIES_PER_LEVEL				12
#define NUMBER_OF_BIOMES					6

#define CAVE_NAME							"Cave"
#define BOG_NAME							"Bog"
#define CATACOMBS_NAME						"Catacombs"
#define ANCIENT_CITY_NAME					"Ancient City"
#define ARCANE_LABYRINTH_NAME				"Arcane Labyrinth"
#define VOID_HOLLOW_NAME					"Void Hollow"

#define POTENTIAL_ENEMY_SPAWN_CHAR			'*'
#define POTENTIAL_CHEST_SPAWN_CHAR			'$'
#define POTENTIAL_TRAP_SPAWN_CHAR			'X'

#define MAX_ENEMIES_PER_TILE				8
#define MAX_ITEMS_PER_TILE				8
#define MAX_ENEMIES							100

#define ENEMY_TYPE_COUNT					17
#define MAX_ENEMY_NAME_LENGTH				32
#define NULL_ENEMY_NAME						"NULL"
#define SLIME_ENEMY_NAME 					"Slime"
#define BAT_ENEMY_NAME						"Bat"
#define RAT_ENEMY_NAME						"Rat"
#define SKELETON_ENEMY_NAME					"Skeleton"
#define DRAGON_ENEMY_NAME					"Dragon"
#define BABY_DRAGON_ENEMY_NAME				"Baby Dragon"
#define GOLBIN_ENEMY_NAME					"Goblin"
#define GHOST_ENEMY_NAME					"Ghost"
#define LOOT_GOBLIN_ENEMY_NAME				"Loot Goblin"
#define MOSS_BEAST_ENEMY_NAME				"Moss Beast"
#define JESTER_ENEMY_NAME					"Jester"
#define VOIDLING_ENEMY_NAME					"Voidling"
#define MARROW_CRAWLER_ENEMY_NAME			"Marrow Crawler"
#define VOID_MAW_ENEMY_NAME					"Void Maw"
#define MARROW_LEECH_ENEMY_NAME				"Marrow Leech"
#define MUD_CRAWLER_ENEMY_NAME				"Mud Crawler"
#define BOG_LURKER_ENEMY_NAME				"Bog Lurker"

#define INV_SIZE					128

#define BLANK_NAME					"Blank"
#define TELEPORT_SCROLL_NAME		"Teleport Scroll"
#define TELEPORT_SCROLL_TELEPORT_LOCATION  ((location_t){ .x = 1, .y = 1, .global_x = 0, .global_y = 0 })
#define HEALTH_POTION_NAME			"Health Potion"
#define HEALTH_POTION_HEAL_AMOUNT	100
#define	APPLE_NAME					"Apple"
#define APPLE_HEAL_AMOUNT			5
#define CHICKEN_DINNER_NAME			"Chicken Dinner"
#define CHICKEN_DINNER_HEAL_AMOUNT	500

#define PLAYER_MIN_VISION_RANGE				2

#define BLACKSTONE_ARMOR_NAME				"Blackstone Armor"
#define BRONZE_ARMOR_NAME					"Bronze Armor"
#define IRON_ARMOR_NAME						"Iron Armor"
#define STEEL_ARMOR_NAME					"Steel Armor"
#define MITHRIL_ARMOR_NAME					"Mithril Armor"
#define SOULCRUST_ARMOR_NAME				"Soulcrust Armor"
#define TERRACITE_ARMOR_NAME				"Terracite Armor"
#define GRAVEMARROW_ARMOR_NAME				"Gravemarrow Armor"
#define VOIDMARROW_ARMOR_NAME				"Voidmarrow Armor"
#define RAT_HIDE_ARMOR_NAME					"Rat Hide Armor"
#define SLIME_ARMOR_NAME					"Slime Armor"
#define BOG_IRON_ARMOR_NAME					"Bog Iron Armor"
#define BONE_ARMOR_NAME						"Bone Armor"
#define SUNSTEEL_ARMOR_NAME					"Sunsteel Armor"
#define FALSE_IRON_ARMOR_NAME				"False Iron Armor"
#define BARKMAIL_ARMOR_NAME					"Barkmail Armor"
#define DRAGON_BONE_ARMOR_NAME				"Dragon Bone Armor"
#define CLOAK_NAME							"Cloak"
#define LEATHER_CLOAK_NAME					"Leather Cloak"
#define BOGSTICK_VEST_NAME					"Bogstick Vest"
#define VOIDLACE_CLOAK_NAME					"Voidlace Cloak"

#define ARMOR_TYPE_LIGHT_NAME				"light"
#define ARMOR_TYPE_MEDIUM_NAME				"medium"
#define ARMOR_TYPE_HEAVY_NAME				"heavy"

#define MAX_ARMOR_MODIFIERS					4

#define STRENGTH_NAME						"Strength"
#define DEXTERITY_NAME						"Dexterity"
#define INTELLIGENCE_NAME					"Intelligence"
#define CONSTITUTION_NAME					"Constitution"
#define SPEED_NAME							"Speed"

#define NULL_CLASS_NAME					"NULL"
#define SWORDSMAN_CLASS_NAME			"Swordsman"
#define BARBARIAN_CLASS_NAME			"Barbarian"
#define SPEARMAN_CLASS_NAME				"Spearman"
#define ARCHER_CLASS_NAME				"Archer"
#define WIZARD_CLASS_NAME				"Wizard"
#define MERCHANT_CLASS_NAME				"Merchant"

#define BASE_STRENGTH					10
#define BASE_DEXTERITY					10
#define BASE_INTELLIGENCE				10
#define BASE_CONSTITUTION				10
#define BASE_SPEED						10

#define MAX_CLASSES						32

#define HUD_WIDTH					59
#define HUD_HEIGHT					24

#define PLAYER_STATS_HUD_SPACE		7
#define ENEMY_STATS_HUD_SPACE		7
#define MESSAGE_HUD_SPACE			10
#define HUD_LENGTH					50

#define WORLD_HEIGHT				100
#define WORLD_WIDTH					100

#define SCREEN_WIDTH				80
#define SCREEN_HEIGHT				24

// Lantern
#define LANTERN_MAX_POWER								10
#define LANTERN_NORMAL_POWER							5
#define LANTERN_OVER_LOAD_POWER							10
#define LANTERN_OVER_LOAD_THRESHOLD						4
#define LANTERN_OVER_LOAD_LOWER_BOUNDS					6
#define LANTERN_TURNS_TO_DIM							10

#endif
