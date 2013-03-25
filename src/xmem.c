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

#include "xmem.h"

#include "includes.h"

#include <stdlib.h>

/* Wrapper function for calloc */
void* xcalloc(int nmemb, int size)
{
	void *ret = calloc(nmemb, size);

	if (ret == NULL) {
                debug_print("Failed to allocate memory.");
		exit(EXIT_FAILURE);
        }

	return ret;
}

/* Wrapper function for malloc */
void* xmalloc(int size)
{
	void *ret = malloc(size);

	if (ret == NULL) {
                debug_print("Failed to allocate memory.");
		exit(EXIT_FAILURE);
        }

	return ret;
}
