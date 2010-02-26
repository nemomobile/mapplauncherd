/*
 * Copyright © 2005 Nokia Corporation
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

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "report.h"
#include "invokelib.h"

bool
invoke_send_msg(int fd, uint32_t msg)
{
  debug("%s: %08x\n", __FUNCTION__, msg);

  write(fd, &msg, sizeof(msg));

  return true;
}

bool
invoke_recv_msg(int fd, uint32_t *msg)
{
  read(fd, msg, sizeof(*msg));

  debug("%s: %08x\n", __FUNCTION__, *msg);

  return true;
}

bool
invoke_send_str(int fd, char *str)
{
  uint32_t size;

  /* Send size. */
  size = strlen(str) + 1;
  invoke_send_msg(fd, size);

  debug("%s: '%s'\n", __FUNCTION__, str);

  /* Send the string. */
  write(fd, str, size);

  return true;
}

char *
invoke_recv_str(int fd)
{
  uint32_t size, ret;
  char *str;

  /* Get the size. */
  invoke_recv_msg(fd, &size);
  str = malloc(size);
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

