/* 
 * rcmbot - IRC bot
 * Copyright (C) 2013  Andreas "vible" Ölund
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "settings.h"

#include "includes.h"

#include <stdlib.h>
#include <string.h>

#include "xmem.h"

/* Statics functions for linked list manipulation */
static struct settings* set_create(struct settings *set, char *name,
                char *value)
{
        struct settings *ret = xmalloc(sizeof(struct settings));

        ret->name = strdup(name);
        ret->value = strdup(value);
        
        ret->next = set;

        return ret;
}

/* Not used */
/*static void set_destroy(struct settings *set)
{
        if (set)
        {
                free(set->name);
                free(set->value);
                free(set->next);
                free(set);
        }
}*/

static struct settings* set_find(struct settings *set, char *name)
{
        if (!(set && name))
                return NULL;

        for (; set && strcmp(set->name, name) != 0; set = set->next);

        return set;
}

/* Not used */
/*static void set_remove(struct settings *set, char *name)
{ 
        struct settings *cur;

        if (!(set && name))
                return;

        for (cur = set; set && strcmp(set->name, name) != 0;
                        cur = set, set = set->next);

        if (set) {
                cur->next = set->next;
                set->next = NULL;
                set_destroy(set);
        }
}*/

/* Changes an already existing setting, if setting doesnt exist: NOTHING */
void set_change_value(struct settings *set, char *name, char *value)
{
        struct settings *res = set_find(set, name);

        if (res) {
                free(res->value);
                res->value = strdup(value);
        }
}

/* Creates a new setting with the default value if it doesnt exist */
struct settings* set_default(struct settings *set, char *name, char *value)
{
        if (!set_find(set, name))
                return (set_create(set, name, value));

        return set;
}

/* Loads all settings from file */
struct settings *set_load_file(char *filename)
{
        FILE *file;
        char buffer[512];
        char *value;
        char *n;
        struct settings *set = NULL;

        file = fopen(filename, "r");

        if (!file) {
                debug_print("Couldnt open file");
                return NULL;
        }

        while (fgets(buffer, 512, file)) {
                n = strchr(buffer, '=');

                if (!n)
                        continue;

                value = strtok(buffer, "=");
                value = strtok(NULL, "\n");
                n[0] = '\0';

                n = strchr(value, '\n');

                if (n)
                        n[0] = '\0';

                set = set_create(set, buffer, value);
        }

        fclose(file);

        return set;
}

/* Returns the value from the setting "name" */
char* set_lookup(struct settings *set, char *name)
{
        struct settings *ret = set_find(set, name);

        if (!ret) {
                debug_print("Couldnt find named setting");
                return(NULL);
        }

        return ret->value;
}

/* Saves the settings from set to filename */
int set_save_file(struct settings *set, char *filename)
{
        FILE *file;

        file = fopen(filename, "w");

        if (!file) {
                debug_print("Couldn't open file");
                return -1;
        }

        for (; set; set = set->next) {
                fprintf(file, "%s=%s\n", set->name, set->value);
        }

        fclose(file);

        return 0;
}
