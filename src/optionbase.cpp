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
 : m_curses_label_field(0)
{
	m_description = new_description;
}

void OptionBase::setDescription( string new_description )
{
	m_description = new_description;
}

string OptionBase::description() const
{
	return m_description;
}

FIELD* OptionBase::createCursesLabelField( int x, int y, int width, int height )
{
	if( ! m_curses_label_field )
	{
		m_curses_label_field = new_field( height, width, y, x, 0, 0 );
		set_field_opts( m_curses_label_field, field_opts( m_curses_label_field ) & ~O_ACTIVE );
		set_field_buffer( m_curses_label_field, 0, ( m_description + ":" ).c_str() );
	}
	return m_curses_label_field;
}

FIELD* OptionBase::cursesLabelField() const
{
	return m_curses_label_field;
}

void OptionBase::deleteCursesLabelField()
{
	free_field( m_curses_label_field );
	m_curses_label_field = 0;
}

