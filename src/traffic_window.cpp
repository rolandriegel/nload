/***************************************************************************
                             traffic_window.cpp
                             -------------------
    begin                : Thu Jul 04 2002
    copyright            : (C) 2002 - 2007 by Roland Riegel
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

vector<Device*>& TrafficWindow::devices()
{
    return m_devices;
}

void TrafficWindow::processKey(int key)
{
    switch(key)
    {
        case KEY_RIGHT:
        case KEY_DOWN:
        case KEY_NPAGE:
        case KEY_ENTER:
        case 'n':
        case '\n':
        case '\t':
        case '\015':
            m_curDev += showMultipleDevices() ? getHeight() / 9 : 1;
            if((unsigned int) m_curDev >= m_devices.size())
                m_curDev = 0;
            break;
        case KEY_LEFT:
        case KEY_UP:
        case KEY_PPAGE:
        case 'p':
            m_curDev -= showMultipleDevices() ? getHeight() / 9 : 1;
            if(m_curDev < 0)
                m_curDev = m_devices.size() - 1;
            break;
    }
    if(showMultipleDevices() && (unsigned int) getHeight() / 9 >= m_devices.size())
        m_curDev = 0;
}

void TrafficWindow::print()
{
    // update all devices and print the data of the current one
    for(int i = 0; (unsigned int) i < m_devices.size(); i++)
    {
        m_devices[i]->update();
        if(!showMultipleDevices())
        {
            if(i == m_curDev)
                m_devices[i]->print(*this);
        }
        else
        {
            if(i >= m_curDev && getHeight() - getY() >= 9)
                m_devices[i]->print(*this);
        }
    }
}

bool TrafficWindow::showMultipleDevices()
{
    return SettingStore::get("multiple_devices");
}

