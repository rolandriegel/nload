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

/*
Dev::Dev(char *network_device, long max_b_in, long max_b_out, int avg_smoothness, WINDOW *win, int dev_number, int total_number_of_devs) : Proc::Proc()
{
	
	for( int i = 0; i < 2; i++ )
	{
		
		device_status[i] = new Status();
		traffic_graph[i] = new Graph();
		
		switch(i)
		{
			case 0:
				traffic_graph[i]->setTrafficWithMaxDeflectionOfBars( max_b_in );
				break;
			case 1:
				traffic_graph[i]->setTrafficWithMaxDeflectionOfBars( max_b_out );
				break;
		}
		
		traffic_graph[i]->setWindow( win );
		device_status[i]->setWindow( win );
		device_status[i]->setAverageSmoothness( avg_smoothness );
		
	}
	
	setWindow( win );
	
	setProcDev( network_device );
	
	device_number = dev_number;
	total_number_of_devices = total_number_of_devs;
	
}
*/

Dev::~Dev()
{
}

void Dev::update( bool print )
{

	char fText[100] = "";
	int x = 0, y = 0, curx = 0, cury = 0;
	
	float *currentio = readLoad();
	getmaxyx( m_window, y, x );
	
	if ( currentio[0] == -1.0 && currentio[1] == -1.0 )
	{
		if( print )
		{
			sprintf( fText, "Device %s (%i/%i): does not exist\n", ProcDev(), m_devicenumber, m_totalnumberofdevices );
			addstr( fText );
			for( int i = 0; i < x; i++ )
				addch( '=' );
			addch( '\n' );
		}
		return;
	}
	
	currentio[0] = currentio[0] / ( getElapsedTime() / (float) 1000 );
	currentio[1] = currentio[1] / ( getElapsedTime() / (float) 1000 );
	
	for( int i = 0; i < 2; i++ )
	{
		device_status[i] -> update( currentio[i], i == 0 ? totalIn() : totalOut() );
		traffic_graph[i] -> update( currentio[i] );
	}
	
	if( ! print ) return;
	
	sprintf( fText, "Device %s (%i/%i):\n", ProcDev(), m_devicenumber, m_totalnumberofdevices );
	addstr( fText );
	for( int i = 0; i < x; i++ )
		addch( '=' );
	
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

void Dev::setWindow( WINDOW *new_window )
{
	m_window = new_window;
	for( int i = 0; i < 2; i++ )
	{
		traffic_graph[i] -> setWindow( new_window );
		device_status[i] -> setWindow( new_window );
	}
}

void Dev::setShowGraphs( bool new_showgraphs )
{
	m_showgraphs = new_showgraphs;
}

void Dev::setAverageSmoothness( int new_averagesmoothness )
{
	for( int i = 0; i < 2; i++ )
		device_status[i] -> setAverageSmoothness( new_averagesmoothness );
}

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

void Dev::setDeviceNumber( int new_devicenumber )
{
	m_devicenumber = new_devicenumber;
}

void Dev::setTotalNumberOfDevices( int new_totalnumberofdevices )
{
	m_totalnumberofdevices = new_totalnumberofdevices;
}

