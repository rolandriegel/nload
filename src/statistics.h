/***************************************************************************
                               statistics.h
                             -------------------
    begin                : Fri Nov 16 2007
    copyright            : (C) 2007 - 2008 by Roland Riegel
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

#ifndef STATISTICS_H
#define STATISTICS_H

#include "dataframe.h"

#include <string>
#include <vector>

class Statistics
{
    public:
        enum dataUnit
        {
            humanReadableBit = -2,
            humanReadableByte = -1,
            bit = 0,
            byte = 1,
            kiloBit = 2,
            kiloByte = 3,
            megaBit = 4,
            megaByte = 5,
            gigaBit = 6,
            gigaByte = 7
        };
        
        Statistics() {}
        ~Statistics() {}

        static float getUnitFactor(dataUnit unit, long long value);
        static std::string getUnitString(dataUnit unit, long long value);
        
        void insertDataFrame(const DataFrame& dataFrame);
        void reset();

        bool isValid() const { return !m_dataFrames.empty(); }

        long long getDataInTotal() const { return m_dataFrames.empty() ? 0 : m_dataFrames.back().getTotalDataIn(); }
        long long getDataOutTotal() const { return m_dataFrames.empty() ? 0 : m_dataFrames.back().getTotalDataOut(); }
        long long getPacketsInTotal() const { return m_dataFrames.empty() ? 0 : m_dataFrames.back().getTotalPacketsIn(); }
        long long getPacketsOutTotal() const { return m_dataFrames.empty() ? 0 : m_dataFrames.back().getTotalPacketsOut(); }
        long long getErrorsInTotal() const { return m_dataFrames.empty() ? 0 : m_dataFrames.back().getTotalErrorsIn(); }
        long long getErrorsOutTotal() const { return m_dataFrames.empty() ? 0 : m_dataFrames.back().getTotalErrorsOut(); }
        long long getDropsInTotal() const { return m_dataFrames.empty() ? 0 : m_dataFrames.back().getTotalDropsIn(); }
        long long getDropsOutTotal() const { return m_dataFrames.empty() ? 0 : m_dataFrames.back().getTotalDropsOut(); }

        long long getDataInPerSecond() const { return m_second.getTotalDataIn(); }
        long long getDataOutPerSecond() const { return m_second.getTotalDataOut(); }
        long long getPacketsInPerSecond() const { return m_second.getTotalPacketsIn(); }
        long long getPacketsOutPerSecond() const { return m_second.getTotalPacketsOut(); }
        long long getErrorsInPerSecond() const { return m_second.getTotalErrorsIn(); }
        long long getErrorsOutPerSecond() const { return m_second.getTotalErrorsOut(); }
        long long getDropsInPerSecond() const { return m_second.getTotalDropsIn(); }
        long long getDropsOutPerSecond() const { return m_second.getTotalDropsOut(); }

        long long getDataInAverage() const { return m_average.getTotalDataIn(); }
        long long getDataOutAverage() const { return m_average.getTotalDataOut(); }
        long long getPacketsInAverage() const { return m_average.getTotalPacketsIn(); }
        long long getPacketsOutAverage() const { return m_average.getTotalPacketsOut(); }
        long long getErrorsInAverage() const { return m_average.getTotalErrorsIn(); }
        long long getErrorsOutAverage() const { return m_average.getTotalErrorsOut(); }
        long long getDropsInAverage() const { return m_average.getTotalDropsIn(); }
        long long getDropsOutAverage() const { return m_average.getTotalDropsOut(); }

        long long getDataInMin() const { return m_min.getTotalDataIn(); }
        long long getDataOutMin() const { return m_min.getTotalDataOut(); }
        long long getPacketsInMin() const { return m_min.getTotalPacketsIn(); }
        long long getPacketsOutMin() const { return m_min.getTotalPacketsOut(); }
        long long getErrorsInMin() const { return m_min.getTotalErrorsIn(); }
        long long getErrorsOutMin() const { return m_min.getTotalErrorsOut(); }
        long long getDropsInMin() const { return m_min.getTotalDropsIn(); }
        long long getDropsOutMin() const { return m_min.getTotalDropsOut(); }

        long long getDataInMax() const { return m_max.getTotalDataIn(); }
        long long getDataOutMax() const { return m_max.getTotalDataOut(); }
        long long getPacketsInMax() const { return m_max.getTotalPacketsIn(); }
        long long getPacketsOutMax() const { return m_max.getTotalPacketsOut(); }
        long long getErrorsInMax() const { return m_max.getTotalErrorsIn(); }
        long long getErrorsOutMax() const { return m_max.getTotalErrorsOut(); }
        long long getDropsInMax() const { return m_max.getTotalDropsIn(); }
        long long getDropsOutMax() const { return m_max.getTotalDropsOut(); }

    private:
        void calculateAverage(const DataFrame& dataFrameFrom, const DataFrame& dataFrameTo, DataFrame& result);
        void calculateMinMax(const DataFrame& dataFrame, DataFrame& min, DataFrame& max);
        unsigned int getAverageWindow();
        unsigned int getSecondWindow();

        DataFrame m_second;
        DataFrame m_average;
        DataFrame m_min;
        DataFrame m_max;

        std::vector<DataFrame> m_dataFrames;
};

#endif

