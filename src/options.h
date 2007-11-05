/***************************************************************************
                                  options.h
                             -------------------
    begin                : Sun Dec 23 2001
    copyright            : (C) 2001 - 2007 by Roland Riegel
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

#ifndef OPTIONS_H
#define OPTIONS_H

#include "optionbase.h"
#include "status.h"

class OptionBool : public OptionBase
{
	
public:
	OptionBool( bool new_value, string new_description = "" );
	~OptionBool();
	
	string asString() const;
	void assignString( const string new_value );
	
	Field* editField( int x = -1, int y = -1, int width = -1, int height = -1 );
	
	operator bool() const;
	
	OptionBool operator= (bool);
	
private:
	
	bool m_value;
	Field* m_edit_field;
	
};

class OptionInt : public OptionBase
{
	
public:
	OptionInt( int new_value, string new_description = "" );
	~OptionInt();
	
	string asString() const;
	void assignString( const string new_value );
	
	Field* editField( int x = -1, int y = -1, int width = -1, int height = -1 );
	
	void min( int new_min );
	void max( int new_max );
	
	operator int() const;
	
	OptionInt operator= (int);
	
private:
	
	int m_value;
	Field* m_edit_field;
	int m_min;
	int m_max;
	
};

class OptionLong : public OptionBase
{
	
public:
	OptionLong( long new_value, string new_description = "" );
	~OptionLong();
	
	string asString() const;
	void assignString( const string new_value );
	
	Field* editField( int x = -1, int y = -1, int width = -1, int height = -1 );
	
	void min( int new_min );
	void max( int new_max );
	
	operator long() const;
	
	OptionLong operator= (long);
	
private:
	
	long m_value;
	Field* m_edit_field;
	int m_min;
	int m_max;
	
};

class OptionStatusFormat : public OptionBase
{
	
public:
	OptionStatusFormat( Status::status_format new_value, string new_description = "" );
	~OptionStatusFormat();
	
	string asString() const;
	void assignString( const string new_value );
	
	Field* editField( int x = -1, int y = -1, int width = -1, int height = -1 );
	
	operator Status::status_format() const;
	
	OptionStatusFormat operator= (Status::status_format);
	
private:
	
	Status::status_format m_value;
	Field* m_edit_field;
	
};

#endif
