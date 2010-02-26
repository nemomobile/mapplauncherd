/*
 * report.h
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

#ifndef REPORT_H
#define REPORT_H

#ifdef __GNUC__
#define ATTR_NORET __attribute__((noreturn))
#else
#define ATTR_NORET
#endif

enum report_output {
  report_console,
  report_syslog,
  report_none
};

enum report_type {
  report_debug,
  report_info,
  report_warning,
  report_error,
  report_fatal
};

extern void report_set_output(enum report_output new_output);
extern void report(enum report_type type, char *msg, ...);

#define DEBUG
#ifdef DEBUG
#define debug(msg, ...) report(report_debug, msg, ##__VA_ARGS__)
#else
#define debug(...)
#endif

#define info(msg, ...) report(report_info, msg, ##__VA_ARGS__)
#define warning(msg, ...) report(report_warning, msg, ##__VA_ARGS__)
#define error(msg, ...) report(report_error, msg, ##__VA_ARGS__)

extern void ATTR_NORET die(int status, char *msg, ...);

#endif

