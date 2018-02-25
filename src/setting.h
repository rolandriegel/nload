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

#ifndef SETTING_H
#define SETTING_H

#include "stringutils.h"

#include <iosfwd>
#include <list>
#include <string>

class SettingFilter;

class Setting
{
    public:
        Setting(const std::string& id = "", const std::string& description = "")
            : m_id(id), m_description(description), m_value() {}
        template<class T>
        Setting(const std::string& id, const std::string& description, const T& value)
            : m_id(id), m_description(description), m_value(toString(value)) {}
        ~Setting();

        const std::string& getId() const { return m_id; }
        const std::string& getDescription() const { return m_description; }
        const std::string& getValue() const { return m_value; }

        void setId(const std::string& id) { m_id = id; }
        void setDescription(const std::string& description) { m_description = description; }

        void pushFilter(SettingFilter* filter);
        void popFilter();
        SettingFilter* findFilterWithId(const std::string& id);
        const SettingFilter* findFilterWithId(const std::string& id) const;

        std::string getThroughFilter() const;
        bool setThroughFilter(const std::string& value);

        template<class T>
        operator T() { return fromString<T>(m_value); }

        template<class T>
        Setting& operator=(const T& t) { m_value = toString(t); return *this; }

        template<class T>
        bool operator==(const T& t) const { return toString(t) == m_value; }
        template<class T>
        bool operator!=(const T& t) const { return toString(t) != m_value; }
        template<class T>
        bool operator<(const T& t) const { return fromString<T>(m_value) < t; }
        template<class T>
        bool operator>(const T& t) const { return fromString<T>(m_value) > t; }
        template<class T>
        bool operator<=(const T& t) const { return fromString<T>(m_value) <= t; }
        template<class T>
        bool operator>=(const T& t) const { return fromString<T>(m_value) >= t; }

        bool operator==(const Setting& s) const { return s.m_value == m_value; }
        bool operator!=(const Setting& s) const { return s.m_value != m_value; }

    private:
        std::string m_id;
        std::string m_description;
        std::string m_value;

        std::list<SettingFilter*> m_filters;
};

std::istream& operator>>(std::istream& in, Setting& setting);
std::ostream& operator<<(std::ostream& out, const Setting& setting);

#endif

