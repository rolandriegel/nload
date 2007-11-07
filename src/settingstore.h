/***************************************************************************
                               settingstore.h
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

#ifndef SETTINGSTORE_H
#define SETTINGSTORE_H

#include <map>
#include <string>

class Setting;

class SettingStore
{
    public:
        static Setting& get(const std::string& key);
        static void add(const Setting& setting);
        static void remove(const std::string& key);
        static bool exists(const std::string& key);

        static std::map<std::string, Setting>& getAll();

    private:
        static std::map<std::string, Setting> m_settings;
};

#endif

