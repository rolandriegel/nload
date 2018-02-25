/*
 * nload
 * real time monitor for network traffic
 * Copyright (C) 2001 - 2018 by Roland Riegel <feedback@roland-riegel.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

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

        unsigned long long calcMaxDeflection();

    private:
        unsigned int m_heightOfBars;
        unsigned long long m_maxDeflection;
        std::list<unsigned long long> m_values;
};

#endif
