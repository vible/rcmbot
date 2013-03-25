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

#ifndef XML_H
#define XML_H

#include <curl/curl.h>

struct xml_memory
{ 
        CURL *curl;
        char *string;
        int size;
};

/* Cleans up and frees xml_memory */
void vxml_cleanup(struct xml_memory *memory);

/* Downloads anything with curl and places it into memory */
int vxml_download(struct xml_memory *memory, char *url);

/* Initializes the xml_memory struct */
struct xml_memory* vxml_init();

/* Parses the memory of the xml and returns whats in the XPath */
char *vxml_parse(struct xml_memory *mem, char *xpath);

#endif
