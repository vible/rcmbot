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

#ifndef SETTINGS_H
#define SETTINGS_H

struct settings {
        char *name;
        char *value;

        struct settings *next;
};

/* Changes an already existing setting, if setting doesnt exist: NOTHING */
void set_change_value(struct settings *set, char *name, char *value);

/* Creates a new setting with the default value if it doesnt exist */
struct settings* set_default(struct settings *set, char *name, char *value);

/* Loads all settings from file */
struct settings* set_load_file(char *filename);

/* Returns the value from the setting "name" */
char* set_lookup(struct settings *set, char *name);

/* Saves the settings from set to filename */
int set_save_file(struct settings *set, char *filename);
#endif 
