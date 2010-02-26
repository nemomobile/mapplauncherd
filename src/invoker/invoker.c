/*
 * invoker.c
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


static bool invoke_recv_ack(int fd)
{
    uint32_t action;

    /* Receive ACK. */
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


    info("invoking execution: '%s'\n", prog_name);

    fd = invoker_init(app_type);

    invoker_send_magic(fd, magic_options);
    invoker_send_name(fd, prog_argv[0]);
    invoker_send_exec(fd, prog_name);
    invoker_send_args(fd, prog_argc, prog_argv);
    invoker_send_env(fd);
    invoker_send_end(fd);

    if (prog_name)
        free(prog_name);

    close(fd);

    return 0;
}

