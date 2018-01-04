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

SettingFilter::FilterResult SettingFilterDefault::filterWrite(string& valueNew)
{
    if(valueNew.empty())
        valueNew = m_default;

    return ResultSuccessContinue;
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

SettingFilter::FilterResult SettingFilterExclusive::filterWrite(string& valueNew)
{
    substituteExclusive(valueNew);
    return ResultSuccessContinue;
}

SettingFilter::FilterResult SettingFilterExclusive::filterRead(string& value)
{
    substituteExclusive(value);
    return ResultSuccessContinue;
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

SettingFilter::FilterResult SettingFilterMap::filterWrite(string& valueNew)
{
    for(map<string, string>::const_iterator itMapping = m_filterMap.begin(); itMapping != m_filterMap.end(); ++itMapping)
    {
        if(itMapping->second == valueNew)
        {
            valueNew = itMapping->first;
            return ResultSuccessContinue;
        }
    }

    return ResultFailure;
}

SettingFilter::FilterResult SettingFilterMap::filterRead(string& value)
{
    map<string, string>::const_iterator itMapping = m_filterMap.find(value);
    if(itMapping != m_filterMap.end())
    {
        value = itMapping->second;
        return ResultSuccessContinue;
    }

    return ResultFailure;
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

SettingFilter::FilterResult SettingFilterMin::filterWrite(string& valueNew)
{
    if(fromString<int>(valueNew) < m_min)
        valueNew = toString(m_min);

    return ResultSuccessContinue;
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

SettingFilter::FilterResult SettingFilterMax::filterWrite(string& valueNew)
{
    if(fromString<int>(valueNew) > m_max)
        valueNew = toString(m_max);

    return ResultSuccessContinue;
}

