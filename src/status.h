/***************************************************************************
                          status.h  -  description
                             -------------------
    begin                : Sat Sep 29 2001
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

#ifndef STATUS_H
#define STATUS_H

#include <iostream.h>
#include <curses.h>
#include <limits.h>
#include <math.h>

class Status
{

public:
	Status();
	~Status();
	
	enum status_format
	{
		human_readable = -1,
		bit = 0,
		kilobit = 1,
		megabit = 2,
		gigabit = 3
	};
	
	void update( int, unsigned int );
	void print( int, int, status_format, status_format );
	void resetTrafficData();
	
	void setAverageSmoothness( int );
	void setWindow( WINDOW * );
	
private:
	void minMax( int );
	void average( int );
	
	const char* getUnitString( status_format, long long, bool );
	int getUnitFactor( status_format, long long );
	
	int m_min, m_max, m_average, m_cur;
	long long m_total;
	int m_averagesmoothness;
	
	WINDOW *m_window;

};

#endif
