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

#ifndef MENUENTRY__HH
#define MENUENTRY__HH


#include <string>
#include <vector>


class MenuEntry {
public:
    MenuEntry() {};
    MenuEntry(std::string title, std::string description) {
        m_title = title;
        m_description = description;
    }
    virtual ~MenuEntry() {};
    virtual int type() {
        return 0;
    };
    virtual std::string getTitle() {
        return m_title;
    }
    virtual std::string getDescription() {
        return m_description;
    }
private:
    std::string m_title;
    std::string m_description;
};




#endif //MENUENTRY__HH
