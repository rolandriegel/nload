/***************************************************************************
                                opt_window.h
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

#ifndef OPT_WINDOW_H
#define OPT_WINDOW_H

#include <vector>
#include <curses.h>
#include "options.h"

class OptWindow
{
public:
	
	OptWindow();
	~OptWindow();
	
	void show( int, int, int, int );
	void hide();
	bool getVisible();
	
	vector<OptionBase *>& getOptions();
	
private:
	
	bool m_visible;
	vector<OptionBase *> m_options;
	WINDOW *m_window;
	
};

#endif
