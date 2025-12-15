#include "items/spells.h"
#include <string.h>
#include <stdlib.h>
#include "buff.h"
#include "items/items.h"
#include "types.h"
#include "enemy.h"

void load_spell_effects(world_t *world) {
	item_data_t *item_data = world->item_data;
	FILE *fp = fopen("./data/spells/spell_effects.csv", "r");
	if(!fp) {
		perror("File open failed");
		return;
	}

	char line[2048];

	if(fgets(line, sizeof(line), fp) == NULL) {
		fclose(fp);
		return;
	}

	while(fgets(line, sizeof(line), fp)) {
		line[strcspn(line, "\n")] = '\0';
		char *spell_name = strtok(line, ",");
		char *target_stat = strtok(NULL, ",");
		char *value = strtok(NULL, ",");

		if(!spell_name || !target_stat || !value) {
			continue;
		}

		int i = 0;
		while(i < MAX_ITEMS) {
			if(strcmp(spell_name, item_data[i].name) != 0) {
				i++;
				continue;
			}
			int idx = item_data[i].stat_type.spell.modifier_count;

			item_data[i].stat_type.spell.modifier_stats[idx].modifier = (int16_t)atoi(value);
			item_data[i].stat_type.spell.modifier_stats[idx].stat = get_stat(target_stat);
			item_data[i].stat_type.spell.equipped = false;
			item_data[i].stat_type.spell.modifier_count++;
			break;
		}
	}
}

void load_spell_data(world_t *world) {
	item_data_t *item_data = world->item_data;
	DEBUG_LOG("%s", "loading spells...");
	FILE *fp = fopen("./data/spells/spells.csv", "r");
	if(!fp) {
		DEBUG_LOG("%s", "Failed to open load_spell_data file");
		return;
	}

	char line[2048];

	if(fgets(line, sizeof(line), fp) == NULL) {
		DEBUG_LOG("%s", "File is empty\n");
		fclose(fp);
		return;
	}
	while(fgets(line, sizeof(line), fp)) {
		line[strcspn(line, "\n")] = '\0';
		DEBUG_LOG("parsing: %s", line);
		char *spell_name = strtok(line, ",");
		char *min_damage = strtok(NULL, ",");
		char *max_damage = strtok(NULL, ",");
		char *cost = strtok(NULL, ",");
		char *range = strtok(NULL, ",");
		char *crit_chance = strtok(NULL, ",");
		char *status_chance = strtok(NULL, ",");
		char *stat_grade = strtok(NULL, ",");
		char *buff_type = strtok(NULL, ",");

		if(!spell_name || !min_damage || !max_damage || !range || !crit_chance
			|| !status_chance || !buff_type) {
			continue;
		}

		snprintf(item_data[world->item_data_count].name, sizeof(item_data[world->item_data_count].name), "%s", spell_name);
		item_data[world->item_data_count].id = item_get_id(spell_name);
		item_data[world->item_data_count].value_type = VALUE_TYPE_SPELL;

		item_data[world->item_data_count].stat_type.spell.min_damage = (int16_t)atoi(min_damage);
		item_data[world->item_data_count].stat_type.spell.max_damage = (int16_t)atoi(max_damage);
		item_data[world->item_data_count].stat_type.spell.mana_cost = (int8_t)atoi(cost);
		item_data[world->item_data_count].stat_type.spell.range = (int8_t)atoi(range);
		item_data[world->item_data_count].stat_type.spell.crit_chance = atof(crit_chance);
		item_data[world->item_data_count].stat_type.spell.status_chance = atof(status_chance);
		item_data[world->item_data_count].stat_type.spell.stat_grade = get_grade(stat_grade);
		item_data[world->item_data_count].stat_type.spell.buff_type = buff_get_type(buff_type);


		snprintf(item_data[world->item_data_count].desc, MAX_ITEM_DESC_LEN,
			"damage: %d - %d\nscaling stat: INT (A)\nrange: %d\ncritical chance: %f\nstatus chance: %f",
			item_data[world->item_data_count].stat_type.spell.min_damage,
			item_data[world->item_data_count].stat_type.spell.max_damage,
			item_data[world->item_data_count].stat_type.spell.range,
			item_data[world->item_data_count].stat_type.spell.crit_chance,
			item_data[world->item_data_count].stat_type.spell.status_chance);

		DEBUG_LOG("Loaded Spell Data: %d, %s, %d, %s",
			item_data[world->item_data_count].id, item_data[world->item_data_count].name,
			item_data[world->item_data_count].stat_type.spell.max_damage,
			item_data[world->item_data_count].desc);
		world->item_data_count++;
	}
	load_spell_effects(world);
}
