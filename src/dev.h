/***************************************************************************
                          dev.h  -  description
                             -------------------
    begin                : Wed Aug 1 2001
    copyright            : (C) 2001 by Roland Riegel
    email                : support@roland-riegel.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DEV_H
#define DEV_H


/**
  *@author Roland Riegel
  */

#include <iostream.h>
#include <curses.h>
#include "proc.h"
#include "graph.h"
#include "status.h"

const long STANDARD_BAR_MAX_IN = 10240;
const long STANDARD_BAR_MAX_OUT = 10240;
const bool STANDARD_SHOW_GRAPHS = true;
const int STANDARD_SLEEP_INTERVAL = 500;
char* const STANDARD_NETWORK_DEVICE = "eth0";
const int STANDARD_AVERAGE_SMOOTHNESS = 9;

class Dev : public Proc
{

public:
	Dev();
	~Dev();

	void update( bool );
	void setDeviceNumber( int );
	void setTotalNumberOfDevices( int );
	void setAverageSmoothness( int );
	void setShowGraphs( bool );
	void setTrafficWithMaxDeflectionOfGraphs( int, int );
	
	void setWindow( WINDOW * );
	
private:
	int m_devicenumber;
	int m_totalnumberofdevices;
	bool m_showgraphs;
	
	Status *device_status[2];
	Graph *traffic_graph[2];
	
	WINDOW *m_window;
	
};

#endif
