/***************************************************************************
                                  dev.cpp
                             -------------------
    begin                : Wed Aug 1 2001
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

#include "dev.h"
#include "graph.h"
#include "setting.h"
#include "settingstore.h"
#include "window.h"

Dev::Dev() : Proc::Proc()
{
	
	for(int i = 0; i < 2; i++)
	{
		device_status[i] = new Status();
		traffic_graph[i] = new Graph();
	}
	
	setProcDev(STANDARD_NETWORK_DEVICE);
	
	setDeviceNumber(0);
	setTotalNumberOfDevices(0);
	
}

Dev::~Dev()
{
	for(int i = 0; i < 2; i++)
	{
		
		delete device_status[i];
		delete traffic_graph[i];
		
	}
}

// update the device's data
void Dev::update()
{
	unsigned long currentio[2];
	
	// read current traffic
	readLoad(currentio[0], currentio[1]);

	// calculate the traffic (Bytes/s)
	currentio[0] = (unsigned long) (currentio[0] * 1000.0 / getElapsedTime());
	currentio[1] = (unsigned long) (currentio[1] * 1000.0 / getElapsedTime());

	// update graphs and statistics
	for(int i = 0; i < 2; i++)
	{
		if(procDevExists())
		{
			device_status[i]->update(currentio[i], i == 0 ? totalIn() : totalOut());
			traffic_graph[i]->update(currentio[i]);
		}
		else
		{
			device_status[i]->resetTrafficData();
			traffic_graph[i]->resetTrafficData();
		}
	}
}

// print the device's data
void Dev::print(Window& window)
{
	char fText[100] = "";
	
	// if device does not exist
	if(!procDevExists())
	{
		// ... print warning message ...
		sprintf(fText, "Device %s (%i/%i): does not exist\n", procDev(), m_devicenumber, m_totalnumberofdevices);
		window.print(fText);
		for(int i = 0; i < window.width(); i++)
			window.print('=');
		window.print('\n');
		
		// ... and exit
		return;
	}
	
	// print header
	if(strlen(ip()) > 0)
		sprintf(fText, "Device %s [%s] (%i/%i):\n", procDev(), ip(), m_devicenumber, m_totalnumberofdevices);
	else
		sprintf(fText, "Device %s (%i/%i):\n", procDev(), m_devicenumber, m_totalnumberofdevices);
	window.print(fText);
	for(int i = 0; i < window.width(); i++)
		window.print('=');
	
    Status::status_format trafficFormat = (Status::status_format) ((int) SettingStore::get("traffic_format"));
    Status::status_format dataFormat = (Status::status_format) ((int) SettingStore::get("data_format"));

	// if graphs should be hidden ...
	if(SettingStore::get("multiple_devices"))
	{
		window.print("Incoming:");
		window.setX(window.width() / 2);
		window.print("Outgoing:\n");
		
		int status_y = window.y();
		
		device_status[0]->print(window, 0, status_y, trafficFormat, dataFormat); // incoming traffic
		device_status[1]->print(window, window.width() / 2, status_y, trafficFormat, dataFormat); // outgoing traffic
		
		window.print('\n');
	}
	// ... or not
	else
	{
		// incoming traffic
		window.print("Incoming:\n");
		
		traffic_graph[0]->setNumOfBars(window.width() * 2 / 3);
		traffic_graph[0]->setHeightOfBars((window.height() - window.y() - 1) / 2);
		traffic_graph[0]->print(window, 0, window.y());
		
		device_status[0]->print(window, window.width() * 2 / 3 + 2, window.y() - 5, trafficFormat, dataFormat);
		
		// outgoing traffic
		window.print("Outgoing:\n");
		
		traffic_graph[1]->setNumOfBars(window.width() * 2 / 3);
		traffic_graph[1]->setHeightOfBars(window.height() - window.y());
		traffic_graph[1]->print(window, 0, window.y());
		
		device_status[1]->print(window, window.width() * 2 / 3 + 2, window.y() - 4, trafficFormat, dataFormat);
	}
}

// set the number identifying the device (for display only)
void Dev::setDeviceNumber(int new_devicenumber)
{
	m_devicenumber = new_devicenumber;
}

// set the total number of shown devices (for display only)
void Dev::setTotalNumberOfDevices(int new_totalnumberofdevices)
{
	m_totalnumberofdevices = new_totalnumberofdevices;
}

