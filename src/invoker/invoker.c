/*
 * invoker.c
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

#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <bits/socket.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <limits.h>

#include "report.h"
#include "protocol.h"
#include "invokelib.h"
#include "search.h"

#ifdef HAVE_CREDS
    #include <sys/creds.h>
#endif

static const int DEFAULT_DELAY = 0;

enum APP_TYPE { M_APP, QT_APP, UNKNOWN_APP };

extern char ** environ;



/*
 * Show a list of credentials that the client has
 */
static void showcredentials(void)
{
#ifdef HAVE_CREDS
    creds_t creds;
    creds_value_t value;
    creds_type_t type;
    int i;

    creds = creds_gettask(0);
    for (i = 0; (type = creds_list(creds, i,  &value)) != CREDS_BAD; ++i) {
        char buf[200];
        (void)creds_creds2str(type, value, buf, sizeof(buf));
        buf[sizeof(buf)-1] = 0;
        printf("\t%s\n", buf);
    }
    creds_free(creds);
#else
    printf("credentials information isn't available \n");
#endif

    exit(0);
}


static bool invoke_recv_ack(int fd)
{
    uint32_t action;

    /* Receive ACK. */
    invoke_recv_msg(fd, &action);

    if (action == INVOKER_MSG_BAD_CREDS)
    {
        die(1, "credentials check failed \n");
    }
    else if (action != INVOKER_MSG_ACK)
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

    sun.sun_family = AF_UNIX;  //AF_FILE;

    int maxSize = sizeof(sun.sun_path) - 1;
    if(app_type == M_APP)
        strncpy(sun.sun_path, INVOKER_M_SOCK, maxSize);
    else if (app_type == QT_APP)
        strncpy(sun.sun_path, INVOKER_QT_SOCK, maxSize);
    else
        die(1, "unknown type of application: %d \n", app_type);

    sun.sun_path[maxSize] = '\0';

    if (connect(fd, (struct sockaddr *)&sun, sizeof(sun)) < 0)
        die(1, "connecting to the launcher\n");

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

static bool invoker_send_prio(int fd, int prio)
{
    /* Send action. */
    invoke_send_msg(fd, INVOKER_MSG_PRIO);
    invoke_send_msg(fd, prio);

    invoke_recv_ack(fd);

    return true;
}

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

static bool invoker_send_io(int fd)
{
    struct msghdr msg;
    struct cmsghdr *cmsg;
    int io[3] = { 0, 1, 2 };
    char buf[CMSG_SPACE(sizeof(io))];
    struct iovec iov;
    int dummy;

    memset(&msg, 0, sizeof(struct msghdr));

    iov.iov_base = &dummy;
    iov.iov_len = 1;

    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_len = CMSG_LEN(sizeof(io));
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;

    memcpy(CMSG_DATA(cmsg), io, sizeof(io));

    msg.msg_controllen = cmsg->cmsg_len;

    invoke_send_msg(fd, INVOKER_MSG_IO);
    if (sendmsg(fd, &msg, 0) < 0)
    {
        warning("sendmsg failed in invoker_send_io: %s /n", strerror(errno));
        return  false;
    }

    return true;
}

static bool invoker_send_end(int fd)
{
    /* Send action. */
    invoke_send_msg(fd, INVOKER_MSG_END);
    invoke_recv_ack(fd);

    return true;
}

static void version(void)
{
    printf("%s (%s) %s\n", PROG_NAME, PACKAGE, VERSION);
    exit(0);
}

static void usage(int status)
{
    printf("\nUsage: %s [options] [--type=TYPE]  [file] [args]\n"
           "Launch m or qt application.\n\n"
           "TYPE chooses the type of booster used. Qt-booster may be used to launch anything.\n"
           "Possible values for TYPE: \n"
           "  m                   Launch a MeeGo Touch application.\n"
           "  qt                  Launch a Qt application.\n\n"
           "Options:\n"
           "  --delay SECS        After invoking sleep for SECS seconds (default %d).\n"
           "  --version           Print program version.\n"
           "  --help              Print this help message.\n\n"
           "Example: %s --type=m /usr/bin/helloworld \n",
           PROG_NAME, DEFAULT_DELAY, PROG_NAME);

    exit(status);
}

static unsigned int get_delay(char *delay_arg)
{
    unsigned int delay;

    if (delay_arg)
    {
        errno = 0;    /* To distinguish success/failure after call */
        delay = strtoul(delay_arg, NULL, 10);

        /* Check for various possible errors */
        if ((errno == ERANGE && delay == ULONG_MAX) || delay == 0)
        {
            report(report_error, "wrong value of delay parameter: %s \n", delay_arg);
            usage(1);
        }
    }
    else
        delay = DEFAULT_DELAY;

    return delay;
}

int main(int argc, char *argv[])
{
    int i;
    int fd;
    int prog_argc = 0;
    char **prog_argv;
    char *prog_name = NULL;
    int prog_prio = 0;
    char *delay_str = NULL;
    unsigned int delay;
    int magic_options = 0;
    enum APP_TYPE app_type = UNKNOWN_APP;

    if (strstr(argv[0], PROG_NAME))
    {
        /* Check application type to start */
        if (argc < 2)
        {
            report(report_error, "parameters are missing \n");
            usage(1);
        }

        else if (argc == 2)
        {
            /* just print some info and exit, no need to launch an application */
            if (strcmp(argv[1], "--version") == 0)
                version();
            else if (strcmp(argv[1], "--creds") == 0)
                showcredentials();
            else if (strcmp(argv[1], "--help") == 0)
                usage(0);
            else
            {
                report(report_error, "application name or type is missing \n");
                usage(1);
            }
        }

        for (i = 1; i < argc; ++i)
        {
            if (strcmp(argv[i], "--delay") == 0)
            {
                if (argv[++i])
                {
                    delay = get_delay(argv[i]);
                }
            }
            else if (strcmp(argv[i], "--version") == 0)
                continue;
            else if (strcmp(argv[i], "--help") == 0)
                continue;
            else if (strcmp(argv[i], "--creds") == 0)
                continue;
            else if (strcmp(argv[i], "--type=m") == 0)
                app_type = M_APP;
            else if (strcmp(argv[i], "--type=qt") == 0)
                app_type = QT_APP;
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
                    report(report_error, "can't find application to invoke\n");
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
        die(1, "wrong way of using, don't use symlinks");
    }

    if (!prog_name)
    {
        die(1, "application's name is unknown \n");
    }

    info("invoking execution: '%s'\n", prog_name);

    errno = 0;
    prog_prio = getpriority(PRIO_PROCESS, 0);
    if (errno && prog_prio < 0)
      prog_prio = 0;


    fd = invoker_init(app_type);

    invoker_send_magic(fd, magic_options);
    invoker_send_name(fd, prog_argv[0]);
    invoker_send_exec(fd, prog_name);
    invoker_send_args(fd, prog_argc, prog_argv);
    invoker_send_prio(fd, prog_prio);
    invoker_send_io(fd);
    invoker_send_env(fd);
    invoker_send_end(fd);

    if (prog_name)
        free(prog_name);

    if (delay)
    {
        /* DBUS cannot cope some times if the invoker exits too early. */
        debug("delaying exit for %d seconds\n", delay);
        sleep(delay);
    }

    close(fd);

    return 0;
}

