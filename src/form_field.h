/***************************************************************************
                                form_field.h
                             -------------------
    begin                : Sat Jul 27 2002
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

#ifndef FORM_FIELD_H
#define FORM_FIELD_H

#include <curses.h>
#include <vector>
using std::vector;

#include "window.h"

class Form;

class Field
{
public:

	Field( int x, int y, int width, int height ) : m_field(0)
	{
		m_field = new_field( height, width, y, x, 0, 0 );
	}
	
	~Field()
	{
		free_field( m_field );
	}
	
	void setBuffer( const char* new_buffer )
	{
		set_field_buffer( m_field, 0, new_buffer );
	}
	
	const char* buffer()
	{
		return field_buffer( m_field, 0 );
	}
	
	void move( int x, int y )
	{
		move_field( m_field, y, x );
	}
	
	void setVisible( bool new_visible )
	{
		set_field_opts( m_field, new_visible ? field_opts( m_field ) | O_VISIBLE : field_opts( m_field ) & ~O_VISIBLE );
	}
	
	bool visible()
	{
		return ( field_opts( m_field ) & O_VISIBLE ) == O_VISIBLE;
	}
	
	void setEnabled( bool new_enabled )
	{
		set_field_opts( m_field, new_enabled ? field_opts( m_field ) | O_ACTIVE : field_opts( m_field ) & ~O_ACTIVE );
	}
	
	bool enabled()
	{
		return ( field_opts( m_field ) & O_ACTIVE ) == O_ACTIVE;
	}
	
	void setIntegerField( int min, int max )
	{
		set_field_type( m_field, TYPE_INTEGER, 0, min, max );
	}
	
	void setEnumField( const char* elements[] )
	{
		set_field_type( m_field, TYPE_ENUM, elements, false, false );
	}
	
	void setFixed( bool new_fixed )
	{
		set_field_opts( m_field, new_fixed ? field_opts( m_field ) & ~O_EDIT : field_opts( m_field ) | O_EDIT );
	}
	
	bool fixed()
	{
		return ( field_opts( m_field ) & O_EDIT ) != O_EDIT;
	}
	
	void setBlankWithFirstChar( bool new_blankwithfirstchar )
	{
		set_field_opts( m_field, new_blankwithfirstchar ? field_opts( m_field ) | O_BLANK : field_opts( m_field ) & ~O_BLANK );
	}
	
	bool blankWithFirstChar()
	{
		return ( field_opts( m_field ) & O_BLANK ) == O_BLANK;
	}
	
	void setAutoSkip( bool new_autoskip )
	{
		set_field_opts( m_field, new_autoskip ? field_opts( m_field ) | O_AUTOSKIP : field_opts( m_field ) & ~O_AUTOSKIP );
	}
	
	bool autoSkip()
	{
		return ( field_opts( m_field ) & O_AUTOSKIP ) == O_AUTOSKIP;
	}
	
	void setValidateBlank( bool new_validateblank )
	{
		set_field_opts( m_field, new_validateblank ? field_opts( m_field ) & ~O_NULLOK : field_opts( m_field ) | O_NULLOK );
	}
	
	bool validateBlank()
	{
		return ( field_opts( m_field ) & O_NULLOK ) != O_NULLOK;
	}
	
	
	void setNewPage( bool new_newpage )
	{
		set_new_page( m_field, new_newpage );
	}
	
	bool newPage()
	{
		return new_page( m_field );
	}
	
	friend bool operator==( const Field& field1, const Field& field2 )
	{
		return field1.m_field == field2.m_field;
	}
	
	friend bool operator==( const Field& field1, const FIELD* field2 )
	{
		return field1.m_field == field2;
	}
	
private:
	friend class Form;
	FIELD* m_field;
	
};

class Form
{
public:
	Form() : m_form(0), m_visible( false ) {}
	~Form() {}
	
	vector<Field *>& fields()
	{
		return m_fields;
	}
	
	void show( Window* main_window, SubWindow* sub_window )
	{
		m_curses_fields = new FIELD* [ m_fields.size() ];
		vector<Field *>::const_iterator r = m_fields.begin(); 
		int i = 0;
		while( r != m_fields.end() )
		{
			m_curses_fields[i] = (*r) -> m_field;
			r++; i++;
		}
		
		m_curses_fields[ m_fields.size() ] = 0;
		m_form = new_form( m_curses_fields );
		
		if( ! m_form ) return;
		
		set_form_win( m_form, main_window -> m_window );
		set_form_sub( m_form, sub_window -> m_window );
		
		post_form( m_form );
		
		m_visible = true;
	}
	
	void hide()
	{
		
		unpost_form( m_form );
		free_form( m_form );
		m_form = 0;
		
		delete[] m_curses_fields;
		m_curses_fields = 0;
		
		m_visible = false;
	}
	
	bool visible()
	{
		return m_visible;
	}
	
	void processKey( int key )
	{
		if( m_form )
			form_driver( m_form, key );
	}
	
	int page()
	{
		if( ! m_form ) return 0;
		return form_page( m_form ) + 1;
	}
	
	int countPages()
	{
		int pages = 0;
		for( vector<Field *>::const_iterator i = m_fields.begin(); i != m_fields.end(); i++ )
		{
			if( (*i) -> newPage() ) pages++;
		}
		return pages;
	}
	
	FORM* handle()
	{
		return m_form;
	}
	
private:
	FORM* m_form;
	vector<Field *> m_fields;
	FIELD** m_curses_fields;
	bool m_visible;
	
};

#endif
