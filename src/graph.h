/***************************************************************************
                          graph.h  -  description
                             -------------------
    begin                : Sat Sep 29 2001
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

#ifndef GRAPH_H
#define GRAPH_H


/**
  *@author Roland Riegel
  */

#include <iostream.h>
#include <curses.h>
#include <list>

class Graph
{

public:
	Graph();
	~Graph();
	
	void setNumOfBars( int );
	void setHeightOfBars( int );
	void setTrafficWithMaxDeflectionOfBars( int );
	
	void update( int );
	void setWindow( WINDOW * );
	void print( int, int );

private:
	int m_heightofbars;
	int m_trafficwithmaxdeflectionofbars;
	
	list<int> m_values;
	
	WINDOW *m_window;
	
};

#endif
