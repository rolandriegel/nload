/***************************************************************************
                                   main.h
                             -------------------
    begin                : Wed Jul 25 2001
    copyright            : (C) 2001, 2002 by Roland Riegel
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
#include "traffic_window.h"
#include "opt_window.h"
#include "screen.h"

using std::string;
using std::vector;

int main( int argc, char *argv[] );

void optwindow_fieldChanged( FORM* form );

void init();
void finish();
void end( int signal = 0 );
void terminal_resized( int signal );

void printhelp();

OptWindow m_optwindow;
TrafficWindow m_mainwindow;

#endif
