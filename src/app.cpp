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

#include "app.h"

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include "device.h"
#include "devreader.h"
#include "devreaderfactory.h"
#include "output-file.h"
#include "output-ncurses.h"
#include "setting.h"
#include "settingfilter.h"
#include "settingstore.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define STANDARD_AVERAGE_WINDOW 300
#define STANDARD_DATA_FORMAT Statistics::humanReadableByte
#define STANDARD_HIDE_GRAPHS false
#define STANDARD_MAX_DEFLECTION 0
#define STANDARD_REFRESH_INTERVAL 500
#define STANDARD_TRAFFIC_FORMAT Statistics::humanReadableBit

using namespace std;

App::App()
    : m_quit(false)
{
    // create setting infrastructure
    SettingStore::add(Setting("AverageWindow", "Window length for average (s)", STANDARD_AVERAGE_WINDOW));
    SettingStore::add(Setting("BarMaxIn", "Max Incoming deflection (kBit/s)", STANDARD_MAX_DEFLECTION));
    SettingStore::add(Setting("BarMaxOut", "Max Outgoing deflection (kBit/s)", STANDARD_MAX_DEFLECTION));
    SettingStore::add(Setting("DataFormat", "Unit for data numbers", STANDARD_DATA_FORMAT));
    SettingStore::add(Setting("Devices", "Devices to show", "all"));
    SettingStore::add(Setting("MultipleDevices", "Show multiple devices", STANDARD_HIDE_GRAPHS));
    SettingStore::add(Setting("RefreshInterval", "Refresh interval (ms)", STANDARD_REFRESH_INTERVAL));
    SettingStore::add(Setting("TrafficFormat", "Unit for traffic numbers", STANDARD_TRAFFIC_FORMAT));

    SettingStore::get("AverageWindow").pushFilter(new SettingFilterMin(5));
    SettingStore::get("BarMaxIn").pushFilter(new SettingFilterAllowValue("0"));
    SettingStore::get("BarMaxIn").pushFilter(new SettingFilterMin(10));
    SettingStore::get("BarMaxOut").pushFilter(new SettingFilterAllowValue("0"));
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

    // load system and user settings
    loadSettings();
}

App::~App()
{
}

int App::run(const vector<string>& arguments)
{
    // parse the command line
    bool deleteDevicesRequested = true;
    bool printHelpAndExit = false;
    string outputFile;
    for(vector<string>::const_iterator itArg = arguments.begin(); itArg != arguments.end(); ++itArg)
    {
        vector<string>::const_iterator itNextArg = itArg + 1;
        bool haveNextArg = (itNextArg != arguments.end());

        // does the user want help?
        if(*itArg == "-h" || *itArg == "--help")
        {
            printHelp(false);
            return 0;
        }
        // has the user set a non-default average time window?
        else if(*itArg == "-a")
        {
            Setting& setting = SettingStore::get("AverageWindow");
            
            int val = 0;
            if(haveNextArg && fromString(*itNextArg, val))
            {
                setting = val;
                if(setting < 1)
                    setting = STANDARD_AVERAGE_WINDOW;

                ++itArg;
            }
            else
            {
                cerr << "Wrong argument for the -a parameter." << endl;
                printHelpAndExit = true;
                break;
            }
        }
        // has the user set a non-default 100% mark for
        // the incoming bandwidth bar?
        else if(*itArg == "-i")
        {
            Setting& setting = SettingStore::get("BarMaxIn");
            
            long val = 0;
            if(haveNextArg && fromString(*itNextArg, val))
            {
                setting = val;
                if(setting <= 0)
                    setting = STANDARD_MAX_DEFLECTION;

                ++itArg;
            }
            else
            {
                cerr << "Wrong argument for the -i parameter." << endl;
                printHelpAndExit = true;
                break;
            }
        }
        // has the user set a non-default 100% mark for
        // the outgoing bandwidth bar?
        else if(*itArg == "-o")
        {
            Setting& setting = SettingStore::get("BarMaxOut");
            
            long val = 0;
            if(haveNextArg && fromString(*itNextArg, val))
            {
                setting = val;
                if(setting <= 0)
                    setting = STANDARD_MAX_DEFLECTION;

                ++itArg;
            }
            else
            {
                cerr << "Wrong argument for the -o parameter." << endl;
                printHelpAndExit = true;
                break;
            }
        }
        // has the user set a non-default refresh interval?
        else if(*itArg == "-t")
        {
            Setting& setting = SettingStore::get("RefreshInterval");
            
            int val = 0;
            if(haveNextArg && fromString(*itNextArg, val))
            {
                setting = val;
                if(setting <= 0)
                    setting = STANDARD_REFRESH_INTERVAL;

                ++itArg;
            }
            else
            {
                cerr << "Wrong argument for the -t parameter." << endl;
                printHelpAndExit = true;
                break;
            }
        }
        // has the user set to show statistics of single interval instead of second?
        else if (*itArg == "-I")
        {
            SettingStore::get("StatisticsOfInterval") = true;
        }
        // has the user set a non-default unit for traffic numbers?
        else if(*itArg == "-u")
        {
            Setting& setting = SettingStore::get("TrafficFormat");
            
            if(haveNextArg && itNextArg->length() == 1)
            {
                switch((*itNextArg)[0])
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
                        printHelpAndExit = true;
                        break;
                }

                ++itArg;
            }
            else
            {
                printHelpAndExit = true;
            }

            if(printHelpAndExit)
            {
                cerr << "Wrong argument for the -u parameter." << endl;
                break;
            }
        }
        // has the user set a non-default unit for numbers of amount of data?
        else if(*itArg == "-U")
        {
            Setting& setting = SettingStore::get("DataFormat");
            
            if(haveNextArg && itNextArg->length() == 1)
            {
                switch((*itNextArg)[0])
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
                        printHelpAndExit = true;
                        break;
                }

                ++itArg;
            }
            else
            {
                printHelpAndExit = true;
            }

            if(printHelpAndExit)
            {
                cerr << "Wrong argument for the -U parameter." << endl;
                break;
            }
        }
        // has the user chosen to display multiple devices and thus not to display graphs?
        else if(*itArg == "-m")
        {
            SettingStore::get("MultipleDevices") = true;
        }
        else if(*itArg == "-f")
        {
            if(!haveNextArg)
            {
                cerr << "Missing argument for the -f parameter." << endl;
                printHelpAndExit = true;
                break;
            }

            outputFile = *itNextArg;
            ++itArg;
        }
        // has the user set to show statistics of phy(nic) rx/tx bytes instead of rx/tx bytes?
        else if (*itArg == "--phy")
        {
            SettingStore::get("StatisticsOfPhyBytes") = true;
        }
        else if (*itArg == "--rdma")
        {
            SettingStore::get("StatisticsOfRDMABytes") = true;
        }
        else if (*itArg == "--list-props")
        {
            SettingStore::get("ListStatProps") = true;
        }
        else if(*itArg == "--rx")
        {
            if(!haveNextArg)
            {
                cerr << "Missing argument for the --rx parameter." << endl;
                printHelpAndExit = true;
                break;
            }

            SettingStore::get("NameOfRxProp") = *itNextArg;
            ++itArg;
        }
        else if(*itArg == "--tx")
        {
            if(!haveNextArg)
            {
                cerr << "Missing argument for the --tx parameter." << endl;
                printHelpAndExit = true;
                break;
            }

            SettingStore::get("NameOfTxProp") = *itNextArg;
            ++itArg;
        }
        // obsolete -b option
        else if(*itArg == "-b")
        {
        }
        // obsolete -s option
        else if(*itArg == "-s")
        {
        }
        // unknown parameter
        else if((*itArg)[0] == '-')
        {
            printHelpAndExit = true;
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

            devices.setThroughFilter(trim(devices.getThroughFilter() + " " + *itArg));
        }
    }

    if(printHelpAndExit)
    {
        printHelp(true);
        return 1;
    }

    // auto-detect network devices
    DevReaderFactory::findAllDevices();
    const map<string, DevReader*>& deviceReaders = DevReaderFactory::getAllDevReaders();

    // list all props if required
    if (SettingStore::get("ListStatProps")) {
        for(map<string, DevReader*>::const_iterator itDevice = deviceReaders.begin(); itDevice != deviceReaders.end(); ++itDevice)
        {
            Device* device = new Device(itDevice->second);
            device->update();
            delete device;
        }
        return 0;
    }


    // create one instance of the Device class per device
    vector<Device*> devices;
    for(map<string, DevReader*>::const_iterator itDevice = deviceReaders.begin(); itDevice != deviceReaders.end(); ++itDevice)
    {
        Device* device = new Device(itDevice->second);
        device->update();

        devices.push_back(device);
    }

    // create output instances
    if(outputFile.empty() == false)
        m_outputs.push_back(new OutputFile(this, devices, outputFile));

    if(isatty(STDOUT_FILENO))
        m_outputs.push_back(new OutputNcurses(this, devices));
    else
        m_outputs.push_back(new OutputFile(this, devices, "-"));

    while(!m_quit)
    {
        // wait RefreshInterval milliseconds (in steps of 100 ms)
        struct timespec wantedTime;
        wantedTime.tv_sec = 0;
        
        int restOfRefreshInterval = SettingStore::get("RefreshInterval");
        
        while(restOfRefreshInterval > 0 && !m_quit)
        {
            restOfRefreshInterval -= 100;
            wantedTime.tv_nsec = (restOfRefreshInterval >= 0 ? 100 : 100 + restOfRefreshInterval) * 1000000L;
            
            nanosleep(&wantedTime, 0);

            // let outputs do any work
            for(vector<Output*>::const_iterator itOutput = m_outputs.begin(); itOutput != m_outputs.end(); ++itOutput)
                (*itOutput)->process();
        }

        if(m_quit)
            break;

        // update all devices
        for(vector<Device*>::const_iterator itDevice = devices.begin(); itDevice != devices.end(); ++itDevice)
            (*itDevice)->update();

        // generate output
        for(vector<Output*>::const_iterator itOutput = m_outputs.begin(); itOutput != m_outputs.end(); ++itOutput)
            (*itOutput)->output();
    }

    for(vector<Output*>::const_iterator itOutput = m_outputs.begin(); itOutput != m_outputs.end(); ++itOutput)
        delete *itOutput;
    m_outputs.clear();

    for(vector<Device*>::const_iterator itDevice = devices.begin(); itDevice != devices.end(); ++itDevice)
        delete *itDevice;
    devices.clear();
    
    return 0;
}

void App::processSignal(int signal)
{
    bool handled = false;

    for(vector<Output*>::const_iterator itOutput = m_outputs.begin(); itOutput != m_outputs.end(); ++itOutput)
    {
        handled |= (*itOutput)->processSignal(signal);
        if(handled)
            break;
    }

    if(!handled)
    {
        if(signal == SIGINT || signal == SIGTERM)
        {
            m_quit = true;
        }
    }
}

void App::loadSettings()
{
    // load system-wide settings
    SettingStore::readFromFile(SYSCONFDIR "/nload.conf");

    // load user settings
    const char* homeDir = getenv("HOME");
    if(homeDir)
    {
        SettingStore::readFromFile(string(homeDir) + "/.nload");
    }
    else
    {
        cerr << "Could not retrieve home directory for loading settings." << endl;
    }
}

void App::saveSettings()
{
    // save all settings as user settings
    const char* homeDir = getenv("HOME");
    if(homeDir)
    {
        SettingStore::writeToFile(string(homeDir) + "/.nload");
    }
    else
    {
        cerr << "Could not retrieve home directory for saving settings." << endl;
    }
}

void App::quit()
{
    m_quit = true;
}

void App::printHelp(bool error)
{
    // print disclaimer
    (error ? cerr : cout)
        << "\n"
        << PACKAGE << " version " << VERSION << "\n"
        << "Copyright (C) 2001 - 2018 by Roland Riegel <feedback@roland-riegel.de>\n"
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
        << "                incoming bandwidth usage. Use 0 for automatic scaling.\n"
        << "                Ignored if the switch -m is given.\n"
        << "                Default is " << STANDARD_MAX_DEFLECTION << ".\n"
        << "-m              Show multiple devices at a time; no traffic graphs.\n"
        << "-o max_scaling  Same as -i but for the graph indicating the outgoing bandwidth\n"
        << "                usage.\n"
        << "                Default is " << STANDARD_MAX_DEFLECTION << ".\n"
        << "-t interval     Determines the refresh interval of the display in milliseconds.\n"
        << "                Default is " << STANDARD_REFRESH_INTERVAL << ".\n"
        << "-I              Show traffic stats of each interval instead of one second.\n"
        << "-u h|b|k|m|g    Sets the type of unit used for the display of traffic numbers.\n"
        << "   H|B|K|M|G    h: auto, b: Bit/s, k: kBit/s, m: MBit/s etc.\n"
        << "                H: auto, B: Byte/s, K: kByte/s, M: MByte/s etc.\n"
        << "                Default is h.\n"
        << "-U h|b|k|m|g    Same as -u, but for a total amount of data (without \"/s\").\n"
        << "   H|B|K|M|G    Default is H.\n"
        << "-f filename     Append traffic data to the named file.\n"
        << "devices         Network devices to use.\n"
        << "                Default is to use all auto-detected devices.\n"
        << "--phy           Show traffic stats of phy(nic) rx/tx bytes instead of rx/tx bytes.\n"
        << "                It helps to monitor RDMA traffics.\n"
        << "--rdma          Show traffic stats of [rx|tx]_vport_rdma_unicast_bytes instead of rx/tx bytes.\n"
        << "                It helps to monitor RDMA traffics.\n"
        << "--list-props    List all statistics propertis available. (ioctl/SIOCETHTOOL)\n"
        << "--rx propname   Use propname to monitor rx bytes\n"
        << "--tx propname   Use propname to monitor tx bytes\n"
        << "                It helps to monitor RDMA traffics.\n"
        << "--help\n"
        << "-h              Print this help.\n\n"

        << "example: " << PACKAGE << " -t 200 -i 1024 -o 128 -U M\n\n"

        << "The options above can also be changed at run time by pressing the 'F2' key.\n"
        << endl;
}

