/***************************************************************************
                                stringutils.h
                             -------------------
    begin                : Tue Nov 06 2007
    copyright            : (C) 2007 - 2012 by Roland Riegel
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

#endif

