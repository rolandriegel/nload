/***************************************************************************
                             traffic_window.cpp
                             -------------------
    begin                : Thu Jul 04 2002
    copyright            : (C) 2002 by Roland Riegel
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

#include "traffic_window.h"

TrafficWindow::TrafficWindow()
    : Window(), m_cur_dev(0), m_show_multiple_devices( false )
{
}

TrafficWindow::~TrafficWindow()
{
}

void TrafficWindow::setDevices( vector<Dev *>& new_devs )
{
	m_devs = new_devs;
}

vector<Dev *>& TrafficWindow::devices()
{
	return m_devs;
}

void TrafficWindow::processKey( int key )
{
	switch( key )
	{
		case KEY_RIGHT:
			m_cur_dev += m_show_multiple_devices ? height() / 9 : 1;
			if( m_cur_dev >= m_devs.size() )
				m_cur_dev = 0;
			break;
		case KEY_LEFT:
			m_cur_dev -= m_show_multiple_devices ? height() / 9 : 1;
			if( m_cur_dev < 0 )
				m_cur_dev = m_devs.size() - 1;
			break;
	}
	if( m_show_multiple_devices && height() / 9 >= m_devs.size() )
		m_cur_dev = 0;
}

void TrafficWindow::print()
{
	//update all devices and print the data of the current one
	for( int i = 0; i < m_devs.size(); i++ )
	{
		m_devs[i] -> update();
		if( ! m_show_multiple_devices )
		{
			if( i == m_cur_dev )
				m_devs[i] -> print( *this );
		}
		else
		{
			if( i >= m_cur_dev && height() - y() >= 9 )
				m_devs[i] -> print( *this );
		}
	}
}

void TrafficWindow::setShowMultipleDevices( bool new_smd )
{
	m_show_multiple_devices = new_smd;
	for( vector<Dev *>::const_iterator i = m_devs.begin(); i != m_devs.end(); i++ )
		(*i) -> setShowGraphs( ! new_smd );
	
}

bool TrafficWindow::showMultipleDevices()
{
	return m_show_multiple_devices;
}

