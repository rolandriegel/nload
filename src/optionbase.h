/***************************************************************************
                                optionbase.h
                             -------------------
    begin                : Sun Jan 20 2002
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

#ifndef OPTIONBASE_H
#define OPTIONBASE_H

#include <form.h>

#include <string>
using std::string;

class OptionBase
{

public:
	OptionBase( string new_description = "" );
	virtual ~OptionBase(){};
	
	void setDescription( string );
	string description() const;
	
	virtual string asString() const = 0;
	virtual void assignString( const string new_value ) = 0;
	
	virtual FIELD* createCursesEditField( int x, int y, int width, int height ) = 0;
	virtual FIELD* cursesEditField() const = 0;
	virtual void deleteCursesEditField() = 0;
	
	FIELD* createCursesLabelField( int x, int y, int width, int height );
	FIELD* cursesLabelField() const;
	void deleteCursesLabelField();
	
private:
	
	string m_description;
	FIELD* m_curses_label_field;
	
};

#endif
