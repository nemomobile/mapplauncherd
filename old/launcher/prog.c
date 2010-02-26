/*
 * Copyright © 2005, 2006, 2007, 2008 Nokia Corporation
 *
 * Authors: Guillem Jover <guillem.jover@nokia.com>,
 *          Janne Karhunen <janne.karhunen@nokia.com>
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

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef HAVE_PRCTL_SET_NAME
#include <sys/prctl.h>
#endif
#include <dlfcn.h>

#include "report.h"
#include "prog.h"

typedef unsigned long long ulong64;

#ifdef DEBUG
extern char **environ;

void
print_prog_env_argv(prog_t *prog)
{
  int i;

  for (i = 0; environ[i] != NULL; i++)
    debug("env[%i]='%s'\n", i, environ[i]);

  for (i = 0; i < prog->argc; i++)
    debug("argv[%i]='%s'\n", i, prog->argv[i]);
}
#else
void
print_prog_env_argv(prog_t *prog)
{
}
#endif

ulong64  __attribute__ ((visibility ("hidden")))
gettime_us (void)
{
  struct timeval tv;
  ulong64 rc;
  int r;

  r = gettimeofday(&tv,0);
  if ( r<0 )
    return 0;

  rc  = tv.tv_sec;
  rc *= 1000000;
  rc += tv.tv_usec;

  return rc;
}

void
load_main(prog_t *prog)
{
  void *module;
  char *error_s;
  ulong64 tv2,tv1;

  tv1 = gettime_us();
  module = dlopen (prog->filename, RTLD_LAZY|RTLD_GLOBAL);

  if (!module)
    die(1, "loading invoked application: '%s'\n", dlerror());

  dlerror();
  prog->entry = (entry_t)dlsym(module, "main");
  error_s = dlerror();

  tv2 = gettime_us();
  info("opening of %s took %llu usec\n", prog->filename, tv2-tv1);

  if (error_s != NULL)
    die(1, "loading symbol 'main': '%s'\n", error_s);
}

#ifdef HAVE_PRCTL_SET_NAME
static void
set_process_name(const char *progname)
{
  prctl(PR_SET_NAME, basename(progname));
}
#else
static inline void
set_process_name(const char *progname)
{
}
#endif

void
set_progname(const char *progname, int argc, char **argv, int copy_index)
{
  int argvlen = 0;
  int proglen = strlen(progname) + 1;
  int i;

  for (i = 0; i < argc; i++)
    argvlen += strlen(argv[i]) + 1;

  if (proglen > argvlen)
    proglen = argvlen;

  memmove(argv[0], progname, proglen);

  if (copy_index > 0) {
    int j;

    for (j = 0; j + copy_index < argc; j++)
    {
      int arglen = strlen(argv[j + copy_index]) + 1;

      argv[j + 1] = argv[0] + proglen;
      memmove(argv[j + 1], argv[j + copy_index], arglen);
      proglen += arglen;
    }
  }

  memset(&argv[0][proglen], 0, argvlen - proglen);

  set_process_name(progname);

  setenv("_", progname, true);
}

