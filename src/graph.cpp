/***************************************************************************
                          graph.cpp  -  description
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
void Graph::setTrafficWithMaxDeflectionOfBars( int new_trafficwithmaxdeflectionofbars )
{
	//[new_trafficwithmaxdeflectionofbars] = Bytes/s
	
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

//set the curses window where to write to
void Graph::setWindow(WINDOW *new_window)
{
	m_window = new_window;
}

//print the graph with the upper left corner at the coordinates (x, y)
void Graph::print( int x, int y )
{
	int curx, cury;
	
	move( y, x );
	
	//cycle through through the lines
	for( int l = 0; l < m_heightofbars; l++ )
	{
		//for each line cycle through the rows
		for( list<int>::reverse_iterator r = m_values.rbegin(); r != m_values.rend() ; r++ )
		{
			if( (*r) / (float) m_trafficwithmaxdeflectionofbars >= 1.0 - ( l / (float) m_heightofbars ) )
				addch( '#' );
			else
				addch( ' ' );
		}
		addch( '\n' );
		getyx( m_window, cury, curx );
		move( cury, x );
	}
	
}

