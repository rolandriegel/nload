/***************************************************************************
                              traffic_window.h
                             -------------------
    begin                : Thu Jul 04 2002
    copyright            : (C) 2002 - 2007 by Roland Riegel
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

#ifndef TRAFFIC_WINDOW_H
#define TRAFFIC_WINDOW_H

#include "window.h"
#include <vector>

class Dev;

class TrafficWindow : public Window
{
    public:
        
        TrafficWindow();
        ~TrafficWindow();
        
        void processKey(int key);
        void print();
        
        std::vector<Dev *>& devices();
        void setDevices(std::vector<Dev *>&);
        
    private:
        
        bool showMultipleDevices();
        
        std::vector<Dev *> m_devs;
        
        int m_cur_dev;
};

#endif
