/***************************************************************************
                                   main.cpp
                             -------------------
    begin                : Wed Jul 25 2001
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
/*
 * nload
 * real time monitor for network traffic
 * Copyright (C) 2001 Roland Riegel <support@roland-riegel.de>
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "main.h"

int main (int argc, char *argv[])
{

int sleep_interval = STANDARD_SLEEP_INTERVAL;
bool show_graphs = STANDARD_SHOW_GRAPHS;
long bar_max_in = STANDARD_BAR_MAX_IN;
long bar_max_out = STANDARD_BAR_MAX_OUT;
int average_smoothness = STANDARD_AVERAGE_SMOOTHNESS;
bool print_only_once = false;

struct timespec wanted_time;
int index = 0;
int cur_dev = 0;
int key = 0;
int x, y, curx, cury;

network_device = (char **) malloc( sizeof( char * ) );
if ( network_device == NULL )
{
    fprintf( stderr, "Running out of memory.\n\n" );
    exit( EXIT_FAILURE );
}

//parse the command line
for ( int i = 1; i < argc; i++ )
{
  //wants the user help?
	if ( strcmp( argv[i], "-h" ) == 0 || strcmp( argv[i], "--help" ) == 0 )
	{
		printhelp( argv[0] );
		exit(0);
	}
	//has the user set a non-default 100% mark for
	//the incoming bandwidth bar?
	else if ( strcmp( argv[i], "-i" ) == 0 )
	{
		
		if ( i < argc - 1 && isdigit( argv[ i + 1 ][0] ) != 0 )
		{
			bar_max_in = atol( argv[ i + 1 ] );
			if ( bar_max_in == 0 ) bar_max_in = STANDARD_BAR_MAX_IN;
			i++;
		}
		else
		{
			fprintf( stderr, "Wrong argument for the -i parameter.\n\n" );
			printhelp( argv[0] );
			exit(1);
		}
		
	}
	//has the user set a non-default 100% mark for
	//the outgoing bandwidth bar?
	else if ( strcmp( argv[i], "-o" ) == 0)
	{
		
		if ( i < argc - 1 && isdigit( argv[ i + 1 ][0] ) != 0 )
		{
			bar_max_out = atol( argv[ i + 1 ] );
			if ( bar_max_out == 0 ) bar_max_out = STANDARD_BAR_MAX_OUT;
			i++;
		}
		else
		{
			fprintf( stderr, "Wrong argument for the -o parameter.\n\n" );
			printhelp( argv[0] );
			exit(1);
		}
		
	}
	//has the user set a non-default refresh interval?
	else if ( strcmp( argv[i], "-t" ) == 0 )
	{
		if ( i < argc - 1 && isdigit( argv[ i + 1 ][0] ) != 0 )
		{
			sleep_interval = atoi( argv[ i + 1 ] );
			i++;
			if ( sleep_interval == 0 )
			{
				print_only_once = true;
				sleep_interval = STANDARD_SLEEP_INTERVAL;
			}
		}
		else
		{
			fprintf( stderr, "Wrong argument for the -t parameter.\n\n" );
			printhelp( argv[0] );
			exit(1);
		}
	}
	//has the user set a non-default average smoothness?
	else if ( strcmp( argv[i], "-s" ) == 0 )
	{
		if ( i < argc - 1 && isdigit( argv[ i + 1 ][0] ) != 0 )
		{
			average_smoothness = atoi( argv[ i + 1 ] );
			if ( average_smoothness < 1 || average_smoothness > 9 ) average_smoothness = STANDARD_AVERAGE_SMOOTHNESS;
			i++;
		}
		else
		{
			fprintf( stderr, "Wrong argument for the -s parameter.\n\n" );
			printhelp( argv[0] );
			exit(1);
		}
	}
	//has the user chosen to display multiple devices and thus not to desplay graphs?
	else if ( strcmp( argv[i], "-m" ) == 0 )
	{
		show_graphs = false;
	}
	//obsolete -b option
	else if ( strcmp( argv[i], "-b" ) == 0 ) {}
	//assume unknown parameter to be the network device
	else
	{
		network_device = (char **) realloc( network_device, ( index + 1 ) * sizeof( char * ) );
		if ( network_device == NULL )
		{
			fprintf( stderr, "Running out of memory.\n\n" );
			exit( EXIT_FAILURE );
		}
	  network_device[ index++ ] = argv[i];
	}

}

if ( index == 0 )
	network_device[ index++ ] = STANDARD_NETWORK_DEVICE;

devs = (Dev **) malloc( index * sizeof( Dev * ) );
if ( devs == NULL )
{
	fprintf( stderr, "Running out of memory.\n\n" );
	exit( EXIT_FAILURE );
}

//handle interrrupt signal
signal( SIGINT, finish );
signal( SIGTERM, finish );

//initialize ncurses
WINDOW *window = initscr();
keypad( stdscr, true );
nodelay( stdscr, true );
noecho();

//create one instance of the Dev class per device
for ( int i = 0; i < index; i++ )
{
	devs[i] = new Dev();
	devs[i] -> setProcDev( network_device[i] );
	devs[i] -> setShowGraphs( show_graphs );
	devs[i] -> setTrafficWithMaxDeflectionOfGraphs( bar_max_in * 1024 / 8, bar_max_out * 1024 / 8 );
	devs[i] -> setAverageSmoothness( average_smoothness );
	devs[i] -> setWindow( window );
	devs[i] -> setDeviceNumber( i + 1 );
	devs[i] -> setTotalNumberOfDevices( index );
}

do
{

	//wait sleep_interval milliseconds
	wanted_time.tv_sec = sleep_interval / 1000;
	wanted_time.tv_nsec = sleep_interval % 1000 * (long) 1000000;
	nanosleep( &wanted_time, NULL );
	
	getmaxyx( window, y, x );
	
	//process keyboard
	while( ( key = getch() ) != ERR )
		switch( key )
		{
			case KEY_RIGHT:
				cur_dev += show_graphs ? 1 : ( y / 9 >= index ? 0 : y / 9 );
				if( cur_dev >= index )
					cur_dev = 0;
				break;
			case KEY_LEFT:
				cur_dev -= show_graphs ? 1 : ( y / 9 >= index ? 0 : y / 9 );
				if( cur_dev < 0 )
					cur_dev = index - 1;
				break;
			case 'q':
			case 'Q':
				finish (0);
				break;
		}
	
	if( ! show_graphs && y / 9 >= index )
		cur_dev = 0;
	
	//clear the screen
	clear();
	move( 0, 0 ); //this shouldn't be necessary at all
	
	//update all devices and print the data of the current one
	for( int i = 0; i < index; i++ )
	{
		if( show_graphs )
		{
			devs[i] -> update( i == cur_dev );
		}
		else
		{
			getyx( window, cury, curx );
			
			devs[i] -> update( i >= cur_dev && y - cury >= 9 );
		}
	}
	
	//refresh the screen
	refresh();
	
} while ( print_only_once != true ); //do this endless except the user said "-t 0"

finish(0);

}

void finish( int signal )
{

//stop ncurses
endwin();

free( network_device );
free( devs );

exit( EXIT_SUCCESS );

}

void printhelp( char *appname )
{

//print disclaimer
fprintf( stderr,
	"\nnload version %s, Copyright (C) 2001 Roland Riegel <support@roland-riegel.de>\n"
	"nload comes with ABSOLUTELY NO WARRANTY. This is free software, and you are\n"
	"welcome to redistribute it under certain conditions. For more details see the\n"
	"GNU General Public License Version 2 (http://www.gnu.org/copyleft/gpl.html).\n\n"

	"Command line syntax:\n"
	"%s [-i max_scaling] [-m] [-o max_scaling] [-s smoothness] [-t interval] [devices]\n"
	"%s --help|-h\n\n"
	
	"-i max_scaling	specifies the 100%% mark in kBit/s of the graph indicating the\n"
	"		incoming bandwidth usage\n"
	"		ignored if max_scaling is 0 or the switch -m is given\n"
	"		default is %i\n"
	"-m		show multiple devices at a time; do not show the traffic graphs\n"
	"-o max_scaling	same as -i but for the graph indicating the outgoing bandwidth\n"
	"		usage\n"
	"		default is %i\n"
	"-s smoothness	sets the \"smoothness\" of the average in/out values\n"
	"		value of 1 means little smoothness (average over a short period of time)\n"
	"		value of 9 means high smoothness (average over a long period of time)\n"
	"		default is %i\n"
	"-t intervall	determines the refresh interval of the display in milliseconds\n"
	"		if 0 print net load only one time and exit\n"
	"		default is %i\n"
	"devices		network devices to use\n"
	"		default is \"%s\"\n"
	"--help\n"
	"-h		print this help\n"
	"-b		obsolete - ignored\n\n"
	"example: %s -t 200 -s 7 -i 1024 -o 128 eth0 eth1\n\n",
	
	NLOAD_VERSION,
	appname,
	appname,
	STANDARD_BAR_MAX_IN,
	STANDARD_BAR_MAX_OUT,
	STANDARD_AVERAGE_SMOOTHNESS,
	STANDARD_SLEEP_INTERVAL,
	STANDARD_NETWORK_DEVICE,
	appname
	);

}
