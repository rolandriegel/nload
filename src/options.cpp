/***************************************************************************
                                 options.cpp
                             -------------------
    begin                : Sun Dec 23 2001
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

#include "options.h"

#include <sstream>
using std::stringstream;
using std::ostringstream;
#include "form_field.h"

const char* status_format_strings[] = { "Human Readable (Bit)", "Human Readable (Byte)",
                          "Bit", "Byte",
                          "kBit", "kByte",
                          "MBit", "MByte",
                          "GBit", "GByte", 0 };
const char* bool_strings[] = { "[X]", "[ ]", 0 };

OptionBool::OptionBool( bool new_value, string new_description )
 : OptionBase( new_description ), m_edit_field(0)
{
	m_value = new_value;
}

OptionBool::~OptionBool()
{
	if( m_edit_field )
		delete m_edit_field;
}

string OptionBool::asString() const
{
	return m_value ? string("[X]") : string("[ ]");
}

void OptionBool::assignString( const string new_value )
{
	m_value = ( new_value.substr(0, 3) == "[X]" );
}

Field* OptionBool::editField( int x, int y, int width, int height )
{

	if( x < 0 || y < 0 ) return m_edit_field;
	if( width < 1 || height < 1 ) return m_edit_field;
	
	if( m_edit_field )
	{
		delete m_edit_field;
	}
	
	m_edit_field = new Field( x, y, width, height );
	m_edit_field -> setFixed( true );
	m_edit_field -> setBlankWithFirstChar( false );
	m_edit_field -> setAutoSkip( false );
	m_edit_field -> setValidateBlank( true );
	m_edit_field -> setBuffer( asString().c_str() );
	m_edit_field -> setEnumField( bool_strings );
	
	return m_edit_field;
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
   m_edit_field(0), m_min(0), m_max(0)
{
	m_value = new_value;
}

OptionInt::~OptionInt()
{
	if( m_edit_field )
		delete m_edit_field;
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

Field* OptionInt::editField( int x, int y, int width, int height )
{
	if( x < 0 || y < 0 ) return m_edit_field;
	if( width < 1 || height < 1 ) return m_edit_field;
	
	if( m_edit_field )
	{
		delete m_edit_field;
	}
	
	m_edit_field = new Field( x, y, width, height );
	m_edit_field -> setBlankWithFirstChar( false );
	m_edit_field -> setAutoSkip( false );
	m_edit_field -> setValidateBlank( true );
	m_edit_field -> setBuffer( asString().c_str() );
	m_edit_field -> setIntegerField( m_min, m_max );
	
	return m_edit_field;
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
   m_edit_field(0), m_min(0), m_max(0)
{
	m_value = new_value;
}

OptionLong::~OptionLong()
{
	if( m_edit_field )
		delete m_edit_field;
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

Field* OptionLong::editField( int x, int y, int width, int height )
{
	if( x < 0 || y < 0 ) return m_edit_field;
	if( width < 1 || height < 1 ) return m_edit_field;
	
	if( m_edit_field )
	{
		delete m_edit_field;
	}
	
	m_edit_field = new Field( x, y, width, height );
	m_edit_field -> setBlankWithFirstChar( false );
	m_edit_field -> setAutoSkip( false );
	m_edit_field -> setValidateBlank( true );
	m_edit_field -> setBuffer( asString().c_str() );
	m_edit_field -> setIntegerField( m_min, m_max );
	
	return m_edit_field;
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
 : OptionBase( new_description ), m_edit_field(0)
{
	m_value = new_value;
}

OptionStatusFormat::~OptionStatusFormat()
{
	if( m_edit_field )
		delete m_edit_field;
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

Field* OptionStatusFormat::editField( int x, int y, int width, int height )
{
	if( x < 0 || y < 0 ) return m_edit_field;
	if( width < 1 || height < 1 ) return m_edit_field;
	
	if( m_edit_field )
	{
		delete m_edit_field;
	}
	
	m_edit_field = new Field( x, y, width, height );
	m_edit_field -> setBlankWithFirstChar( false );
	m_edit_field -> setAutoSkip( false );
	m_edit_field -> setValidateBlank( true );
	m_edit_field -> setFixed( true );
	m_edit_field -> setBuffer( asString().c_str() );
	m_edit_field -> setEnumField( status_format_strings );
	
	return m_edit_field;
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
