/***************************************************************************
                          status.h  -  description
                             -------------------
    begin                : Sat Sep 29 2001
    copyright            : (C) 2001, 2002 by Roland Riegel
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

#ifndef STATUS_H
#define STATUS_H

#include <curses.h>
#include <limits.h>
#include <math.h>
#include "window.h"

class Status
{

public:
	Status();
	~Status();
	
	enum status_format
	{
		human_readable_bit = -2,
		human_readable_byte = -1,
		bit = 0,
		byte = 1,
		kilobit = 2,
		kilobyte = 3,
		megabit = 4,
		megabyte = 5,
		gigabit = 6,
		gigabyte = 7
	};
	
	void update( int, unsigned int );
	void print( Window&, int, int, status_format, status_format );
	void resetTrafficData();
	
	void setAverageSmoothness( int );
	
private:
	void minMax( int );
	void average( int );
	
	const char* getUnitString( status_format, long long );
	double getUnitFactor( status_format, long long );
	
	int m_min, m_max, m_average, m_cur;
	long long m_total;
	int m_averagesmoothness;
	
};

#endif
