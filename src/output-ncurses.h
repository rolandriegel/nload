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

#ifndef OUTPUT_NCURSES_H
#define OUTPUT_NCURSES_H

#include "output.h"

#include <vector>

class AppControl;
class Device;
class DeviceView;
class OptWindow;
class TrafficWindow;

class OutputNcurses : public Output
{
    public:
        OutputNcurses(AppControl* appControl, std::vector<Device*> devices);
        virtual ~OutputNcurses();

        virtual void process();
        virtual bool processSignal(int signal);

        virtual void output();

    private:
        void updateGraphs();
        void refresh();

        void showOptions(bool show);
        void init();
        void finish();
        void terminalResized();

    private:
        AppControl* m_appControl;
        std::vector<Device*> m_devices;
        std::vector<DeviceView*> m_deviceViews;

        OptWindow* m_optWindow;
        TrafficWindow* m_mainWindow;
};

#endif

