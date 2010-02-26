/*
 * Copyright © 2005, 2006, 2007, 2008, 2009, 2010 Nokia Corporation
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

#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#include "report.h"
#include "invokelib.h"
#include "search.h"

static const int DEFAULT_DELAY = 0;

enum APP_TYPE { DUI_APP, QT_APP, UNKNOWN_APP };

extern char ** environ;

//static pid_t invoked_pid = 0;

//static void sig_forwarder(int sig)
//{
//    if (invoked_pid >= 0)
//        kill(invoked_pid, sig);
//}
//
//static void sigs_set(struct sigaction *sig)
//{
//    sigaction(SIGHUP, sig, NULL);
//    sigaction(SIGINT, sig, NULL);
//    sigaction(SIGQUIT, sig, NULL);
//    sigaction(SIGTERM, sig, NULL);
//    sigaction(SIGUSR1, sig, NULL);
//    sigaction(SIGUSR2, sig, NULL);
//    sigaction(SIGWINCH, sig, NULL);
//}

//static void sigs_init(void)
//{
//    struct sigaction sig;
//
//    memset(&sig, 0, sizeof(sig));
//    sig.sa_flags = SA_RESTART;
//    sig.sa_handler = sig_forwarder;
//
//    sigs_set(&sig);
//}
//
//static void sigs_restore(void)
//{
//    struct sigaction sig;
//
//    memset(&sig, 0, sizeof(sig));
//    sig.sa_flags = SA_RESTART;
//    sig.sa_handler = SIG_DFL;
//
//    sigs_set(&sig);
//}
//
//static void disable_self_core_dump(void)
//{
//    struct rlimit rlim = { 0, 0 };
//
//    if (setrlimit(RLIMIT_CORE, &rlim) != 0)
//        warning("could not disable self core dumping");
//}

static bool invoke_recv_ack(int fd)
{
    uint32_t action;

    /* Revceive ACK. */
    invoke_recv_msg(fd, &action);

    if (action != INVOKER_MSG_ACK)
    {
        die(1, "receiving wrong ack (%08x)\n", action);
    }
    else
        return true;
}

static int invoker_init(enum APP_TYPE app_type)
{
    int fd;
    struct sockaddr_un sun;

    fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (fd < 0)
        die(1, "opening invoker socket\n");

    sun.sun_family = AF_FILE; //AF_UNIX;

    if(app_type ==  DUI_APP)
        strcpy(sun.sun_path, INVOKER_DUI_SOCK);
    else if (app_type ==  QT_APP)
        strcpy(sun.sun_path, INVOKER_QT_SOCK);
    else
        die(1, "unknown type of application: %d\n", app_type);

    debug("before connect()");
    if (connect(fd, (struct sockaddr *)&sun, sizeof(sun)) < 0)
        die(1, "connecting to the launcher\n");
    debug("after connect()");
    return fd;
}

static bool invoker_send_magic(int fd, int options)
{
    /* Send magic. */
    invoke_send_msg(fd, INVOKER_MSG_MAGIC | INVOKER_MSG_MAGIC_VERSION | options);

    invoke_recv_ack(fd);

    return true;
}

static bool invoker_send_name(int fd, char *name)
{
    /* Send action. */
    invoke_send_msg(fd, INVOKER_MSG_NAME);
    invoke_send_str(fd, name);

    invoke_recv_ack(fd);

    return true;
}

static bool invoker_send_exec(int fd, char *exec)
{
    /* Send action. */
    invoke_send_msg(fd, INVOKER_MSG_EXEC);
    invoke_send_str(fd, exec);

    invoke_recv_ack(fd);

    return true;
}

static bool invoker_send_args(int fd, int argc, char **argv)
{
    int i;

    /* Send action. */
    invoke_send_msg(fd, INVOKER_MSG_ARGS);
    invoke_send_msg(fd, argc);
    for (i = 0; i < argc; i++)
    {
        debug("param %d %s \n", i, argv[i]);
        invoke_send_str(fd, argv[i]);
    }

    invoke_recv_ack(fd);

    return true;
}

//static bool invoker_send_prio(int fd, int prio)
//{
//    invoke_send_msg(fd, INVOKER_MSG_PRIO);
//    invoke_send_msg(fd, prio);
//
//    invoke_recv_ack(fd);
//
//    return true;
//}
//
//static bool invoker_send_io(int fd)
//{
//    struct msghdr msg;
//    //struct msghdr msg = { 0 };
//    struct cmsghdr *cmsg;
//    int io[3] = { 0, 1, 2 };
//    char buf[CMSG_SPACE(sizeof(io))];
//    struct iovec iov;
//    int dummy;
//
//    iov.iov_base = &dummy;
//    iov.iov_len = 1;
//
//    msg.msg_iov = &iov;
//    msg.msg_iovlen = 1;
//    msg.msg_control = buf;
//    msg.msg_controllen = sizeof(buf);
//
//    cmsg = CMSG_FIRSTHDR(&msg);
//    cmsg->cmsg_len = CMSG_LEN(sizeof(io));
//    cmsg->cmsg_level = SOL_SOCKET;
//    cmsg->cmsg_type = SCM_RIGHTS;
//
//    memcpy(CMSG_DATA(cmsg), io, sizeof(io));
//
//    msg.msg_controllen = cmsg->cmsg_len;
//
//    invoke_send_msg(fd, INVOKER_MSG_IO);
//    if (sendmsg(fd, &msg, 0) < 0)
//    {
//        warning("sendmsg failed in invoker_send_io: %s", strerror(errno));
//        return  false;
//    }
//
//    return true;
//}


static bool invoker_send_env(int fd)
{
    int i, n_vars;

    /* Count the amount of environment variables. */
    for (n_vars = 0; environ[n_vars] != NULL; n_vars++) ;

    /* Send action. */
    invoke_send_msg(fd, INVOKER_MSG_ENV);
    invoke_send_msg(fd, n_vars);
    for (i = 0; i < n_vars; i++)
        invoke_send_str(fd, environ[i]);

    return true;
}

static bool invoker_send_end(int fd)
{
    /* Send action. */
    invoke_send_msg(fd, INVOKER_MSG_END);

    invoke_recv_ack(fd);

    return true;
}
//
//static bool invoker_recv_pid(int fd)
//{
//    uint32_t action, pid;
//
//    // Receive action
//    invoke_recv_msg(fd, &action);
//
//    if (action != INVOKER_MSG_PID)
//    {
//        die(1, "receiving bad pid (%08x)\n", action);
//    }
//
//    // Receive pid
//    invoke_recv_msg(fd, &pid);
//    invoked_pid = pid;
//
//    return true;
//}
//
//static int invoker_recv_exit(int fd)
//{
//    uint32_t action, status;
//
//    /* Receive action. */
//    invoke_recv_msg(fd, &action);
//
//    if (action != INVOKER_MSG_EXIT)
//    {
//        die(1, "receiving bad exit status (%08x)\n", action);
//    }
//    /* Receive status. */
//    invoke_recv_msg(fd, &status);
//
//    return status;
//}
//
//static uint32_t  get_file_int(const char *filename)
//{
//    FILE *f;
//    uint32_t u;
//
//    f = fopen(filename, "r");
//    if (f == NULL)
//        return 0;
//
//    if (fscanf(f, "%u", &u) < 1)
//        u = 0;
//
//    fclose(f);
//
//    return u;
//}

///*
// * This function is Linux specific and depends on the lowmem LSM kernel module,
// * but should behave as a nop on systems not supporting it.
// */
//static unsigned int  get_linux_lowmem_modifier(void)
//{
//    uint32_t mem_allowed;
//    uint32_t mem_used;
//    const unsigned int lowmem_threshold = 80; /* Memory use percentage. */
//    int modifier;
//
//    mem_allowed = get_file_int("/proc/sys/vm/lowmem_allowed_pages");
//    mem_used = get_file_int("/proc/sys/vm/lowmem_used_pages");
//
//    if (mem_used && mem_allowed)
//    {
//        modifier = ((mem_used * 100) / mem_allowed) - lowmem_threshold;
//        if (modifier < 0)
//            modifier = 0;
//    }
//    else
//        modifier = 0;
//
//    return modifier + 1;
//}

//static unsigned int get_delay(char *delay_arg)
//{
//    unsigned int delay;
//
//    if (delay_arg)
//        delay = strtoul(delay_arg, NULL, 10);
//    else
//        delay = DEFAULT_DELAY;
//
//    return delay;
//}

static void version(void)
{
    printf("%s (%s) %s\n", PROG_NAME, PACKAGE, VERSION);
    exit(0);
}

static void usage(int status)
{
    printf("\nUsage: %s TYPE [options] file [file-options]\n"
           "Launch dui or qt application. \n"
           "Example %s --type=dui /usr/bin/hello \n"
           "TYPE: \n"
           "  dui               Launch DUI application. \n"
           "  qt                Launch Qt application.  \n"
           "Options:\n"
           "  --delay SECS        After invoking sleep for SECS seconds (default %d).\n"
           "  --version           Print program version.\n"
           "  --help              Print this help message.\n",
           PROG_NAME, PROG_NAME, DEFAULT_DELAY);

    exit(status);
}

int main(int argc, char *argv[])
{
    int i;
    int fd;
    int prog_argc = 0;
    char **prog_argv = NULL;
    char *prog_name = NULL;
//    int prog_prio = 0;
    int prog_ret = 0;
    char *delay_str = NULL;
    int magic_options = 0;
    enum APP_TYPE app_type = UNKNOWN_APP;

    if (strstr(argv[0], PROG_NAME))
    {
        /* Check application type to start */
        if (strcmp(argv[1], "--type=dui") == 0)
            app_type = DUI_APP;
        else if (strcmp(argv[1], "--type=qt") == 0)
            app_type = QT_APP;
        else if (strcmp(argv[1], "--version") == 0)
            version();
        else if (strcmp(argv[1], "--help") == 0)
            usage(0);
        else
        {
            report(report_error, "unknown type of application %s \n", argv[1]);
            usage(1);
        }

        /* Parse invoker options and search binary to launch */
        for (i = 2; i < argc; ++i)
        {
            if (strcmp(argv[i], "--delay") == 0)
            {
                if (argv[++i])
                    delay_str = argv[i];

            }
            else if (strcmp(argv[i], "--version") == 0)
                continue;
            else if (strcmp(argv[i], "--help") == 0)
                continue;
            else if (strncmp(argv[i], "--", 2) == 0)
            {
                report(report_error, "unknown parameter %s \n", argv[i]);
                usage(1);
            }
            else
            {

                prog_name = search_program(argv[i]);
                if (!prog_name)
                {
                    report(report_error, "nothing to invoke\n");
                    usage(0);
                }

                char *period = strstr(argv[i], ".launch");
                if (period)
                    *period = '\0';

                prog_argc = argc - i;
                prog_argv = &argv[i];
                break;
            }
        }
    }
    else
    {
        /* Called with a different name, old way of using invoker */
        die(1, "wrong way of using, call dui-invoker or qt-invoker");
    }


//    errno = 0;
//    prog_prio = getpriority(PRIO_PROCESS, 0);
//    if (errno && prog_prio < 0)
//        prog_prio = 0;

    info("invoking execution: '%s'\n", prog_name);

    fd = invoker_init(app_type);

    invoker_send_magic(fd, magic_options);
    invoker_send_name(fd, prog_argv[0]);
    invoker_send_exec(fd, prog_name);
    invoker_send_args(fd, prog_argc, prog_argv);
//    invoker_send_prio(fd, prog_prio);
//    invoker_send_io(fd);
    invoker_send_env(fd);
    invoker_send_end(fd);

    if (prog_name)
        free(prog_name);

//    int status;
//
    info("waiting for invoked program to exit\n");
//
//    invoker_recv_pid(fd);
//
//    /* We should disable self core dumps as late as possible, so that we have
//         * a chance to dump on our own bugs. */
//    disable_self_core_dump();
//
//    sigs_init();
//    status = invoker_recv_exit(fd);
//    sigs_restore();
//
//    if (WIFSIGNALED(status))
//        raise(WTERMSIG(status));
//    else if (WIFEXITED(status))
//        prog_ret = WEXITSTATUS(status);

    close(fd);

    return prog_ret;
}

