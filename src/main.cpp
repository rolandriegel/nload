/***************************************************************************
                                  main.cpp
                             -------------------
    begin                : Wed Jul 25 2001
    copyright            : (C) 2001 - 2012 by Roland Riegel 
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
/*
 * nload
 * real time monitor for network traffic
 * Copyright (C) 2001 - 2012 Roland Riegel <feedback@roland-riegel.de>
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

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include "device.h"
#include "devreader.h"
#include "devreaderfactory.h"
#include "graph.h"
#include "main.h"
#include "screen.h"
#include "setting.h"
#include "settingfilter.h"
#include "settingstore.h"

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

#define STANDARD_AVERAGE_WINDOW 300
#define STANDARD_DATA_FORMAT Statistics::humanReadableByte
#define STANDARD_HIDE_GRAPHS false
#define STANDARD_MAX_DEFLECTION 10240
#define STANDARD_REFRESH_INTERVAL 500
#define STANDARD_TRAFFIC_FORMAT Statistics::humanReadableBit

using namespace std;

static OptWindow m_optWindow;
static TrafficWindow m_mainWindow;

static bool quit = false;

int main(int argc, char *argv[])
{
    SettingStore::add(Setting("AverageWindow", "Window length for average (s)", STANDARD_AVERAGE_WINDOW));
    SettingStore::add(Setting("BarMaxIn", "Max Incoming deflection (kBit/s)", STANDARD_MAX_DEFLECTION));
    SettingStore::add(Setting("BarMaxOut", "Max Outgoing deflection (kBit/s)", STANDARD_MAX_DEFLECTION));
    SettingStore::add(Setting("DataFormat", "Unit for data numbers", STANDARD_DATA_FORMAT));
    SettingStore::add(Setting("Devices", "Devices to show", "all"));
    SettingStore::add(Setting("MultipleDevices", "Show multiple devices", STANDARD_HIDE_GRAPHS));
    SettingStore::add(Setting("RefreshInterval", "Refresh interval (ms)", STANDARD_REFRESH_INTERVAL));
    SettingStore::add(Setting("TrafficFormat", "Unit for traffic numbers", STANDARD_TRAFFIC_FORMAT));

    SettingStore::get("AverageWindow").pushFilter(new SettingFilterMin(5));
    SettingStore::get("BarMaxIn").pushFilter(new SettingFilterMin(10));
    SettingStore::get("BarMaxOut").pushFilter(new SettingFilterMin(10));
    SettingStore::get("RefreshInterval").pushFilter(new SettingFilterMin(50));

    SettingStore::get("Devices").pushFilter(new SettingFilterDefault("all"));
    SettingStore::get("Devices").pushFilter(new SettingFilterExclusive("all"));

    map<string, string> valueMapping;

    valueMapping[toString(false)] = "[ ]";
    valueMapping[toString(true)] = "[x]";
    SettingStore::get("MultipleDevices").pushFilter(new SettingFilterMap(valueMapping));
    valueMapping.clear();

    valueMapping[toString(Statistics::humanReadableBit)] = "Human Readable (Bit)";
    valueMapping[toString(Statistics::humanReadableByte)] = "Human Readable (Byte)";
    valueMapping[toString(Statistics::bit)] = "Bit";
    valueMapping[toString(Statistics::byte)] = "Byte";
    valueMapping[toString(Statistics::kiloBit)] = "kBit";
    valueMapping[toString(Statistics::kiloByte)] = "kByte";
    valueMapping[toString(Statistics::megaBit)] = "MBit";
    valueMapping[toString(Statistics::megaByte)] = "MByte";
    valueMapping[toString(Statistics::gigaBit)] = "GBit";
    valueMapping[toString(Statistics::gigaByte)] = "GByte";
    SettingStore::get("TrafficFormat").pushFilter(new SettingFilterMap(valueMapping));
    SettingStore::get("DataFormat").pushFilter(new SettingFilterMap(valueMapping));
    valueMapping.clear();

    // retrieve home directory
    const char* homeDirArray = getenv("HOME");
    if(!homeDirArray)
    {
        cerr << "Could not retrieve home directory." << endl;
        exit(1);
    }

    string homeDir = homeDirArray;
    SettingStore::readFromFile(SYSCONFDIR "/nload.conf");
    SettingStore::readFromFile(homeDir + "/.nload");

    // parse the command line
    bool deleteDevicesRequested = true;
    for(int i = 1; i < argc; i++)
    {
        // does the user want help?
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            printHelp(false);
            exit(0);
        }
        // has the user set a non-default average time window?
        else if(strcmp(argv[i], "-a") == 0)
        {
            Setting& setting = SettingStore::get("AverageWindow");
            
            if(i < argc - 1 && isdigit(argv[ i + 1 ][0]) != 0)
            {
                setting = atoi(argv[ i + 1 ]);
                if(setting < 1)
                    setting = STANDARD_AVERAGE_WINDOW;

                i++;
            }
            else
            {
                cerr << "Wrong argument for the -a parameter." << endl;
                printHelp(true);
                exit(1);
            }
        }
        // has the user set a non-default 100% mark for
        // the incoming bandwidth bar?
        else if(strcmp(argv[i], "-i") == 0)
        {
            Setting& setting = SettingStore::get("BarMaxIn");
            
            if(i < argc - 1 && isdigit(argv[ i + 1 ][0]) != 0)
            {
                setting = atol(argv[ i + 1 ]);
                if(setting == 0)
                    setting = STANDARD_MAX_DEFLECTION;

                i++;
            }
            else
            {
                cerr << "Wrong argument for the -i parameter." << endl;
                printHelp(true);
                exit(1);
            }
        }
        // has the user set a non-default 100% mark for
        // the outgoing bandwidth bar?
        else if(strcmp(argv[i], "-o") == 0)
        {
            Setting& setting = SettingStore::get("BarMaxOut");
            
            if(i < argc - 1 && isdigit(argv[ i + 1 ][0]) != 0)
            {
                setting = atol(argv[ i + 1 ]);
                if(setting == 0)
                    setting = STANDARD_MAX_DEFLECTION;

                i++;
            }
            else
            {
                cerr << "Wrong argument for the -o parameter." << endl;
                printHelp(true);
                exit(1);
            }
        }
        // has the user set a non-default refresh interval?
        else if(strcmp(argv[i], "-t") == 0)
        {
            Setting& setting = SettingStore::get("RefreshInterval");
            
            if(i < argc - 1 && isdigit(argv[ i + 1 ][0]) != 0)
            {
                setting = atoi(argv[ i + 1 ]);
                if(setting == 0)
                    setting = STANDARD_REFRESH_INTERVAL;

                i++;
            }
            else
            {
                cerr << "Wrong argument for the -t parameter." << endl;
                printHelp(true);
                exit(1);
            }
        }
        // has the user set a non-default unit for traffic numbers?
        else if(strcmp(argv[i], "-u") == 0)
        {
            Setting& setting = SettingStore::get("TrafficFormat");
            
            if(i < argc - 1 && isalpha(argv[ i + 1 ][0]) != 0)
            {
                switch(argv[ i + 1 ][0])
                {
                    case 'H':
                        setting = Statistics::humanReadableByte;
                        break;
                    case 'h':
                        setting = Statistics::humanReadableBit;
                        break;
                    case 'B':
                        setting = Statistics::byte;
                        break;
                    case 'b':
                        setting = Statistics::bit;
                        break;
                    case 'K':
                        setting = Statistics::kiloByte;
                        break;
                    case 'k':
                        setting = Statistics::kiloBit;
                        break;
                    case 'M':
                        setting = Statistics::megaByte;
                        break;
                    case 'm':
                        setting = Statistics::megaBit;
                        break;
                    case 'G':
                        setting = Statistics::gigaByte;
                        break;
                    case 'g':
                        setting = Statistics::gigaBit;
                        break;
                    default:
                        cerr << "Wrong argument for the -u parameter." << endl;
                        printHelp(true);
                        exit(1);
                }

                i++;
            }
            else
            {
                cerr << "Wrong argument for the -u parameter." << endl;
                printHelp(true);
                exit(1);
            }
        }
        // has the user set a non-default unit for numbers of amount of data?
        else if(strcmp(argv[i], "-U") == 0)
        {
            Setting& setting = SettingStore::get("DataFormat");
            
            if(i < argc - 1 && isalpha(argv[ i + 1 ][0]) != 0)
            {
                switch(argv[ i + 1 ][0])
                {
                    case 'H':
                        setting = Statistics::humanReadableByte;
                        break;
                    case 'h':
                        setting = Statistics::humanReadableBit;
                        break;
                    case 'B':
                        setting = Statistics::byte;
                        break;
                    case 'b':
                        setting = Statistics::bit;
                        break;
                    case 'K':
                        setting = Statistics::kiloByte;
                        break;
                    case 'k':
                        setting = Statistics::kiloBit;
                        break;
                    case 'M':
                        setting = Statistics::megaByte;
                        break;
                    case 'm':
                        setting = Statistics::megaBit;
                        break;
                    case 'G':
                        setting = Statistics::gigaByte;
                        break;
                    case 'g':
                        setting = Statistics::gigaBit;
                        break;
                    default:
                        cerr << "Wrong argument for the -U parameter." << endl;
                        printHelp(true);
                        exit(1);
                }

                i++;
            }
            else
            {
                cerr << "Wrong argument for the -U parameter." << endl;
                printHelp(true);
                exit(1);
            }
        
        }
        // has the user chosen to display multiple devices and thus not to display graphs?
        else if(strcmp(argv[i], "-m") == 0)
        {
            SettingStore::get("MultipleDevices") = true;
        }
        // obsolete -b option
        else if(strcmp(argv[i], "-b") == 0)
        {
        }
        // obsolete -s option
        else if(strcmp(argv[i], "-s") == 0)
        {
        }
        // assume unknown parameter to be the network device
        else
        {
            Setting& devices = SettingStore::get("Devices");

            if(deleteDevicesRequested)
            {
                devices = "";
                deleteDevicesRequested = false;
            }

            devices.setThroughFilter(trim(devices.getThroughFilter() + " " + argv[i]));
        }
    }

    // auto-detect network devices
    DevReaderFactory::findAllDevices();
    const map<string, DevReader*>& deviceReaders = DevReaderFactory::getAllDevReaders();

    // create one instance of the Device class per device
    map<string, Device*> deviceHandlers;
    for(map<string, DevReader*>::const_iterator itDevice = deviceReaders.begin(); itDevice != deviceReaders.end(); ++itDevice)
    {
        Device* device = new Device(*itDevice->second);
        device->update();

        deviceHandlers[itDevice->first] = device;
    }

    init();

    while(!quit)
    {
        // wait RefreshInterval milliseconds (in steps of 100 ms)
        struct timespec wantedTime;
        wantedTime.tv_sec = 0;
        
        int restOfRefreshInterval = SettingStore::get("RefreshInterval");
        
        while(restOfRefreshInterval > 0 && !quit)
        {
            restOfRefreshInterval -= 100;
            wantedTime.tv_nsec = (restOfRefreshInterval >= 0 ? 100 : 100 + restOfRefreshInterval) * 1000000L;
            
            nanosleep(&wantedTime, 0);
            
            // process keyboard
            int key;
            while((key = getch()) != ERR)
            {
                if(m_optWindow.isVisible())
                {
                    if(key == KEY_F(2))
                    {
                        m_optWindow.hide();
                        m_mainWindow.resize(0, 0, Screen::width(), Screen::height());
                        restOfRefreshInterval = 0; // update the screen
                    }
                    else
                    {
                        m_optWindow.processKey(key);
                    }
                }
                else
                {
                    switch(key)
                    {
                        case KEY_F(2):
                            m_mainWindow.resize(0, Screen::height() / 4, Screen::width(), Screen::height() - Screen::height() / 4);
                            m_optWindow.show(0, 0, Screen::width(), Screen::height() / 4);
                            restOfRefreshInterval = 0; // update the screen
                            break;
                        case KEY_F(5):
                            SettingStore::writeToFile(homeDir + "/.nload");
                            break;
                        case KEY_F(6):
                            SettingStore::readFromFile("/etc/nload.conf");
                            SettingStore::readFromFile(homeDir + "/.nload");
                            break;
                        case 'q':
                        case 'Q':
                            quit = true;
                            break;
                        default:
                            m_mainWindow.processKey(key);
                    }
                }
            }
        }

        if(quit)
            break;

        vector<string> devicesRequested = split(trim(SettingStore::get("Devices").getThroughFilter()), " ");
        vector<Device*> devicesToShow;

        if(!devicesRequested.empty() && devicesRequested.front() != "all")
        {
            // check if requested devices are available
            for(vector<string>::const_iterator itRequested = devicesRequested.begin(); itRequested != devicesRequested.end(); ++itRequested)
            {
                map<string, Device*>::const_iterator itDetectedDevice = deviceHandlers.find(*itRequested);
                if(itDetectedDevice != deviceHandlers.end())
                    devicesToShow.push_back(itDetectedDevice->second);
            }
        }

        if(devicesToShow.empty())
        {
            // use all detected devices
            for(map<string, Device*>::const_iterator itDevice = deviceHandlers.begin(); itDevice != deviceHandlers.end(); ++itDevice)
                devicesToShow.push_back(itDevice->second);
        }

        // enumerate devices for display
        unsigned int deviceIndex = 0;
        for(vector<Device*>::const_iterator itDevice = devicesToShow.begin(); itDevice != devicesToShow.end(); ++itDevice)
        {
            (*itDevice)->setDeviceNumber(deviceIndex++);
            (*itDevice)->setTotalNumberOfDevices(devicesToShow.size());
        }

        // update all devices
        for(map<string, Device*>::const_iterator itDevice = deviceHandlers.begin(); itDevice != deviceHandlers.end(); ++itDevice)
            itDevice->second->update();

        // clear the screen
        m_mainWindow.clear();
        
        // print device data
        m_mainWindow.printTraffic(devicesToShow);
        
        // refresh the screen
        m_mainWindow.refresh();
        
        if(m_optWindow.isVisible())
            m_optWindow.refresh(); // always show cursor in option dialog
        
    }

    finish();
    
    for(map<string, Device*>::const_iterator itDevice = deviceHandlers.begin(); itDevice != deviceHandlers.end(); ++itDevice)
        delete itDevice->second;
    deviceHandlers.clear();
    
    return 0;
}

void init()
{
    // handle interrrupt signal
    signal(SIGINT, end);
    signal(SIGTERM, end);
    signal(SIGWINCH, terminalResized);
    
    // initialize ncurses
    initscr();
    keypad(stdscr, true);
    nodelay(stdscr, true);
    noecho();
    nonl();
    cbreak();
    
    // create main window
    m_mainWindow.show(0, 0, Screen::width(), Screen::height());
}

void finish()
{
    // destroy main window
    m_mainWindow.hide();
    
    // stop ncurses
    endwin();
}

void end(int signal)
{
    quit = true;
}

void terminalResized(int signal)
{
    bool optWindowWasVisible = m_optWindow.isVisible();

    m_optWindow.hide();

    finish();   
    init();
    
    if(optWindowWasVisible)
    {
        m_mainWindow.resize(0, Screen::height() / 4, Screen::width(), Screen::height() - Screen::height() / 4);
        m_optWindow.show(0, 0, Screen::width(), Screen::height() / 4);
    }
}

void printHelp(bool error)
{
    // print disclaimer
    (error ? cerr : cout)
        << "\n"
        << PACKAGE << " version " << VERSION << "\n"
        << "Copyright (C) 2001 - 2012 by Roland Riegel <feedback@roland-riegel.de>\n"
        << PACKAGE << " comes with ABSOLUTELY NO WARRANTY. This is free software, and you are\n"
        << "welcome to redistribute it under certain conditions. For more details see the\n"
        << "GNU General Public License Version 2 (http://www.gnu.org/copyleft/gpl.html).\n\n"

        << "Command line syntax:\n"
        << PACKAGE << " [options] [devices]\n"
        << PACKAGE << " --help|-h\n\n"

        << "Options:\n"
        << "-a period       Sets the length in seconds of the time window for average\n"
        << "                calculation.\n"
        << "                Default is " << STANDARD_AVERAGE_WINDOW << ".\n"
        << "-i max_scaling  Specifies the 100% mark in kBit/s of the graph indicating the\n"
        << "                incoming bandwidth usage. Ignored if max_scaling is 0 or the\n"
        << "                switch -m is given.\n"
        << "                Default is " << STANDARD_MAX_DEFLECTION << ".\n"
        << "-m              Show multiple devices at a time; no traffic graphs.\n"
        << "-o max_scaling  Same as -i but for the graph indicating the outgoing bandwidth\n"
        << "                usage.\n"
        << "                Default is " << STANDARD_MAX_DEFLECTION << ".\n"
        << "-t interval     Determines the refresh interval of the display in milliseconds.\n"
        << "                Default is " << STANDARD_REFRESH_INTERVAL << ".\n"
        << "-u h|b|k|m|g    Sets the type of unit used for the display of traffic numbers.\n"
        << "   H|B|K|M|G    h: auto, b: Bit/s, k: kBit/s, m: MBit/s etc.\n"
        << "                H: auto, B: Byte/s, K: kByte/s, M: MByte/s etc.\n"
        << "                Default is h.\n"
        << "-U h|b|k|m|g    Same as -u, but for a total amount of data (without \"/s\").\n"
        << "   H|B|K|M|G    Default is H.\n"
        << "devices         Network devices to use.\n"
        << "                Default is to use all auto-detected devices.\n"
        << "--help\n"
        << "-h              Print this help.\n\n"

        << "example: " << PACKAGE << " -t 200 -i 1024 -o 128 -U M\n\n"

        << "The options above can also be changed at run time by pressing the 'F2' key.\n"
        << endl;
}

