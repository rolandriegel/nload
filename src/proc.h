/***************************************************************************
                                   proc.h
                             -------------------
    begin                : Mon Aug 9 1999
    copyright            : (C) 1999 by Markus Gustavsson
                           (C) 2001 - 2003 by Roland Riegel
    email                : mighty@fragzone.se
                           feedback@roland-riegel.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

#ifndef PROC_H
#define PROC_H

class Proc
{
public:

	Proc();
	~Proc();

	void setProcDev(const char *);
	char* procDev();
	
	bool procDevExists();
	
	char* ip();
	
	float* readLoad();

	float totalIn();
	float totalOut();
	
	float getElapsedTime();
	
private:

	float m_elapsed_time;
	
	bool m_dev_exists;
	char m_dev[128];
	
	char m_ip[16];
	
	float m_ret[2];
	
	float m_total[2];
	float m_total_new[2];

};

#endif
