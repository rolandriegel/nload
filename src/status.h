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


/**
  *@author Roland Riegel
  */

#include <iostream.h>
#include <curses.h>

#include <limits.h>

class Status
{

public:
	Status();
	~Status();
	
	void update( int, long long );
	void setWindow( WINDOW * );
	void print( int, int );
	
	void setAverageSmoothness( int );
	
private:
	void minMax( int );
	void average( int );
	
	int m_min, m_max, m_average, m_cur;
	long long m_total;
	int m_averagesmoothness;
	
	WINDOW *m_window;

};

#endif
