/***************************************************************************
                          dev.h  -  description
                             -------------------
    begin                : Wed Aug 1 2001
    copyright            : (C) 2001, 20002 by Roland Riegel
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

#ifndef DEV_H
#define DEV_H

#include <iostream.h>
#include <curses.h>
#include <config.h>
#include "proc.h"
#include "graph.h"
#include "status.h"
#include "window.h"

const int STANDARD_BAR_MAX_IN = 10240;
const int STANDARD_BAR_MAX_OUT = 10240;
const bool STANDARD_SHOW_GRAPHS = true;
const int STANDARD_SLEEP_INTERVAL = 500;
const int STANDARD_AVERAGE_SMOOTHNESS = 9;
const Status::status_format STANDARD_TRAFFIC_FORMAT = Status::kilobit;
const Status::status_format STANDARD_DATA_FORMAT = Status::megabyte;

#ifdef HAVE_LINUX
char* const STANDARD_NETWORK_DEVICE = "eth0";
#endif

#ifdef HAVE_BSD
char* const STANDARD_NETWORK_DEVICE = "fxp0";
#endif

#ifdef HAVE_SOLARIS
char* const STANDARD_NETWORK_DEVICE = "hme0";
#endif

class Dev : public Proc
{

public:
	Dev();
	~Dev();

	void update();
	void print( Window& );
	void setDeviceNumber( int );
	void setTotalNumberOfDevices( int );
	void setAverageSmoothness( int );
	void setShowGraphs( bool );
	void setTrafficWithMaxDeflectionOfGraphs( int, int );
	void setStatusFormat( Status::status_format, Status::status_format );
	
private:
	int m_devicenumber;
	int m_totalnumberofdevices;
	bool m_showgraphs;
	Status::status_format m_trafficformat;
	Status::status_format m_dataformat;
	
	Status *device_status[2];
	Graph *traffic_graph[2];
	
};

#endif
