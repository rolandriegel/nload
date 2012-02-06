/***************************************************************************
                              settingfilter.cpp
                             -------------------
    begin                : Wed Nov 28 2007
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

#include "settingfilter.h"
#include "stringutils.h"

using namespace std;

SettingFilterDefault::SettingFilterDefault(const string& def)
    : m_default(def)
{
}

SettingFilterDefault::~SettingFilterDefault()
{
}

string SettingFilterDefault::getId() const
{
    return "default";
}

void SettingFilterDefault::setDefault(const string& def)
{
    m_default = def;
}

const string& SettingFilterDefault::getDefault() const
{
    return m_default;
}

bool SettingFilterDefault::filterWrite(string& valueNew)
{
    if(valueNew.empty())
        valueNew = m_default;

    return true;
}

void SettingFilterDefault::filterRead(string& value)
{
}

SettingFilterExclusive::SettingFilterExclusive(const string& exclusive)
    : m_exclusive(exclusive)
{
}

SettingFilterExclusive::~SettingFilterExclusive()
{
}

string SettingFilterExclusive::getId() const
{
    return "exclusive";
}

void SettingFilterExclusive::setExclusive(const string& exclusive)
{
    m_exclusive = exclusive;
}

const string& SettingFilterExclusive::getExclusive() const
{
    return m_exclusive;
}

bool SettingFilterExclusive::filterWrite(string& valueNew)
{
    substituteExclusive(valueNew);
    return true;
}

void SettingFilterExclusive::filterRead(string& value)
{
    substituteExclusive(value);
}

void SettingFilterExclusive::substituteExclusive(string& value)
{
    if(value.find(m_exclusive + " ") == 0 ||
       value.find(string(" ") + m_exclusive) == value.length() - (m_exclusive.length() + 1) ||
       value.find(string(" ") + m_exclusive + " ") != string::npos
      )
        value = m_exclusive;
}

SettingFilterMap::SettingFilterMap(const map<string, string>& filterMap)
    : m_filterMap(filterMap)
{
}

SettingFilterMap::~SettingFilterMap()
{
}

string SettingFilterMap::getId() const
{
    return "map";
}

void SettingFilterMap::setMap(const map<string, string>& filterMap)
{
    m_filterMap = filterMap;
}

const map<string, string>& SettingFilterMap::getMap() const
{
    return m_filterMap;
}

bool SettingFilterMap::filterWrite(string& valueNew)
{
    for(map<string, string>::const_iterator itMapping = m_filterMap.begin(); itMapping != m_filterMap.end(); ++itMapping)
    {
        if(itMapping->second == valueNew)
        {
            valueNew = itMapping->first;
            return true;
        }
    }

    return false;
}

void SettingFilterMap::filterRead(string& value)
{
    if(m_filterMap.empty())
        return;

    map<string, string>::const_iterator itMapping = m_filterMap.find(value);
    if(itMapping != m_filterMap.end())
        value = itMapping->second;
}

SettingFilterMin::SettingFilterMin(int min)
    : m_min(min)
{
}

SettingFilterMin::~SettingFilterMin()
{
}

string SettingFilterMin::getId() const
{
    return "min";
}

void SettingFilterMin::setMin(int min)
{
    m_min = min;
}

int SettingFilterMin::getMin() const
{
    return m_min;
}

bool SettingFilterMin::filterWrite(string& valueNew)
{
    if(fromString<int>(valueNew) < m_min)
        valueNew = toString(m_min);

    return true;
}

void SettingFilterMin::filterRead(string& value)
{
}

SettingFilterMax::SettingFilterMax(int max)
    : m_max(max)
{
}

SettingFilterMax::~SettingFilterMax()
{
}

string SettingFilterMax::getId() const
{
    return "max";
}

void SettingFilterMax::setMax(int max)
{
    m_max = max;
}

int SettingFilterMax::getMax() const
{
    return m_max;
}

bool SettingFilterMax::filterWrite(string& valueNew)
{
    if(fromString<int>(valueNew) > m_max)
        valueNew = toString(m_max);

    return true;
}

void SettingFilterMax::filterRead(string& value)
{
}

