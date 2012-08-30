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

#include "launcher.hh"
#include <sstream>

Launcher* Launcher::_instance = NULL;


Launcher::Launcher() {
    run = 1; // keeps the program running until it is set to 0
}


Launcher* Launcher::Instance() {
    if(_instance == NULL) {
        _instance = new Launcher();
    }
    return _instance;
}


void Launcher::notify(std::string message) {
    move(myMenuItems.size()+2, 0);
    clrtoeol();
    mvprintw(myMenuItems.size()+2, 0, message.c_str());
}


void Launcher::run_mainloop() {
    myMenuItems.push_back(new MenuEntry("Start all","starts all applications")); // do not alter this text
    myMenuItems.push_back(new MenuEntry("Stop all","stops all applications (normal TERM)")); // do not alter this text
    myMenuItems.push_back(new MenuEntry("--",""));

    //WARNING:
    // - arguments are currently not implemented still they can be assigned below
    // - check that the working directory is correct and exists
    // - create nice comments for the commands
    // - commands which require a shell don't work. example: env, export,
    // - some programs have problem with stdin (fd 0) like tail; i wasn't able to make them work!
    // - if using relative paths for the binary NOTE that it MUST be relative to the given 'working directory' not the 
    //   path where you are starting ./Launcher from! 
    // - if a binary can't be found but is started from Launcher using execl (or whatever is implemented) it might
    //   eat 100% cpu; this is a bug and i didn't fix it yet

    myMenuItems.push_back(new Task("kate", "a nice text editor", "/usr/bin/kate", "", "/tmp", "logs/kate"));
    myMenuItems.push_back(new Task("kcalc", "a kdelibs based calculator", "/usr/bin/kcalc", "", "/tmp", "logs/kcalc"));
    myMenuItems.push_back(new Task("dmesg", "kernel log for system events", "/bin/dmesg", "", "/tmp", "logs/dmesg"));

    myMenuItems.push_back(new MenuEntry("--",""));
    myMenuItems.push_back(new MenuEntry("Exit","")); // do not alter this text


    /* Initialize curses */
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK);

    /* Initialize items */
    my_items = (ITEM **)calloc(myMenuItems.size() + 1, sizeof(ITEM *));

    for(i = 0; i < myMenuItems.size(); ++i) {
        std::string key = myMenuItems[i]->getTitle();
        my_items[i] = new_item(key.c_str(), myMenuItems[i]->getDescription().c_str());
        if (key == "--")
            item_opts_off(my_items[i], O_SELECTABLE);
    }

    /* Create menu */
    my_menu = new_menu((ITEM **)my_items);

    /* Set fore ground and back ground of the menu */
    set_menu_fore(my_menu, COLOR_PAIR(1) | A_REVERSE);
    set_menu_back(my_menu, COLOR_PAIR(2));
    set_menu_grey(my_menu, COLOR_PAIR(3));

    /* Post the menu */
    mvprintw(LINES - 10, 0, "to see the logs, do this: 'cd logs; multitail *' in a different shell ");
    mvprintw(LINES - 9, 0, "");
    mvprintw(LINES - 8, 0, "s - start selected application");
    mvprintw(LINES - 7, 0, "k - stop (kill 15 (TERM)) selected application");
    mvprintw(LINES - 6, 0, "f - stop (kill -9 (KILL)) selected application");
    mvprintw(LINES - 5, 0, "");
    mvprintw(LINES - 4, 0, "The RETURN key can be used to start a selected application (if not already started)");
    mvprintw(LINES - 3, 0, "Up and Down arrow keys to naviage (ESC to Exit - exiting will force_kill all applications)");
    mvprintw(LINES - 2, 0,"-- launcher is licensed GPLv2 --");
    post_menu(my_menu);
    refresh();

    Task* w;
    while( run && (c = getch()))
    {   switch(c)
        {
        case 115: // 's' key
            w = resolv(item_name(current_item(my_menu)));
            if (w != NULL)
                if (w->getState() == 0) {
                    notify(std::string("Starting application: " + std::string(item_name(current_item(my_menu)))));
                    w->launch();
                }
            break;
        case 102: // 'f' key
            w = resolv(item_name(current_item(my_menu)));
            if (w != NULL)
                if (w->getState() == 1) {
                    notify(std::string("Shutting down (using kill -9) application: " + std::string(item_name(current_item(my_menu)))));
                    w->force_kill();
                }
            break;
        case 107: // 'k' key
            w = resolv(item_name(current_item(my_menu)));
            if (w != NULL)
                if (w->getState() == 1) {
                    notify(std::string("Shutting down (using kill) application: " + std::string(item_name(current_item(my_menu)))));
                    w->kill();
                }
            break;
        case 27: // ESC key
            run=0;
            break;
        case KEY_F(1): // F1 key
            run=0;
            break;
        case KEY_DOWN:
            menu_driver(my_menu, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(my_menu, REQ_UP_ITEM);
            break;
        case 10: /* RETURN key */
            notify(std::string("Item selected is : " + std::string(item_name(current_item(my_menu)))));
            pos_menu_cursor(my_menu);

            if(strcmp(item_name(current_item(my_menu)), "Exit") == 0)
                run=0;
            if(strcmp(item_name(current_item(my_menu)), "Start all") == 0)
                start_all();
            if(strcmp(item_name(current_item(my_menu)), "Stop all") == 0)
                stop_all();
            w = resolv(item_name(current_item(my_menu)));
            if (w != NULL)
                if (w->getState() == 0)
                    w->launch();
            break;
        default:
//             printf("%u\n", c); // to find out which key binds to which int
            break;
        }
    }

    for(i = 0; i < myMenuItems.size(); ++i) {
        delete myMenuItems[i];
    }

    unpost_menu(my_menu);
    for(i = 0; i < myMenuItems.size(); ++i)
        free_item(my_items[i]);
    free_menu(my_menu);

    endwin(); // quits the ncurses window manager
}



void Launcher::update_status() {
    if (run == 1) {
        for(unsigned int i=0; i < myMenuItems.size(); i++) {
            if (myMenuItems[i]->type() == 1) {
                Task* t = static_cast<Task*>(myMenuItems[i]);
                if (t->getState())
                    item_opts_off(my_items[i], O_SELECTABLE);
                else
                    item_opts_on(my_items[i], O_SELECTABLE);
            }
        }
        refresh();
    }
}


Task* Launcher::resolv(std::string identifier) {
    for(unsigned int i=0; i < myMenuItems.size(); i++) {
//         printf("%i %i %s %s", myMenuItems.size(), i, identifier.c_str(), myMenuItems[i]->getTitle().c_str());
        if (myMenuItems[i]->getTitle() == identifier)
            if (myMenuItems[i]->type() == 1) {
                return static_cast<Task*>(myMenuItems[i]);
            }
    }
    return NULL;
}


//signal handler so that programs which are quit by the user: by using alt+f4 for examples are recognized
void Launcher::signal_callback_handler(int signum) {
    // check all running tasks if they react; if not then the task's state is corrected
    for(unsigned int i=0; i < myMenuItems.size(); i++) {
        if (myMenuItems[i]->type() == 1) {
            Task* t = static_cast<Task*>(myMenuItems[i]);
            if (kill(t->getPID(), 0) == -1) {
                t->updateState(0);
            }
        }
    }
    update_status();
}



void Launcher::start_all() {
    int delay = 200;

    std::stringstream sstr;
    sstr << "Starting all applications in sequencial order using a '" << delay << "' ms delay between each launch";
    notify(sstr.str());

    for(unsigned int i=0; i < myMenuItems.size(); i++) {
        if (myMenuItems[i]->type() == 1) {
            Task* t = static_cast<Task*>(myMenuItems[i]);
            t->launch();
            usleep(delay * 1000);
        }
    }
}


void Launcher::stop_all() {
    notify("Stopping all running applications using (kill");
    for(unsigned int i=0; i < myMenuItems.size(); i++) {
        if (myMenuItems[i]->type() == 1) {
            Task* t = static_cast<Task*>(myMenuItems[i]);
            if (kill(t->getPID(),0) <= 0) {
                t->kill();
            }
        }
    }
}


