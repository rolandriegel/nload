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

#define STANDARD_BAR_MAX_IN 10240
#define STANDARD_BAR_MAX_OUT 10240
#define STANDARD_SLEEP_INTERVAL 500
#define STANDARD_NETWORK_DEVICE "eth0"
#define STANDARD_AVERAGE_SMOOTHNESS 9

class Dev : public Proc
{

public:
	Dev();
	Dev( char *, long, long, int, WINDOW *, int, int );
	~Dev();

	void update( bool );
	void setDeviceNumber( int );
	void setTotalNumberOfDevices( int );
	
  void setWindow( WINDOW * );
	
private:
	int device_number;
	int total_number_of_devices;
	
	Status *device_status[2];
	Graph *traffic_graph[2];
	
	WINDOW *window;
	
};

#endif
