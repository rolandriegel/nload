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

void Graph::setNumOfBars( int new_numofbars )
{
	
	if( new_numofbars < 0 )
		return;
	else if( new_numofbars == 0 )
	{
		m_numofbars = max_numofbars = new_numofbars;
		free( m_values );
		return;
	}
	else if( new_numofbars <= max_numofbars )
	{
		m_numofbars = new_numofbars;
		return;
	}
	else if( new_numofbars > max_numofbars )
	{
		if( max_numofbars != 0 )
			m_values = (int *) realloc( m_values, new_numofbars * sizeof( int ) );
		else
			m_values = (int *) malloc( new_numofbars * sizeof( int ) );
		
		if ( m_values == NULL )
		{
			fprintf( stderr, "Running out of memory.\n\n" );
			exit( EXIT_FAILURE );
		}
		
		for( int i = m_numofbars; i < new_numofbars; i++ )
			m_values[i] = 0;
		
		m_numofbars = max_numofbars = new_numofbars;
	}
}

void Graph::setHeightOfBars( int new_heightofbars )
{
	m_heightofbars = new_heightofbars;
}

void Graph::setTrafficWithMaxDeflectionOfBars( int new_trafficwithmaxdeflectionofbars )
{
	m_trafficwithmaxdeflectionofbars = new_trafficwithmaxdeflectionofbars;
}

void Graph::update(int new_value)
{

	if( m_numofbars <= 0 ) return;
	
	for( int i = m_numofbars - 2 - 1; i >= 0; i--)
		m_values[ i + 1 ] = m_values[i];
	
	m_values[0] = new_value;
	
}

void Graph::setWindow(WINDOW *new_window)
{
	m_window = new_window;
}

void Graph::print( int x, int y )
{
	
	int curx, cury;
	
	move( y, x );
	
	for( int l = 0; l < m_heightofbars; l++ )
	{
		for( int r = 0; r < m_numofbars; r++ )
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

