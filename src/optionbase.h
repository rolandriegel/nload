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
	enum ValueType
	{
		Bool,
		Int,
		Long,
		Status
	};
	
	OptionBase();
	OptionBase( ValueType new_valuetype, string new_description = "" );
	~OptionBase();
	
	ValueType valueType();
	
	void setDescription( string );
	string description();
	
	void setCursesField( FIELD* );
	FIELD *cursesField();
	
private:
	
	ValueType m_valuetype;
	string m_description;
	FIELD *m_cursesfield;
	
};

#endif
