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

#include "irc.h"

#include "includes.h"
#include "../config.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

#include "xmem.h"
#include "xml.h"

static void cmd_register(struct irc_struct *ircst);
static void cmd_rejoin(struct irc_struct *ircst);

static void cmd_changenick(struct irc_struct *ircst)
{
        int len;
        char buffer[32];
        char *current = set_lookup(ircst->set, "nick");

        strcpy(buffer, current);
        len = strlen(buffer);

        buffer[len] = buffer[len-1];
        buffer[len+1] = '\0';

        set_change_value(ircst->set, "nick", buffer);

        cmd_register(ircst);
}

static void cmd_kick(struct irc_struct *ircst, char *params[])
{
        if (!params[1])
                return;
        if (strcmp(params[1], set_lookup(ircst->set, "nick")) == 0)
                cmd_rejoin(ircst);
}

static void cmd_ping(struct irc_struct *ircst, char *params[])
{
        if (!(params[0]))
                return;

        fprintf(ircst->socket, "PONG %s\r\n", params[0]);
        fflush(ircst->socket);
}

static void cmd_privmsg(struct irc_struct *ircst, char *name, char *params[])
{
        char *n;
        if (!(name && params[0] && params[1]))
                return;

        name++;

        n = strchr(name, '!');

        if (!n)
                return;

        n[0] = '\0';

        if (strcmp(params[0], set_lookup(ircst->set, "channel")) == 0) {
                ircst->songTreshold = time(NULL)+IRC_SONG_TRESHOLD;

                irc_current_song(ircst);
                return;
        }

        if (strcmp(params[0], set_lookup(ircst->set, "nick")) == 0 &&
                        strcmp(params[1], ":\001VERSION\001") == 0) {
              fprintf(ircst->socket, 
                              "NOTICE %s :\001VERSION %s %s by vible\001\r\n",
                              name, PACKAGE, VERSION);
              fflush(ircst->socket);
              return;
        }
}

static void cmd_register(struct irc_struct *ircst)
{
        fprintf(ircst->socket, "NICK %s\r\n", set_lookup(ircst->set, "nick"));
        fprintf(ircst->socket, "USER %s 0 * %s\r\n",
                        set_lookup(ircst->set, "nick"),
                        set_lookup(ircst->set, "nick"));
        fflush(ircst->socket);
}

static void cmd_rejoin(struct irc_struct *ircst)
{
        if (strcmp(set_lookup(ircst->set, "quakeauth"), "1") == 0) {
                fprintf(ircst->socket, 
                               "privmsg q@cserve.quakenet.org :AUTH %s %s\r\n"
                                ,set_lookup(ircst->set, "quakeauthname"),
                                set_lookup(ircst->set, "quakeauthpw"));
                fprintf(ircst->socket, "mode %s +x\r\n", 
                                set_lookup(ircst->set, "nick"));
        }

        fprintf(ircst->socket, "JOIN %s\r\n",
                        set_lookup(ircst->set, "channel"));
        fflush(ircst->socket);
}

/* Takes the commands and sends them to the desired function */
static void handle_command(struct irc_struct *ircst, char *msg)
{
        char *cmd;
	char *params[16];
	char *saveptr;
	unsigned int i;

        if (strcmp("1", set_lookup(ircst->set, "logging")) == 0)
                printf("%s\n", msg);    

	if (!(cmd = strtok_r(msg, " ", &saveptr)))
		return;
	/* Strip prefix. */
	if (cmd[0] == ':')
		cmd = strtok_r(NULL, " ", &saveptr);
	for (i = 0; i < sizeof(params) / sizeof(*params); i++)
		params[i] = strtok_r(NULL, " ", &saveptr);

	if (strcmp(cmd, "001") == 0)
		/* 001 means registered, after this we can join channels */
		cmd_rejoin(ircst);
        else if (strcmp(cmd, "433") == 0)
                /* 436 NICK COLLISION */
                cmd_changenick(ircst);
	else if (strcmp(cmd, "PING") == 0)
		cmd_ping(ircst, params);
	else if (strcmp(cmd, "PRIVMSG") == 0)
		cmd_privmsg(ircst, msg, params);
	else if (strcmp(cmd, "KICK") == 0)
		cmd_kick(ircst, params);
}

/* Frees everything in ircst */
void irc_cleanup(struct irc_struct *ircst)
{
        if (ircst) {
                if (ircst->buffer)
                        free(ircst->buffer);

                if (ircst->socket)
                        fclose(ircst->socket);

                vxml_cleanup(ircst->xmem);

                free(ircst);
        }
}

/* Connects to the server/hostname and tries to register */
int irc_connect(struct irc_struct *ircst, char *host, char *port)
{
        struct hostent *hent;
        struct sockaddr_in *sockaddr;
        int i;

        if(!(hent = gethostbyname(host))) {
                debug_print("Could not resolve hostname.");
                return -1;
        }

        /* To randomize which ip to use */
        for (i = 0; hent->h_addr_list[i] != NULL; i++);

        ircst->sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (ircst->sockfd == -1) {
                debug_print("Coldn't open the socket.");
                return -1;
        }

        sockaddr = xmalloc(sizeof(struct sockaddr_in));

        sockaddr->sin_family = AF_INET;
        sockaddr->sin_port = htons(atoi(port));

        sockaddr->sin_addr = *(struct in_addr*)hent->h_addr_list[rand()%i];

        if(connect(ircst->sockfd, (struct sockaddr*)sockaddr, 
                                sizeof(struct sockaddr_in)) < 0) {
                debug_print("Couldn't connect to the server.");
                close(ircst->sockfd);
                free(sockaddr);
                return -1;
        }

        free(sockaddr);
        
        if (!(ircst->socket = fdopen(ircst->sockfd, "w"))) {
                debug_print("Couldnt open the sockfd.");
                close(ircst->sockfd);
                return -1;
        }

        cmd_register(ircst);

        return 0;
}

/* Writes the currently playing song (or not) */
void irc_current_song(struct irc_struct *ircst)
{
        char *buf;
        int curTime = time(NULL);

        if (ircst->songTreshold < curTime || ircst->songTimeout > curTime)
                return;

        if (vxml_download(ircst->xmem, 
                            "http://www.rivalcastmedia.com/ajax/live/") < 0)
                return;

        buf = vxml_parse(ircst->xmem, "//live/shoutcast/currentSong");

        if (!buf)
                return;

        if (!ircst->songCurrent || strcmp(buf, ircst->songCurrent) != 0) {
                fprintf(ircst->socket, 
                                "PRIVMSG %s :%c12,08%s\r\n",
                                set_lookup(ircst->set, "channel"), 3, buf);
                fflush(ircst->socket);

                free(ircst->songCurrent);
                ircst->songCurrent = buf;
        } else {
                free(buf);
        }

        ircst->songTimeout = time(NULL) + 10;
}

/* Initiates the irc_struct, set can be NULL */
struct irc_struct* irc_init(struct settings *set)
{
        struct irc_struct *ircst = xmalloc(sizeof(struct irc_struct));
        
        ircst->buffer = xmalloc(IRC_BUFFER_SIZE);
        ircst->socket = NULL;
        ircst->sockfd = 0;

        ircst->xmem = vxml_init();
        ircst->songCurrent = NULL;
        ircst->songTimeout = 0;
        ircst->songTreshold = 0;

        ircst->set = set;

        srand(time(NULL));

        return ircst;
}

/* Starts the main loop */
void irc_main_loop(struct irc_struct *ircst)
{
        fd_set sockset;
        struct timeval tv;
        int rsize = 0;
        int offset = 0;
        int timeouts = 0;

        char *buf = ircst->buffer;

        if (!(ircst && ircst->sockfd && ircst->socket))
                return;

        FD_ZERO(&sockset);

        while (timeouts++ < IRC_MAX_TIMEOUTS) {
                FD_SET(ircst->sockfd, &sockset);
                
                tv.tv_sec = IRC_READ_TIMEOUT;
                tv.tv_usec = 0;

        while (select(ircst->sockfd+1, &sockset, NULL, NULL, &tv) != 0) {
                tv.tv_sec = IRC_READ_TIMEOUT;
                tv.tv_usec = 0;
                timeouts = 0;

                rsize = read(ircst->sockfd, buf, IRC_BUFFER_SIZE);

                while (offset < rsize -1) {
                        if (!(buf[offset++] == '\r' && buf[offset] == '\n'))
                                continue;

                        buf[offset-1] = '\0';

                        handle_command(ircst, buf);
                        offset++;

                        if (rsize == offset) {
                                rsize = 0;
                                offset = 0;
                                break;
                        }

                        rsize -= offset;
                        memmove(buf, buf+offset, rsize);
                        offset = 0;
                }

                irc_current_song(ircst);
        }
        irc_current_song(ircst);
        }
}

/* Sends the server a quit message */
void irc_quit(struct irc_struct *ircst, char *msg)
{	
        fd_set sockset;
	struct timeval tv;

        fprintf(stderr, "Quitting: %s\n", msg);

        if (ircst->socket) {
                fprintf(ircst->socket, "QUIT :%s\r\n", msg);
                fflush(ircst->socket);

                FD_ZERO(&sockset);
                FD_SET(ircst->sockfd, &sockset);
                tv.tv_sec = 4;
                tv.tv_usec = 0;

                select(ircst->sockfd+1, &sockset, NULL, NULL, &tv);
        }
}
