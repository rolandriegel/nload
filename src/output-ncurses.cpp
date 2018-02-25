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

#include "output-ncurses.h"

#include "appcontrol.h"
#include "device.h"
#include "deviceview.h"
#include "opt_window.h"
#include "screen.h"
#include "setting.h"
#include "settingfilter.h"
#include "settingstore.h"
#include "traffic_window.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <ctype.h>
#include <time.h>
#include <curses.h>
#include <signal.h>
#include <string.h>
#undef clear
#undef erase
#undef refresh

using namespace std;

OutputNcurses::OutputNcurses(AppControl* appControl, vector<Device*> devices)
    : Output(), m_appControl(appControl), m_devices(devices)
{
    for(vector<Device*>::const_iterator itDevice = m_devices.begin(); itDevice != m_devices.end(); ++itDevice)
    {
        m_deviceViews.push_back(new DeviceView(*itDevice));
    }

    m_mainWindow = new TrafficWindow();
    m_optWindow = new OptWindow();

    init();
}

OutputNcurses::~OutputNcurses()
{
    finish();

    delete m_mainWindow;
    delete m_optWindow;

    m_mainWindow = 0;
    m_optWindow = 0;

    for(vector<DeviceView*>::const_iterator itDeviceView = m_deviceViews.begin(); itDeviceView != m_deviceViews.end(); ++itDeviceView)
    {
        delete *itDeviceView;
    }
    m_deviceViews.clear();
}

void OutputNcurses::process()
{
    // process keyboard
    int key;
    while((key = getch()) != ERR)
    {
        if(m_optWindow->isVisible())
        {
            if(key != KEY_F(2))
            {
                m_optWindow->processKey(key);
            }
            else
            {
                showOptions(false);
                refresh(); // update the screen
            }
        }
        else
        {
            switch(key)
            {
                case KEY_F(2):
                    showOptions(true);
                    refresh(); // update the screen
                    break;
                case KEY_F(5):
                    m_appControl->saveSettings();
                    break;
                case KEY_F(6):
                    m_appControl->loadSettings();
                    break;
                case 'q':
                case 'Q':
                    m_appControl->quit();
                    break;
                default:
                    m_mainWindow->processKey(key);
                    break;
            }
        }
    }
}

bool OutputNcurses::processSignal(int signal)
{
    if(signal == SIGWINCH)
    {
        bool optWindowWasVisible = m_optWindow->isVisible();

        finish();   
        init();
        
        showOptions(optWindowWasVisible);
        return true;
    }

    return false;
}

void OutputNcurses::output()
{
    updateGraphs();
    refresh();
}

void OutputNcurses::updateGraphs()
{
    // update all device views, even if they will not be displayed
    for(vector<DeviceView*>::const_iterator itDeviceView = m_deviceViews.begin(); itDeviceView != m_deviceViews.end(); ++itDeviceView)
    {
        (*itDeviceView)->update();
    }
}

void OutputNcurses::refresh()
{
    // collect devices requested for display
    vector<string> devicesRequested = split(trim(SettingStore::get("Devices").getThroughFilter()), " ");
    vector<DeviceView*> devicesToShow;

    if(!devicesRequested.empty() && devicesRequested.front() != "all")
    {
        vector<Device*>::size_type deviceCount = m_devices.size();

        // check if requested devices are available
        for(vector<string>::const_iterator itRequested = devicesRequested.begin(); itRequested != devicesRequested.end(); ++itRequested)
        {
            for(vector<Device*>::size_type deviceIndex = 0; deviceIndex < deviceCount; ++deviceIndex)
            {
                if(m_devices[deviceIndex]->getName() == *itRequested)
                {
                    devicesToShow.push_back(m_deviceViews[deviceIndex]);
                    break;
                }
            }
        }
    }

    if(devicesToShow.empty())
    {
        // use all detected devices
        devicesToShow = m_deviceViews;
    }

    // enumerate devices for display
    unsigned int deviceIndex = 0;
    vector<DeviceView*>::size_type numberOfDevicesShown = devicesToShow.size();
    for(vector<DeviceView*>::const_iterator itDeviceView = devicesToShow.begin(); itDeviceView != devicesToShow.end(); ++itDeviceView)
    {
        (*itDeviceView)->setDeviceNumber(deviceIndex++);
        (*itDeviceView)->setTotalNumberOfDevices(numberOfDevicesShown);
    }

    if(m_mainWindow->isVisible())
    {
        // clear the screen
        m_mainWindow->clear();

        // print device data
        if(!devicesToShow.empty())
            m_mainWindow->printTraffic(devicesToShow);
        else
            m_mainWindow->print() << "No devices detected!" << endl;

        // refresh the screen
        m_mainWindow->refresh();
    }
    
    if(m_optWindow->isVisible())
    {
        m_optWindow->refresh(); // always show cursor in option dialog
    }
}

void OutputNcurses::showOptions(bool show)
{
    if(show)
    {
        m_optWindow->hide();

        int optWindowHeight = Screen::height() / 4;
        if(optWindowHeight >= 4)
        {
            m_mainWindow->resize(0, optWindowHeight, Screen::width(), Screen::height() - optWindowHeight);
            m_optWindow->show(0, 0, Screen::width(), optWindowHeight);
        }
        else
        {
            m_mainWindow->hide();
            m_optWindow->show(0, 0, Screen::width(), Screen::height());
        }
    }
    else
    {
        m_optWindow->hide();

        if(m_mainWindow->isVisible())
            m_mainWindow->resize(0, 0, Screen::width(), Screen::height());
        else
            m_mainWindow->show(0, 0, Screen::width(), Screen::height());
    }
}

void OutputNcurses::init()
{
    // initialize ncurses
    initscr();
    keypad(stdscr, true);
    nodelay(stdscr, true);
    noecho();
    nonl();
    cbreak();
    
    // create main window
    m_mainWindow->show(0, 0, Screen::width(), Screen::height());
}

void OutputNcurses::finish()
{
    // destroy option window
    m_optWindow->hide();
    // destroy main window
    m_mainWindow->hide();
    
    // stop ncurses
    endwin();
}

