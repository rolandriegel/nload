/***************************************************************************
                                  options.h
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

#ifndef OPTIONS_H
#define OPTIONS_H

#include "optionbase.h"

template<class T>
class Option : public OptionBase
{
	
public:
	Option();
	Option( OptionBase::ValueType new_valuetype, T new_value, string new_description = "" );
	~Option();
	
	operator T() const;
	
	Option operator= (T);
	
private:
	
	T m_value;
	
};

#include "options.cpp"

#endif