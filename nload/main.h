/***************************************************************************
                                   main.h
                             -------------------
    begin                : Wed Jul 25 2001
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

#ifndef MAIN_H
#define MAIN_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <ctype.h>
#include <time.h>
#include <curses.h>
#include <signal.h>
#include <string>
#include <vector>
#include "dev.h"
#include "options.h"
#include "opt_window.h"

using std::string;
using std::vector;

int main( int, char ** );

void optwindow_fieldChanged( FORM * );

void finish( int );
void terminal_resized( int );

void printhelp();

WINDOW* m_window;

OptWindow m_optwindow;

#endif
