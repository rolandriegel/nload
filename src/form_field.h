/***************************************************************************
                                form_field.h
                             -------------------
    begin                : Sat Jul 27 2002
    copyright            : (C) 2002 - 2003 by Roland Riegel
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
#include <form.h>
#include <vector>
#include <list>
using std::vector;
using std::list;

class Window;
class SubWindow;
class Form;

class Field
{
public:

	Field( int x, int y, int width, int height );
	~Field();

	void setBuffer( const char* new_buffer );
	const char* buffer();
	
	void move( int x, int y );
	
	void setVisible( bool new_visible );
	bool visible();
	
	void setEnabled( bool new_enabled );
	bool enabled();
	
	void setIntegerField( int min, int max );
	void setEnumField( const char* elements[] );
	
	void setFixed( bool new_fixed );
	bool fixed();
	
	void setBlankWithFirstChar( bool new_blankwithfirstchar );
	bool blankWithFirstChar();
	
	void setAutoSkip( bool new_autoskip );
	bool autoSkip();
	
	void setValidateBlank( bool new_validateblank );
	bool validateBlank();
	
	void setNewPage( bool new_newpage );
	bool newPage();
	
	friend bool operator==( const Field& field1, const Field& field2 );
	friend bool operator==( const Field& field1, const FIELD* field2 );
	
private:
	friend class Form;
	FIELD* m_field;
	
};

class Form
{
public:
	
	class Slots
	{
	public:
		Slots() {}
		virtual ~Slots() {}
		virtual void slot_fieldChanged( Field* field ) {}
	};
	
	Form( Slots* slots = 0 );
	~Form();
	
	vector<Field *>& fields();
	
	void show( Window* main_window, SubWindow* sub_window );
	void hide();
	bool visible();
	
	void processKey( int key );
	
	int page();
	int countPages();
	
private:
	
	static void fieldChanged( FORM* form );
	static list<Form *> m_instances;
	Slots* m_slots;
	
	FORM* m_form;
	vector<Field *> m_fields;
	FIELD** m_curses_fields;
	bool m_visible;
	
};

#endif
