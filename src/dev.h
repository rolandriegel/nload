/***************************************************************************
                                   dev.h
                             -------------------
    begin                : Wed Aug 1 2001
    copyright            : (C) 2001 - 20003 by Roland Riegel
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

#include <curses.h>
#include <config.h>
#include "proc.h"
#include "status.h"

class Status;
class Graph;
class Window;
class OptionBool;
class OptionInt;
class OptionLong;
class OptionStatusFormat;

const bool STANDARD_HIDE_GRAPHS = false;
const int STANDARD_SLEEP_INTERVAL = 500;
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
	void setAverageSmoothness( OptionInt* );
	void setHideGraphs( OptionBool* );
	void setTrafficWithMaxDeflectionOfGraphs( OptionLong*, OptionLong* );
	void setStatusFormat( OptionStatusFormat*, OptionStatusFormat* );
	
private:
	bool hideGraphs();
	Status::status_format trafficFormat();
	Status::status_format dataFormat();
	
	int m_devicenumber;
	int m_totalnumberofdevices;
	OptionBool* m_hidegraphs;
	OptionStatusFormat* m_trafficformat;
	OptionStatusFormat* m_dataformat;
	
	Status *device_status[2];
	Graph *traffic_graph[2];
	
};

#endif
