/***************************************************************************
                                 proc.h
                             -------------------                                         
    begin                : Mon Aug 9 1999                                           
    copyright            : (C) 1999 by Markus Gustavsson                         
    email                : mighty@fragzone.se                                     
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

class Proc
{
public:

	Proc();
	~Proc();

	void setProcDev(char *);
	char *ProcDev();

	float *readLoad(void);

	float totalIn(void);
	float totalOut(void);
	
	float getElapsedTime();
	
private:

	struct timeval was_time;
	struct timeval is_time;
	float elapsed_time;
	
	FILE *fd;

	char *buf, *tag, *dev, *tmp, *tmp2;

	float *S, *cS, *ret;

};

#endif
