/***************************************************************************
                               opt_window.cpp
                             -------------------
    begin                : Thu Jan 17 2002
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

#include "opt_window.h"

const int BORDER_LEFT = 1;
const int BORDER_RIGHT = 1;
const int BORDER_TOP = 2;
const int BORDER_BOTTOM = 1;

OptWindow::OptWindow()
    : Window()
{
}

OptWindow::~OptWindow()
{
}

void OptWindow::setFieldChangedFunc( void ( *fieldchangedfunc )( FORM * ) )
{
	m_fieldchangedfunc = fieldchangedfunc;
	if( m_form )
		set_field_term( m_form, m_fieldchangedfunc );
}

void OptWindow::createForm( int width, int height )
{
	const int value_field_width = ( width - BORDER_LEFT - BORDER_RIGHT ) / 2;
	
	m_fields = new FIELD* [ 2 * m_options.size() + 1 ];
	vector<OptionBase *>::const_iterator r = m_options.begin(); 
	int i = 0, line = 0;
	while( r != m_options.end() )
	{
		line %= height - BORDER_TOP - BORDER_BOTTOM;
		
		m_fields[i] = (*r) -> createCursesLabelField( 0, line, width - value_field_width, 1 );
		m_fields[ i + 1 ] = (*r) -> createCursesEditField( value_field_width, line, value_field_width, 1 );
		
		if( line == 0 )
			set_new_page( m_fields[i], true );
		
		r++; i += 2; line++;
	}
	
	m_fields[ 2 * m_options.size() ] = 0;
	m_form = new_form( m_fields );
}

void OptWindow::createWindow( int x, int y, int width, int height )
{
	m_window = newwin( height, width, y, x );
}

void OptWindow::createSubWindow( int x, int y, int width, int height )
{
	if( m_window && m_form )
	{
		int sub_width = 0, sub_height = 0;
		scale_form( m_form, &sub_height, &sub_width );
		m_sub_window = derwin( m_window, sub_height, sub_width, BORDER_TOP, BORDER_BOTTOM );
		set_form_win( m_form, m_window );
		set_form_sub( m_form, m_sub_window );
	}
}

void OptWindow::deleteForm()
{
	free_form( m_form );
	m_form = 0;
	
	for( vector<OptionBase *>::const_iterator r = m_options.begin(); r != m_options.end(); r++ )
	{
		(*r) -> deleteCursesEditField();
		(*r) -> deleteCursesLabelField();
	}
	
	delete[] m_fields;
	m_fields = 0;
}

void OptWindow::deleteWindow()
{
	delwin( m_window );
	m_window = 0;
}

void OptWindow::deleteSubWindow()
{
	delwin( m_sub_window );
	m_sub_window = 0;
}

//create option window and display the current settings
void OptWindow::show( int x, int y, int width, int height )
{
	if( m_visible )
		hide();
	
	createForm( width, height );
	createWindow( x, y, width, height );
	createSubWindow( x, y, width, height );
	
	post_form( m_form );
	
	setFieldChangedFunc( m_fieldchangedfunc );
	
	m_visible = true;
}

//this function is called when a form field changes
//it is called by the function m_fieldchangedfunc
//this can not be done in a nicer way as ncurses does not accept pointers to member functions
void OptWindow::fieldChanged( FORM *changedform )
{
	for( vector<OptionBase *>::const_iterator r = m_options.begin(); r != m_options.end(); r++ )
	{
		if( (*r) -> cursesEditField() == current_field( m_form ) )
			(*r) -> assignString( field_buffer( (*r) -> cursesEditField(), 0 ) );
	}
}

//hide window and destroy it
void OptWindow::hide()
{
	unpost_form( m_form );
	deleteForm();
	deleteSubWindow();
	deleteWindow();
	
	m_visible = false;
}

//process key presses
void OptWindow::processRequest( int request )
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
			case '\t':
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
				request = REQ_NEXT_CHOICE;
				break;
			case KEY_HOME:
				request = REQ_BEG_LINE;
				break;
			case KEY_END:
				request = REQ_END_LINE;
				break;
			case '+':
				request = REQ_NEXT_PAGE;
				break;
			case '-':
				request = REQ_PREV_PAGE;
				break;
		}
		form_driver( m_form, request );
		wrefresh( m_sub_window );
	}
}


//export the collection of options shown in the window
vector<OptionBase *>& OptWindow::options()
{
	return m_options;
}

void OptWindow::refresh()
{
	mvwaddstr( m_window, 0, 0, "Options:\n" );
	for( int x = 0; x < width(); x++ )
		waddch( m_window, '=' );
	
	char fText[40] = "";
	sprintf( fText, " <-- (-) page %i/%i (+) --> ", page(), countPages() );
	mvwaddstr( m_window, 1, width() - strlen( fText ) - 1, fText );
	wrefresh( m_window );
	wrefresh( m_sub_window );
}

int OptWindow::page()
{
	if( ! m_form ) return 0;
	return form_page( m_form ) + 1;	
}

int OptWindow::countPages()
{
	if( ! m_fields ) return 0;
	
	int pages = 0;
	int i = 0;
	while( m_fields[i] )
	{
		if( new_page( m_fields[i] ) )
			pages++;
		i++;
	}
	return pages;
}
