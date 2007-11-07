/***************************************************************************
                                 status.cpp
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

#include "setting.h"
#include "settingstore.h"
#include "status.h"
#include "window.h"

using namespace std;

Status::Status()
{
    m_min = m_max = m_cur = m_total = 0;
}

Status::~Status()
{
}

// new traffic measurement has been made => update statistics
void Status::update(unsigned long new_value, long long new_total)
{
    
    m_cur = new_value;
    minMax(m_cur); // calculate new min/max traffic values
    updateAverage(m_cur); // calculate new average
    
    /*
     * set new total transfered data
     * the following is a workaround for the value limitations of the
     * unsigned int variable used in the kernel to produce
     * the /proc/net/dev file
     * (the total bytes value reaches 4GB and then switches to 0)
     */
    if(new_total >= UINT_MAX)
        m_total = new_total;
    else if(new_total < (m_total % UINT_MAX))
        m_total = ((m_total / UINT_MAX) + 1) * UINT_MAX + new_total;
    else
        m_total = (m_total / UINT_MAX) * UINT_MAX + new_total;
    
}

// print statistics
void Status::print(Window& window, int x, int y, status_format traff_format, status_format data_format)
{
    unsigned long value;
    char fText[100] = "";
    string unit_string;
    float unit_factor;
    
    // print current traffic
    window.setXY(x, y);
    getUnit(traff_format, m_cur, unit_string, unit_factor);
    sprintf(fText, "Curr: %.2f %s/s\n", m_cur / unit_factor, unit_string.c_str());
    window.print(fText);
    
    // print average traffic
    window.setX(x);
    value = calcAverage();
    getUnit(traff_format, value, unit_string, unit_factor);
    sprintf(fText, "Avg: %.2f %s/s\n", value / unit_factor, unit_string.c_str());
    window.print(fText);
    
    // print min traffic since nload start
    window.setX(x);
    getUnit(traff_format, m_min, unit_string, unit_factor);
    sprintf(fText, "Min: %.2f %s/s\n", m_min / unit_factor, unit_string.c_str());
    window.print(fText);
    
    // print max traffic since nload start
    window.setX(x);
    getUnit(traff_format, m_max, unit_string, unit_factor);
    sprintf(fText, "Max: %.2f %s/s\n", m_max / unit_factor, unit_string.c_str());
    window.print(fText);
    
    // print total traffic since last system reboot
    window.setX(x);
    getUnit(data_format, m_total, unit_string, unit_factor);
    sprintf(fText, "Ttl: %.2f %s\n", m_total / unit_factor, unit_string.c_str());
    window.print(fText);
}

// reset all displayed values to zero
void Status::resetTrafficData()
{
    m_cur = m_min = m_max = m_total = 0;
    m_average_values.clear();
}

// determine the matching unit string, e.g. "kBit" for status_format::kilobit, and
// the matching conversion factor between Byte and e.g. status_format::kilobit
void Status::getUnit(status_format format, long long value, string& description, float& factor)
{
    factor = (float) 1 / (format % 2 == 0 ? 8 : 1);
    description = format % 2 == 0 ? "Bit" : "Byte";
    
    switch(format)
    {
        case human_readable_bit:
        case human_readable_byte:
            factor *= 1024 * 1024 * 1024;
            for(int i = 3; i >= 0; i--)
            {
                if(value * (format % 2 == 0 ? 8 : 1) >= factor)
                    switch(i)
                    {
                        case 3:
                            description = 'G' + description;
                            return;
                        case 2:
                            description = 'M' + description;
                            return;
                        case 1:
                            description = 'k' + description;
                            return;
                        default:
                            return;
                    }
                factor /= 1024;
            }
            return;
        case bit:
        case byte:
            return;
        case kilobit:
        case kilobyte:
            factor *= 1024;
            description = 'k' + description;
            return;
        case megabit:
        case megabyte:
            factor *= 1024 * 1024;
            description = 'M' + description;
            return;
        case gigabit:
        case gigabyte:
            factor *= 1024 * 1024 * 1024;
            description = 'G' + description;
            return;
        default: // should never be executed
            return;
    }

}

// calculate min and max traffic values
void Status::minMax(unsigned long new_value)
{
    
    // if this is the first time call, set min/max to current value
    if(m_min == 0 && m_max == 0)
    {
        m_min = new_value;
        m_max = new_value;
        return;
    }
    
    m_min = new_value < m_min ? new_value : m_min;
    
    m_max = new_value > m_max ? new_value : m_max;
    
}

// put new value into average calculation
void Status::updateAverage(unsigned long new_value)
{
    unsigned int averageSmoothness = SettingStore::get("average_smoothness");
    
    /*
     * average calculation is not very good at the moment as it is dependent
     * from the display refresh interval.
     * could need some help here.
     */
    
    m_average_values.push_front(new_value);
    
    // limit value count dependent of the average smoothness
    // ranges between 1 * 45 and 9 * 45 single values
    while(m_average_values.size() > averageSmoothness * 45)
    {
        m_average_values.pop_back();
    }
    
}

// calculate current average
unsigned long Status::calcAverage()
{
    if(m_average_values.size() == 0) return 0;

    unsigned long sum = 0;
    for(list<unsigned long>::const_iterator i = m_average_values.begin(); i != m_average_values.end(); i++)
    {
        sum += (*i);
    }
    
    return sum / m_average_values.size();
}

int Status::averageSmoothness()
{
    int avg_smooth = SettingStore::get("average_smoothness");
    return avg_smooth > 9 ? 9 : (avg_smooth < 1 ? 1 : avg_smooth);
}
