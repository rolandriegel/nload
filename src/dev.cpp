/***************************************************************************
                          dev.cpp  -  description
                             -------------------
    begin                : Wed Aug 1 2001
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

#include "dev.h"

Dev::Dev() : Proc::Proc()
{
	
	for( int i = 0; i < 2; i++ )
	{
		
		device_status[i] = new Status();
		traffic_graph[i] = new Graph();
		
		switch(i)
		{
			case 0:
				traffic_graph[i] -> setTrafficWithMaxDeflectionOfBars( STANDARD_BAR_MAX_IN );
				break;
			case 1:
				traffic_graph[i] -> setTrafficWithMaxDeflectionOfBars( STANDARD_BAR_MAX_OUT );
				break;
		}
		
		traffic_graph[i] -> setWindow( NULL );
		device_status[i] -> setWindow( NULL );
		device_status[i] -> setAverageSmoothness( STANDARD_AVERAGE_SMOOTHNESS );
		
	}
	
	setWindow( NULL );
	
	setShowGraphs( STANDARD_SHOW_GRAPHS );
	
	setProcDev( STANDARD_NETWORK_DEVICE );
	
	setDeviceNumber(0);
	setTotalNumberOfDevices(0);
	
}

Dev::~Dev()
{
}

//update and eventually print the device's data
void Dev::update( bool print )
{

	char fText[100] = "";
	int x = 0, y = 0, curx = 0, cury = 0;
	
	//read current traffic
	float *currentio = readLoad();
	getmaxyx( m_window, y, x );
	
	//if device does not exist
	if ( ! ProcDevExists() )
	{
		if( print )
		{
			//... print warning message ...
			sprintf( fText, "Device %s (%i/%i): does not exist\n", ProcDev(), m_devicenumber, m_totalnumberofdevices );
			addstr( fText );
			for( int i = 0; i < x; i++ )
				addch( '=' );
			addch( '\n' );
		}
		//... and exit
		return;
	}
	
	//calculate the traffic (Bytes/s)
	currentio[0] = currentio[0] / ( getElapsedTime() / 1000.0F );
	currentio[1] = currentio[1] / ( getElapsedTime() / 1000.0F );
	
	//update graphs and statistics
	for( int i = 0; i < 2; i++ )
	{
		device_status[i] -> update( (int) currentio[i], (unsigned int) (i == 0 ? totalIn() : totalOut()) );
		traffic_graph[i] -> update( (int) currentio[i] );
	}
	
	//if this device is currently not visible on the screen, exit here
	if( ! print ) return;
	
	//print header
	sprintf( fText, "Device %s (%i/%i):\n", ProcDev(), m_devicenumber, m_totalnumberofdevices );
	addstr( fText );
	for( int i = 0; i < x; i++ )
		addch( '=' );
	
	//if graphs should be shown ...
	if( m_showgraphs )
	{
		//incoming traffic
		addstr( "Incoming:\n" );
		
		getyx( m_window, cury, curx );
		
		traffic_graph[0] -> setNumOfBars( x * 2 / 3 );
		traffic_graph[0] -> setHeightOfBars( ( y - cury - 1 ) / 2 );
		traffic_graph[0] -> print( 0, cury );
		
		getyx( m_window, cury, curx );
		device_status[0] -> print( x * 2 / 3 + 2, cury - 5 );
		
		//outgoing traffic
		addstr( "Outgoing:\n" );
		
		getyx( m_window, cury, curx );
		traffic_graph[1] -> setNumOfBars( x * 2 / 3 );
		traffic_graph[1] -> setHeightOfBars( y - cury );
		traffic_graph[1] -> print( 0, cury );
		
		getyx( m_window, cury, curx );
		device_status[1] -> print( x * 2 / 3 + 2, cury - 4 );
	}
	//... or not
	else
	{
		addstr( "Incoming:" );
		getyx( m_window, cury, curx );
		move( cury, x / 2 );
		addstr( "Outgoing:\n" );
		
		getyx( m_window, cury, curx );
		
		device_status[0] -> print( 0, cury ); //incoming traffic
		device_status[1] -> print( x / 2, cury ); //outgoing traffic
		
		addch( '\n' );
	}
	
}

//set the curses window where to print to
void Dev::setWindow( WINDOW *new_window )
{
	m_window = new_window;
	for( int i = 0; i < 2; i++ )
	{
		traffic_graph[i] -> setWindow( new_window );
		device_status[i] -> setWindow( new_window );
	}
}

//sets if the graphs should be shown or not
void Dev::setShowGraphs( bool new_showgraphs )
{
	m_showgraphs = new_showgraphs;
}

//set the in- and outcoming graphs' averagesmoothness
void Dev::setAverageSmoothness( int new_averagesmoothness )
{
	for( int i = 0; i < 2; i++ )
		device_status[i] -> setAverageSmoothness( new_averagesmoothness );
}

//set the graphs' max deflection (max traffic level)
void Dev::setTrafficWithMaxDeflectionOfGraphs( int new_trafficinwithmaxdeflectionofgraphs, int new_trafficoutwithmaxdeflectionofgraphs  )
{
	
	for( int i = 0; i < 2; i++ )
		switch(i)
		{
			case 0:
				traffic_graph[i] -> setTrafficWithMaxDeflectionOfBars( new_trafficinwithmaxdeflectionofgraphs );
				break;
			case 1:
				traffic_graph[i] -> setTrafficWithMaxDeflectionOfBars( new_trafficoutwithmaxdeflectionofgraphs );
				break;
		}

}

//set the number identifying the device (for display only)
void Dev::setDeviceNumber( int new_devicenumber )
{
	m_devicenumber = new_devicenumber;
}

//set the total number of shown devices (for display only)
void Dev::setTotalNumberOfDevices( int new_totalnumberofdevices )
{
	m_totalnumberofdevices = new_totalnumberofdevices;
}

