/***************************************************************************
                             devreaderfactory.h
                             -------------------
    begin                : Fri Nov 16 2007
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

#ifndef DEVREADERFACTORY_H
#define DEVREADERFACTORY_H

#include <string>
#include <map>

class DevReader;

class DevReaderFactory
{
    public:
        DevReaderFactory();
        ~DevReaderFactory();

        static int findAllDevices();
        static int getDeviceCount();
        
        static const std::map<std::string, DevReader*>& getAllDevReaders();

    private:
        static DevReader* createDevReader(const std::string& deviceName);
        
        static std::map<std::string, DevReader*> m_devReaders;
        static DevReaderFactory m_instance;
};

#endif

