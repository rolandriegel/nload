/***************************************************************************
                               settingfilter.h
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

#ifndef SETTINGFILTER_H
#define SETTINGFILTER_H

#include <map>
#include <string>

class SettingFilter
{
    public:
        enum FilterResult
        {
            ResultSuccessContinue,
            ResultSuccessQuit,
            ResultFailure
        };

        virtual ~SettingFilter() {}

        virtual std::string getId() const = 0;

        virtual FilterResult filterWrite(std::string& valueNew)
        {
            return ResultSuccessContinue;
        }

        virtual FilterResult filterRead(std::string& value)
        {
            return ResultSuccessContinue;
        }
};

class SettingFilterDefault : public SettingFilter
{
    public:
        SettingFilterDefault(const std::string& def);
        ~SettingFilterDefault();

        std::string getId() const;

        void setDefault(const std::string& def);
        const std::string& getDefault() const;

        FilterResult filterWrite(std::string& valueNew);
        
    private:
        std::string m_default;
};

class SettingFilterExclusive : public SettingFilter
{
    public:
        SettingFilterExclusive(const std::string& exclusive);
        ~SettingFilterExclusive();

        std::string getId() const;

        void setExclusive(const std::string& exclusive);
        const std::string& getExclusive() const;

        FilterResult filterWrite(std::string& valueNew);
        FilterResult filterRead(std::string& value);
        
    private:
        void substituteExclusive(std::string& value);

        std::string m_exclusive;
};

class SettingFilterMap : public SettingFilter
{
    public:
        SettingFilterMap(const std::map<std::string, std::string>& filterMap);
        ~SettingFilterMap();

        std::string getId() const;

        void setMap(const std::map<std::string, std::string>& filterMap);
        const std::map<std::string, std::string>& getMap() const;

        FilterResult filterWrite(std::string& valueNew);
        FilterResult filterRead(std::string& value);
        
    private:
        std::map<std::string, std::string> m_filterMap;
};

class SettingFilterMin : public SettingFilter
{
    public:
        SettingFilterMin(int min);
        ~SettingFilterMin();

        std::string getId() const;

        void setMin(int min);
        int getMin() const;

        FilterResult filterWrite(std::string& valueNew);
        
    private:
        int m_min;
};

class SettingFilterMax : public SettingFilter
{
    public:
        SettingFilterMax(int max);
        ~SettingFilterMax();

        std::string getId() const;

        void setMax(int max);
        int getMax() const;

        FilterResult filterWrite(std::string& valueNew);
        
    private:
        int m_max;
};

#endif

