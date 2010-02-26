/*
 * Copyright © 2005, 2006, 2007, 2008, 2009 Nokia Corporation
 *
 * Authors: Guillem Jover <guillem.jover@nokia.com>
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

#ifndef PROG_H
#define PROG_H

typedef int (*entry_t)(int, char **);

typedef struct
{
  int options;
  int argc;
  char **argv;
  const char *filename;
  char *name;
  int prio;
  int io[3];
  entry_t entry;
} prog_t;

void load_main(prog_t *prog);
void print_prog_env_argv(prog_t *prog);

void set_progname(const char *progname, int argc, char **argv, int copy_index);

#endif

