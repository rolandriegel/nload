/***************************************************************************
                             traffic_window.cpp
                             -------------------
    begin                : Thu Jul 04 2002
    copyright            : (C) 2002 - 2012 by Roland Riegel
    email                : feedback@roland-riegel.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "device.h"
#include "setting.h"
#include "settingstore.h"
#include "traffic_window.h"

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

void TrafficWindow::printTraffic(const vector<Device*>& devices)
{
    if((unsigned int) m_curDev >= devices.size() || m_curDev < 0)
        m_curDev = 0;

    // print data of the current device(s)
    if(!showMultipleDevices())
    {
        devices[m_curDev]->print(*this);
    }
    else
    {
        if((unsigned int) getHeight() / 9 >= devices.size())
            m_curDev = 0;

        int i = m_curDev;
        while(getHeight() - getY() >= 9)
        {
            devices[i++]->print(*this);

            if((unsigned int) i >= devices.size())
                break;
        }
    }
}

bool TrafficWindow::showMultipleDevices()
{
    return SettingStore::get("MultipleDevices");
}

