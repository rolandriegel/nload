/***************************************************************************
                          graph.cpp  -  description
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

#include "graph.h"

Graph::Graph()
{
	m_heightofbars = 0;
	m_trafficwithmaxdeflectionofbars = 0;
}

Graph::~Graph()
{
}

//sets the number of the graph's vertical #-bars
void Graph::setNumOfBars( int new_numofbars )
{
	//logically wrong: number of #-bars has always to be positive
	if( new_numofbars <= 0 ) return;
	
	//vertically resize the graph's value list
	m_values.resize( new_numofbars );
}

//sets the height of the graph's vertical #-bars
void Graph::setHeightOfBars( int new_heightofbars )
{
	m_heightofbars = new_heightofbars;
}

//set the traffic at which the graph shows 100% deflection
void Graph::setTrafficWithMaxDeflectionOfBars( OptionLong* new_trafficwithmaxdeflectionofbars )
{
	//[new_trafficwithmaxdeflectionofbars] = kBit/s
	
	m_trafficwithmaxdeflectionofbars = new_trafficwithmaxdeflectionofbars;
}

//new traffic measurement has been made => update the graph's value list
void Graph::update(int new_value)
{
	//[new_value] = Bytes/s
	
	if( m_values.size() == 0 ) return;
	
	//put new value to the beginning of the list, it becomes the first #-bar
	m_values.push_front( new_value );
	//delete the last #-bar of the list
	m_values.pop_back();
	
}

//print the graph with the upper left corner at the coordinates (x, y)
void Graph::print( Window& window, int x, int y )
{
	window.setXY( x, y );
	
	//cycle through through the lines
	for( int l = 0; l < m_heightofbars; l++ )
	{
		//for each line cycle through the rows
		for( list<int>::reverse_iterator r = m_values.rbegin(); r != m_values.rend() ; r++ )
		{
			int trafficperline = trafficWithMaxDeflectionOfBars() / m_heightofbars;
			int restoftraffic = ( (*r) - ( m_heightofbars - l - 1 ) * trafficperline ) % trafficperline;
			if( (float) (*r) / trafficWithMaxDeflectionOfBars() >= (float) ( m_heightofbars - l ) / m_heightofbars )
				window.print( '#' );
			else if( restoftraffic >= 0.7 * trafficperline )
				window.print( '|' );
			else if( restoftraffic >= 0.3 * trafficperline )
				window.print( '.' );
			else
				window.print( ' ' );
		}
		window.print( '\n' );
		window.setX( x );
	}
	
}

//reset all traffic values in the graph to zero
void Graph::resetTrafficData()
{
	int size = m_values.size();
	m_values.clear();
	m_values.resize( size );
}

long Graph::trafficWithMaxDeflectionOfBars()
{
	int tr = m_trafficwithmaxdeflectionofbars ? (long) *m_trafficwithmaxdeflectionofbars : STANDARD_MAX_DEFLECTION;
	return tr * 1024 / 8; //recalculate from kBit/s to Bytes/s
}

