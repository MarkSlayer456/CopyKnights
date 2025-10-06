#include <string.h>
#include "items/misc.h"
#include "types.h"
#include "items/items.h"

void load_misc_data(world_t *world) {
	item_data_t *item_data = world->item_data;
	FILE *fp = fopen("./data/misc/misc.csv", "r");
	if(!fp) {
		perror("misc.csv File open failed");
		return;
	}
	
	char line[2048];
	
	if(fgets(line, sizeof(line), fp) == NULL) {
		fprintf(stderr, "File is empty\n");
		fclose(fp);
		return;
	}
	
	while(fgets(line, sizeof(line), fp)) {
		line[strcspn(line, "\n")] = '\0';
		int col = 0;
		char *token = strtok(line, ",");
		while(token) {
			switch(col) {
				case 0:
					snprintf(item_data[world->item_data_count].name, sizeof(item_data[world->item_data_count].name), "%s", token);
					item_data[world->item_data_count].id = item_get_id(token);
					item_data[world->item_data_count].value_type = VALUE_TYPE_NONE;
					break;
			}
			token = strtok(NULL, ",");
			col++;
		}
		snprintf(item_data[world->item_data_count].desc, MAX_ITEM_DESC_LEN, "%s", "The currency of the world");
		DEBUG_LOG("Loaded Misc Data: %d, %s", item_data[world->item_data_count].id, 
			item_data[world->item_data_count].name);
		world->item_data_count++;
		col = 0;
	}
}
