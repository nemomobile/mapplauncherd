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

using std::string;

#include <vector>

using std::vector;

typedef int (*entry_t)(int, char **);

//! Structure for application data read from the invoker
class AppData
{
public:

    //! Constructor
    AppData();

    //! Destructor
    ~AppData();

    //! Set options
    void setOptions(int options);

    //! Return options
    int options() const;

    //! Set argument count
    void setArgc(int argc);

    //! Return argument count
    int argc() const;

    //! Set address of the argument vector
    void setArgv(const char ** argv);

    //! Return address of the argument vector
    const char ** argv() const;

    //! Set application name
    void setAppName(const string & appName);

    //! Return application name
    const string & appName() const;

    //! Set file name
    void setFileName(const string & fileName);

    //! Return file name
    const string & fileName() const;

    //! Set priority
    void setPriority(int priority);

    //! Return priority
    int priority() const;

    //! Set entry point for the application
    void setEntry(entry_t entry);

    //! Get the entry point
    entry_t entry() const;

    //! Get I/O descriptors
    const vector<int> & ioDescriptors() const;

    //! Set I/O descriptors
    void setIODescriptors(const vector<int> & ioDescriptors);

    //! Frees the memory reserved for argv
    void deleteArgv();

private:

    AppData(const AppData & r);
    AppData & operator= (const AppData & r);

    int         m_options;
    int         m_argc;
    const char ** m_argv;
    string      m_appName;
    string      m_fileName;
    int         m_prio;
    entry_t     m_entry;
    vector<int> m_ioDescriptors;
};

#endif // APPDATA_H
