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

#include "dev.h"
#include "setting.h"
#include "settingstore.h"
#include "traffic_window.h"

using namespace std;

TrafficWindow::TrafficWindow()
    : Window(), m_cur_dev(0)
{
}

TrafficWindow::~TrafficWindow()
{
}

void TrafficWindow::setDevices(vector<Dev *>& new_devs)
{
	m_devs = new_devs;
}

vector<Dev *>& TrafficWindow::devices()
{
	return m_devs;
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
			m_cur_dev += showMultipleDevices() ? height() / 9 : 1;
			if((unsigned int) m_cur_dev >= m_devs.size())
				m_cur_dev = 0;
			break;
		case KEY_LEFT:
		case KEY_UP:
		case KEY_PPAGE:
		case 'p':
			m_cur_dev -= showMultipleDevices() ? height() / 9 : 1;
			if(m_cur_dev < 0)
				m_cur_dev = m_devs.size() - 1;
			break;
	}
	if(showMultipleDevices() && (unsigned int) height() / 9 >= m_devs.size())
		m_cur_dev = 0;
}

void TrafficWindow::print()
{
	// update all devices and print the data of the current one
	for(int i = 0; (unsigned int) i < m_devs.size(); i++)
	{
		m_devs[i]->update();
		if(!showMultipleDevices())
		{
			if(i == m_cur_dev)
				m_devs[i] -> print(*this);
		}
		else
		{
			if(i >= m_cur_dev && height() - y() >= 9)
				m_devs[i] -> print(*this);
		}
	}
}

bool TrafficWindow::showMultipleDevices()
{
    return SettingStore::get("multiple_devices");
}

