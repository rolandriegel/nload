/***************************************************************************
                                  window.h
                             -------------------
    begin                : Thu Jul 04 2002
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

#ifndef WINDOW_H
#define WINDOW_H

#include <curses.h>

class Window
{
public:
	
	Window() : m_visible( false ), m_window(0) {};
	virtual ~Window()
	{
		hide();
	}
	
	//create window and display it
	virtual void show( int x, int y, int width, int height )
	{
		hide();
		
		m_window = newwin( y, x, height, width );
		clear();
		refresh();
		
		m_visible = true;
	}
	
	//hide window and destroy it
	virtual void hide()
	{
		if( ! m_window ) return;
		
		clear();
		refresh();
		delwin( m_window );
		
		m_window = 0;
		
		m_visible = false;
	}
	
	//is the window currently visible?
	virtual bool visible()
	{
		return m_visible;
	}
	
	//refresh window
	virtual void refresh()
	{
		if( m_window ) wrefresh( m_window );
	}
	
	//clear the content of the window
	virtual void clear()
	{
		if( m_window ) wclear( m_window );
	}
	
	//move and resize window
	virtual void resize( int x, int y, int width, int height )
	{
		if( ! m_window ) return;
		wresize( m_window, height, width );
		mvwin( m_window, y, x );
	}
	
	//return current window width
	virtual int width()
	{
		if( ! m_window ) return 0;
		int width, height;
		getmaxyx( m_window, height, width );
		return width;
	}
	
	//return current window height
	virtual int height()
	{
		if( ! m_window ) return 0;
		int width, height;
		getmaxyx( m_window, height, width );
		return height;
	}
	
	//return current distance to left screen edge
	virtual int x()
	{
		if( ! m_window ) return 0;
		int x, y;
		getbegyx( m_window, y, x );
		return x;
	}
	
	//return current distance to top screen edge
	virtual int y()
	{
		if( ! m_window ) return 0;
		int x, y;
		getbegyx( m_window, y, x );
		return y;
	}
	
protected:
	
	bool m_visible;
	WINDOW* m_window;
	
};

#endif
