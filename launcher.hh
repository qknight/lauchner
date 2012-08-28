#ifndef LAUNCHER__HH
#define LAUNCHER__HH


#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>


#include "MenuEntry.hh"
#include "task.hh"


class Launcher {
public:
    Launcher();

    static Launcher* Instance();
    Task* resolv(std::string identifier);
    void update_status();
    void signal_callback_handler(int signum);
    void run_mainloop();
    void start_all();
    void stop_all();
    void notify(std::string message);

private:
    MENU *my_menu;
    ITEM **my_items;
    std::vector<MenuEntry*> myMenuItems;
    static Launcher* _instance;
    unsigned int i;
    int run;
    int c;

};

#endif //LAUNCHER__HH
