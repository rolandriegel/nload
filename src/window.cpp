/***************************************************************************
                                 window.cpp
                             -------------------
    begin                : Thu Nov 25 2003
    copyright            : (C) 2003 by Roland Riegel
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

#include "window.h"
#include <curses.h>

class Form;

Window::Window()
 : m_visible( false ), m_window(0)
{
}

Window::~Window()
{
	hide();
}

//create window and display it
void Window::show( int x, int y, int width, int height )
{
	if( m_window )
		return;
	
	m_window = newwin( height, width, y, x );
	clear();
	refresh();
	
	m_visible = true;
}

//hide window and destroy it
void Window::hide()
{
	if( ! m_window )
		return;
	
	clear();
	refresh();
	delwin( m_window );
	
	m_window = 0;
	
	m_visible = false;
}

//is the window currently visible?
bool Window::visible()
{
	return m_visible;
}

//refresh window
void Window::refresh()
{
	if( m_window )
		wrefresh( m_window );
}

//clear the content of the window
void Window::clear()
{
	if( m_window )
		wclear( m_window );
}

//move and resize window
void Window::resize( int x, int y, int width, int height )
{
	if( ! m_window )
		return;
	wresize( m_window, height, width );
	mvwin( m_window, y, x );
}

//return current window width
int Window::width()
{
	if( ! m_window )
		return 0;
	int width, height;
	getmaxyx( m_window, height, width );
	return width;
}

//return current window height
int Window::height()
{
	if( ! m_window )
		return 0;
	int width, height;
	getmaxyx( m_window, height, width );
	return height;
}

//return current distance to left screen edge
int Window::left()
{
	if( ! m_window )
		return 0;
	int x, y;
	getbegyx( m_window, y, x );
	return x;
}

//return current distance to top screen edge
int Window::top()
{
	if( ! m_window )
		return 0;
	int x, y;
	getbegyx( m_window, y, x );
	return y;
}

//return current cursor position on the x-axis
int Window::x()
{
	if( ! m_window )
		return 0;
	int x, y;
	getyx( m_window, y, x );
	return x;
}

//return current cursor position on the y-axis
int Window::y()
{
	if( ! m_window )
		return 0;
	int x, y;
	getyx( m_window, y, x );
	return y;
}

//set current cursor x position
void Window::setX( int new_x )
{
	if( ! m_window )
		return;
	wmove( m_window, y(), new_x );
}

//set current cursor y position
void Window::setY( int new_y )
{
	if( ! m_window )
		return;
	wmove( m_window, new_y, x() );
}

//set current cursor position
void Window::setXY( int new_x, int new_y )
{
	setX( new_x );
	setY( new_y );
}

//print some text to the window
void Window::print( const char* text, int new_x, int new_y )
{
	if( ! m_window )
		return;
	if( new_x <= -1 )
		new_x = x();
	if( new_y <= -1 )
		new_y = y();
	
	mvwaddstr( m_window, new_y, new_x, text );
}

//print a char to the window
void Window::print( char text, int new_x, int new_y )
{
	if( ! m_window )
		return;
	if( new_x <= -1 )
		new_x = x();
	if( new_y <= -1 )
		new_y = y();
	
	mvwaddch( m_window, new_y, new_x, text );
}

SubWindow::SubWindow( Window* parent )
 : Window(), m_parent( parent )
{
}

SubWindow::~SubWindow()
{
	hide();
}

//return parent window
Window* SubWindow::parent()
{
	return m_parent;
}

//create window and display it
void SubWindow::show( int x, int y, int width, int height )
{
	if( m_window )
		return;
	
	m_window = derwin( m_parent -> m_window, height, width, y, x );
	clear();
	refresh();
	
	m_visible = true;
}
