/***************************************************************************
                                 dataframe.h
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

#ifndef DATAFRAME_H
#define DATAFRAME_H

#include <string>

class DataFrame
{
    public:
        DataFrame()
            : m_totalDataIn(0), m_totalDataOut(0), m_totalPacketsIn(0), m_totalPacketsOut(0),
              m_totalErrorsIn(0), m_totalErrorsOut(0), m_totalDropsIn(0), m_totalDropsOut(0),
              m_timeStampSeconds(0), m_timeStampMicroseconds(0), m_valid(false) {}
        ~DataFrame() {}

        bool isValid() const { return m_valid; }
        void setValid(bool valid) { m_valid = valid; }

        const std::string& getDeviceName() const { return m_deviceName; }
        void setDeviceName(const std::string& deviceName) { m_deviceName = deviceName; }

        const std::string& getIpV4() const { return m_ipv4; }
        void setIpV4(const std::string& ipv4) { m_ipv4 = ipv4; }

        unsigned long long getTotalDataIn() const { return m_totalDataIn; }
        void setTotalDataIn(unsigned long long totalDataIn) { m_totalDataIn = totalDataIn; }

        unsigned long long getTotalDataOut() const { return m_totalDataOut; }
        void setTotalDataOut(unsigned long long totalDataOut) { m_totalDataOut = totalDataOut; }

        unsigned long long getTotalPacketsIn() const { return m_totalPacketsIn; }
        void setTotalPacketsIn(unsigned long long totalPacketsIn) { m_totalPacketsIn = totalPacketsIn; }

        unsigned long long getTotalPacketsOut() const { return m_totalPacketsOut; }
        void setTotalPacketsOut(unsigned long long totalPacketsOut) { m_totalPacketsOut = totalPacketsOut; }

        unsigned long long getTotalErrorsIn() const { return m_totalErrorsIn; }
        void setTotalErrorsIn(unsigned long long totalErrorsIn) { m_totalErrorsIn = totalErrorsIn; }

        unsigned long long getTotalErrorsOut() const { return m_totalErrorsOut; }
        void setTotalErrorsOut(unsigned long long totalErrorsOut) { m_totalErrorsOut = totalErrorsOut; }

        unsigned long long getTotalDropsIn() const { return m_totalDropsIn; }
        void setTotalDropsIn(unsigned long long totalDropsIn) { m_totalDropsIn = totalDropsIn; }

        unsigned long long getTotalDropsOut() const { return m_totalDropsOut; }
        void setTotalDropsOut(unsigned long long totalDropsOut) { m_totalDropsOut = totalDropsOut; }

        unsigned long getTimeStampSeconds() const { return m_timeStampSeconds; }
        void setTimeStampSeconds(unsigned long seconds) { m_timeStampSeconds = seconds; }

        unsigned long getTimeStampMicroseconds() const { return m_timeStampMicroseconds; }
        void setTimeStampMicroseconds(unsigned long microseconds) { m_timeStampMicroseconds = microseconds; }

    private:
        std::string m_deviceName;
        std::string m_ipv4;
        unsigned long long m_totalDataIn;
        unsigned long long m_totalDataOut;
        unsigned long long m_totalPacketsIn;
        unsigned long long m_totalPacketsOut;
        unsigned long long m_totalErrorsIn;
        unsigned long long m_totalErrorsOut;
        unsigned long long m_totalDropsIn;
        unsigned long long m_totalDropsOut;
        unsigned long m_timeStampSeconds;
        unsigned long m_timeStampMicroseconds;
        bool m_valid;
};

#endif

