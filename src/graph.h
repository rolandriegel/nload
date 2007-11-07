/***************************************************************************
                                  graph.h
                             -------------------
    begin                : Sat Sep 29 2001
    copyright            : (C) 2001 - 2007 by Roland Riegel
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

#ifndef GRAPH_H
#define GRAPH_H

#include <list>

class Window;

#define STANDARD_MAX_DEFLECTION 10240 // [kBit/s]  10 MBit/s = 10240 kBit/s

class Graph
{
    public:
        Graph();
        ~Graph();
        
        void setNumOfBars(int);
        void setHeightOfBars(int);
        
        void update(int);
        void print(Window&, int, int);
        void resetTrafficData();

    private:
        long trafficWithMaxDeflectionOfBars();
        
        int m_heightofbars;
        
        std::list<int> m_values;
};

#endif
