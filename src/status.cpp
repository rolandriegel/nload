/***************************************************************************
                          status.cpp  -  description
                             -------------------
    begin                : Sat Sep 29 2001
    copyright            : (C) 2001, 2002 by Roland Riegel
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

#include "status.h"

Status::Status()
{
	m_min = m_max = m_average = -1;
	m_cur = m_total = 0;
}

Status::~Status()
{
}

//new traffic measurement has been made => update statistics
void Status::update( int new_value, unsigned int new_total )
{
	
	m_cur = new_value;
	minMax( m_cur ); //calculate new min/max traffic values
	average( m_cur ); //calculate new average
	
	/*
	 *set new total transfered data
	 *the following is a workaround for the value limitations of the
	 *unsigned int variable used in the kernel to produce
	 *the /proc/net/dev file
	 *(the total bytes value reaches 4GB and then switches to 0)
	 */
	if( new_total < ( m_total % UINT_MAX ) )
		m_total = ( ( m_total / UINT_MAX ) + 1 ) * UINT_MAX + new_total;
	else
		m_total = ( m_total / UINT_MAX ) * UINT_MAX + new_total;
	
}

//set the curses window where to print to
void Status::setWindow( WINDOW *new_window )
{
	m_window = new_window;
}

//print statistics
void Status::print( int x, int y, status_format traff_format, status_format data_format )
{
	
	int curx, cury;
	double value;
	char fText[100] = "";
	
	//print current traffic
	wmove( m_window, y, x );
	value = m_cur * getUnitFactor( traff_format, m_cur );
	sprintf( fText, "Curr: %.2f %s/s\n", value, getUnitString( traff_format, m_cur ) );
	waddstr( m_window, fText );
	//print average traffic since nload start
	getyx( m_window, cury, curx );
	wmove( m_window, cury, x );
	value = m_average * getUnitFactor( traff_format, m_average );
	sprintf( fText, "Avg: %.2f %s/s\n", value, getUnitString( traff_format, m_average ) );
	waddstr( m_window, fText );
	//print min traffic since nload start
	getyx( m_window, cury, curx );
	wmove( m_window, cury, x );
	value = m_min * getUnitFactor( traff_format, m_min );
	sprintf( fText, "Min: %.2f %s/s\n", value, getUnitString( traff_format, m_min ) );
	waddstr( m_window, fText );
	//print max traffic since nload start
	getyx( m_window, cury, curx );
	wmove( m_window, cury, x );
	value = m_max * getUnitFactor( traff_format, m_max );
	sprintf( fText, "Max: %.2f %s/s\n", value, getUnitString( traff_format, m_max ) );
	waddstr( m_window, fText );
	//print total traffic since last system reboot
	getyx( m_window, cury, curx );
	wmove( m_window, cury, x );
	value = m_total * getUnitFactor( data_format, m_total );
	sprintf( fText, "Ttl: %.2f %s\n", value,  getUnitString( data_format, m_total ) );
	waddstr( m_window, fText );
	
}

//reset all displayed values to zero
void Status::resetTrafficData()
{
	m_cur = m_min = m_average = m_max = m_total = 0;
}

//return the matching unit string, e.g. "kBit" for status_format::kilobit
const char* Status::getUnitString( status_format format, long long value )
{
	switch( format )
	{
		case human_readable_bit:
		case human_readable_byte:
			for( int i = 3; i >= 0; i-- )
				if ( value * ( format % 2 == 0 ? 8 : 1 ) >= pow( 1024, i ) )
					switch(i)
					{
						case 3:
							return format % 2 == 0 ? "GBit" : "GByte";
						case 2:
							return format % 2 == 0 ? "MBit" : "MByte";
						case 1:
							return format % 2 == 0 ? "kBit" : "kByte";
						case 0:
							return format % 2 == 0 ? "Bit" : "Byte";
					}
		case bit:
			return "Bit";
		case byte:
			return "Byte";
		case kilobit:
			return "kBit";
		case kilobyte:
			return "kByte";
		case megabit:
			return "MBit";
		case megabyte:
			return "MByte";
		case gigabit:
			return "GBit";
		case gigabyte:
			return "GByte";
		default: //should never be executed
			return "";
	}
}

//return the matching conversion factor between Bit and e.g. status_format::kilobit
double Status::getUnitFactor( status_format format, long long value )
{
	if( format < 0 ) //human readable
	{
		for( int i = 3; i >= 0; i-- )
			if ( value * ( format % 2 == 0 ? 8 : 1 ) >= pow( 1024, i ) )
				return ( (double) ( format % 2 == 0 ? 8 : 1 ) / pow( 1024, i ) );
		return ( (double) ( format % 2 == 0 ? 8 : 1 ) );
	}
	else
	{
		return ( (double) ( format % 2 == 0 ? 8 : 1 ) / pow( 1024, format / 2 ) );
	}
}

//calculate min and max traffic values
void Status::minMax( int new_value )
{
	
	//if this is the first time call, set min/max to current value
	if( m_min == -1 && m_max == -1 )
	{
		m_min = new_value;
		m_max = new_value;
		return;
	}
	
	m_min = new_value < m_min ? new_value : m_min;
	
	m_max = new_value > m_max ? new_value : m_max;
	
}

//set the "reaction time" to the current traffic situation of the average values
void Status::setAverageSmoothness( int new_averagesmoothness )
{
	
	if( ( new_averagesmoothness >= 1 ) && ( new_averagesmoothness <= 9 ) )
		m_averagesmoothness = new_averagesmoothness;
	
}

//calculate new average
void Status::average( int new_value )
{
	
	/*
	 * average calculation is not very good at the moment as it is not a real average
	 * and is dependent from the display refresh interval.
	 * it is a running estimate updated by forming a weighted average.
	 * could need some help here.
	 */
	
	//if this is the first time call, set average to current value
	if( m_average == -1 )
	{
		m_average = new_value;
		return;
	}
	
	m_average = (int) ( new_value * ( 1.0 - m_averagesmoothness / 10.0 ) + m_average * ( m_averagesmoothness / 10.0 ) );
	
}
