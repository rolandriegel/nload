/***************************************************************************
                                 setting.cpp
                             -------------------
    begin                : Tue Nov 06 2007
    copyright            : (C) 2007 by Roland Riegel
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
#include "stringutils.h"

#include <istream>
#include <ostream>
#include <sstream>

using namespace std;

string Setting::mapToString() const
{
    map<string, string>::const_iterator itMapping = m_mapping.find(m_value);

    if(itMapping == m_mapping.end())
        return m_value;

    return itMapping->second;
}

void Setting::assignThroughMap(const string& value)
{
    for(map<string, string>::const_iterator itMapping = m_mapping.begin(); itMapping != m_mapping.end(); ++itMapping)
    {
        if(itMapping->second == value)
        {
            m_value = itMapping->first;
            break;
        }
    }
}

istream& Setting::operator>>(std::istream& in)
{
    while(!in.eof())
    {
        // extract line from stream
        string line;
        getline(in, line);

        if(in.fail())
            return in;

        // strip whitespace
        line = trim(line);

        // skip empty lines
        if(line.empty())
            continue;

        // skip comments
        if(line[0] == '#')
            continue;

        // split line into id and value
        vector<string> words = splitQuoted(line, "=");

        if(words.size() < 2)
        {
            in.setstate(ios_base::failbit);
            return in;
        }

        m_id = words[0];
        m_value = words[1];

        break;
    }

    return in;
}

ostream& Setting::operator<<(std::ostream& out) const
{
    out << m_id << "=\"" << m_value << "\"" << endl;
    return out;
}


