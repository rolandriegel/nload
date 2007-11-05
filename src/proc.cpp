/***************************************************************************
                                  proc.cpp
                             -------------------
    begin                : Mon Aug 9 1999
    copyright            : (C) 1999 by Markus Gustavsson
                           (C) 2001 - 2007 by Roland Riegel
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

#ifdef HAVE_HPUX
	#include <fcntl.h>
	#include <sys/stropts.h>
	#include <sys/dlpi.h>
	#include <sys/dlpi_ext.h>
	#include <sys/mib.h>
#endif

Proc::Proc()
{
	m_total[0] = m_total[1] = 0;
	m_time_last_read = m_elapsed_time = 0;
}

Proc::~Proc()
{
}

void Proc::setProcDev(const char *new_procdev)
{
	unsigned long dummy;
	strcpy( m_dev, new_procdev );
	readLoad( dummy, dummy );
}

const char* Proc::procDev()
{
	return m_dev;
}

bool Proc::procDevExists()
{
	return m_dev_exists;
}

const char* Proc::ip()
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

void Proc::readLoad( unsigned long& in, unsigned long& out )
{
	long long total_new[2] = { 0, 0 };
	int curr_time = 0;
	struct timeval time;

	in = out = 0;
	
	//measure the ellapsed time since the last function call
	gettimeofday( &time, NULL );

	curr_time = time.tv_sec * 1000 + time.tv_usec / 1000;
	m_elapsed_time = abs( curr_time - m_time_last_read );

	m_time_last_read = curr_time;
	
	m_dev_exists = false;
	
	if( m_dev[0] == 0 )
		return;
	
// === Linux specific network data reading code ===
// Code taken out of knetload: Copyright by Markus Gustavsson <mighty@fragzone.se>
//                             modified by Roland Riegel <feedback@roland-riegel.de>
// ================================================
#ifdef HAVE_LINUX
	FILE *fd;
	char buf[512] = "";
	char dev[128] = "";
	char *tmp, *tmp2;

	do
	{
		if( ( fd = fopen( "/proc/net/dev", "r" ) ) == NULL )
			break;
		
		fgets( buf, 512, fd );
		fgets( buf, 512, fd );
		
		m_dev_exists = false;
		
		while( !feof( fd ) )
		{
			fgets( buf, 512, fd );
			
			memset( dev, 0, 32 );
			
			tmp = buf;
			tmp2 = dev;
			
			while( *tmp == ' ' ) tmp++;
			while( ( *tmp2++ = *tmp++ ) != ':' );
			
			*--tmp2 = '\0';
			
			if( ! strcmp( m_dev, dev ) )
			{
				sscanf( tmp, "%llu %*u %*u %*u %*u %*u %*u %*u %llu", &total_new[0], &total_new[1] );
				
				if( total_new[0] > m_total[0] )
					in = total_new[0] - m_total[0];
				m_total[0] = total_new[0];
				
				if( total_new[1] > m_total[1] )
					out = total_new[1] - m_total[1];
				m_total[1] = total_new[1];
				
				m_dev_exists = true;
				
				break;
			}
		}
	} while( 0 );

	if( fd != NULL )
        fclose(fd);
	
#endif
// === End Linux specific network data reading code ===
	
// === Free/Net/OpenBSD specific network data reading code ===
// Code taken out of gkrellm: Copyright by Bill Wilson <bill@gkrellm.net>
//                            FreeBSD code contributed by Hajimu Umemoto <ume@mahoroba.org>
//                            NetBSD code contributed by Anthony Mallet <anthony.mallet@useless-ficus.net>
//                            Hajimu Umemoto merged Free/Net/OpenBSD code
//                            Roland Riegel fixed a memory leak <feedback@roland-riegel.de>
// ===========================================================
#ifdef HAVE_BSD
	struct if_msghdr *ifm, *nextifm;
	struct sockaddr_dl *sdl;
	char *lim, *next;
	size_t needed;
	char s[32];
	int mib[] = { CTL_NET, PF_ROUTE, 0, 0, NET_RT_IFLIST, 0 };
	char *buf = 0;
	
	do
	{
		if( sysctl( mib, 6, NULL, &needed, NULL, 0 ) < 0 )
			break;
		
		buf = (char *) malloc( needed );
		if( buf == NULL )
			break;
		
		if( sysctl( mib, 6, buf, &needed, NULL, 0 ) < 0 )
			break;
		
		lim = buf + needed;
		
		next = buf;
		while( next < lim )
		{
			ifm = (struct if_msghdr *) next;
			if( ifm->ifm_type != RTM_IFINFO )
				break;
			
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
					total_new[0] = ifm->ifm_data.ifi_ibytes;
					if( total_new[0] > m_total[0] )
						in = total_new[0] - m_total[0];
					m_total[0] = total_new[0];
					
					total_new[1] = ifm->ifm_data.ifi_obytes;
					if( total_new[1] > m_total[1] )
						out = total_new[1] - m_total[1];
					m_total[1] = total_new[1];
					
					m_dev_exists = true;
					
					break;
				}
			}
		}
	} while ( 0 );
	
	free( buf );
	
#endif
// === End Free/Net/OpenBSD specific network data reading code ===
	
// === Solaris specific network data reading code ===
// Code taken out of gkrellm: Copyright by Bill Wilson <bill@gkrellm.net>
//                            Solaris code by Daisuke Yabuki <dxy@acm.org>
//                            adapted for nload by Roland Riegel <feedback@roland-riegel.de>
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
			total_new[0] = knp->value.ui32;
			if( total_new[0] > m_total[0] )
				in = total_new[0] - m_total[0];
			m_total[0] = total_new[0];
		}

		knp = (kstat_named_t *) kstat_data_lookup( ksp, "obytes" );
		if( knp )
		{
			total_new[1] = knp->value.ui32;
			if( total_new[1] > m_total[1] )
				out = total_new[1] - m_total[1];
			m_total[1] = total_new[1];
		}

		m_dev_exists = true;
	}

	kstat_close( kc );
#endif
// === End Solaris specific network data reading code ===
	
// === HP-UX specific network data reading code ===
/**
  *
  * Copyright 1998 by Hewlett-Packard Company
  *
  * Permission to use, copy, modify, and distribute this
  * software and its documentation for any purpose and without
  * fee is hereby granted, provided that the above copyright
  * notice appear in all copies and that both that copyright
  * notice and this permission notice appear in supporting
  * documentation, and that the name of Hewlett-Packard Company not
  * be used in advertising or publicity pertaining to distribution
  * of the software without specific, written prior permission.
  * Hewlett-Packard makes no representations about the
  * suitability of this software for any purpose.  It is provided
  * "as is" without express or implied warranty.
  *
  *
  *     Modification History:
  *
*/
/*
        int_stats.c : Get network statistics from the dlpi device
        On 10.20 only get a mib_ifEntry, on 11.0 this is followed
        by an mib_Dot3StatsEntry. Accessing the latter structure
        is left as an exercise for the reader, the struct is defined
        in /usr/include/sys/mib.h along with mib_ifEntry.

        Author: Jon Dewis, SPP 3/26/98
        (With ackn to 'lanadmin' source code and streams class notes)

        24-June-1998            Add mib3 stats (AJD) when compiling
                                for 11.0 ensure HP_UX11 macro defined
                                e.g. cc -o int_stats -DHP_UX11 int_stats.c
        16-Dec-1999             Change to obviate need for lanscan to get
                                nmid - thus assumes any old nmid will do
                                and just iterates thru the 1st 100 possibilities
                                (MAX_NMID). Works on 10.20 and 11.0
        Dec 2003                
        Modified by Roshan Sequeira roshan.sequeira@hp.com, to get network 
        statistics for the nload port to HP-UX. Original code available at 
        http://h21007.www2.hp.com/dspp/tech/tech_TechDocumentDetailPage_IDX/1,1701,2599,00.html
*/

	#ifdef HAVE_HPUX
	#define AREA_SIZE 40000
	#define LONG_AREA_SIZE  (AREA_SIZE / sizeof(u_long))
	#define TRUE 1
	
	static u_long ctrl_area[LONG_AREA_SIZE];	/* for control messages */
	static u_long data_area[LONG_AREA_SIZE];	/* for data messages */
	static u_long ppa_area[LONG_AREA_SIZE]; 	/* for saving ppa area */
	static struct strbuf ctrl_buf = {AREA_SIZE, 0, (char*)ctrl_area};
	static struct strbuf data_buf = {AREA_SIZE, 0, (char*)data_area};
	
	dl_get_statistics_req_t *get_statistics_req =
	                (dl_get_statistics_req_t *) ctrl_area;
    dl_get_statistics_ack_t *get_statistics_ack =
	                (dl_get_statistics_ack_t *) ctrl_area;
	
	dl_hp_ppa_info_t ppa_info, *ppa_info_temp;
	dl_hp_ppa_req_t *ppa_req;
	dl_hp_ppa_ack_t *ppa_ack;
	dl_attach_req_t *attach_req;
	mib_ifEntry     *mib_ptr;
	
	char *ppa_no = NULL;
	unsigned int ppa = 0;
	int fd = 0, flags = 0, ppa_count = 0, count = 0, found = 0;
	
	do
	{
		//Make sure interface name begins with lan
		if ( strspn(m_dev, "lan") < 3 )
			break;
		
		//Get the PPA from the interface name
		ppa_no = (char*)malloc((strlen(m_dev)-3));
		strcpy(ppa_no, (m_dev+3));
		ppa = atoi(ppa_no);
		free(ppa_no);
		
		if ( (fd = open("/dev/dlpi", O_RDWR)) < 0) {
			perror("Open /dev/dlpi");
			break;
		}
		
		ppa_req = (dl_hp_ppa_req_t *)ctrl_area;
		ppa_ack = (dl_hp_ppa_ack_t *)ctrl_area;
		
		ppa_req->dl_primitive = DL_HP_PPA_REQ;
		
		ctrl_buf.len = sizeof(dl_hp_ppa_req_t);
		if (putmsg(fd, &ctrl_buf, 0, 0) < 0) {
			perror("putmsg DL_HP_PPA_REQ");
			break;
	    }
	    flags = 0;
	    ctrl_area[0] = 0;
		
		if (getmsg(fd, &ctrl_buf, &data_buf, &flags) < 0) {
			perror("getmsg DL_HP_PPA_REQ");
			break;
		}
		
		if (ppa_ack->dl_length == 0) {
			fprintf(stderr, "Error: No PPAs available\n");
			break;
		}
		
		//Save all the PPA information.
		memcpy((u_char *)ppa_area, (u_char *)ctrl_area+ppa_ack->dl_offset,
		       ppa_ack->dl_length);
		ppa_count = ppa_ack->dl_count;
		
		for (count = found = 0, ppa_info_temp = (dl_hp_ppa_info_t *)ppa_area;
		     count<ppa_count; count++, ppa_info_temp++) {
			if ( ppa_info_temp->dl_ppa == ppa ) {
				found = TRUE;
				break;
			}
		}
		if (!found) {
			fprintf(stderr, "Error: PPA %d not found\n", ppa);
			break;
		}
		
		attach_req = (dl_attach_req_t *)ctrl_area;
		attach_req->dl_primitive = DL_ATTACH_REQ;
		attach_req->dl_ppa = ppa;
		ctrl_buf.len = sizeof(dl_attach_req_t);
		if (putmsg(fd, &ctrl_buf, 0, 0) < 0) {
			perror("putmsg DL_ATTACH_REQ");
			break;
	    }
		
		ctrl_area[0] = 0;
		if (getmsg(fd, &ctrl_buf, &data_buf, &flags) < 0) {
			perror("getmsg DL_ATTACH_REQ");
			break;
		}
		memcpy(&ppa_info, ppa_info_temp, sizeof(ppa_info));
		
		get_statistics_req->dl_primitive = DL_GET_STATISTICS_REQ;
		ctrl_buf.len = sizeof(dl_get_statistics_req_t);
		flags = 0;
		
		if ( putmsg(fd, &ctrl_buf, NULL, 0) < 0) {
			perror("putmsg DL_GET_STATISTICS_REQ");
			break;
		}
		
		if (getmsg(fd, &ctrl_buf, NULL, &flags) < 0) {
			perror("getmsg DL_GET_STATISTICS_REQ");
			break;
		}
		if (get_statistics_ack->dl_primitive != DL_GET_STATISTICS_ACK)
			fprintf(stderr, "Error: Wrong primitive\n");
		
		mib_ptr = (mib_ifEntry *)((u_char *) ctrl_area + get_statistics_ack->dl_stat_offset);
		
		close(fd);
		
		total_new[0] = mib_ptr->ifInOctets;
		total_new[1] = mib_ptr->ifOutOctets;
		
		if( total_new[0] > m_total[0] )
			in = total_new[0] - m_total[0];
		m_total[0] = total_new[0];
		
		if( total_new[1] > m_total[1] )
			out = total_new[1] - m_total[1];
		m_total[1] = total_new[1];
		
		m_dev_exists = true;
	} while( 0 );
	
#endif
// === End HP-UX specific network data reading code ===
	
	if( ! m_dev_exists )
		m_total[0] = m_total[1] = 0;
	
	return;
}

int Proc::getElapsedTime()
{
	return m_elapsed_time;
}

long long Proc::totalIn()
{
	return m_total[0];
}

long long Proc::totalOut()
{
	return m_total[1];
}

