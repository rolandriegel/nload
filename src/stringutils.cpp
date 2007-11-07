/***************************************************************************
                               stringutils.cpp
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

#include "stringutils.h"

using namespace std;

std::string trim(const std::string& s)
{
    // find end of whitespace at the beginning
    string::size_type posBegin = s.find_first_not_of(" \011\012\015");
    if(posBegin == string::npos)
        return string();

    // find beginning of whitespace at the end
    string::size_type posEnd = s.find_last_not_of(" \011\012\015");

    return s.substr(posBegin, posEnd - posBegin + 1);
}

vector<string> split(const string& s, const string& separators)
{
    vector<string> words;
    if(s.empty())
        return words;

    string::size_type pos = s.find_first_of(separators);
    string::size_type posOld = 0;
    while(pos != string::npos)
    {
        words.push_back(s.substr(posOld, pos - posOld));
        posOld = pos + 1;

        pos = s.find_first_of(separators, posOld);
    }

    words.push_back(s.substr(posOld));

    return words;
}

vector<string> splitQuoted(const string& s, const string& separators, const string& quotes)
{
    vector<string> words;

    if(s.empty())
        return words;

    bool quoted = false;
    string::size_type pos = s.find_first_of(quotes);
    string::size_type posOld = 0;
    while(pos != string::npos)
    {
        if(pos != posOld)
        {
            if(quoted)
            {
                words.push_back(s.substr(posOld, pos - posOld));
            }
            else
            {
                vector<string> unquoted = split(s.substr(posOld, pos - posOld), separators);
                words.insert(words.end(), unquoted.begin(), unquoted.end());
            }
        }

        posOld = pos + 1;
        quoted = !quoted;

        if(posOld < s.length())
            pos = s.find_first_of(quotes, posOld);
        else
            pos = string::npos;
    }

    if(posOld < s.length())
    {
        if(quoted)
        {
            words.push_back(s.substr(posOld));
        }
        else
        {
            vector<string> unquoted = split(s.substr(posOld, pos - posOld), separators);
            words.insert(words.end(), unquoted.begin(), unquoted.end());
        }
    }

    return words;
}

