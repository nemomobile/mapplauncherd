/*
 * Copyright © 2005, 2006, 2007, 2008, 2009 Nokia Corporation
 *
 * Authors: Michael Natterer <mitch@imendio.com>
 *          Guillem Jover <guillem.jover@nokia.com>
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

#define _GNU_SOURCE

#include "config.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include "report.h"
#include "invokelib.h"
#include "comm_msg.h"
#include "comm_dbus.h"
//#include "booster.h"
#include "prog.h"

/* FIXME: Should go into '/var/run/'. */
#define LAUNCHER_PIDFILE "/tmp/"PROG_NAME".pid"
#define LAUNCHER_STATEFILE "/tmp/"PROG_NAME".state"

typedef struct
{
  char *name;
  pid_t pid;
  int sock;
} child_t;

typedef struct
{
  int n;
  int used;
  child_t *list;
} kindergarten_t;

static char *pidfilename = LAUNCHER_PIDFILE;
static char *statefilename = LAUNCHER_STATEFILE;
static pid_t is_parent = 1;
static volatile bool sigchild_catched = false;
static volatile bool sighup_catched = false;
static volatile bool sigreexec_catched = false;
static bool send_app_died = false;

#define OOM_ENABLE "0"

static void
oom_unprotect(void)
{
  const char *filename = "/proc/self/oom_adj";
  int fd;

  fd = open(filename, O_WRONLY);
  if (fd < 0)
  {
    error("opening file '%s'\n", filename);
    return;
  }

  if (write(fd, OOM_ENABLE, strlen(OOM_ENABLE)) < 0)
  {
    if (errno == EPERM)
      error("kernel w/o support for user processes raising the oom value\n");
    else
      error("could not write to file '%s'\n", filename);
  }

  close(fd);
}

static bool
rise_oom_defense(pid_t pid)
{
  pid_t defender_pid;
  int status;

  defender_pid = fork();
  if (defender_pid)
    waitpid(defender_pid, &status, 0);
  else
  {
    char pidstr[20];

    snprintf(pidstr, sizeof(pidstr), "%d", pid);
    execl(DEFENDER, DEFENDER, pidstr, NULL);
    _exit(1);
  }

  if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
    return true;
  else
  {
    warning("raising the oom shield for pid=%d status=%d\n", pid, status);
    return false;
  }
}

static void
launch_process(prog_t *prog)
{
  int cur_prio;
  int i;

  /* Possibly restore process priority. */
  errno = 0;
  cur_prio = getpriority(PRIO_PROCESS, 0);
  if (!errno && cur_prio < prog->prio)
    setpriority(PRIO_PROCESS, 0, prog->prio);

  /* Unprotect our worthless childs from the oom monster. */
  if (prog->prio >= 0)
    oom_unprotect();

  load_main(prog);

  print_prog_env_argv(prog);

  debug("launching process: '%s'\n", prog->filename);

#ifdef DEBUG
  for (i = 0; i < 3; i++)
    if (prog->io[i] > 0)
      debug("redirecting stdio[%d] to %d\n", i, prog->io[i]);

  report_set_output(report_console);
#else
  report_set_output(report_none);
#endif

  for (i = 0; i < 3; i++)
    if (prog->io[i] > 0)
      dup2(prog->io[i], i);

  exit(prog->entry(prog->argc, prog->argv));
}

static int
invoked_init(void)
{
  int fd;
  struct sockaddr_un sun;

  fd = socket(PF_UNIX, SOCK_STREAM, 0);
  if (fd < 0)
    die(1, "opening invoker socket\n");

  unlink(INVOKER_SOCK);

  sun.sun_family = AF_UNIX;
  strcpy(sun.sun_path, INVOKER_SOCK);

  if (bind(fd, &sun, sizeof(sun)) < 0)
    die(1, "binding to invoker socket\n");

  if (listen(fd, 10) < 0)
    die(1, "listening to invoker socket\n");

  return fd;
}

static bool
invoked_get_magic(int fd, prog_t *prog)
{
  uint32_t magic;

  /* Receive the magic. */
  invoke_recv_msg(fd, &magic);
  if ((magic & INVOKER_MSG_MASK) == INVOKER_MSG_MAGIC)
  {
    if ((magic & INVOKER_MSG_MAGIC_VERSION_MASK) == INVOKER_MSG_MAGIC_VERSION)
      invoke_send_msg(fd, INVOKER_MSG_ACK);
    else
    {
      error("receiving bad magic version (%08x)\n", magic);
      return false;
    }
  }
  else
  {
    error("receiving bad magic (%08x)\n", magic);
    return false;
  }

  prog->options = magic & INVOKER_MSG_MAGIC_OPTION_MASK;

  return true;
}

static bool
invoked_get_name(int fd, prog_t *prog)
{
  uint32_t msg;

  /* Get the action. */
  invoke_recv_msg(fd, &msg);
  if (msg != INVOKER_MSG_NAME)
  {
    error("receiving invalid action (%08x)\n", msg);
    return false;
  }

  prog->name = invoke_recv_str(fd);
  if (!prog->name)
    return false;

  invoke_send_msg(fd, INVOKER_MSG_ACK);

  return true;
}

static bool
invoked_get_exec(int fd, prog_t *prog)
{
  prog->filename = invoke_recv_str(fd);
  if (!prog->filename)
    return false;

  invoke_send_msg(fd, INVOKER_MSG_ACK);

  return true;
}

static bool
invoked_get_args(int fd, prog_t *prog)
{
  int i;
  uint32_t argc;
  size_t size;

  /* Get argc. */
  invoke_recv_msg(fd, &argc);
  prog->argc = argc;
  size = argc * sizeof(char *);
  if (size < argc)
  {
    error("on buggy or malicious invoker code, heap overflow avoided\n");
    return false;
  }
  prog->argv = malloc(size);
  if (!prog->argv)
  {
    error("mallocing\n");
    return false;
  }

  /* Get argv. */
  for (i = 0; i < prog->argc; i++)
  {
    prog->argv[i] = invoke_recv_str(fd);
    if (!prog->argv[i])
    {
      error("receiving argv[%i]\n", i);
      return false;
    }
  }

  invoke_send_msg(fd, INVOKER_MSG_ACK);

  return true;
}

static bool
invoked_get_prio(int fd, prog_t *prog)
{
  uint32_t prio;

  invoke_recv_msg(fd, &prio);
  prog->prio = prio;

  invoke_send_msg(fd, INVOKER_MSG_ACK);

  return true;
}

static bool
invoked_get_io(int fd, prog_t *prog)
{
  struct msghdr msg = { 0 };
  struct cmsghdr *cmsg;
  char buf[CMSG_SPACE(sizeof(prog->io))];
  struct iovec iov;
  int dummy;

  iov.iov_base = &dummy;
  iov.iov_len = 1;

  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = buf;
  msg.msg_controllen = sizeof(buf);

  cmsg = CMSG_FIRSTHDR(&msg);
  cmsg->cmsg_len = CMSG_LEN(sizeof(prog->io));
  cmsg->cmsg_level = SOL_SOCKET;
  cmsg->cmsg_type = SCM_RIGHTS;

  memcpy(CMSG_DATA(cmsg), prog->io, sizeof(prog->io));

  if (recvmsg(fd, &msg, 0) < 0)
  {
    warning("recvmsg failed in invoked_get_io: %s", strerror(errno));
    return false;
  }

  if (msg.msg_flags)
  {
    warning("unexpected msg flags in invoked_get_io");
    return false;
  }

  cmsg = CMSG_FIRSTHDR(&msg);
  if (cmsg == NULL || cmsg->cmsg_len != CMSG_LEN(sizeof(prog->io)) ||
      cmsg->cmsg_level != SOL_SOCKET || cmsg->cmsg_type != SCM_RIGHTS)
  {
    warning("invalid cmsg in invoked_get_io");
    return false;
  }

  memcpy(prog->io, CMSG_DATA(cmsg), sizeof(prog->io));

  return true;
}

static bool
invoked_get_env(int fd, prog_t *prog)
{
  int i;
  uint32_t n_vars;

  /* Get number of environment variables. */
  invoke_recv_msg(fd, &n_vars);

  /* Get environ. */
  for (i = 0; i < n_vars; i++)
  {
    char *var;

    var = invoke_recv_str(fd);
    if (var == NULL)
    {
      error("receiving environ[%i]\n", i);
      return false;
    }

    debug("setting environment variable '%s'\n", var);

    /* In case of error, just warn and try to continue, as the other side is
     * going to keep sending the reset of the message. */
    if (putenv(var) != 0)
      warning("allocating environment variable");
  }

  return true;
}

static bool
invoked_send_action(int fd, int action, int value)
{
  invoke_send_msg(fd, action);
  invoke_send_msg(fd, value);

  return true;
}

static bool
invoked_send_pid(int fd, pid_t pid)
{
  return invoked_send_action(fd, INVOKER_MSG_PID, pid);
}

static bool
invoked_get_actions(int fd, prog_t *prog)
{
  while (1)
  {
    uint32_t action;

    /* Get the action. */
    invoke_recv_msg(fd, &action);

    switch (action)
    {
    case INVOKER_MSG_EXEC:
      invoked_get_exec(fd, prog);
      break;
    case INVOKER_MSG_ARGS:
      invoked_get_args(fd, prog);
      break;
    case INVOKER_MSG_ENV:
      invoked_get_env(fd, prog);
      break;
    case INVOKER_MSG_PRIO:
      invoked_get_prio(fd, prog);
      break;
    case INVOKER_MSG_IO:
      invoked_get_io(fd, prog);
      break;
    case INVOKER_MSG_END:
      invoke_send_msg(fd, INVOKER_MSG_ACK);

      if (prog->options & INVOKER_MSG_MAGIC_OPTION_WAIT)
        invoked_send_pid(fd, getpid());

      return true;
    default:
      error("receiving invalid action (%08x)\n", action);
      return false;
    }
  }
}

static bool
invoked_send_exit(int fd, int status)
{
  return invoked_send_action(fd, INVOKER_MSG_EXIT, status);
}

static bool
invoked_send_fake_exit(int fd)
{
  /* Send a fake exit code, so the invoker does not wait for us. */
  return invoked_send_exit(fd, 0);
}

static void
clean_daemon(int signal)
{
  if (is_parent)
  {
    unlink(statefilename);
    unlink(pidfilename);
    unlink(INVOKER_SOCK);
    killpg(0, signal);
  }

  exit(0);
}

static inline void
child_clone(child_t *a, child_t *b)
{
  *a = *b;
}

static void
child_destroy(child_t *child)
{
  child->pid = 0;
  close(child->sock);
  child->sock = 0;
  free(child->name);
  child->name = NULL;
}

static kindergarten_t *
kindergarten_new(int n)
{
  kindergarten_t *kg;

  kg = malloc(sizeof(kindergarten_t));
  if (!kg)
  {
    error("allocating a kindergarten\n");
    return NULL;
  }

  kg->used = 0;
  kg->n = n;
  kg->list = calloc(n, sizeof(child_t));
  if (!kg->list)
  {
    error("allocating a child list\n");
    free(kg);
    return NULL;
  }

  return kg;
}

static bool
kindergarten_grow(kindergarten_t *kg)
{
  int halfsize = kg->n * sizeof(child_t);
  int size = halfsize * 2;
  void *p;

  p = realloc(kg->list, size);
  if (!p)
    return false;

  kg->list = p;
  memset(kg->list + kg->n, 0, halfsize);

  kg->n *= 2;

  return true;
}

static bool
kindergarten_destroy(kindergarten_t *kg)
{
  int i;
  child_t *list = kg->list;

  for (i = 0; i < kg->n; i++)
    if (list[i].pid)
      child_destroy(&list[i]);

  kg->used = 0;
  kg->n = 0;
  free(kg->list);

  return true;
}

static int
kindergarten_get_child_slot_by_pid(kindergarten_t *kg, pid_t pid)
{
  child_t *list = kg->list;
  int i;

  for (i = 0; i < kg->n; i++)
    if (list[i].pid == pid)
      return i;

  return -1;
}

static bool
kindergarten_insert_child(kindergarten_t *kg, child_t *child)
{
  int id;

  if (kg->used == kg->n && !kindergarten_grow(kg))
  {
    error("cannot make a bigger kindergarten, not tracking child %d\n",
	  child->pid);
    return false;
  }

  id = kindergarten_get_child_slot_by_pid(kg, 0);
  if (id < 0)
  {
    error("this cannot be happening! no free slots on the kindergarten,\n"
	  "not tracking child %d\n", child->pid);
    return false;
  }

  child_clone(&kg->list[id], child);
  kg->used++;

  return true;
}

static bool
child_died_painfully(int status)
{
  if ((WIFEXITED(status) && WEXITSTATUS(status) != 0) ||
      (WIFSIGNALED(status) && (WTERMSIG(status) != SIGINT &&
			       WTERMSIG(status) != SIGTERM &&
			       WTERMSIG(status) != SIGKILL)))
    return true;
  else
    return false;
}

static bool
kindergarten_release_child(kindergarten_t *kg, pid_t pid, int status)
{
  int id = kindergarten_get_child_slot_by_pid(kg, pid);

  if (id >= 0)
  {
    child_t *child = &kg->list[id];

    if (send_app_died && child_died_painfully(status))
      comm_send_app_died(child->name, pid, status);

    invoked_send_exit(child->sock, status);

    child_destroy(child);
    kg->used--;
  }
  else
    info("no child %i found in the kindergarten.\n", pid);

  return true;
}

static void
kindergarten_reap_childs(kindergarten_t *kg)
{
  int status;
  pid_t childpid;

  while ((childpid = waitpid(-1, &status, WNOHANG)) > 0)
  {
    kindergarten_release_child(kg, childpid, status);

    if (WIFEXITED(status))
      info("child (pid=%d) terminated due to exit()=%d\n", childpid,
	   WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
      info("child (pid=%d) terminated due to signal=%d\n", childpid,
	   WTERMSIG(status));
    else
      info("child (pid=%d) terminated due to unknown status=%d\n", status);
  }
}

/* Persistence support */

#define LAUNCHER_STATE_SIG 0x30534c4d

static bool
store_state(kindergarten_t *childs, int invoker_fd)
{
  int i;
  int fd;
  child_t *list = childs->list;
  comm_msg_t *msg;

  unlink(statefilename);

  fd = open(statefilename, O_WRONLY | O_CREAT, 0644);
  if (fd < 0)
  {
    error("opening persistence file '%s'\n", statefilename);
    return false;
  }

  msg = comm_msg_new(512, 0);

  comm_msg_put_magic(msg, LAUNCHER_STATE_SIG);

  comm_msg_put_int(msg, invoker_fd);
  comm_msg_put_int(msg, childs->used);

  for (i = 0; i < childs->n; i++)
  {
    if (!list[i].pid)
      continue;

    comm_msg_put_int(msg, list[i].pid);
    comm_msg_put_int(msg, list[i].sock);
    comm_msg_put_str(msg, list[i].name);
  }

  comm_msg_send(fd, msg);

  comm_msg_destroy(msg);
  close(fd);

  return true;
}

static kindergarten_t *
load_state(int *invoker_fd)
{
  int i;
  int fd;
  uint32_t w, magic;
  const char *s;
  kindergarten_t *childs;
  child_t *list;
  comm_msg_t *msg;

  fd = open(statefilename, O_RDONLY);
  if (fd < 0)
  {
    if (errno != ENOENT)
      error("opening persistence file '%s'\n", statefilename);
    return NULL;
  }

  msg = comm_msg_new(512, 0);
  comm_msg_recv(fd, msg);

  close(fd);

  comm_msg_get_magic(msg, &magic);
  if (LAUNCHER_STATE_SIG != magic)
  {
    error("wrong signature on persistence file '%s'\n", statefilename);
    comm_msg_destroy(msg);

    return NULL;
  }

  comm_msg_get_int(msg, invoker_fd);

  comm_msg_get_int(msg, &w);
  childs = kindergarten_new(w);
  childs->used = w;
  list = childs->list;

  for (i = 0; i < childs->used; i++)
  {
    comm_msg_get_int(msg, &list[i].pid);
    comm_msg_get_int(msg, &list[i].sock);
    comm_msg_get_str(msg, &s);
    list[i].name = strdup(s);
  }

  comm_msg_destroy(msg);

  return childs;
}

static void
sigchild_handler(int sig)
{
  sigchild_catched = sig;
}

static void
sighup_handler(int sig)
{
  sighup_catched = true;
}

static void
sigreexec_handler(int sig)
{
  sigreexec_catched = true;
}

static void
sigs_init(void)
{
  struct sigaction sig;

  memset(&sig, 0, sizeof(sig));
  sig.sa_flags = SA_RESTART;

  sig.sa_handler = SIG_IGN;
  sigaction(SIGPIPE, &sig, NULL);

  sig.sa_handler = clean_daemon;
  sigaction(SIGINT, &sig, NULL);
  sigaction(SIGTERM, &sig, NULL);

  sig.sa_handler = sigchild_handler;
  sigaction(SIGCHLD, &sig, NULL);

  sig.sa_handler = sighup_handler;
  sigaction(SIGHUP, &sig, NULL);

  sig.sa_handler = sigreexec_handler;
  sigaction(SIGUSR1, &sig, NULL);
}

static void
sigs_restore(void)
{
  struct sigaction sig;

  memset(&sig, 0, sizeof(sig));
  sig.sa_handler = SIG_DFL;
  sig.sa_flags = SA_RESTART;

  sigaction(SIGPIPE, &sig, NULL);
  sigaction(SIGINT, &sig, NULL);
  sigaction(SIGTERM, &sig, NULL);
  sigaction(SIGCHLD, &sig, NULL);
  sigaction(SIGHUP, &sig, NULL);
  sigaction(SIGUSR1, &sig, NULL);
}

static void
sigs_interrupt(int flag)
{
  siginterrupt(SIGCHLD, flag);
  siginterrupt(SIGUSR1, flag);
}

static void
env_init(void)
{
  unsetenv("LD_BIND_NOW");
}

static void
create_pidfile(void)
{
  FILE *pidfile = fopen(pidfilename, "w");

  if (pidfile)
  {
    pid_t pid = getpid();
    fprintf(pidfile, "%d\n", pid);
    fclose(pidfile);
  }
}

static void
daemonize(void)
{
  pid_t pid;

  if (fork())
    _exit(0);

  if (setsid() < 0)
    die(1, "setting session id");

  pid = fork();
  if (pid < 0)
    die(1, "forking while daemonizing");
  else if (pid)
    _exit(0);
}

static void
fs_init(void)
{
  umask(022);
  chdir("/");
}

static void
console_quiet(void)
{
  report_set_output(report_syslog);

  close(0);
  close(1);
  close(2);

  if (open("/dev/null", O_RDONLY) < 0)
    die(1, "opening /dev/null readonly");
  if (dup(open("/dev/null", O_WRONLY)) < 0)
    die(1, "opening /dev/null writeonly");
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
  printf("Usage: %s [options]\n"
	 "\n"
	 "Options:\n"
	 "  --daemon            Fork and go into the background.\n"
	 "  --pidfile FILE      Specify a different pid file (default %s).\n"
	 "  --booster MODULE,...\n"
	 "                      Specify a comma-separated list of boosters to use"
	 "                      (default '%s').\n"
	 "  --send-app-died     Send application died signal.\n"
	 "  --quiet             Do not print anything.\n"
	 "  --version           Print program version.\n"
	 "  --help              Print this help message.\n"
	 "\n"
	 "Use the invoker to start a <shared object> from the launcher.\n"
	 "Where <shared object> is a binary including a 'main' symbol.\n"
	 "Note that the binary needs to be linked with -shared or -pie.\n",
	 PROG_NAME, LAUNCHER_PIDFILE, "booster");

  exit(status);
}

int
main(int argc, char *argv[])
{
  //booster_t *boosters = NULL;
  kindergarten_t *kg;
  const int initial_child_slots = 64;
  int i;
  int fd;
  bool daemon = false;
  bool quiet = false;
  bool upgrading = false;

  /*
   * Parse arguments.
   */
  for (i = 1; i < argc; ++i)
  {
    if (strcmp(argv[i], "--daemon") == 0)
      daemon = true;
    else if (strcmp(argv[i], "--quiet") == 0)
      quiet = true;
    else if (strcmp(argv[i], "--pidfile") == 0)
    {
      if (argv[++i])
	pidfilename = argv[i];
    }
    else if (strcmp(argv[i], "--booster") == 0)
    {
      //if (argv[++i])
	//boosters_alloc(&boosters, argv[i]);
    }
    else if (strcmp(argv[i], "--send-app-died") == 0)
      send_app_died = true;
    else if (strcmp(argv[i], "--version") == 0)
      version();
    else if (strcmp(argv[i], "--help") == 0)
      usage(0);
    else
      usage(-1);
  }

  //if (!boosters)
  //  boosters_alloc(&boosters, BOOSTER_DEFAULT);

  /*
   * Daemon initialization.
   */
  //boosters_load(boosters, &argc, &argv);

  sigs_init();
  env_init();
  fs_init();

  kg = load_state(&fd);

  if (kg)
    upgrading = true;
  else
  {
    /* Setup child tracking. */
    kg = kindergarten_new(initial_child_slots);

    /* Setup the conversation channel with the invoker. */
    fd = invoked_init();

    if (daemon)
      daemonize();
  }

  /* Protect us from the oom monster. */
  rise_oom_defense(getpid());

  if (quiet)
    console_quiet();

  if (upgrading)
    info("restored persitent state\n");
  else
    create_pidfile();

  /*
   * Application invokation loop.
   */
  while (1)
  {
    prog_t prog;
    int sd;
    int errno_accept;

    /* Accept a new invokation. */
    sigs_interrupt(1);
    sd = accept(fd, NULL, NULL);
    errno_accept = errno;
    sigs_interrupt(0);

    /* Handle signals received. */
    if (sigchild_catched)
    {
      kindergarten_reap_childs(kg);
      sigchild_catched = false;
    }

    if (sighup_catched)
    {
      //boosters_reload(boosters);
      sighup_catched = false;
    }

    if (sigreexec_catched)
    {
      info("storing kindergarten state\n");
      store_state(kg, fd);

      info("reexecuting self\n");
      execv(argv[0], argv);

      error("while trying to reexec self, trying to continue\n");
      sigreexec_catched = false;
    }

    /* Minimal error handling. */
    if (sd < 0)
    {
      if (errno_accept != EINTR)
	error("accepting connections (%s)\n", strerror(errno_accept));

      continue;
    }

    /* Start conversation with the invoker. */
    memset(&prog, 0, sizeof(prog));
    prog.io[0] = -1;
    prog.io[1] = -1;
    prog.io[2] = -1;

    if (!invoked_get_magic(sd, &prog))
    {
      close(sd);
      continue;
    }
    if (!invoked_get_name(sd, &prog))
    {
      close(sd);
      continue;
    }

    is_parent = fork();
    switch (is_parent)
    {
    case -1: /* Error. */
      error("forking while invoking\n");
      break;

    case 0: /* Child. */
      if (setsid() < 0)
	error("setting session id\n");

      sigs_restore();

      invoked_get_actions(sd, &prog);

      close(sd);
      close(fd);
      free(prog.name);

      kindergarten_destroy(kg);

      /* Invoke it. */
      if (prog.filename)
      {
        info("invoking '%s'\n", prog.filename);
        set_progname(prog.argv[0], argc, argv, -1);

	//boosters_init(boosters, prog.argv[0]);

	launch_process(&prog);
      }
      else
        error("nothing to invoke\n");

      _exit (0);
      break;

    default: /* Parent. */
      if (prog.options & INVOKER_MSG_MAGIC_OPTION_WAIT)
      {
	child_t child;

	child.pid = is_parent;
	child.sock = sd;
	child.name = prog.name;

	if (!kindergarten_insert_child(kg, &child))
	{
	  invoked_send_fake_exit(child.sock);
	  close(child.sock);
	}
      }
      else
	close(sd);
      break;
    }
  }

  return 0;
}

