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
	
	m_numofbars = max_numofbars = 0;
	m_heightofbars = 0;
	m_trafficwithmaxdeflectionofbars = 0;
	
}

Graph::~Graph()
{
	setNumOfBars(0);
}

//sets the number of the graph's vertical #-bars
void Graph::setNumOfBars( int new_numofbars )
{
	//logically wrong: number of #-bars has always to be positive
	if( new_numofbars < 0 )
		return;
	//logically wrong, but used for freeing value array memory
	else if( new_numofbars == 0 )
	{
		m_numofbars = max_numofbars = new_numofbars;
		free( m_values );
		return;
	}
	/*
	 *set new number, but don't reserve more array elements, as there had already been more
	 *elements some time before
	 */
	else if( new_numofbars <= max_numofbars )
	{
		m_numofbars = new_numofbars;
		return;
	}
	//number has never been larger before => reserve more array elements
	else if( new_numofbars > max_numofbars )
	{
		//if there are already some bars, resize memory block
		if( max_numofbars != 0 )
			m_values = (int *) realloc( m_values, new_numofbars * sizeof( int ) );
		//if not, get new memory
		else
			m_values = (int *) malloc( new_numofbars * sizeof( int ) );
		
		//not enough memory left
		if ( m_values == NULL )
		{
			fprintf( stderr, "Running out of memory.\n\n" );
			exit( EXIT_FAILURE );
		}
		
		//initialize memory
		for( int i = m_numofbars; i < new_numofbars; i++ )
			m_values[i] = 0;
		
		m_numofbars = max_numofbars = new_numofbars;
	}
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

//new traffic measurement has been made => update the graph's value array
void Graph::update(int new_value)
{
	//[new_value] = Bytes/s
	
	if( m_numofbars <= 0 ) return;
	
	//put all values one position back (this deletes the last one)
	for( int i = m_numofbars - 2 - 1; i >= 0; i--)
		m_values[ i + 1 ] = m_values[i];
	
	//new traffic value becomes the first one in the graph
	m_values[0] = new_value;
	
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
	
	for( int l = 0; l < m_heightofbars; l++ ) //cycle through through the lines
	{
		for( int r = 0; r < m_numofbars; r++ ) //for each line cycle through the rows
		{
			if( m_values[ m_numofbars - r - 1 ] / (float) m_trafficwithmaxdeflectionofbars >= 1.0 - ( l / (float) m_heightofbars ) )
				addch( '#' );
			else
				addch( ' ' );
		}
		addch( '\n' );
		getyx( m_window, cury, curx );
		move( cury, x );
	}
	
}

