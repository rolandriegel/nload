/*
 * nload
 * real time monitor for network traffic
 * Copyright (C) 2001 - 2018 by Roland Riegel <feedback@roland-riegel.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef APP_H
#define APP_H

#include "appcontrol.h"

#include <string>
#include <vector>

class Output;

class App : public AppControl
{
    public:
        App();
        ~App();

        int run(const std::vector<std::string>& arguments);

        void processSignal(int signal);

        virtual void loadSettings();
        virtual void saveSettings();

        virtual void quit();

    private:
        bool m_quit;
        std::vector<Output*> m_outputs;

        void printHelp(bool error);
};

#endif

