/***************************************************************************
                               opt_window.cpp
                             -------------------
    begin                : Thu Jan 17 2002
    copyright            : (C) 2002 by Roland Riegel
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

#include "opt_window.h"

OptWindow::OptWindow()
{
	m_visible = false;
}

OptWindow::~OptWindow()
{
	if( m_visible )
		hide();
}

//create window and show it with the given coordinates and sizes
void OptWindow::show( int x, int y, int width, int height )
{
	if( m_visible )
		hide();
	
	m_window = newwin( height, width, y, x );
	
	const int value_field_width = width - 3 - 42;
	
	m_fields = new FIELD* [ 2 * m_options.size() + 1 ];
	vector<OptionBase *>::const_iterator r = m_options.begin(); int i = 0;
	while( r != m_options.end() )
	{
		m_fields[i] = new_field( 1, width - value_field_width - 3, i / 2 + 1, 1, 0, 0 );
		m_fields[ i + 1 ] = new_field( 1, value_field_width, i / 2 + 1, width - value_field_width - 1, 0, 0 );
		
		set_field_opts( m_fields[i], field_opts( m_fields[i] ) & ~O_ACTIVE );
		set_field_opts( m_fields[ i + 1 ], field_opts( m_fields[ i + 1 ] ) & ~O_BLANK & ~O_AUTOSKIP );
		
		set_field_buffer( m_fields[i], 0, ( (*r) -> description() + ":" ).c_str() );
		
		char value[ value_field_width + 1 ];
		switch( (*r) -> valueType() )
		{
			case OptionBase::Bool:
				Option<bool> *bool_option;
				bool_option = static_cast<Option<bool> *> (*r);
				sprintf( value, (bool) *bool_option ? "[X]" : "[ ]" );
				break;
			case OptionBase::Int:
				Option<int> *int_option;
				int_option = static_cast<Option<int> *> (*r);
				sprintf( value, "%i", (int) *int_option );
				break;
			case OptionBase::Long:
				Option<long> *long_option;
				long_option = static_cast<Option<long> *> (*r);
				sprintf( value, "%li", (long) *long_option );
				break;
			case OptionBase::Status:
				Option<Status::status_format> *status_option;
				status_option = static_cast<Option<Status::status_format> *> (*r);
                                switch( (Status::status_format) *status_option )
				{
					case Status::human_readable_bit:
						sprintf( value, "Human Readable (Bit)" );
						break;
					case Status::human_readable_byte:
						sprintf( value, "Human Readable (Byte)" );
						break;
					case Status::bit:
						sprintf( value, "Bit" );
						break;
					case Status::byte:
						sprintf( value, "Byte" );
						break;
					case Status::kilobit:
						sprintf( value, "kBit" );
						break;
					case Status::kilobyte:
						sprintf( value, "kByte" );
						break;
					case Status::megabit:
						sprintf( value, "MBit" );
						break;
					case Status::megabyte:
						sprintf( value, "MByte" );
						break;
					case Status::gigabit:
						sprintf( value, "GBit" );
						break;
					case Status::gigabyte:
						sprintf( value, "GByte" );
						break;
				}
				break;
		}
		
		set_field_buffer( m_fields[ i + 1 ], 0, value );
		
		(*r) -> setCursesField( m_fields[ i + 1 ] );
		
		r++; i += 2;
	}
	
	m_fields[ 2 * m_options.size() ] = 0;
	m_form = new_form( m_fields );
	
	set_form_win( m_form, m_window );
	
	post_form( m_form );
	wrefresh( m_window );
	
	m_visible = true;
}

//hide window and destroy it
void OptWindow::hide()
{

	unpost_form( m_form );
	free_form( m_form );
	m_form = 0;
	
	for( unsigned int i = 0; i < 2 * m_options.size(); i++ )
		free_field( m_fields[i] );
	
	delete( m_fields );
	m_fields = 0;
	
	delwin( m_window );
	m_window = 0;
	
	m_visible = false;
}

//is the window currently visible?
bool OptWindow::visible()
{
	return m_visible;
}

//process key presses
void OptWindow::processRequest( int request )
{
	if( m_visible )
	{
		switch( request )
		{
			case KEY_LEFT:
				request = REQ_PREV_CHAR;
				break;
			case KEY_RIGHT:
				request = REQ_NEXT_CHAR;
				break;
			case KEY_UP:
				request = REQ_SPREV_FIELD;
				break;
			case KEY_DOWN:
			case '\t':
				request = REQ_SNEXT_FIELD;
				break;
			case KEY_DC:
				request = REQ_DEL_CHAR;
				break;
			case KEY_IC:
				beep();
				break;
			case KEY_PPAGE:
				request = REQ_PREV_CHOICE;
				break;
			case KEY_NPAGE:
				request = REQ_NEXT_CHOICE;
				break;
		}
		form_driver( m_form, request );
		wrefresh( m_window );
	}
}


//export the collection of options shown in the window
vector<OptionBase *>& OptWindow::options()
{
	return m_options;
}
