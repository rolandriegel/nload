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

	void update(bool);
	void setDeviceNumber( int );
	void setTotalNumberOfDevices( int );
  void setPrintBars( bool );
  void setBarMaxIn( long );
  void setBarMaxOut( long );
  void setNumOfGraphValues( int );
  void setAverageSmoothness( int );
  void setWindow( WINDOW * );

private:

	void printGraph( bool, int, int );
	void MinMax( float &, float &, float );
	void average( float &, float );

	int device_number;
	int total_number_of_devices;
	
  long bar_max_in;
	long bar_max_out;
	int cur_num_values;
	int average_smoothness;
	
	float *traffic[2];
	float min_in, max_in, avg_in;
	float min_out, max_out, avg_out;
	
	WINDOW *window;
	
};

#endif
