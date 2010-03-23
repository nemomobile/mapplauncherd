/*
 * logger.h
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

#ifndef LOGGER_H
#define LOGGER_H

#include <QTextStream>
#include <QFile>
#include <cstdarg>

/*!
 * \class Logger
 * \brief Logging utility class
 *
 */
class Logger
{
public:

    /*!
     * \brief Open the log
     * \param progName Program name as it will be seen in the log.
     */
    static void openLog(const char * progName);

    /*!
     * \brief Close the log
     */
    static void closeLog();


    /*!
     * \brief Log a notice to the system message logger
     * \param format String identical to a printf format string
     * \param additionalArgs Depending on the format string, the function may expect a 
     *        sequence of additional arguments, each containing one value to be inserted
     *        in the format parameter, if any. 
     */
    static void logNotice(const char * format, ...);


    /*!
     * \brief Log an error to the system message logger
     * \param format String identical to a printf format string
     * \param additionalArgs Depending on the format string, the function may expect a 
     *        sequence of additional arguments, each containing one value to be inserted
     *        in the format parameter, if any. 
     */
    static void logError(const char * format, ...);


    /*!
     * \brief Log a warning to the system message logger
     * \param format String identical to a printf format string
     * \param additionalArgs Depending on the format string, the function may expect a 
     *        sequence of additional arguments, each containing one value to be inserted
     *        in the format parameter, if any. 
     */
    static void logWarning(const char * format, ...);


    /*!
     * \brief Log a piece of information to the system message logger
     * \param format String identical to a printf format string
     * \param additionalArgs Depending on the format string, the function may expect a 
     *        sequence of additional arguments, each containing one value to be inserted
     *        in the format parameter, if any. 
     */
    static void logInfo(const char * format, ...);

    /*!
     * \brief Log an error to the system message logger and exit
     * \param format String identical to a printf format string
     * \param additionalArgs Depending on the format string, the function may expect a
     *        sequence of additional arguments, each containing one value to be inserted
     *        in the format parameter, if any.
     */
    static void logErrorAndDie(int code, const char * format, ...);


    /*!
     * \brief Forces Logger to echo everything to stdout if set to true.
     */
    static void setEchoMode(bool enable);

private:

    static void writeLog(const int priority, const char * format, va_list ap); 
    //! True if the log is open
    static bool m_isOpened;
    //! Log file which is used if the syslog is not available    
    //static QFile m_logFile;
    static QFile m_logFile;
    //! Text stream for log file
    static QTextStream m_logStream;
    //! True if the syslog is available
    static bool m_useSyslog; 
    //! Echo everything to stdout if true
    static bool m_echoMode;
};

#endif // LOGGER_H

