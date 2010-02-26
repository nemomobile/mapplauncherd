/*
 * invokelib.c
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

