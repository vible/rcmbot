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

#ifndef INCLUDES_H
#define INCLUDES_H

#include <stdio.h>

/* Used to easier print errors */
#define debug_print(fmt) \
        fprintf(stderr, "%s:%d:%s(): %s\n", __FILE__, __LINE__, __func__, fmt)

#endif 