/***************************************************************************
                                 options.cpp
                             -------------------
    begin                : Sun Dec 23 2001
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

#include "options.h"

const char* status_format_strings[] = { "Human Readable (Bit)", "Human Readable (Byte)",
                          "Bit", "Byte",
                          "kBit", "kByte",
                          "MBit", "MByte",
                          "GBit", "GByte" };
const char* bool_strings[] = { "[X]", "[ ]" };

OptionBool::OptionBool( bool new_value, string new_description )
 : OptionBase( new_description ), m_curses_edit_field(0)
{
	m_value = new_value;
}

OptionBool::~OptionBool()
{
	deleteCursesEditField();
	deleteCursesLabelField();
}

string OptionBool::asString() const
{
	return m_value ? string("[X]") : string("[ ]");
}

void OptionBool::assignString( const string new_value )
{
	m_value = ( new_value.substr(0, 3) == "[X]" );
}

FIELD* OptionBool::createCursesEditField( int x, int y, int width, int height )
{
	if( ! m_curses_edit_field )
	{
		m_curses_edit_field = new_field( height, width, y, x, 0, 0 );
		set_field_opts( m_curses_edit_field, field_opts( m_curses_edit_field ) & ~O_BLANK & ~O_AUTOSKIP );
		set_field_buffer( m_curses_edit_field, 0, asString().c_str() );
		set_field_type( m_curses_edit_field, TYPE_ENUM, bool_strings, false, false );
	}
	return m_curses_edit_field;
}

FIELD* OptionBool::cursesEditField() const
{
	return m_curses_edit_field;
}

void OptionBool::deleteCursesEditField()
{
	free_field( m_curses_edit_field );
	m_curses_edit_field = 0;
}

OptionBool::operator bool() const
{
	return m_value;
}

OptionBool OptionBool::operator= ( bool new_value )
{
	m_value = new_value;
	
	return *this;
}

OptionInt::OptionInt( int new_value, string new_description )
 : OptionBase( new_description ),
   m_curses_edit_field(0), m_min(0), m_max(0)
{
	m_value = new_value;
}

OptionInt::~OptionInt()
{
	deleteCursesEditField();
	deleteCursesLabelField();
}

string OptionInt::asString() const
{
	ostringstream oss;
	oss << m_value;
	return oss.str();
}

void OptionInt::assignString( const string new_value )
{
	stringstream ss;
	ss << new_value;
	ss >> m_value;
}

FIELD* OptionInt::createCursesEditField( int x, int y, int width, int height )
{
	if( ! m_curses_edit_field )
	{
		m_curses_edit_field = new_field( height, width, y, x, 0, 0 );
		set_field_opts( m_curses_edit_field, field_opts( m_curses_edit_field ) & ~O_BLANK & ~O_AUTOSKIP );
		set_field_buffer( m_curses_edit_field, 0, asString().c_str() );
		set_field_type( m_curses_edit_field, TYPE_INTEGER, 0, m_min, m_max );
	}
	return m_curses_edit_field;
}

FIELD* OptionInt::cursesEditField() const
{
	return m_curses_edit_field;
}

void OptionInt::deleteCursesEditField()
{
	free_field( m_curses_edit_field );
	m_curses_edit_field = 0;
}

void OptionInt::min( int new_min )
{
	m_min = new_min;
}

void OptionInt::max( int new_max )
{
	m_max = new_max;
}

OptionInt::operator int() const
{
	return m_value;
}

OptionInt OptionInt::operator= ( int new_value )
{
	m_value = new_value;
	
	return *this;
}

OptionLong::OptionLong( long new_value, string new_description )
 : OptionBase( new_description ),
   m_curses_edit_field(0), m_min(0), m_max(0)
{
	m_value = new_value;
}

OptionLong::~OptionLong()
{
	deleteCursesEditField();
	deleteCursesLabelField();
}

string OptionLong::asString() const
{
	ostringstream oss;
	oss << m_value;
	return oss.str();
}

void OptionLong::assignString( const string new_value )
{
	stringstream ss;
	ss << new_value;
	ss >> m_value;
}

FIELD* OptionLong::createCursesEditField( int x, int y, int width, int height )
{
	if( ! m_curses_edit_field )
	{
		m_curses_edit_field = new_field( height, width, y, x, 0, 0 );
		set_field_opts( m_curses_edit_field, field_opts( m_curses_edit_field ) & ~O_BLANK & ~O_AUTOSKIP );
		set_field_buffer( m_curses_edit_field, 0, asString().c_str() );
		set_field_type( m_curses_edit_field, TYPE_INTEGER, 0, m_min, m_max );
	}
	return m_curses_edit_field;
}

FIELD* OptionLong::cursesEditField() const
{
	return m_curses_edit_field;
}

void OptionLong::deleteCursesEditField()
{
	free_field( m_curses_edit_field );
	m_curses_edit_field = 0;
}

void OptionLong::min( int new_min )
{
	m_min = new_min;
}

void OptionLong::max( int new_max )
{
	m_max = new_max;
}

OptionLong::operator long() const
{
	return m_value;
}

OptionLong OptionLong::operator= ( long new_value )
{
	m_value = new_value;
	
	return *this;
}

OptionStatusFormat::OptionStatusFormat( Status::status_format new_value, string new_description )
 : OptionBase( new_description ), m_curses_edit_field(0)
{
	m_value = new_value;
}

OptionStatusFormat::~OptionStatusFormat()
{
	deleteCursesEditField();
	deleteCursesLabelField();
}

string OptionStatusFormat::asString() const
{
	return string( status_format_strings[ m_value + 2 ] );
}

void OptionStatusFormat::assignString( const string new_value )
{
	for( int i = Status::human_readable_bit; i <= Status::gigabyte; i++ )
	{
		if( new_value.substr(0, string( status_format_strings[ i + 2 ] ).size() ) == ( status_format_strings[ i + 2 ] ) )
		{
			m_value = (Status::status_format) i;
			break;
		}
	}
}

FIELD* OptionStatusFormat::createCursesEditField( int x, int y, int width, int height )
{
	if( ! m_curses_edit_field )
	{
		m_curses_edit_field = new_field( height, width, y, x, 0, 0 );
		set_field_opts( m_curses_edit_field, field_opts( m_curses_edit_field ) & ~O_BLANK & ~O_AUTOSKIP );
		set_field_buffer( m_curses_edit_field, 0, asString().c_str() );
		set_field_type( m_curses_edit_field, TYPE_ENUM, status_format_strings, false, false );
	}
	return m_curses_edit_field;
}

FIELD* OptionStatusFormat::cursesEditField() const
{
	return m_curses_edit_field;
}

void OptionStatusFormat::deleteCursesEditField()
{
	free_field( m_curses_edit_field );
	m_curses_edit_field = 0;
}

OptionStatusFormat::operator Status::status_format() const
{
	return m_value;
}

OptionStatusFormat OptionStatusFormat::operator= ( Status::status_format new_value )
{
	m_value = new_value;
	
	return *this;
}
