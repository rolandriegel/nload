/***************************************************************************
                               opt_window.cpp
                             -------------------
    begin                : Thu Jan 17 2002
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

#include "opt_window.h"

OptWindow::OptWindow()
{
	m_visible = false;
}

OptWindow::~OptWindow()
{
	if( m_visible )
		hide();
}

//create window and show it with the given coordinates and sizes
void OptWindow::show( int x, int y, int width, int height )
{
	if( ! m_visible )
	{
		m_window = newwin( height, width, y, x );
	}
	else
	{
		wclear( m_window );
		wresize( m_window, height, width );
	}
	
	waddstr( m_window, "This is the future configure window.\n" );
	wnoutrefresh( m_window );
	m_visible = true;
}

//hide window and destroy it
void OptWindow::hide()
{
	delwin( m_window );
	m_visible = false;

}

//is the window currently visible?
bool OptWindow::getVisible()
{
	return m_visible;
}

//export the collection of options shown in the window
vector<OptionBase *>& OptWindow::getOptions()
{
	return m_options;
}
