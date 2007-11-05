/***************************************************************************
                                optionbase.h
                             -------------------
    begin                : Sun Jan 20 2002
    copyright            : (C) 2002 - 2007 by Roland Riegel
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

#ifndef OPTIONBASE_H
#define OPTIONBASE_H

#include <string>
using std::string;

class Field;

class OptionBase
{

public:
	OptionBase( string new_description = "" );
	virtual ~OptionBase();
	
	void setDescription( string );
	string description() const;
	
	virtual string asString() const = 0;
	virtual void assignString( const string new_value ) = 0;
	
	virtual Field* editField( int x = -1, int y = -1, int width = -1, int height = -1 ) = 0;
	
	Field* labelField( int x = -1, int y = -1, int width = -1, int height = -1 );
	
private:
	
	string m_description;
	Field* m_label_field;
	
};

#endif
