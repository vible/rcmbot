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

#ifndef IRC_H
#define IRC_H

#include <stdio.h>

#include "settings.h"
#include "xml.h"

#define IRC_BUFFER_SIZE 512
#define IRC_READ_TIMEOUT 20
#define IRC_MAX_TIMEOUTS 30
#define IRC_SONG_TRESHOLD 900
#define IRC_SONG_TIMEOUT 10

struct irc_struct {
        char *buffer;
        int sockfd;
        FILE *socket;

        /* XML */
        struct xml_memory *xmem;
        char *songCurrent;
        int songTimeout;
        int songTreshold;

        struct settings *set;
};

/* Frees everything in ircst */
void irc_cleanup(struct irc_struct *ircst);

/* Connects to the server/hostname and tries to register */
int irc_connect(struct irc_struct *ircst, char *hostname, char *port);

/* Writes the currently playing song (or not) */
void irc_current_song(struct irc_struct *ircst);

/* Initiates the irc_struct, set can be NULL */
struct irc_struct* irc_init(struct settings *set);

/* Starts the main loop */
void irc_main_loop(struct irc_struct *ircst);

/* Sends the server a quit message */
void irc_quit(struct irc_struct *ircst, char *msg);

#endif
