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

setBarMaxIn( STANDARD_BAR_MAX_IN );
setBarMaxOut( STANDARD_BAR_MAX_OUT );
setAverageSmoothness( STANDARD_AVERAGE_SMOOTHNESS );
setWindow( NULL );
setProcDev( STANDARD_NETWORK_DEVICE );

device_number = total_number_of_devices = 0;

cur_num_values = 0;
min_in = max_in = avg_in = -1.0;
min_out = max_out = avg_out = -1.0;

}

Dev::Dev(char *network_device, long max_b_in, long max_b_out, int avg_smoothness, WINDOW *win, int dev_number, int total_number_of_devs) : Proc::Proc()
{

setBarMaxIn( max_b_in );
setBarMaxOut( max_b_out );
setAverageSmoothness( avg_smoothness );
setWindow( win );
setProcDev( network_device );

device_number = dev_number;
total_number_of_devices = total_number_of_devs;

cur_num_values = 0;
min_in = max_in = avg_in = -1.0;
min_out = max_out = avg_out = -1.0;

}

Dev::~Dev()
{

setNumOfGraphValues(0);

}

void Dev::update(bool print)
{

char fText[100] = "";
int x = 0, y = 0, curx = 0, cury = 0;

float *currentio = readLoad();
getmaxyx( window, y, x );

if ( currentio[0] == -1.0 && currentio[1] == -1.0 )
{
	if( print )
	{
		sprintf( fText, "Device %s (%i/%i): does not exist\n", ProcDev(), device_number, total_number_of_devices );
		addstr( fText );
		for( int i = 0; i < x / 2; i++ )
			addch( '=' );
		addch( '\n' );
	}
	return;
}

currentio[0] = currentio[0] * 8 / 1024 / ( getElapsedTime() / (float) 1000 );
currentio[1] = currentio[1] * 8 / 1024 / ( getElapsedTime() / (float) 1000 );

setNumOfGraphValues(1);
for( int i = cur_num_values - 2 - 1; i >= 0; i--)
{
	traffic[0][ i + 1 ] = traffic[0][i];
	traffic[1][ i + 1 ] = traffic[1][i];
}
traffic[0][0] = currentio[0];
traffic[1][0] = currentio[1];

MinMax( min_in, max_in, currentio[0] );
average( avg_in, currentio[0] );
MinMax( min_out, max_out, currentio[1] );
average( avg_out, currentio[1] );

if( ! print ) return;

sprintf( fText, "Device %s (%i/%i):\n", ProcDev(), device_number, total_number_of_devices );
addstr( fText );
for( int i = 0; i < x / 2; i++ )
	addch( '=' );
addch( '\n' );

//incoming traffic

addstr( "\nIncoming:\n" );

//print graph
getyx( window, cury, curx );
printGraph( false, ( y - cury - 2 ) / 2, bar_max_in );
//print current traffic
getyx( window, cury, curx );
move( cury - 5, x * 2 / 3 + 2);
sprintf( fText, "Curr: %.2f kBit/s\n", currentio[0] );
addstr( fText );
//print average traffic since nload start
getyx( window, cury, curx );
move( cury, x * 2 / 3 + 2 );
sprintf( fText, "Avg: %.2f kBit/s\n", avg_in );
addstr( fText );
//print min traffic since nload start
getyx( window, cury, curx );
move( cury, x * 2 / 3 + 2 );
sprintf( fText, "Min: %.2f kBit/s\n", min_in );
addstr( fText );
//print max traffic since nload start
getyx( window, cury, curx );
move( cury, x * 2 / 3 + 2 );
sprintf( fText, "Max: %.2f kBit/s\n", max_in );
addstr( fText );
//print total traffic since last system reboot
getyx( window, cury, curx );
move( cury, x * 2 / 3 + 2 );
sprintf( fText, "Ttl: %.2f MB\n", totalIn() / ( 1024 * 1024 ) );
addstr( fText );

//outgoing traffic

addstr( "\nOutgoing:\n" );

//print graph
getyx( window, cury, curx );
printGraph( true, y - cury, bar_max_out );
//print current traffic
getyx( window, cury, curx );
move( cury - 4, x * 2 / 3 + 2 );
sprintf( fText, "Curr: %.2f kBit/s\n", currentio[1] );
addstr( fText );
//print average traffic since nload start
getyx( window, cury, curx );
move( cury, x * 2 / 3 + 2 );
sprintf( fText, "Avg: %.2f kbit/s\n", avg_out );
addstr( fText );
//print min traffic since nload start
getyx( window, cury, curx );
move( cury, x * 2 / 3 + 2 );
sprintf( fText, "Min: %.2f kBit/s\n", min_out );
addstr( fText );
//print max traffic since nload start
getyx( window, cury, curx );
move( cury, x * 2 / 3 + 2 );
sprintf( fText, "Max: %.2f kBit/s\n", max_out );
addstr( fText );
//print total traffic since last system reboot
getyx( window, cury, curx );
move( cury, x * 2 / 3 + 2 );
sprintf( fText, "Ttl: %.2f MB\n", totalOut() / ( 1024 * 1024 ) );
addstr( fText );

return;

}

void Dev::setBarMaxIn(long max_in)
{
if (max_in > 0)
	bar_max_in = max_in;
}

void Dev::setBarMaxOut(long max_out)
{
if (max_out > 0)
	bar_max_out = max_out;
}

void Dev::setNumOfGraphValues( int num_values )
{

if( num_values == 0 )
{
	free( traffic[0] ); free( traffic[1] );
	cur_num_values = 0;
}

if( num_values <= 0 || num_values <= cur_num_values ) return;

if( cur_num_values != 0 )
{
	traffic[0] = (float *) realloc( traffic[0], num_values * sizeof( float ) );
	traffic[1] = (float *) realloc( traffic[1], num_values * sizeof( float ) );
}
else
{
	traffic[0] = (float *) malloc( num_values * sizeof( float ) );
	traffic[1] = (float *) malloc( num_values * sizeof( float ) );
}

if ( traffic[0] == NULL || traffic[1] == NULL )
{
	fprintf( stderr, "Running out of memory.\n\n" );
	exit( EXIT_FAILURE );
}

for( int i = cur_num_values; i < num_values; i++ )
{
	traffic[0][i] = 0;
	traffic[1][i] = 0;
}

cur_num_values = num_values;

}

void Dev::setWindow(WINDOW *win)
{
window = win;
}

void Dev::printGraph( bool outgoing_instead_of_incoming, int height, int max_graph_value)
{
int width;
int dummy;

getmaxyx( window, dummy, width );
width = width * 2 / 3;
setNumOfGraphValues( width );

for( int l = 0; l < height; l++ )
{
	for( int r = 0; r < width; r++ )
	{
		if( traffic[ outgoing_instead_of_incoming ? 1 : 0 ][ width - r - 1 ] / max_graph_value >= 1.0 - ( (float) l / height ) )
			addch( '#' );
		else
			addch( ' ' );
	}
	addch( '\n' );
}
}

void Dev::MinMax(float &curmin, float &curmax, float newval)
{

if (curmin==-1.0 && curmax==-1.0)
{
	curmin = newval;
	curmax = newval;
	return;
}

curmin = newval < curmin ? newval : curmin;

curmax = newval > curmax ? newval : curmax;

}

void Dev::setAverageSmoothness(int avg_smoothness)
{
if ((avg_smoothness >= 1) && (avg_smoothness <= 9))
	average_smoothness = avg_smoothness;
}

void Dev::average(float &curavg, float new_value)
{

/*
 * average calculation is not very good at the moment as it is not a real average
 * and is dependent from the display refresh interval.
 * it is a running estimate updated by forming a weighted average.
 * could need some help here.
 */

if ( curavg==-1.0 )
{
	curavg = new_value;
	return;
}

curavg = new_value * ( 1.0 - average_smoothness / 10.0 ) + curavg * ( average_smoothness / 10.0 );

}

void Dev::setDeviceNumber(int number)
{
if( number > 0 )
	device_number = number;
}

void Dev::setTotalNumberOfDevices(int number)
{
if( number > 0 )
	total_number_of_devices = number;
}
