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


//======== Linux includes ========
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
//======== Linux includes (end) ========
/*
//======== Free/Open/NetBSD includes ========
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/route.h>
//======== Free/Open/NetBSD includes (end) ========

//======== Solaris includes ========
#include <sys/socket.h>
#include <kstat.h>
#include <net/if.h>
#include <sys/sockio.h>
//======== Solaris includes (end) ========
*/

class Proc
{
public:

	Proc();
	~Proc();

	void setProcDev(const char *);
	char *ProcDev();
	
	bool ProcDevExists();

	float *readLoad(void);

	float totalIn(void);
	float totalOut(void);
	
	float getElapsedTime();
	
private:

	struct timeval was_time;
	struct timeval is_time;
	float elapsed_time;
	
	bool dev_exists;
	char dev[128];
	
	float ret[2];
	
	float total[2];
	float total_new[2];

};

#endif
