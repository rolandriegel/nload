/***************************************************************************
                                  status.h
                             -------------------
    begin                : Sat Sep 29 2001
    copyright            : (C) 2001 - 2003 by Roland Riegel
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
#include <string>
#include <list>
using std::string;
using std::list;

class Window;
class OptionInt;

const int STANDARD_AVERAGE_SMOOTHNESS = 9;

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
	
	void update( unsigned long, unsigned long );
	void print( Window&, int, int, status_format traff_format, status_format data_format );
	void resetTrafficData();

	void setAverageSmoothness( OptionInt* );
	
private:
	void minMax( unsigned long );
	
	void updateAverage( unsigned long );
	unsigned long calcAverage();
	
	int averageSmoothness();
	
	void getUnit( status_format format, long long value, string& description, float& factor );
	
	unsigned long m_min, m_max, m_cur;
	long long m_total;
	
	list<unsigned long> m_average_values;
	
	OptionInt* m_averagesmoothness;
	
};

#endif
