/***************************************************************************
                              settingstore.cpp
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
#include "settingstore.h"

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

