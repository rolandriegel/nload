/***************************************************************************
                                 proc.cpp
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


#include "proc.h"

Proc::Proc()
{
}

Proc::~Proc()
{
}

void Proc::setProcDev(const char *new_procdev)
{
	strcpy(dev, new_procdev);
	readLoad();
}

char *Proc::ProcDev()
{
	return dev;
}

bool Proc::ProcDevExists()
{
	return dev_exists;
}

float *Proc::readLoad(void)
{
	//measure the ellapsed time since the last function call
	gettimeofday( &is_time, NULL );
	elapsed_time = labs( is_time.tv_sec - was_time.tv_sec ) * 1000 + (float) labs( is_time.tv_usec - was_time.tv_usec ) / 1000;
	was_time = is_time;

	ret[0] = 0;
	ret[1] = 0;

// === Linux specific network data reading code ===
// Code taken out of knetload: Copyright by Markus Gustavsson <mighty@fragzone.se>
// ================================================
#ifdef HAVE_LINUX
	FILE *fd;
	char buf[512] = "";
	char tag[128] = "";
	char *tmp, *tmp2;

	if( ( fd = fopen( "/proc/net/dev", "r" ) ) == NULL )
		return ret;

	fgets( buf, 512, fd );
	fgets( buf, 512, fd );

	while( !feof( fd ) )
	{
		fgets( buf, 512, fd );

		memset( tag, 0, 32 );

		tmp = buf;
		tmp2 = tag;

		while( *tmp == ' ' ) tmp++;
		while( ( *tmp2++ = *tmp++ ) != ':' );

		*--tmp2 = '\0';

		float d;
		sscanf( tmp, "%f %f %f %f %f %f %f %f %f", &total_new[0], &d, &d, &d, &d, &d, &d, &d, &total_new[1] );

		if( !strcmp( dev, tag ) )
		{

			if( total_new[0] > total[0] )
				ret[0] = total_new[0] - total[0];
			total[0] = total_new[0];

			if( total_new[1] > total[1] )
				ret[1] = total_new[1] - total[1];
			total[1] = total_new[1];

			fclose( fd );
			
			dev_exists = true;
			
			return ret;
		}

	}

	total[0] = 0;
	total[1] = 0;

	fclose(fd);
	
	dev_exists = false;
#endif
// === EndLinux specific network data reading code ===

// === Free/Net/OpenBSD specific network data reading code ===
// Code taken out of gkrellm: Copyright by Bill Wilson <bill@gkrellm.net>
//                            FreeBSD code contributed by Hajimu Umemoto <ume@mahoroba.org>
//                            NetBSD code contributed by Anthony Mallet <anthony.mallet@useless-ficus.net>
//                            Hajimu Umemoto merged Free/Net/OpenBSD code
// ===========================================================
#ifdef HAVE_BSD
	struct if_msghdr *ifm, *nextifm;
	struct sockaddr_dl *sdl;
	char *lim, *next;
	size_t needed;
	char s[32];
	int mib[] = { CTL_NET, PF_ROUTE, 0, 0, NET_RT_IFLIST, 0 };
	char *buf = 0;
	size_t alloc = 0;
	
	if( sysctl(mib, 6, NULL, &needed, NULL, 0) < 0 )
		return ret;
	if( alloc < needed )
	{
		if( buf != NULL )
			free( buf );
		buf = (char *) malloc( needed );
		if( buf == NULL )
			return ret;
		alloc = needed;
	}
	
	if( sysctl( mib, 6, buf, &needed, NULL, 0 ) < 0 )
		return ret;
	lim = buf + needed;

	next = buf;
	while( next < lim )
	{
		ifm = (struct if_msghdr *) next;
		if( ifm->ifm_type != RTM_IFINFO )
			return ret;
		next += ifm->ifm_msglen;
		
		while( next < lim )
		{
			nextifm = (struct if_msghdr *) next;
			if( nextifm->ifm_type != RTM_NEWADDR )
				break;
			next += nextifm->ifm_msglen;
		}
		
		if( ifm->ifm_flags & IFF_UP )
		{
			sdl = (struct sockaddr_dl *) ( ifm + 1 );
			if( sdl->sdl_family != AF_LINK )
				continue;
			strncpy( s, sdl->sdl_data, sdl->sdl_nlen );
			s[ sdl->sdl_nlen ] = '\0';
			
			if( strcmp( dev, s ) == 0 )
			{
				total_new[0] = ifm->ifm_data.ifi_ibytes;
				if( total_new[0] > total[0] )
					ret[0] = total_new[0] - total[0];
				total[0] = total_new[0];
				
				total_new[1] = ifm->ifm_data.ifi_obytes;
				if( total_new[1] > total[1] )
					ret[1] = total_new[1] - total[1];
				total[1] = total_new[1];
				
				dev_exists = true;
				
				break;
			}
			else
			{
				dev_exists = false;
			}
		}
	}
	
	if( !dev_exists )
		total[0] = total[1] = 0;
#endif
// === End Free/Net/OpenBSD specific network data reading code ===

// === Solaris specific network data reading code ===
// Code taken out of gkrellm: Copyright by Bill Wilson <bill@gkrellm.net>
//                            Solaris code by Daisuke Yabuki <dxy@acm.org>
// ==================================================
#ifdef HAVE_SOLARIS
        kstat_ctl_t *kc;
        kstat_t *ksp;
        kstat_named_t *knp;
	
	kc = kstat_open();
	ksp = kstat_lookup( kc, NULL, -1, dev );
	if( ksp && kstat_read( kc, ksp, NULL ) >= 0 )
	{
		knp = (kstat_named_t *) kstat_data_lookup( ksp, "rbytes" );
		if( knp )
		{
			total_new[0] = knp->value.ui32;
			if( total_new[0] > total[0] )
				ret[0] = total_new[0] - total[0];
			total[0] = total_new[0];
		}
		knp = (kstat_named_t *) kstat_data_lookup( ksp, "obytes" );
		if( knp )
		{
			total_new[1] = knp->value.ui32;
			if( total_new[1] > total[1] )
				ret[1] = total_new[1] - total[1];
			total[1] = total_new[1];
		}
		dev_exists = true;
	}
	else
	{
		dev_exists = false;
		total[0] = total[1] = 0;
	}
	kstat_close( kc );
#endif
// === End Solaris specific network data reading code ===
	
	return ret;
}

float Proc::getElapsedTime()
{
	return elapsed_time;
}

float Proc::totalIn(void)
{
	return total[0];
}

float Proc::totalOut(void)
{
	return total[1];
}
