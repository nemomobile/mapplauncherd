/*
 * report.c
 *
 * This file is part of applauncherd
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <syslog.h>

#include "report.h"

static enum report_output output = report_console;

void report_set_output(enum report_output new_output)
{
    if (output == new_output)
        return;

    if (output == report_syslog)
        closelog();

    if (new_output == report_syslog)
        openlog(PROG_NAME, LOG_PID, LOG_DAEMON);

    output = new_output;
}

static void vreport(enum report_type type, char *msg, va_list arg)
{
    char str[400];
    char *str_type = "";
    int log_type;

    switch (type)
    {
    case report_debug:
        log_type = LOG_DEBUG;
        break;
    default:
    case report_info:
        log_type = LOG_INFO;
        break;
    case report_warning:
        str_type = "warning ";
        log_type = LOG_WARNING;
        break;
    case report_error:
        str_type = "error ";
        log_type = LOG_ERR;
        break;
    case report_fatal:
        str_type = "died ";
        log_type = LOG_ERR;
        break;
    }

    vsnprintf(str, sizeof(str), msg, arg);

    if (output == report_console)
        printf("%s: %s%s", PROG_NAME, str_type, str);
    else if (output == report_syslog)
        syslog(log_type, "%s%s", str_type, str);
}

void report(enum report_type type, char *msg, ...)
{
    va_list arg;

    va_start(arg, msg);
    vreport(type, msg, arg);
    va_end(arg);
}

void die(int status, char *msg, ...)
{
    va_list arg;

    va_start(arg, msg);
    vreport(report_fatal, msg, arg);
    va_end(arg);

    exit(status);
}

