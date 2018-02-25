/*
 * nload
 * real time monitor for network traffic
 * Copyright (C) 2001 - 2018 by Roland Riegel <feedback@roland-riegel.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <sstream>
#include <string>
#include <vector>

std::string trim(const std::string& s);
std::vector<std::string> split(const std::string& s, const std::string& separators);
std::vector<std::string> splitQuoted(const std::string& s, const std::string& separators, const std::string& quotes = "\"");

template<class T>
std::string toString(const T& t)
{
    std::ostringstream stream;
    stream << t;

    return stream.str();
}

template<class T>
T fromString(const std::string& s)
{
    std::istringstream stream(s);
    T t = T();
    stream >> t;

    return t;
}

template<class T>
bool fromString(const std::string& s, T& t)
{
    std::istringstream stream(s);
    stream >> t;

    return stream.eof() && !stream.fail();
}

struct sizeLess
{
    template<class T> bool operator()(const T& a, const T& b) const
    {
        return a.size() < b.size();
    }
};

#endif

