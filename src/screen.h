/***************************************************************************
                                  screen.h
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

#ifndef SCREEN_H
#define SCREEN_H

#include <curses.h>

class Screen
{
public:
	
	static int width() { int width, height; getmaxyx( stdscr, height, width ); return width; }
	static int height() { int width, height; getmaxyx( stdscr, height, width ); return height; }
	
	static int curr_x() { int x, y; getyx( stdscr, y, x ); return x; }
	static int curr_y() { int x, y; getyx( stdscr, y, x ); return y; }
	
};

#endif
