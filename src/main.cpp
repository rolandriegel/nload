/***************************************************************************
                                   main.cpp
                             -------------------
    begin                : Wed Jul 25 2001
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
/*
 * nload
 * real time monitor for network traffic
 * Copyright (C) 2001, 2002 Roland Riegel <support@roland-riegel.de>
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

vector<string *> network_device;

OptionInt sleep_interval( STANDARD_SLEEP_INTERVAL, "Refresh interval (ms)" );
OptionBool show_multiple_devices( ! STANDARD_SHOW_GRAPHS, "Show multiple devices" );
OptionLong bar_max_in( STANDARD_BAR_MAX_IN, "Max Incoming deflection (kBit/s)" );
OptionLong bar_max_out( STANDARD_BAR_MAX_OUT, "Max Outgoing deflection (kBit/s)" );
OptionInt average_smoothness( STANDARD_AVERAGE_SMOOTHNESS, "Smoothness of average" );
average_smoothness.min(1);
average_smoothness.max(9);
OptionStatusFormat traffic_format( STANDARD_TRAFFIC_FORMAT, "Unit for traffic numbers" );
OptionStatusFormat data_format( STANDARD_DATA_FORMAT, "Unit for data numbers" );
m_optwindow.options().push_back( &sleep_interval );
m_optwindow.options().push_back( &show_multiple_devices );
m_optwindow.options().push_back( &bar_max_in );
m_optwindow.options().push_back( &bar_max_out );
m_optwindow.options().push_back( &average_smoothness );
m_optwindow.options().push_back( &traffic_format );
m_optwindow.options().push_back( &data_format );

bool print_only_once = false;

//parse the command line
for ( int i = 1; i < argc; i++ )
{
	//wants the user help?
	if ( strcmp( argv[i], "-h" ) == 0 || strcmp( argv[i], "--help" ) == 0 )
	{
		printhelp();
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
			printhelp();
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
			printhelp();
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
			printhelp();
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
			printhelp();
			exit(1);
		}
	}
	//has the user set a non-default unit for traffic numbers?
	else if ( strcmp( argv[i], "-u" ) == 0 )
	{
		if ( i < argc - 1 && isalpha( argv[ i + 1 ][0] ) != 0 )
		{
			switch( argv[ i + 1 ][0] )
			{
				case 'H':
					traffic_format = Status::human_readable_byte;
					break;
				case 'h':
					traffic_format = Status::human_readable_bit;
					break;
				case 'B':
					traffic_format = Status::byte;
					break;
				case 'b':
					traffic_format = Status::bit;
					break;
				case 'K':
					traffic_format = Status::kilobyte;
					break;
				case 'k':
					traffic_format = Status::kilobit;
					break;
				case 'M':
					traffic_format = Status::megabyte;
					break;
				case 'm':
					traffic_format = Status::megabit;
					break;
				case 'G':
					traffic_format = Status::gigabyte;
					break;
				case 'g':
					traffic_format = Status::gigabit;
					break;
				default:
					fprintf( stderr, "Wrong argument for the -u parameter.\n\n" );
					printhelp();
					exit(1);
			}
			i++;
		}
		else
		{
			fprintf( stderr, "Wrong argument for the -u parameter.\n\n" );
			printhelp();
			exit(1);
		}
	}
	//has the user set a non-default unit for numbers of amount of data?
	else if( strcmp( argv[i], "-U" ) == 0 )
	{
		if ( i < argc - 1 && isalpha( argv[ i + 1 ][0] ) != 0 )
		{
			switch( argv[ i + 1 ][0] )
			{
				case 'H':
					data_format = Status::human_readable_byte;
					break;
				case 'h':
					data_format = Status::human_readable_bit;
					break;
				case 'B':
					data_format = Status::byte;
					break;
				case 'b':
					data_format = Status::bit;
					break;
				case 'K':
					data_format = Status::kilobyte;
					break;
				case 'k':
					data_format = Status::kilobit;
					break;
				case 'M':
					data_format = Status::megabyte;
					break;
				case 'm':
					data_format = Status::megabit;
					break;
				case 'G':
					data_format = Status::gigabyte;
					break;
				case 'g':
					data_format = Status::gigabit;
					break;
				default:
					fprintf( stderr, "Wrong argument for the -U parameter.\n\n" );
					printhelp();
					exit(1);
			}
			i++;
		}
		else
		{
			fprintf( stderr, "Wrong argument for the -U parameter.\n\n" );
			printhelp();
			exit(1);
		}
	
	}
	//has the user chosen to display multiple devices and thus not to display graphs?
	else if ( strcmp( argv[i], "-m" ) == 0 )
	{
		show_multiple_devices = true;
	}
	//obsolete -b option
	else if ( strcmp( argv[i], "-b" ) == 0 ) {}
	//assume unknown parameter to be the network device
	else
	{
		network_device.push_back( new string( argv[i] ) );
	}

}

if ( network_device.size() == 0 )
	network_device.push_back( new string( STANDARD_NETWORK_DEVICE ) );

init();

//create one instance of the Dev class per device
for ( vector<string *>::size_type i = 0; i < network_device.size(); i++ )
{
	m_mainwindow.devices().push_back( new Dev() );
	m_mainwindow.devices().back() -> setProcDev( network_device[i] -> c_str() );
	m_mainwindow.devices().back() -> setDeviceNumber( i + 1 );
	m_mainwindow.devices().back() -> setTotalNumberOfDevices( network_device.size() );
}

do
{
	
	//wait sleep_interval milliseconds (in steps of 100 ms)
	struct timespec wanted_time;
	wanted_time.tv_sec = 0;
	
	int rest_of_sleep_interval = sleep_interval;
	
	while( rest_of_sleep_interval > 0 )
	{
		rest_of_sleep_interval -= 100;
		wanted_time.tv_nsec = ( rest_of_sleep_interval >= 0 ? 100 : 100 + rest_of_sleep_interval ) * 1000000L;
		
		nanosleep( &wanted_time, 0 );
		
		//process keyboard
		int key;
		while( ( key = getch() ) != ERR )
		{
			switch( key )
			{
				case 'o':
				case 'O':
					if( m_optwindow.visible() )
					{
						m_optwindow.hide();
						m_mainwindow.resize( 0, 0, Screen::width(), Screen::height() );
					}
					else
					{
						m_mainwindow.resize( 0, Screen::height() / 4, Screen::width(), Screen::height() - Screen::height() / 4 );
						m_optwindow.setFieldChangedFunc( optwindow_fieldChanged );
						m_optwindow.show( 0, 0, Screen::width(), Screen::height() / 4 );
					}
					rest_of_sleep_interval = 0; //update the screen
					break;
				case 'q':
				case 'Q':
					if( ! m_optwindow.visible() )
					{
						finish (0);
						return 0;
					}
					break;
			}
			if( m_optwindow.visible() )
				m_optwindow.processRequest( key );
			else
				m_mainwindow.processKey( key );
		}
	}
	
	//pipe through new settings (could be done in a cleaner way)
	m_mainwindow.setShowMultipleDevices( show_multiple_devices );
	for ( vector<Dev *>::const_iterator r = m_mainwindow.devices().begin(); r != m_mainwindow.devices().end(); r++ )
	{
		(*r) -> setTrafficWithMaxDeflectionOfGraphs( bar_max_in * 1024 / 8, bar_max_out * 1024 / 8 );
		(*r) -> setAverageSmoothness( average_smoothness );
		(*r) -> setStatusFormat( traffic_format, data_format );
	}
	
	//clear the screen
	m_mainwindow.clear();
	
	//print device data
	m_mainwindow.print();
	
	//refresh the screen
	m_mainwindow.refresh();
	
	if( m_optwindow.visible() ) m_optwindow.refresh(); //always show cursor in option dialog
	
} while ( print_only_once != true ); //do this endless except the user said "-t 0"

finish(0);

return 0;

}

void optwindow_fieldChanged( FORM * form )
{
	m_optwindow.fieldChanged( form );
}

void init()
{
	//handle interrrupt signal
	signal( SIGINT, finish );
	signal( SIGTERM, finish );
	signal( SIGWINCH, terminal_resized );
	
	//initialize ncurses
	initscr();
	keypad( stdscr, true );
	nodelay( stdscr, true );
	noecho();
	nonl();
	cbreak();
	
	//create main window
	m_mainwindow.show( 0, 0, 0, 0 );
}

void finish( int signal )
{
	//destroy main window
	m_mainwindow.hide();
	
	//stop ncurses
	endwin();
}

void terminal_resized( int signal )
{
	bool optwindow_was_visible = m_optwindow.visible();

	m_optwindow.hide();

	finish(0);	
	init();
	
	if( optwindow_was_visible )
	{
		m_mainwindow.resize( 0, Screen::height() / 4, Screen::width(), Screen::height() - Screen::height() / 4 );
		m_optwindow.setFieldChangedFunc( optwindow_fieldChanged );
		m_optwindow.show( 0, 0, Screen::width(), Screen::height() / 4 );
	}
}

void printhelp()
{

//print disclaimer
fprintf( stderr,
	"\n%s version %s\n"
	"Copyright (C) 2001, 2002 by Roland Riegel <feedback@roland-riegel.de>\n"
	"%s comes with ABSOLUTELY NO WARRANTY. This is free software, and you are\n"
	"welcome to redistribute it under certain conditions. For more details see the\n"
	"GNU General Public License Version 2 (http://www.gnu.org/copyleft/gpl.html).\n\n"

	"Command line syntax:\n"
	"%s [options] [devices]\n"
	"%s --help|-h\n\n"
	
	"Options:\n"
	"-i max_scaling	specifies the 100%% mark in kBit/s of the graph indicating the\n"
	"		incoming bandwidth usage\n"
	"		ignored if max_scaling is 0 or the switch -m is given\n"
	"		default is %d\n"
	"-m		show multiple devices at a time; do not show the traffic graphs\n"
	"-o max_scaling	same as -i but for the graph indicating the outgoing bandwidth\n"
	"		usage\n"
	"		default is %d\n"
	"-s smoothness	sets the \"smoothness\" of the average in/out values\n"
	"		1 means little smoothness (average over a short period of time)\n"
	"		9 means high smoothness (average over a long period of time)\n"
	"		default is %d\n"
	"-t intervall	determines the refresh interval of the display in milliseconds\n"
	"		if 0 print net load only one time and exit\n"
	"		default is %d\n"
	"-u h|b|k|m|g	sets the type of unit used for the display of traffic numbers\n"
	"   H|B|K|M|G	h: human readable (auto), b: Bit/s, k: kBit/s, m: MBit/s etc.\n"
	"		H: human readable (auto), B: Byte/s, K: kByte/s, M: MByte/s etc.\n"
	"		default is k\n"
	"-U h|b|k|m|g	same as -u, but for a total amount of data (without \"/s\")\n"
	"   H|B|K|M|G	default is M\n"
	"devices		network devices to use\n"
	"		default is \"%s\"\n"
	"--help\n"
	"-h		print this help\n\n"
	"example: %s -t 200 -s 7 -i 1024 -o 128 -U h eth0 eth1\n\n",
	
	PACKAGE,
	VERSION,
	PACKAGE,
	PACKAGE,
	PACKAGE,
	STANDARD_BAR_MAX_IN,
	STANDARD_BAR_MAX_OUT,
	STANDARD_AVERAGE_SMOOTHNESS,
	STANDARD_SLEEP_INTERVAL,
	STANDARD_NETWORK_DEVICE,
	PACKAGE
	);

}
