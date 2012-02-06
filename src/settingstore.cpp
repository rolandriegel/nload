/***************************************************************************
                              settingstore.cpp
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

#include "setting.h"
#include "settingstore.h"

#include <fstream>

using namespace std;

map<string, Setting> SettingStore::m_settings;

Setting& SettingStore::get(const string& key)
{
    return m_settings[key];
}

void SettingStore::add(const Setting& setting)
{
    m_settings[setting.getId()] = setting;
}

void SettingStore::remove(const string& key)
{
    m_settings.erase(key);
}

bool SettingStore::exists(const string& key)
{
    return m_settings.find(key) != m_settings.end();
}

map<std::string, Setting>& SettingStore::getAll()
{
    return m_settings;
}

bool SettingStore::readFromFile(const std::string& file)
{
    if(file.empty())
        return false;

    // open file
    ifstream fin(file.c_str());
    if(!fin.is_open())
        return false;

    // parse file
    while(fin.good())
    {
        Setting setting;

        fin >> setting;
        if(setting.getId().empty())
            break;

        if(exists(setting.getId()))
            get(setting.getId()).setThroughFilter(setting.getValue());
        else
            add(setting);
    }

    return !fin.fail();
}

bool SettingStore::writeToFile(const std::string& file)
{
    if(file.empty())
        return false;

    // open file
    ofstream fout(file.c_str());
    if(!fout.is_open())
        return false;

    fout << "Version=\"1\"\n";

    // output settings
    for(map<string, Setting>::const_iterator itSetting = m_settings.begin(); itSetting != m_settings.end(); ++itSetting)
        fout << itSetting->second;

    return fout.good();
}

