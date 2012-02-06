/***************************************************************************
                                 devreader.h
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

#ifndef DEVREADER_H
#define DEVREADER_H

#include <string>

#include "dataframe.h"

class DevReader
{
    public:
        DevReader(const std::string& deviceName);
        virtual ~DevReader();

        const std::string& getDeviceName() const;
        DataFrame getNewDataFrame();

    protected:
        virtual void readFromDevice(DataFrame& dataFrame) = 0;
        std::string getDeviceIp4Address();
        
        std::string m_deviceName;
};

#endif

