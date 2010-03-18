/*
 * invokelib.c
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

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "report.h"
#include "invokelib.h"

bool invoke_send_msg(int fd, uint32_t msg)
{
    debug("%s: %08x\n", __FUNCTION__, msg);
    return write(fd, &msg, sizeof(msg)) != -1;
}

bool invoke_recv_msg(int fd, uint32_t *msg)
{
    int res = read(fd, msg, sizeof(*msg));
    debug("%s: %08x\n", __FUNCTION__, *msg);
    return res != -1;
}

bool invoke_send_str(int fd, char *str)
{
    uint32_t size;

    /* Send size. */
    size = strlen(str) + 1;
    invoke_send_msg(fd, size);

    debug("%s: '%s'\n", __FUNCTION__, str);

    /* Send the string. */
    return write(fd, str, size) != -1;
}

char* invoke_recv_str(int fd)
{
    uint32_t size, ret;
    char *str;

    /* Get the size. */
    invoke_recv_msg(fd, &size);
    str = (char*)malloc(size);
    if (!str)
    {
        error("mallocing in %s\n", __FUNCTION__);
        return NULL;
    }

    /* Get the string. */
    ret = read(fd, str, size);
    if (ret < size)
    {
        error("getting string, got %u of %u bytes\n", ret, size);
        free(str);
        return NULL;
    }
    str[size - 1] = '\0';

    debug("%s: '%s'\n", __FUNCTION__, str);

    return str;
}

