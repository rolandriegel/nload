/***************************************************************************
                              traffic_window.h
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

#ifndef TRAFFIC_WINDOW_H
#define TRAFFIC_WINDOW_H

#include "screen.h"
#include "window.h"
#include "dev.h"

#include <vector>
using std::vector;

class TrafficWindow : public Window
{
public:
	
	TrafficWindow();
	~TrafficWindow();
	
	void processKey( int key );
	void print();
	
	vector<Dev *>& devices();
	void setDevices( vector<Dev *>& );
	
	void setShowMultipleDevices( bool );
	bool showMultipleDevices();
	
private:
	
	vector<Dev *> m_devs;
	
	int m_cur_dev;
	bool m_show_multiple_devices;
	
};

#endif
