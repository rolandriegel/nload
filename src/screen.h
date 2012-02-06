/***************************************************************************
                                  screen.h
                             -------------------
    begin                : Thu Jul 04 2002
    copyright            : (C) 2002 - 2012 by Roland Riegel
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

class Screen
{
    public:
        static int width();
        static int height();
        
        static int x();
        static int y();
};

#endif

