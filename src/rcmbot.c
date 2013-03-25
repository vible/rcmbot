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

#include "includes.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "irc.h"
#include "settings.h"

struct irc_struct *ircst;

static struct settings* load_settings();
static void signal_handler(int sig);
static void signal_init();

int main(int argc, char *argv[])
{
        struct settings *set = load_settings();
        argc = argc;
        argv = argv;

        signal_init();

        while (1) {
                ircst = irc_init(set);

                if(irc_connect(ircst, set_lookup(set, "server"),
                                        set_lookup(set, "port")) < 0)
                        return EXIT_FAILURE;

                irc_main_loop(ircst);

                irc_cleanup(ircst);
        }
        
        return EXIT_SUCCESS;
}

/* Loads settings from the file and sets them if they are absent */
static struct settings* load_settings()
{
        int nofile = 0;
        struct settings *set = set_load_file("settings.cfg");

        if (!set)
                nofile = 1;

        set = set_default(set, "server", "irc.quakenet.org");
        set = set_default(set, "port", "6667");
        set = set_default(set, "nick", "RCMRadioBot");
        set = set_default(set, "channel", "#rcmbottest");
        set = set_default(set, "logging", "1");
        set = set_default(set, "quakeauth", "0");
        set = set_default(set, "quakeauthname", "RCMRadioBott");
        set = set_default(set, "quakeauthpw", "");

        if (nofile)
                set_save_file(set, "settings.cfg");
        
        return set;
}

/* Handles all the signals by saying bye to the server */
static void signal_handler(int sig)
{
        switch (sig) {
		case SIGHUP:
                        irc_quit(ircst, "Hangup Interrupt");
                        exit(0);
			break;
		case SIGINT:
                        irc_quit(ircst, "Keyboard Interrupt");
                        exit(0);
			break;
		case SIGTERM:
                        irc_quit(ircst, "Caught termination signal");
                        exit(0);
			break;
                case SIGSEGV:
                        irc_quit(ircst, "ILLEGAL MEMORY ACCESS");
                        exit(139);
                        break;
		default:
			return;
        }
}

/* Registers the desired signals */
static void signal_init()
{
	int signals[4] = {SIGHUP, SIGINT, SIGTERM, SIGSEGV};
        struct sigaction action;
        int i;

        action.sa_handler = signal_handler;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;

       for (i = 0; i < 4; i++) {
               if (sigaction(signals[i], &action, NULL) < 0) {
                       debug_print("Couldn't register signals");
                       exit(EXIT_FAILURE);
               }
       }
}
