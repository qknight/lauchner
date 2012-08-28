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

#ifndef TASK__HH
#define TASK__HH


#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <menu.h>
#include <signal.h>

#include "MenuEntry.hh"


class Task : public MenuEntry {
public:
    Task(std::string title, std::string description, std::string command, std::string args, std::string workdir, std::string logfile);
    ~Task();
    int type();
    std::string getTitle();
    std::string getCommand();
    std::string getWorkDir();
    std::string getLogFile();
    std::string getDescription();
    std::string getArgs();
    int getPID();
    int getState();
    void updatePID(int pid);
    // 0 - startable; 1 - started and blocked
    void updateState(int state);
    int launch();
    void kill();
    void force_kill();
private:
    ITEM ** m_item;
    std::string m_title;
    std::string m_args;
    std::string m_command;
    std::string m_workdir;
    std::string m_logfile;
    std::string m_description;
    int m_pid;
    int m_state; // 0 stopped; 1 running
};

#endif // TASK__HH
