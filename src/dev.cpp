/***************************************************************************
                          dev.cpp  -  description
                             -------------------
    begin                : Wed Aug 1 2001
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
	
	setStatusFormat( STANDARD_TRAFFIC_FORMAT, STANDARD_DATA_FORMAT );
	
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
	
	//calculate the traffic (Bytes/s)
	currentio[0] = currentio[0] / ( getElapsedTime() / 1000 );
	currentio[1] = currentio[1] / ( getElapsedTime() / 1000 );
	
	//update graphs and statistics
	for( int i = 0; i < 2; i++ )
	{
		if( ProcDevExists() )
		{
			device_status[i] -> update( (int) currentio[i], (unsigned int) ( i == 0 ? totalIn() : totalOut() ) );
			traffic_graph[i] -> update( (int) currentio[i] );
		}
		else
		{
			device_status[i] -> resetTrafficData();
			traffic_graph[i] -> resetTrafficData();
		}
	}
	
	//if this device is currently not visible on the screen, exit here
	if( ! print ) return;
	
	//if device does not exist
	if ( ! ProcDevExists() )
	{
		//... print warning message ...
		sprintf( fText, "Device %s (%i/%i): does not exist\n", ProcDev(), m_devicenumber, m_totalnumberofdevices );
		waddstr( m_window, fText );
		for( int i = 0; i < x; i++ )
			waddch( m_window, '=' );
		waddch( m_window, '\n' );
		
		//... and exit
		return;
	}
	
	//print header
	sprintf( fText, "Device %s (%i/%i):\n", ProcDev(), m_devicenumber, m_totalnumberofdevices );
	waddstr( m_window, fText );
	for( int i = 0; i < x; i++ )
		waddch( m_window, '=' );
	
	//if graphs should be shown ...
	if( m_showgraphs )
	{
		//incoming traffic
		waddstr( m_window, "Incoming:\n" );
		
		getyx( m_window, cury, curx );
		
		traffic_graph[0] -> setNumOfBars( x * 2 / 3 );
		traffic_graph[0] -> setHeightOfBars( ( y - cury - 1 ) / 2 );
		traffic_graph[0] -> print( 0, cury );
		
		getyx( m_window, cury, curx );
		device_status[0] -> print( x * 2 / 3 + 2, cury - 5, m_trafficformat, m_dataformat );
		
		//outgoing traffic
		waddstr( m_window, "Outgoing:\n" );
		
		getyx( m_window, cury, curx );
		traffic_graph[1] -> setNumOfBars( x * 2 / 3 );
		traffic_graph[1] -> setHeightOfBars( y - cury );
		traffic_graph[1] -> print( 0, cury );
		
		getyx( m_window, cury, curx );
		device_status[1] -> print( x * 2 / 3 + 2, cury - 4, m_trafficformat, m_dataformat );
	}
	//... or not
	else
	{
		waddstr( m_window, "Incoming:" );
		getyx( m_window, cury, curx );
		wmove( m_window, cury, x / 2 );
		waddstr( m_window, "Outgoing:\n" );
		
		getyx( m_window, cury, curx );
		
		device_status[0] -> print( 0, cury, m_trafficformat, m_dataformat ); //incoming traffic
		device_status[1] -> print( x / 2, cury, m_trafficformat, m_dataformat ); //outgoing traffic
		
		waddch( m_window, '\n' );
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

//set the display format (unit) for traffic and data numbers
void Dev::setStatusFormat( Status::status_format new_trafficformat, Status::status_format new_dataformat )
{
	m_trafficformat = new_trafficformat;
	m_dataformat = new_dataformat;
}
