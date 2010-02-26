/*
 * Copyright © 2005, 2006 Nokia Corporation
 *
 * Author: Guillem Jover <guillem.jover@nokia.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
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

