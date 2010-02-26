/*
 * appdata.h
 *
 * This file is part of launcher
 *
 * Copyright (C) 2010 Nokia Corporation. All rights reserved.
 *
 * This software, including documentation, is protected by copyright
 * controlled by Nokia Corporation. All rights are reserved.
 * Copying, including reproducing, storing, adapting or translating,
 * any or all of this material requires the prior written consent of
 * Nokia Corporation. This material also contains confidential
 * information which may not be disclosed to others without the prior
 * written consent of Nokia.
 */

#ifndef APPDATA_H
#define APPDATA_H

#include <string>

typedef int (*entry_t)(int, char **);

//! Structure for application data read from the invoker
struct AppData
{
    AppData() : argv(NULL)
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
};

#endif // APPDATA_H
