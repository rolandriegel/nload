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

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>

#ifdef HAVE_LINUX
#include <sys/time.h>
#include <string>
using std::string;
#endif

#ifdef HAVE_BSD
#include <sys/param.h>
#include <sys/sysctl.h>
#include <net/if_dl.h>
#include <net/route.h>
#include <string>
using std::string;
#endif

#ifdef HAVE_SOLARIS
#include <kstat.h>
#include <sys/sockio.h>
#endif

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

	struct timeval m_was_time;
	struct timeval m_is_time;
	float m_elapsed_time;
	
	bool m_dev_exists;
	char m_dev[128];
	
	char m_ip[16];
	
	float m_ret[2];
	
	float m_total[2];
	float m_total_new[2];

};

#endif
