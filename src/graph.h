/***************************************************************************
                          graph.h  -  description
                             -------------------
    begin                : Sat Sep 29 2001
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

#ifndef GRAPH_H
#define GRAPH_H

#include <curses.h>
#include <list>
using std::list;

#include "window.h"

class Graph
{

public:
	Graph();
	~Graph();
	
	void setNumOfBars( int );
	void setHeightOfBars( int );
	void setTrafficWithMaxDeflectionOfBars( int );
	
	void update( int );
	void print( Window&, int, int );
	void resetTrafficData();

private:
	int m_heightofbars;
	int m_trafficwithmaxdeflectionofbars;
	
	list<int> m_values;
	
};

#endif
