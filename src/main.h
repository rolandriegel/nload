/***************************************************************************
                                   main.h
                             -------------------
    begin                : Wed Jul 25 2001
    copyright            : (C) 2001 - 2012 by Roland Riegel
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

#include "traffic_window.h"
#include "opt_window.h"

int main(int argc, char *argv[]);

void init();
void finish();
void end(int signal = 0);
void terminalResized(int signal);

void printHelp(bool error);

#endif

