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
	buf = (char *) calloc(512, sizeof(char));
	tag = (char *) calloc(128, sizeof(char));
	dev = (char *) calloc(128, sizeof(char));

	cS = (float *) calloc(16, sizeof(float));
	S = (float *) calloc(2, sizeof(float));
	ret = (float *) calloc(2, sizeof(float));

	S[0] = 0;
	S[1] = 0;
	
	was_time.tv_sec = was_time.tv_usec = 0;
	is_time.tv_sec = is_time.tv_usec = 0;
	elapsed_time = 0;
	dev_exists = 0;
	
}

Proc::~Proc()
{
	free(buf);
	free(tag);
	free(dev);
	free(cS);
	free(S);
	free(ret);
}

void Proc::setProcDev(char *new_procdev)
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
	gettimeofday(&is_time, NULL);
	elapsed_time = labs(is_time.tv_sec - was_time.tv_sec) * 1000 + (float) labs(is_time.tv_usec - was_time.tv_usec) / 1000;
	was_time = is_time;
		
	ret[0] = 0;
	ret[1] = 0;

	if((fd = fopen("/proc/net/dev", "r")) == NULL) return ret;

	fgets(buf, 512, fd);
	fgets(buf, 512, fd);

	while(!feof(fd)) {

		fgets(buf, 512, fd);

		memset(tag, 0, 32);

		tmp = buf;
		tmp2 = tag;

		while(*tmp == ' ') tmp++;
		while((*tmp2++ = *tmp++) != ':');

		*--tmp2 = '\0';

		sscanf(tmp, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
		&cS[0], &cS[1], &cS[2], &cS[3], &cS[4], &cS[5], &cS[6], &cS[7], &cS[8], &cS[9], &cS[10], &cS[11], &cS[12], &cS[13], &cS[14], &cS[15]);

		if(!strcmp(dev, tag)) {

			if(cS[0] > S[0]) {
				ret[0] = cS[0] - S[0];
				S[0] = cS[0];
			}

			if(cS[8] > S[1]) {
				ret[1] = cS[8] - S[1];
				S[1] = cS[8];
			}

			if(cS[0] < S[0] | cS[8] < S[1]) {
				S[0] = cS[0];
				S[1] = cS[8];
			}

			fclose(fd);
			
			dev_exists = true;
			
			return(ret);
		}

	}

	S[0] = 0;
	S[1] = 0;

	fclose(fd);
	
	dev_exists = false;
	
	return(ret);
}

float Proc::getElapsedTime()
{
	return(elapsed_time);
}

float Proc::totalIn(void)
{
	return(S[0]);
}

float Proc::totalOut(void)
{
	return(S[1]);
}
