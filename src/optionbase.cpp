/***************************************************************************
                               optionbase.cpp
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

#include "optionbase.h"

OptionBase::OptionBase()
{
}

OptionBase::OptionBase( ValueType new_valuetype, string new_description )
{
	m_valuetype = new_valuetype;
	m_description = new_description;
}

OptionBase::~OptionBase()
{
}

OptionBase::ValueType OptionBase::valueType()
{
	return m_valuetype;
}

void OptionBase::setDescription( string new_description )
{
	m_description = new_description;
}

string OptionBase::description()
{
	return m_description;
}

void OptionBase::setCursesField( FIELD *new_cursesfield )
{
	m_cursesfield = new_cursesfield;
}

FIELD *OptionBase::cursesField()
{
	return m_cursesfield;
}
