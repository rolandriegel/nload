/***************************************************************************
                               optionbase.cpp
                             -------------------
    begin                : Sun Jan 20 2002
    copyright            : (C) 2002 by Roland Riegel
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

#include "optionbase.h"

OptionBase::OptionBase( string new_description )
 : m_label_field(0)
{
	m_description = new_description;
}

OptionBase::~OptionBase()
{
	if( m_label_field )
		delete m_label_field;
}

void OptionBase::setDescription( string new_description )
{
	m_description = new_description;
}

string OptionBase::description() const
{
	return m_description;
}

Field* OptionBase::labelField( int x, int y, int width, int height )
{
	if( ! m_label_field )
	{
		m_label_field = new Field( x, y, width, height );
		m_label_field -> setEnabled( false );
		m_label_field -> setBuffer( ( m_description + ":" ).c_str() );
	}
	return m_label_field;
}
