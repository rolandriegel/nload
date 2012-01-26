/***************************************************************************
                                 graph.cpp
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

#include "graph.h"
#include "setting.h"
#include "settingstore.h"
#include "window.h"

using namespace std;

Graph::Graph()
    : m_heightOfBars(5), m_maxDeflection(10 * 1024 * 1024 / 8)
{
}

Graph::~Graph()
{
}

// sets the number of the graph's vertical #-bars
void Graph::setNumOfBars(unsigned int numOfBars)
{
    // vertically resize the graph's value list
    m_values.resize(numOfBars);
}

// sets the height of the graph's vertical #-bars
void Graph::setHeightOfBars(unsigned int heightOfBars)
{
    m_heightOfBars = heightOfBars;
}

void Graph::setMaxDeflection(unsigned long long maxDeflection)
{
    m_maxDeflection = maxDeflection;
}

// new traffic measurement has been made => update the graph's value list
void Graph::update(unsigned long long value)
{
    // [new_value] = Bytes/s
    
    // put new value to the beginning of the list, it becomes the first #-bar
    m_values.push_front(value);

    // delete the last #-bar of the list, but keep at least one
    if(m_values.size() > 1)
        m_values.pop_back();
}

// print the graph with the upper left corner at the coordinates (x, y)
void Graph::print(Window& window, int x, int y)
{
    window.setXY(x, y);
    
    // cycle through through the lines
    for(unsigned int l = 0; l < m_heightOfBars; l++)
    {
        // for each line cycle through the rows
        for(list<unsigned long long>::reverse_iterator r = m_values.rbegin(); r != m_values.rend() ; r++)
        {
            unsigned long long trafficPerLine = m_maxDeflection / m_heightOfBars;
            unsigned long long lowerLimit = m_maxDeflection * (m_heightOfBars - l - 1) / m_heightOfBars;

            if(*r < lowerLimit)
            {
                window.print(' ');
            }
            else 
            {
                unsigned long long restOfTraffic = *r - lowerLimit;

                if(restOfTraffic >= trafficPerLine)
                    window.print('#');
                else if(restOfTraffic >= trafficPerLine * 7 / 10)
                    window.print('|');
                else if(restOfTraffic >= trafficPerLine * 3 / 10)
                    window.print('.');
                else
                    window.print(' ');
            }
        }
        window.print('\n');
        window.setX(x);
    }
    
}

// reset all traffic values in the graph to zero
void Graph::resetTrafficData()
{
    int size = m_values.size();
    m_values.clear();
    m_values.resize(size);
}

