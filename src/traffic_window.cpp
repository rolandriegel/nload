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

#include "traffic_window.h"

#include "deviceview.h"
#include "setting.h"
#include "settingstore.h"

using namespace std;

TrafficWindow::TrafficWindow()
    : Window(), m_curDev(0)
{
}

TrafficWindow::~TrafficWindow()
{
}

void TrafficWindow::processKey(int key)
{
    switch(key)
    {
        case KEY_RIGHT:
        case KEY_DOWN:
        case KEY_NPAGE:
        case KEY_ENTER:
        case '\n':
        case '\t':
        case '\015':
            m_curDev += showMultipleDevices() ? getHeight() / 9 : 1;
            break;
        case KEY_LEFT:
        case KEY_UP:
        case KEY_PPAGE:
            m_curDev -= showMultipleDevices() ? getHeight() / 9 : 1;
            break;
    }
}

void TrafficWindow::printTraffic(const vector<DeviceView*>& deviceViews)
{
    if(deviceViews.empty())
        return;

    if((unsigned int) m_curDev >= deviceViews.size() || m_curDev < 0)
        m_curDev = 0;

    // print data of the current device(s)
    if(!showMultipleDevices())
    {
        deviceViews[m_curDev]->print(*this);
    }
    else
    {
        if((unsigned int) getHeight() / 9 >= deviceViews.size())
            m_curDev = 0;

        int i = m_curDev;
        while(getHeight() - getY() >= 9)
        {
            deviceViews[i++]->print(*this);

            if((unsigned int) i >= deviceViews.size())
                break;
        }
    }
}

bool TrafficWindow::showMultipleDevices()
{
    return SettingStore::get("MultipleDevices");
}

