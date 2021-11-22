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

#include "main.h"

#include "app.h"

#include <string>
#include <vector>

#include <signal.h>
#include <locale.h>

using namespace std;

static App* m_application = 0;

static void signalHandler(int signal);

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    App application;
    m_application = &application;

    // handle interrrupt signal
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGWINCH, signalHandler);

    vector<string> arguments;
    for(int i = 1; i < argc; ++i)
        arguments.push_back(argv[i]);

    return m_application->run(arguments);
}

void signalHandler(int signal)
{
    m_application->processSignal(signal);
}

