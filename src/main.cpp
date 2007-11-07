/***************************************************************************
                                  main.cpp
                             -------------------
    begin                : Wed Jul 25 2001
    copyright            : (C) 2001 - 2007 by Roland Riegel 
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
 * Copyright (C) 2001 - 2007 Roland Riegel <feedback@roland-riegel.de>
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

#include "main.h"
#include "graph.h"
#include "dev.h"
#include "screen.h"
#include "setting.h"
#include "settingstore.h"

#include <string>
#include <vector>

#include <ctype.h>
#include <time.h>
#include <curses.h>
#include <signal.h>

using namespace std;

int main(int argc, char *argv[])
{
    SettingStore::add(Setting("sleep_interval", "Refresh interval (ms)", STANDARD_SLEEP_INTERVAL));
    SettingStore::add(Setting("multiple_devices", "Show multiple devices", STANDARD_HIDE_GRAPHS));
    SettingStore::add(Setting("bar_max_in", "Max Incoming deflection (kBit/s)", STANDARD_MAX_DEFLECTION));
    SettingStore::add(Setting("bar_max_out", "Max Outgoing deflection (kBit/s)", STANDARD_MAX_DEFLECTION));
    SettingStore::add(Setting("average_smoothness", "Smoothness of average", STANDARD_AVERAGE_SMOOTHNESS));
    SettingStore::add(Setting("traffic_format", "Unit for traffic numbers", STANDARD_TRAFFIC_FORMAT));
    SettingStore::add(Setting("data_format", "Unit for data numbers", STANDARD_DATA_FORMAT));

    map<string, string> valueMapping;

    valueMapping[toString(false)] = "[ ]";
    valueMapping[toString(true)] = "[x]";
    SettingStore::get("multiple_devices").setValueMapping(valueMapping);
    valueMapping.clear();

    valueMapping[toString(Status::human_readable_bit)] = "Human Readable (Bit)";
    valueMapping[toString(Status::human_readable_byte)] = "Human Readable (Byte)";
    valueMapping[toString(Status::bit)] = "Bit";
    valueMapping[toString(Status::byte)] = "Byte";
    valueMapping[toString(Status::kilobit)] = "kBit";
    valueMapping[toString(Status::kilobyte)] = "kByte";
    valueMapping[toString(Status::megabit)] = "MBit";
    valueMapping[toString(Status::megabyte)] = "MByte";
    valueMapping[toString(Status::gigabit)] = "GBit";
    valueMapping[toString(Status::gigabyte)] = "GByte";
    SettingStore::get("traffic_format").setValueMapping(valueMapping);
    SettingStore::get("data_format").setValueMapping(valueMapping);
    valueMapping.clear();

    vector<string *> network_device;
    bool print_only_once = false;

    // parse the command line
    for(int i = 1; i < argc; i++)
    {
        // wants the user help?
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            printhelp();
            exit(0);
        }
        // has the user set a non-default 100% mark for
        // the incoming bandwidth bar?
        else if(strcmp(argv[i], "-i") == 0)
        {
            Setting& setting = SettingStore::get("bar_max_in");
            
            if(i < argc - 1 && isdigit(argv[ i + 1 ][0]) != 0)
            {
                setting = atol(argv[ i + 1 ]);
                if(setting == 0)
                    setting = STANDARD_MAX_DEFLECTION;

                i++;
            }
            else
            {
                fprintf(stderr, "Wrong argument for the -i parameter.\n\n");
                printhelp();
                exit(1);
            }
        }
        // has the user set a non-default 100% mark for
        // the outgoing bandwidth bar?
        else if(strcmp(argv[i], "-o") == 0)
        {
            Setting& setting = SettingStore::get("bar_max_out");
            
            if(i < argc - 1 && isdigit(argv[ i + 1 ][0]) != 0)
            {
                setting = atol(argv[ i + 1 ]);
                if(setting == 0)
                    setting = STANDARD_MAX_DEFLECTION;

                i++;
            }
            else
            {
                fprintf(stderr, "Wrong argument for the -o parameter.\n\n");
                printhelp();
                exit(1);
            }
        }
        // has the user set a non-default refresh interval?
        else if(strcmp(argv[i], "-t") == 0)
        {
            Setting& setting = SettingStore::get("sleep_interval");
            
            if(i < argc - 1 && isdigit(argv[ i + 1 ][0]) != 0)
            {
                setting = atoi(argv[ i + 1 ]);
                if(setting == 0)
                {
                    print_only_once = true;
                    setting = STANDARD_SLEEP_INTERVAL;
                }

                i++;
            }
            else
            {
                fprintf(stderr, "Wrong argument for the -t parameter.\n\n");
                printhelp();
                exit(1);
            }
        }
        // has the user set a non-default average smoothness?
        else if(strcmp(argv[i], "-s") == 0)
        {
            Setting& setting = SettingStore::get("average_smoothness");
            
            if(i < argc - 1 && isdigit(argv[ i + 1 ][0]) != 0)
            {
                setting = atoi(argv[ i + 1 ]);
                if(setting < 1 || setting > 9)
                    setting = STANDARD_AVERAGE_SMOOTHNESS;

                i++;
            }
            else
            {
                fprintf(stderr, "Wrong argument for the -s parameter.\n\n");
                printhelp();
                exit(1);
            }
        }
        // has the user set a non-default unit for traffic numbers?
        else if(strcmp(argv[i], "-u") == 0)
        {
            Setting& setting = SettingStore::get("traffic_format");
            
            if(i < argc - 1 && isalpha(argv[ i + 1 ][0]) != 0)
            {
                switch(argv[ i + 1 ][0])
                {
                    case 'H':
                        setting = Status::human_readable_byte;
                        break;
                    case 'h':
                        setting = Status::human_readable_bit;
                        break;
                    case 'B':
                        setting = Status::byte;
                        break;
                    case 'b':
                        setting = Status::bit;
                        break;
                    case 'K':
                        setting = Status::kilobyte;
                        break;
                    case 'k':
                        setting = Status::kilobit;
                        break;
                    case 'M':
                        setting = Status::megabyte;
                        break;
                    case 'm':
                        setting = Status::megabit;
                        break;
                    case 'G':
                        setting = Status::gigabyte;
                        break;
                    case 'g':
                        setting = Status::gigabit;
                        break;
                    default:
                        fprintf(stderr, "Wrong argument for the -u parameter.\n\n");
                        printhelp();
                        exit(1);
                }

                i++;
            }
            else
            {
                fprintf(stderr, "Wrong argument for the -u parameter.\n\n");
                printhelp();
                exit(1);
            }
        }
        // has the user set a non-default unit for numbers of amount of data?
        else if(strcmp(argv[i], "-U") == 0)
        {
            Setting& setting = SettingStore::get("data_format");
            
            if(i < argc - 1 && isalpha(argv[ i + 1 ][0]) != 0)
            {
                switch(argv[ i + 1 ][0])
                {
                    case 'H':
                        setting = Status::human_readable_byte;
                        break;
                    case 'h':
                        setting = Status::human_readable_bit;
                        break;
                    case 'B':
                        setting = Status::byte;
                        break;
                    case 'b':
                        setting = Status::bit;
                        break;
                    case 'K':
                        setting = Status::kilobyte;
                        break;
                    case 'k':
                        setting = Status::kilobit;
                        break;
                    case 'M':
                        setting = Status::megabyte;
                        break;
                    case 'm':
                        setting = Status::megabit;
                        break;
                    case 'G':
                        setting = Status::gigabyte;
                        break;
                    case 'g':
                        setting = Status::gigabit;
                        break;
                    default:
                        fprintf(stderr, "Wrong argument for the -U parameter.\n\n");
                        printhelp();
                        exit(1);
                }

                i++;
            }
            else
            {
                fprintf(stderr, "Wrong argument for the -U parameter.\n\n");
                printhelp();
                exit(1);
            }
        
        }
        // has the user chosen to display multiple devices and thus not to display graphs?
        else if(strcmp(argv[i], "-m") == 0)
        {
            SettingStore::get("multiple_devices") = true;
        }
        // obsolete -b option
        else if(strcmp(argv[i], "-b") == 0)
        {
        }
        // assume unknown parameter to be the network device
        else
        {
            network_device.push_back(new string(argv[i]));
        }
    }

    if(network_device.size() == 0)
        network_device.push_back(new string(STANDARD_NETWORK_DEVICE));

    init();

    // create one instance of the Dev class per device
    for(vector<string *>::size_type i = 0; i < network_device.size(); i++)
    {
        m_mainwindow.devices().push_back(new Dev());
        m_mainwindow.devices().back()->setProcDev(network_device[i]->c_str());
        m_mainwindow.devices().back()->setDeviceNumber(i + 1);
        m_mainwindow.devices().back()->setTotalNumberOfDevices(network_device.size());
        delete network_device[i];
    }
    network_device.clear();

    do
    {
        // wait sleep_interval milliseconds (in steps of 100 ms)
        struct timespec wanted_time;
        wanted_time.tv_sec = 0;
        
        int rest_of_sleep_interval = SettingStore::get("sleep_interval");
        
        while(rest_of_sleep_interval > 0)
        {
            rest_of_sleep_interval -= 100;
            wanted_time.tv_nsec = (rest_of_sleep_interval >= 0 ? 100 : 100 + rest_of_sleep_interval) * 1000000L;
            
            nanosleep(&wanted_time, 0);
            
            // process keyboard
            int key;
            while((key = getch()) != ERR)
            {
                switch(key)
                {
                    case 'o':
                    case 'O':
                        if(m_optwindow.visible())
                        {
                            m_optwindow.hide();
                            m_mainwindow.resize(0, 0, Screen::width(), Screen::height());
                        }
                        else
                        {
                            m_mainwindow.resize(0, Screen::height() / 4, Screen::width(), Screen::height() - Screen::height() / 4);
                            m_optwindow.show(0, 0, Screen::width(), Screen::height() / 4);
                        }
                        rest_of_sleep_interval = 0; // update the screen
                        break;
                    case 'q':
                    case 'Q':
                        if(!m_optwindow.visible())
                            end();
                        break;
                    default:
                        if(m_optwindow.visible())
                            m_optwindow.processKey(key);
                        else
                            m_mainwindow.processKey(key);
                        break;
                }
            }
        }
        
        // clear the screen
        m_mainwindow.clear();
        
        // print device data
        m_mainwindow.print();
        
        // refresh the screen
        m_mainwindow.refresh();
        
        if(m_optwindow.visible())
            m_optwindow.refresh(); // always show cursor in option dialog
        
    } while(print_only_once != true); // do this endless except the user said "-t 0"

    end();

}

void init()
{
    // handle interrrupt signal
    signal(SIGINT, end);
    signal(SIGTERM, end);
    signal(SIGWINCH, terminal_resized);
    
    // initialize ncurses
    initscr();
    keypad(stdscr, true);
    nodelay(stdscr, true);
    noecho();
    nonl();
    cbreak();
    
    // create main window
    m_mainwindow.show(0, 0, 0, 0);
}

void finish()
{
    // destroy main window
    m_mainwindow.hide();
    
    // stop ncurses
    endwin();
}

void end(int signal)
{
    finish();
    
    vector<Dev *>& devs = m_mainwindow.devices();
    for(vector<Dev *>::const_iterator i = devs.begin(); i != devs.end(); i++)
        delete *i;
    devs.clear();
    
    exit(0);
}

void terminal_resized(int signal)
{
    bool optwindow_was_visible = m_optwindow.visible();

    m_optwindow.hide();

    finish();   
    init();
    
    if(optwindow_was_visible)
    {
        m_mainwindow.resize(0, Screen::height() / 4, Screen::width(), Screen::height() - Screen::height() / 4);
        m_optwindow.show(0, 0, Screen::width(), Screen::height() / 4);
    }
}

void printhelp()
{

    // print disclaimer
    fprintf(stderr,
        "\n%s version %s\n"
        "Copyright (C) 2001 - 2003 by Roland Riegel <feedback@roland-riegel.de>\n"
        "%s comes with ABSOLUTELY NO WARRANTY. This is free software, and you are\n"
        "welcome to redistribute it under certain conditions. For more details see the\n"
        "GNU General Public License Version 2 (http://www.gnu.org/copyleft/gpl.html).\n\n"

        "Command line syntax:\n"
        "%s [options] [devices]\n"
        "%s --help|-h\n\n"
        
        "Options:\n"
        "-i max_scaling specifies the 100%% mark in kBit/s of the graph indicating the\n"
        "       incoming bandwidth usage\n"
        "       ignored if max_scaling is 0 or the switch -m is given\n"
        "       default is %d\n"
        "-m     show multiple devices at a time; do not show the traffic graphs\n"
        "-o max_scaling same as -i but for the graph indicating the outgoing bandwidth\n"
        "       usage\n"
        "       default is %d\n"
        "-s smoothness  sets the \"smoothness\" of the average in/out values\n"
        "       1 means little smoothness (average over a short period of time)\n"
        "       9 means high smoothness (average over a long period of time)\n"
        "       default is %d\n"
        "-t intervall   determines the refresh interval of the display in milliseconds\n"
        "       if 0 print net load only once and exit\n"
        "       default is %d\n"
        "-u h|b|k|m|g   sets the type of unit used for the display of traffic numbers\n"
        "   H|B|K|M|G   h: human readable (auto), b: Bit/s, k: kBit/s, m: MBit/s etc.\n"
        "       H: human readable (auto), B: Byte/s, K: kByte/s, M: MByte/s etc.\n"
        "       default is k\n"
        "-U h|b|k|m|g   same as -u, but for a total amount of data (without \"/s\")\n"
        "   H|B|K|M|G   default is M\n"
        "devices        network devices to use\n"
        "       default is \"%s\"\n"
        "--help\n"
        "-h     print this help\n\n"
        "example: %s -t 200 -s 7 -i 1024 -o 128 -U h eth0 eth1\n\n"
        "The options above can also be changed at run time by pressing the 'o' key.\n\n",
        PACKAGE,
        VERSION,
        PACKAGE,
        PACKAGE,
        PACKAGE,
        STANDARD_MAX_DEFLECTION,
        STANDARD_MAX_DEFLECTION,
        STANDARD_AVERAGE_SMOOTHNESS,
        STANDARD_SLEEP_INTERVAL,
        STANDARD_NETWORK_DEVICE,
        PACKAGE
    );
}

