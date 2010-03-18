/*
 * appdata.h
 *
 * This file is part of applauncherd
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef APPDATA_H
#define APPDATA_H

#include <string>

typedef int (*entry_t)(int, char **);

//! Structure for application data read from the invoker
struct AppData
{
    AppData() : argc(0), argv(NULL)
    {}

    ~AppData()
    {
        if (argv)
        {
            for (int i = 0; i < argc; i++)
            {
                free(argv[i]);
            }
        }
    }

    int     options;
    int     argc;
    char    **argv;
    string  appName;
    string  fileName;
    int     prio;
    entry_t entry;
    int     ioDescriptors[3];
};

#endif // APPDATA_H
