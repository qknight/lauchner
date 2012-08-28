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

#include "task.hh"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "launcher.hh"


Task::Task(std::string title, std::string description, std::string command, std::string args, std::string workdir, std::string logfile) {
    m_title = title;
    m_command = command;
    m_workdir = workdir;
    m_logfile = logfile;
    m_description = description;
    m_args = args;
    m_state = 0;
    m_pid = 0;
}


Task::~Task() {
    force_kill();
}


void Task::kill() {
    if (m_pid != 0) {
        ::kill(m_pid, 15);
        updateState(0);
    }
    Launcher::Instance()->update_status();
}


void Task::force_kill() {
    if (m_pid != 0) {
        ::kill(m_pid, 9);
        updateState(0);
    }
    Launcher::Instance()->update_status();
}


// returns -1 on error; or child pid instead
int Task::launch() {
    if (m_state == 1)
        return 0;
    pid_t childpid = fork();

    if(childpid == -1) {
        printf("Failed to fork, process not starting\n");
        return -1;
    }

    if(childpid == 0) {
        //this is the child
        setsid();

        int fd = open(m_logfile.c_str(), O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);

        // now redirect both stdout and stderr to fd instead of the parent
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);

        if (chdir(m_workdir.c_str()) != 0) {
            printf("chdir(%s) returned the error: '%s'\n", m_workdir.c_str(), strerror(errno));
            return -1;
        }
        int execReturn = execl(m_command.c_str(), m_command.c_str(), (char *) 0);

        if (execReturn != -1) {
//             printf("success: executing: '%s', execl returned %i\n", m_command.c_str(), execReturn);
            return -1;
        } else {
            printf("executing: '%s', execve returned %i\n", m_command.c_str(), execReturn);
        }
    } else {
        //this is the parent do nothing
        updatePID(childpid);
        updateState(1);
    }
    Launcher::Instance()->update_status();
    return 0;
}


void Task::updatePID(int pid) {
    m_pid = pid;
}


void Task::updateState(int state) {
    m_state = state;
}

int Task::type() {
    return 1;
};


std::string Task::getTitle() {
    return m_title;
}


std::string Task::getCommand() {
    return m_command;
}


std::string Task::getWorkDir() {
    return m_workdir;
}


std::string Task::getLogFile() {
    return m_logfile;
}


std::string Task::getDescription() {
    return m_description;
}


int Task::getPID() {
    return m_pid;
}


int Task::getState() {
    return m_state;
}

std::string Task::getArgs() {
    return m_args;
}

