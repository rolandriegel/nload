/***************************************************************************
                               opt_window.cpp
                             -------------------
    begin                : Thu Jan 17 2002
    copyright            : (C) 2002 - 2007 by Roland Riegel
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

#include "opt_window.h"
#include "options.h"

const int BORDER_LEFT = 1;
const int BORDER_RIGHT = 1;
const int BORDER_TOP = 2;
const int BORDER_BOTTOM = 1;

OptWindow::OptWindow()
    : Window(), Form::Slots(), m_sub_window( this ), m_form( this )
{
}

OptWindow::~OptWindow()
{
}

//create option window and display the current settings
void OptWindow::show( int x, int y, int width, int height )
{
	if( m_visible )
		hide();
	
	Window::show( x, y, width, height );
	m_sub_window.show( BORDER_LEFT, BORDER_TOP, width - BORDER_LEFT - BORDER_RIGHT, height - BORDER_TOP - BORDER_BOTTOM );
	
	const int field_width = m_sub_window.width() / 2;
	int line = 0;
	for( vector<OptionBase *>::iterator i = m_options.begin(); i != m_options.end(); i++ )
	{
		line %= m_sub_window.height() < 1 ? 1 : m_sub_window.height();
		
		m_form.fields().push_back( (*i) -> labelField( 0, line, field_width, 1 ) );
		m_form.fields().push_back( (*i) -> editField( field_width, line, field_width, 1 ) );
		
		(*i) -> labelField() -> setNewPage( line == 0 );
		
		line++;
	}
	
	m_form.show( this, &m_sub_window );
	
	m_visible = true;
}

//this function is called when a form field changes
void OptWindow::slot_fieldChanged( Field* field )
{
	for( vector<OptionBase *>::const_iterator r = m_options.begin(); r != m_options.end(); r++ )
	{
		if( *(*r) -> editField() == *field )
			(*r) -> assignString( field -> buffer() );
	}
}

//hide window and destroy it
void OptWindow::hide()
{
	m_form.hide();
	m_form.fields().clear();
	m_sub_window.hide();
	Window::hide();
	
	m_visible = false;
}

//process key presses
void OptWindow::processKey( int request )
{
	if( m_visible )
	{
		switch( request )
		{
			case KEY_LEFT:
				request = REQ_PREV_CHAR;
				break;
			case KEY_RIGHT:
				request = REQ_NEXT_CHAR;
				break;
			case KEY_UP:
				request = REQ_PREV_FIELD;
				break;
			case KEY_DOWN:
			case KEY_ENTER:
			case '\n':
			case '\015':
				request = REQ_NEXT_FIELD;
				break;
			case KEY_DC:
				request = REQ_DEL_CHAR;
				break;
			case KEY_BACKSPACE:
				request = REQ_DEL_PREV;
				break;
			case KEY_PPAGE:
				request = REQ_PREV_CHOICE;
				break;
			case KEY_NPAGE:
			case '\t':
				request = REQ_NEXT_CHOICE;
				break;
			case KEY_HOME:
				request = REQ_BEG_LINE;
				break;
			case KEY_END:
				request = REQ_END_LINE;
				break;
			case '+':
			case 'n':
				request = REQ_NEXT_PAGE;
				break;
			case '-':
			case 'p':
				request = REQ_PREV_PAGE;
				break;
		}
		m_form.processKey( request );
		refresh();
	}
}


//export the collection of options shown in the window
vector<OptionBase *>& OptWindow::options()
{
	return m_options;
}

void OptWindow::refresh()
{
	print( "Options:\n", 0, 0 );
	for( int x = 0; x < width(); x++ )
		print( '=' );
	
	char fText[40] = "";
	sprintf( fText, " <-- (-) page %i/%i (+) --> ", page(), countPages() );
	print( fText, width() - strlen( fText ) - 1, 1 );
	
	wrefresh( m_window );
	m_sub_window.refresh();
}

int OptWindow::page()
{
	return m_form.page();
}

int OptWindow::countPages()
{
	return m_form.countPages();
}
