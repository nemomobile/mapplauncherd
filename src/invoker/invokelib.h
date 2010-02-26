/*
 * invokelib.h
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

#ifndef INVOKELIB_H
#define INVOKELIB_H

#include <stdbool.h>
#include <stdint.h>

bool invoke_send_msg(int fd, uint32_t msg);
bool invoke_recv_msg(int fd, uint32_t *msg);

bool invoke_send_str(int fd, char *str);
char *invoke_recv_str(int fd);

/* FIXME: Should be '/var/run/'. */
//#define INVOKER_SOCK	"/tmp/."PACKAGE
#define INVOKER_DUI_SOCK    "/tmp/duilnchr"
#define INVOKER_QT_SOCK	    "/tmp/qtlnchr"

/* Protocol messages and masks. */
#define INVOKER_MSG_MASK                    0xffff0000
#define INVOKER_MSG_MAGIC                   0xb0070000
#define INVOKER_MSG_MAGIC_VERSION_MASK      0x0000ff00
#define INVOKER_MSG_MAGIC_VERSION           0x00000300
#define INVOKER_MSG_MAGIC_OPTION_MASK       0x000000ff
#define INVOKER_MSG_MAGIC_OPTION_WAIT       0x00000001
#define INVOKER_MSG_NAME                    0x5a5e0000
#define INVOKER_MSG_EXEC                    0xe8ec0000
#define INVOKER_MSG_ARGS                    0xa4650000
#define INVOKER_MSG_ENV                     0xe5710000
#define INVOKER_MSG_PRIO                    0xa1ce0000
#define INVOKER_MSG_IO                      0x10fd0000
#define INVOKER_MSG_END                     0xdead0000
#define INVOKER_MSG_PID                     0x1d1d0000
#define INVOKER_MSG_EXIT                    0xe4170000
#define INVOKER_MSG_ACK                     0x600d0000

#endif
