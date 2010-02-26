/*
 * Copyright © 2006, 2007 Nokia Corporation
 *
 * Authors: Guillem Jover <guillem.jover@nokia.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "report.h"
#include "prog.h"
#include "search.h"

static void
summon_process(prog_t *prog)
{
  load_main(prog);

  print_prog_env_argv(prog);

  debug("summoning process: '%s'\n", prog->filename);

  exit(prog->entry(prog->argc, prog->argv));
}

static void
version(void)
{
  printf("%s (%s) %s\n", PROG_NAME, PACKAGE, VERSION);

  exit(0);
}

static void
usage(int status)
{
  printf("Usage: %s [options] file [file-options]\n"
	 "\n"
	 "Options:\n"
	 "  --version           Print program version.\n"
	 "  --help              Print this help message.\n",
	 PROG_NAME);

  exit(status);
}

int
main(int argc, char *argv[])
{
  int i;
  prog_t prog = { 0 };

  if (strstr(argv[0], PROG_NAME))
  {
    char *argv0 = NULL;

    /* Called with our default name. Parse arguments. */
    for (i = 1; i < argc; ++i)
    {
      if (strcmp(argv[i], "--version") == 0)
	version();
      else if (strcmp(argv[i], "--help") == 0)
	usage(0);
      else if (strncmp(argv[i], "--", 2) == 0)
	usage(1);
      else
      {
	char *period;

	argv0 = strdup(argv[i]);
	period = strstr(argv0, ".launch");
	if (period)
		*period = '\0';

	prog.filename = search_program(argv[i]);
	prog.argc = argc - i;
	prog.argv = argv;
	break;
      }
    }

    if (!prog.filename)
      usage(1);

    set_progname(argv0, argc, argv, i + 1);

    free(argv0);
  }
  else
  {
    char *launch = NULL;

    /* Called with a different name. Add the proper extension and launch it.
     * Do not try to parse any arguments. */
    if (asprintf(&launch, "%s.launch", argv[0]) < 0)
      die(1, "allocating program name buffer");
    prog.filename = search_program(launch);
    prog.argc = argc;
    prog.argv = argv;

    free(launch);
  }

  /* Summon it. */
  if (prog.filename)
  {
    info("summoning '%s'\n", prog.filename);
    summon_process(&prog);
  }
  else
    error("nothing to summon\n");

  return 0;
}

