/***************************************************************************
                                  graph.h
                             -------------------
    begin                : Sat Sep 29 2001
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

#ifndef GRAPH_H
#define GRAPH_H

#include <list>

class Window;

class Graph
{
    public:
        Graph();
        ~Graph();
        
        void setNumOfBars(unsigned int numOfBars);
        void setHeightOfBars(unsigned int heightOfBars);
        void setMaxDeflection(unsigned long long maxDeflection);
        
        void update(unsigned long long value);
        void print(Window& window, int x, int y);
        void resetTrafficData();

    private:
        unsigned int m_heightOfBars;
        unsigned long long m_maxDeflection;
        std::list<unsigned long long> m_values;
};

#endif
