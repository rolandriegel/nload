/***************************************************************************
                                 options.cpp
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

template<class T>
Option<T>::Option()
{
}

template<class T>
Option<T>::Option( OptionBase::ValueType new_valuetype, T new_value, string new_description )
 : OptionBase( new_valuetype, new_description )
{
	m_value = new_value;
}

template<class T>
Option<T>::~Option()
{
}

template<class T>
Option<T>::operator T() const
{
	return m_value;
}

template<class T>
Option<T> Option<T>::operator= ( T new_value )
{
	m_value = new_value;
	
	return *this;
}
