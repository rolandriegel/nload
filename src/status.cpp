/***************************************************************************
                                 status.cpp
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

#include "status.h"
#include "window.h"
#include "options.h"

Status::Status()
{
	m_min = m_max = -1;
	m_cur = m_total = 0;
	m_averagesmoothness = 0;
}

Status::~Status()
{
}

//new traffic measurement has been made => update statistics
void Status::update( int new_value, unsigned int new_total )
{
	
	m_cur = new_value;
	minMax( m_cur ); //calculate new min/max traffic values
	updateAverage( m_cur ); //calculate new average
	
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

//print statistics
void Status::print( Window& window, int x, int y, status_format traff_format, status_format data_format )
{
	double value;
	double tmp_value;
	char fText[100] = "";
	
	//print current traffic
	window.setXY( x, y );
	value = m_cur * getUnitFactor( traff_format, m_cur );
	sprintf( fText, "Curr: %.2f %s/s\n", value, getUnitString( traff_format, m_cur ) );
	window.print( fText );
	//print average traffic
	window.setX( x );
	tmp_value = calcAverage();
	value = tmp_value * getUnitFactor( traff_format, (long long int)tmp_value );
	sprintf( fText, "Avg: %.2f %s/s\n", value, getUnitString( traff_format, (long long int)tmp_value ) );
	window.print( fText );
	//print min traffic since nload start
	window.setX( x );
	value = m_min * getUnitFactor( traff_format, m_min );
	sprintf( fText, "Min: %.2f %s/s\n", value, getUnitString( traff_format, m_min ) );
	window.print( fText );
	//print max traffic since nload start
	window.setX( x );
	value = m_max * getUnitFactor( traff_format, m_max );
	sprintf( fText, "Max: %.2f %s/s\n", value, getUnitString( traff_format, m_max ) );
	window.print( fText );
	//print total traffic since last system reboot
	window.setX( x );
	value = m_total * getUnitFactor( data_format, m_total );
	sprintf( fText, "Ttl: %.2f %s\n", value,  getUnitString( data_format, m_total ) );
	window.print( fText );
}

//reset all displayed values to zero
void Status::resetTrafficData()
{
	m_cur = m_min = m_max = m_total = 0;
	m_average_values.clear();
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
void Status::setAverageSmoothness( OptionInt* new_averagesmoothness )
{
	m_averagesmoothness = new_averagesmoothness;
}

//put new value into average calculation
void Status::updateAverage( int new_value )
{
	
	/*
	 * average calculation is not very good at the moment as it is dependent
	 * from the display refresh interval.
	 * could need some help here.
	 */
	
	m_average_values.push_front( new_value );
	
	//limit value count dependent of the average smoothness
	//ranges between 1 * 45 and 9 * 45 single values
	while( m_average_values.size() > (unsigned int) *m_averagesmoothness * 45 )
	{
		m_average_values.pop_back();
	}
	
}

//calculate current average
int Status::calcAverage()
{
	if( m_average_values.size() == 0 ) return 0;

	int sum = 0;
	for( list<int>::const_iterator i = m_average_values.begin(); i != m_average_values.end(); i++ )
	{
		sum += (*i);
	}
	
	return sum / m_average_values.size();
}

int Status::averageSmoothness()
{
	int avg_smooth =  m_averagesmoothness ? (int) *m_averagesmoothness : STANDARD_AVERAGE_SMOOTHNESS;
	return avg_smooth > 9 ? 9 : ( avg_smooth < 1 ? 1 : avg_smooth );
}
