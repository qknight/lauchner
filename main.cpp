// launcher is a ncurses based simple c/c++ tool to launch and control a list of programs
// Copyright (C) 2012 Joachim Schiele <js@lastlog.de>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>

#include "launcher.hh"


// signal handler so that programs which are quit by the user: by using alt+f4 for examples are recognized
void signal_callback_handler(int signum) {
    while(waitpid(-1,NULL,WNOHANG) > 0)
        Launcher::Instance()->signal_callback_handler(signum);
}


int main(int argc, char *argv[]) {
    // thanks to http://www.linuxprogrammingblog.com/code-examples/SIGCHLD-handler
    struct sigaction act;
    act.sa_handler = signal_callback_handler;
    if (sigaction(SIGCHLD, &act, 0)) {
        printf("fatal: child action handler issue\n");
        exit(1);
    }
    Launcher::Instance()->run_mainloop();

    return 0;
}
