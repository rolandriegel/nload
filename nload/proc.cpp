/***************************************************************************
                                  proc.cpp
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


#include "proc.h"

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

Proc::Proc()
{
	m_total[0] = m_total[1] =
	m_total_new[0] = m_total_new[1] = 0.0;
}

Proc::~Proc()
{
}

void Proc::setProcDev(const char *new_procdev)
{
	strcpy( m_dev, new_procdev );
	readLoad();
}

char* Proc::procDev()
{
	return m_dev;
}

bool Proc::procDevExists()
{
	return m_dev_exists;
}

char* Proc::ip()
{
	struct sockaddr_in* sin;
	struct ifreq ifr;
	int sk;
	
	m_ip[0] = 0;
	
	if( m_dev[0] == 0 ) return m_ip;
	
	/* create a temporary socket: ioctl needs one */
	if( ( sk = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) return m_ip;
	
	/* copy the device name into the ifreq structure */
	strncpy( ifr.ifr_name, m_dev, IFNAMSIZ - 1 );
	ifr.ifr_name[ IFNAMSIZ - 1 ] = 0;
	
	/* make the request */
	if( ! ioctl( sk, SIOCGIFADDR, &ifr ) )
	{
		sin = (struct sockaddr_in *) ( &ifr.ifr_addr );
		
		/* only use the IP number if the address family is really IPv4 */
		if( sin->sin_family == AF_INET )
		{
			char* str_ip = inet_ntoa( sin->sin_addr );
			sprintf( m_ip, "%s", str_ip );
		}
	}
	
	/* close the temporary socket */
	close( sk );
	
	return m_ip;
}

float* Proc::readLoad()
{
	static struct timeval is_time;
	static struct timeval was_time;
	
	//measure the ellapsed time since the last function call
	gettimeofday( &is_time, NULL );
	m_elapsed_time = labs( is_time.tv_sec - was_time.tv_sec ) * 1000 + (float) labs( is_time.tv_usec - was_time.tv_usec ) / 1000;
	was_time = is_time;

	m_ret[0] = 0;
	m_ret[1] = 0;

	if( m_dev[0] == 0 )
	{
		m_dev_exists = false;
		return m_ret;
	}

// === Linux specific network data reading code ===
// Code taken out of knetload: Copyright by Markus Gustavsson <mighty@fragzone.se>
// ================================================
#ifdef HAVE_LINUX
	FILE *fd;
	char buf[512] = "";
	char tag[128] = "";
	char *tmp, *tmp2;

	if( ( fd = fopen( "/proc/net/dev", "r" ) ) == NULL )
		return m_ret;

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
		sscanf( tmp, "%f %f %f %f %f %f %f %f %f", &m_total_new[0], &d, &d, &d, &d, &d, &d, &d, &m_total_new[1] );

		if( ! strcmp( m_dev, tag ) )
		{

			if( m_total_new[0] > m_total[0] )
				m_ret[0] = m_total_new[0] - m_total[0];
			m_total[0] = m_total_new[0];

			if( m_total_new[1] > m_total[1] )
				m_ret[1] = m_total_new[1] - m_total[1];
			m_total[1] = m_total_new[1];

			fclose( fd );
			
			m_dev_exists = true;
			
			return m_ret;
		}

	}

	m_total[0] = 0;
	m_total[1] = 0;

	fclose(fd);
	
	m_dev_exists = false;
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
	
	if( sysctl( mib, 6, NULL, &needed, NULL, 0 ) < 0 )
		return m_ret;
	
	buf = (char *) malloc( needed );
	if( buf == NULL )
		return m_ret;
	
	if( sysctl( mib, 6, buf, &needed, NULL, 0 ) < 0 )
	{
		free( buf );
		return m_ret;
	}
	lim = buf + needed;

	next = buf;
	while( next < lim )
	{
		ifm = (struct if_msghdr *) next;
		if( ifm->ifm_type != RTM_IFINFO )
		{
			free( buf );
			return m_ret;
		}
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
			
			if( strcmp( m_dev, s ) == 0 )
			{
				m_total_new[0] = ifm->ifm_data.ifi_ibytes;
				if( m_total_new[0] > m_total[0] )
					m_ret[0] = m_total_new[0] - m_total[0];
				m_total[0] = m_total_new[0];
				
				m_total_new[1] = ifm->ifm_data.ifi_obytes;
				if( m_total_new[1] > m_total[1] )
					m_ret[1] = m_total_new[1] - m_total[1];
				m_total[1] = m_total_new[1];
				
				m_dev_exists = true;
				
				break;
			}
			else
			{
				m_dev_exists = false;
			}
		}
	}
	
	if( ! m_dev_exists )
		m_total[0] = m_total[1] = 0;

	free( buf );
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
	ksp = kstat_lookup( kc, NULL, -1, m_dev );
	if( ksp && kstat_read( kc, ksp, NULL ) >= 0 )
	{
		knp = (kstat_named_t *) kstat_data_lookup( ksp, "rbytes" );
		if( knp )
		{
			m_total_new[0] = knp->value.ui32;
			if( m_total_new[0] > m_total[0] )
				m_ret[0] = m_total_new[0] - m_total[0];
			m_total[0] = m_total_new[0];
		}
		knp = (kstat_named_t *) kstat_data_lookup( ksp, "obytes" );
		if( knp )
		{
			m_total_new[1] = knp->value.ui32;
			if( m_total_new[1] > m_total[1] )
				m_ret[1] = m_total_new[1] - m_total[1];
			m_total[1] = m_total_new[1];
		}
		m_dev_exists = true;
	}
	else
	{
		m_dev_exists = false;
		m_total[0] = m_total[1] = 0;
	}
	kstat_close( kc );
#endif
// === End Solaris specific network data reading code ===
	
	return m_ret;
}

float Proc::getElapsedTime()
{
	return m_elapsed_time;
}

float Proc::totalIn()
{
	return m_total[0];
}

float Proc::totalOut()
{
	return m_total[1];
}

