/***************************************************************************
                                  dev.cpp
                             -------------------
    begin                : Wed Aug 1 2001
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

#include "dev.h"

#include "graph.h"
#include "window.h"
#include "options.h"

Dev::Dev() : Proc::Proc()
{
	
	for( int i = 0; i < 2; i++ )
	{
		
		device_status[i] = new Status();
		traffic_graph[i] = new Graph();
		
	}
	
	setHideGraphs(0);
	
	setProcDev( STANDARD_NETWORK_DEVICE );
	
	setStatusFormat( 0, 0 );
	
	setDeviceNumber(0);
	setTotalNumberOfDevices(0);
	
}

Dev::~Dev()
{
}

//update the device's data
void Dev::update()
{
	//read current traffic
	float *currentio = readLoad();
	
	//calculate the traffic (Bytes/s)
	currentio[0] = currentio[0] / ( getElapsedTime() / 1000 );
	currentio[1] = currentio[1] / ( getElapsedTime() / 1000 );
	
	//update graphs and statistics
	for( int i = 0; i < 2; i++ )
	{
		if( procDevExists() )
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
}

//print the device's data
void Dev::print( Window& window )
{
	char fText[100] = "";
	
	//if device does not exist
	if ( ! procDevExists() )
	{
		//... print warning message ...
		sprintf( fText, "Device %s (%i/%i): does not exist\n", procDev(), m_devicenumber, m_totalnumberofdevices );
		window.print( fText );
		for( int i = 0; i < window.width(); i++ )
			window.print( '=' );
		window.print( '\n' );
		
		//... and exit
		return;
	}
	
	//print header
	if( strlen( ip() ) > 0 )
		sprintf( fText, "Device %s [%s] (%i/%i):\n", procDev(), ip(), m_devicenumber, m_totalnumberofdevices );
	else
		sprintf( fText, "Device %s (%i/%i):\n", procDev(), m_devicenumber, m_totalnumberofdevices );
	window.print( fText );
	for( int i = 0; i < window.width(); i++ )
		window.print( '=' );
	
	//if graphs should be hidden ...
	if( hideGraphs() )
	{
		window.print( "Incoming:" );
		window.setX( window.width() / 2 );
		window.print( "Outgoing:\n" );
		
		int status_y = window.y();
		
		device_status[0] -> print( window, 0, status_y, trafficFormat(), dataFormat() ); //incoming traffic
		device_status[1] -> print( window, window.width() / 2, status_y, trafficFormat(), dataFormat() ); //outgoing traffic
		
		window.print( '\n' );
	}
	//... or not
	else
	{
		//incoming traffic
		window.print( "Incoming:\n" );
		
		traffic_graph[0] -> setNumOfBars( window.width() * 2 / 3 );
		traffic_graph[0] -> setHeightOfBars( ( window.height() - window.y() - 1 ) / 2 );
		traffic_graph[0] -> print( window, 0, window.y() );
		
		device_status[0] -> print( window, window.width() * 2 / 3 + 2, window.y() - 5, trafficFormat(), dataFormat() );
		
		//outgoing traffic
		window.print( "Outgoing:\n" );
		
		traffic_graph[1] -> setNumOfBars( window.width() * 2 / 3 );
		traffic_graph[1] -> setHeightOfBars( window.height() - window.y() );
		traffic_graph[1] -> print( window, 0, window.y() );
		
		device_status[1] -> print( window, window.width() * 2 / 3 + 2, window.y() - 4, trafficFormat(), dataFormat() );
	}
}

//sets if the graphs should be shown or not
void Dev::setHideGraphs( OptionBool* new_hidegraphs )
{
	m_hidegraphs = new_hidegraphs;
}

//set the in- and outcoming graphs' averagesmoothness
void Dev::setAverageSmoothness( OptionInt* new_averagesmoothness )
{
	for( int i = 0; i < 2; i++ )
		device_status[i] -> setAverageSmoothness( new_averagesmoothness );
}

//set the graphs' max deflection (max traffic level)
void Dev::setTrafficWithMaxDeflectionOfGraphs( OptionLong* new_trafficinwithmaxdeflectionofgraphs, OptionLong* new_trafficoutwithmaxdeflectionofgraphs  )
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
void Dev::setStatusFormat( OptionStatusFormat* new_trafficformat, OptionStatusFormat* new_dataformat )
{
	m_trafficformat = new_trafficformat;
	m_dataformat = new_dataformat;
}

bool Dev::hideGraphs()
{
	return m_hidegraphs ? (bool) *m_hidegraphs : STANDARD_HIDE_GRAPHS;
}

Status::status_format Dev::trafficFormat()
{
	return m_trafficformat ? (Status::status_format) *m_trafficformat : STANDARD_TRAFFIC_FORMAT;
}

Status::status_format Dev::dataFormat()
{
	return m_dataformat ? (Status::status_format) *m_dataformat : STANDARD_DATA_FORMAT;
}

