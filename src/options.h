/***************************************************************************
                                  options.h
                             -------------------
    begin                : Sun Dec 23 2001
    copyright            : (C) 2001 by Roland Riegel
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

#include <string>

using std::string;

//dummy class to put different template class
//instances into a single array
class OptionBase{};

template<class T>
class Option : public OptionBase
{
	
public:
	Option();
	Option( T );
	Option( T, string );
	~Option();
	
	operator T() const;
	
	void setDescription( string );
	string getDescription();
	
private:
	
	string m_description;
	T m_value;
	
};

#include "options.cpp"
