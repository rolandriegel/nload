/***************************************************************************
                                   main.h
                             -------------------
    begin                : Wed Jul 25 2001
    copyright            : (C) 2001 by Roland Riegel
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <iostream.h>
#include <ctype.h>
#include <time.h>
#include <curses.h>
#include <signal.h>
#include "dev.h"

#define NLOAD_VERSION "0.3.0"

char **network_device;
Dev **devs;

void printhelp(char *);
void finish(int);
int main(int, char **);

