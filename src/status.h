/***************************************************************************
                                  status.h
                             -------------------
    begin                : Sat Sep 29 2001
    copyright            : (C) 2001 - 2007 by Roland Riegel
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

#ifndef STATUS_H
#define STATUS_H

#include <string>
#include <list>

class Window;

#define STANDARD_AVERAGE_WINDOW 300

class Status
{
    public:
        Status();
        ~Status();
        
        enum status_format
        {
            human_readable_bit = -2,
            human_readable_byte = -1,
            bit = 0,
            byte = 1,
            kilobit = 2,
            kilobyte = 3,
            megabit = 4,
            megabyte = 5,
            gigabit = 6,
            gigabyte = 7
        };
        
        void update(unsigned long, long long);
        void print(Window&, int, int, status_format traff_format, status_format data_format);
        void resetTrafficData();

    private:
        void minMax(unsigned long);
        
        void updateAverage(unsigned long);
        unsigned long calcAverage();
        
        void getUnit(status_format format, long long value, std::string& description, float& factor);
        
        unsigned long m_min, m_max, m_cur;
        long long m_total;
        
        std::list<unsigned long> m_averageValues;
};

#endif

