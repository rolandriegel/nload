/*
 * nload
 * real time monitor for network traffic
 * Copyright (C) 2001 - 2012 by Roland Riegel <feedback@roland-riegel.de>
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

        static bool readFromFile(const std::string& file);
        static bool writeToFile(const std::string& file);

    private:
        static std::map<std::string, Setting> m_settings;
};

#endif

