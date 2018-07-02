# nload
**Real time network traffic monitor for the text console**

![](https://github.com/logic-wei/nload/blob/master/screenshots/shot_2018-7-2.png)

## Introduction

nload is a console application which monitors network traffic and bandwidth
usage in real time. It visualizes the in- and outgoing traffic using two
graphs and provides additional info like total amount of transfered data and
min/max network usage.

## Installing / Compiling nload

If you downloaded a snapshot from GitHub, you need to install automake and run `./run_autotools`.

Compiling is done by running `./configure && make`.

To install nload, you have to be root: `sudo make install`.

You can find more detailed information in the file INSTALL in this directory.

## Using nload

To learn about the command line switches and the usage of nload in general,
simply run `nload --help`. Alternatively, there is a man page available by
typing `man nload`.

## Uninstalling nload

If you want to uninstall nload run `make uninstall` in the build directory.

## Getting help

If you have problems with compiling, installing or using nload, mail me
at <feedback@roland-riegel.de>.

## Copyright / License

Copyright (C) 2001 - 2018 by Roland Riegel (<feedback@roland-riegel.de>)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
