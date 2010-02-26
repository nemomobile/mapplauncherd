/*
 * logger.h
 *
 * This file is part of applauncherd
 *
 * Copyright (C) 2009 Nokia Corporation. All rights reserved.
 *
 * This software, including documentation, is protected by copyright
 * controlled by Nokia Corporation. All rights are reserved.
 * Copying, including reproducing, storing, adapting or translating,
 * any or all of this material requires the prior written consent of
 * Nokia Corporation. This material also contains confidential
 * information which may not be disclosed to others without the prior
 * written consent of Nokia.
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <QTextStream>
#include <QFile>
#include <stdarg.h>

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

