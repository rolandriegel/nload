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

//create option window and display the current settings
void OptWindow::show( int x, int y, int width, int height, void ( *fieldChanged_func )( FORM * ) )
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
		set_field_buffer( m_fields[ i + 1 ], 0, optionAsString( *r ) );
		
		(*r) -> setCursesField( m_fields[ i + 1 ] );
		
		r++; i += 2;
	}
	
	m_fields[ 2 * m_options.size() ] = 0;
	m_form = new_form( m_fields );
	
	set_form_win( m_form, m_window );
	
	set_field_term( m_form, fieldChanged_func );
	
	post_form( m_form );
	wrefresh( m_window );
	
	m_visible = true;
}

//this function is called when a form field changes
//it is called by the function the fifth argument of show() points to
//this can not be done in a nicer way as ncurses does not accept pointers to member functions
void OptWindow::fieldChanged( FORM *changedform )
{
	for( vector<OptionBase *>::const_iterator r = m_options.begin(); r != m_options.end(); r++ )
	{
		if( (*r) -> cursesField() == current_field( m_form ) )
			assignStringToOption( *r, field_buffer( (*r) -> cursesField(), 0 ) );
	}
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

const char *OptWindow::optionAsString( OptionBase *option )
{
	ostringstream oss;
	switch( option -> valueType() )
	{
		case OptionBase::Bool:
			Option<bool> *bool_option;
			bool_option = static_cast<Option<bool> *> (option);
			oss << ( *bool_option ? "[X]" : "[ ]" );
			break;
		case OptionBase::Int:
			Option<int> *int_option;
			int_option = static_cast<Option<int> *> (option);
			oss << *int_option;
			break;
		case OptionBase::Long:
			Option<long> *long_option;
			long_option = static_cast<Option<long> *> (option);
			oss << *long_option;
			break;
		case OptionBase::Status:
			Option<Status::status_format> *status_option;
			status_option = static_cast<Option<Status::status_format> *> (option);
			switch( *status_option )
			{
				case Status::human_readable_bit:
					oss << "Human Readable (Bit)";
					break;
				case Status::human_readable_byte:
					oss << "Human Readable (Byte)";
					break;
				case Status::bit:
					oss << "Bit";
					break;
				case Status::byte:
					oss << "Byte";
					break;
				case Status::kilobit:
					oss << "kBit";
					break;
				case Status::kilobyte:
					oss << "kByte";
					break;
				case Status::megabit:
					oss << "MBit";
					break;
				case Status::megabyte:
					oss << "MByte";
					break;
				case Status::gigabit:
					oss << "GBit";
					break;
				case Status::gigabyte:
					oss << "GByte";
					break;
			}
	}
	
	return oss.str().c_str();
}

void OptWindow::assignStringToOption( OptionBase *option, const char *value )
{
	stringstream ss;
	ss << value;
	switch( option -> valueType() )
	{
		case OptionBase::Bool:
			Option<bool> *bool_option;
			bool_option = static_cast<Option<bool> *> (option);
			*bool_option = ( ss.str().find( "[X]" ) != string::npos );
			break;
		case OptionBase::Int:
			Option<int> *int_option;
			int_option = static_cast<Option<int> *> (option);
			int int_dummy;
			ss >> int_dummy;
			*int_option = int_dummy;
			break;
		case OptionBase::Long:
			Option<long> *long_option;
			long_option = static_cast<Option<long> *> (option);
			long long_dummy;
			ss >> long_dummy;
			*long_option = long_dummy;
			break;
		case OptionBase::Status:
			Option<Status::status_format> *status_option;
			status_option = static_cast<Option<Status::status_format> *> (option);
			if( ss.str().find( "Human Readable (Bit)" ) != string::npos )
				*status_option = Status::human_readable_bit;
			else if( ss.str().find( "Human Readable (Byte)" ) != string::npos )
				*status_option = Status::human_readable_byte;
			else if( ss.str().find( "GBit" ) != string::npos )
				*status_option = Status::gigabit;
			else if( ss.str().find( "GByte" ) != string::npos )
				*status_option = Status::gigabyte;
			else if( ss.str().find( "MBit" ) != string::npos )
				*status_option = Status::megabit;
			else if( ss.str().find( "MByte" ) != string::npos )
				*status_option = Status::megabyte;
			else if( ss.str().find( "kBit"  ) != string::npos )
				*status_option = Status::kilobit;
			else if( ss.str().find( "kByte" ) != string::npos )
				*status_option = Status::kilobyte;
			else if( ss.str().find( "Bit" ) != string::npos )
				*status_option = Status::bit;
			else if( ss.str().find( "Byte" ) != string::npos )
				*status_option = Status::byte;
			break;
	}
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
				request = REQ_PREV_FIELD;
				break;
			case KEY_DOWN:
			case '\t':
				request = REQ_NEXT_FIELD;
				break;
			case KEY_DC:
				request = REQ_DEL_CHAR;
				break;
			case KEY_BACKSPACE:
				request = REQ_DEL_PREV;
				break;
			case KEY_PPAGE:
				request = REQ_PREV_CHOICE;
				break;
			case KEY_NPAGE:
				request = REQ_NEXT_CHOICE;
				break;
			case KEY_HOME:
				request = REQ_BEG_LINE;
				break;
			case KEY_END:
				request = REQ_END_LINE;
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

WINDOW *OptWindow::window()
{
	return m_window;
}
