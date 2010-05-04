#!/usr/bin/env python
#
# test-perf-launcher.py
#
# This file is part of applauncherd
#
# Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301 USA

"""
This program tests the startup time of the given application with and
without launcher.

Requirements:
1. DISPLAY environment variable must be set correctly.
2. DBus session bus must be running.
3. DBus session bus address must be stored in /tmp/session_bus_address.user.
4. Given application supports launcher with -launcher commandline argument.
5. waitforwindow application should be installed.

Usage:    test-perf-mbooster <launcherable application>

Example:  test-perf-mbooster /usr/bin/testapp

Authors: Nimika Keshri ext-nimika.1.keshri@nokia.com 
"""
import os
import subprocess
import commands
import time
import sys

TESTAPP = '/usr/bin/testapp'
GET_TIME_OF_DAY = '/usr/bin/gettimeofday'
LOG_FILE = '/tmp/testapp.log'
DEV_NULL = file("/dev/null","w")

_start_time = 0
_end_time = 0

def debug(*msg):
    sys.stderr.write('[DEBUG %s] %s\n' % (time.time(), ' '.join([str(s) for s in msg]),))

def error(*msg):
    sys.stderr.write('ERROR %s\n' % (' '.join([str(s) for s in msg]),))
    sys.exit(1)

def basename(filepath):
    """
    return base name of a file
    """
    return os.path.basename(filepath) 
def is_executable_file(filename):
    return os.path.isfile(filename) and os.access(filename, os.X_OK)
    
def check_prerequisites(app_with_launcher, app_no_launcher):
    if os.getenv('DISPLAY') == None:
        error("DISPLAY is not set. Check the requirements.")
        
    if os.getenv('DBUS_SESSION_BUS_ADDRESS') == None:
        error("DBUS_SESSION_BUS_ADDRESS is not set.\n" +
              "You probably want to source /tmp/session_bus_address.user")

    if not is_executable_file(TESTAPP):
        error("'%s' is not an executable file\n" % (TESTAPP,) +
              "(should be an application that supports launcher)")

    if not is_executable_file(GET_TIME_OF_DAY):
        error("'%s' is not an executable file\n" % (GET_TIME_OF_DAY,) +
              "(should be an application that gets time of the day)")

def start_timer():
    global _start_time 
    #get_time = commands.getoutput(GET_TIME_OF_DAY)
    #tp = get_time.split()
    _start_time = time.time()

def run_without_launcher(TESTAPP):                                                       
    """returns process handle with pid attribute and terminate function"""               
    if os.path.exists(LOG_FILE) and os.path.isfile(LOG_FILE):               
        os.system('rm %s' %LOG_FILE)                                                        
    start_timer()
    p = subprocess.Popen(TESTAPP,
                         shell=False,
                         stdout=DEV_NULL, stderr=DEV_NULL)
    debug("app", TESTAPP, "started without launcher")                                   
    time.sleep(2)
    read_log()
    app_time = app_start_time()
    kill_process(TESTAPP)
    return app_time

def run_without_launcher_without_duihome(TESTAPP):                                                       
    """returns process handle with pid attribute and terminate function"""               
    if os.path.exists(LOG_FILE) and os.path.isfile(LOG_FILE):               
        os.system('rm %s' %LOG_FILE)                                                        
    os.system('pkill -STOP duihome')
    start_timer()
    p = subprocess.Popen(TESTAPP,
                         shell=False,
                         stdout=DEV_NULL, stderr=DEV_NULL)
    debug("app", TESTAPP, "started without launcher")                                   
    time.sleep(2)
    os.system('pkill -CONT duihome')
    read_log()
    app_time = app_start_time()
    kill_process(TESTAPP)
    return app_time

def run_with_launcher(TESTAPP):                                                       
    """returns process handle with pid attribute and terminate function"""               
    if os.path.exists(LOG_FILE) and os.path.isfile(LOG_FILE):               
        os.system('rm %s' %LOG_FILE)                                                        

    start_timer()
    os.system('invoker --type=m %s' %TESTAPP)
    debug("app", TESTAPP, "started with launcher")                                   
    time.sleep(2)
    read_log()
    app_time = app_start_time()
    kill_process(TESTAPP)
    return app_time

def run_with_launcher_without_duihome(TESTAPP):                                                       
    """returns process handle with pid attribute and terminate function"""               
    if os.path.exists(LOG_FILE) and os.path.isfile(LOG_FILE):               
        os.system('rm %s' %LOG_FILE)                                                        
    os.system('pkill -STOP duihome')
    start_timer()
    os.system('invoker --type=m %s' %TESTAPP)
    debug("app", TESTAPP, "started with launcher")                                   
    time.sleep(2)
    os.system('pkill -CONT duihome')
    read_log()
    app_time = app_start_time()
    kill_process(TESTAPP)
    return app_time

def read_log():
    global _end_time
    fh = open(LOG_FILE, "r")
    lines = fh.readlines()
    lastline = lines[len(lines)-2]
    _end_time = lastline.split()[0]
    return _end_time

def app_start_time():
    global _app_start_time
    _app_start_time = float(_end_time) - float(_start_time)
    return _app_start_time

def kill_process(appname):
    #pkill takes only up to 15 characters(?)
    commands.getoutput("pkill %s" % (basename(appname)[:15],))

def perftest_with_and_without_launcher(TESTAPP):
    debug("run app without launcher with duihome")
    tnlwd = run_without_launcher(TESTAPP)
    time.sleep(2)

    debug("run app without launcher without duihome")
    tnlnd = run_without_launcher_without_duihome(TESTAPP)
    time.sleep(2)

    debug("run app with launcher with duihome")
    twlwd = run_with_launcher(TESTAPP)
    time.sleep(2)

    debug("run app without launcher without duihome")
    twlnd = run_with_launcher_without_duihome(TESTAPP)
    time.sleep(2)

    return tnlwd, tnlnd, twlwd, twlnd

def print_test_report(with_without_times, fileobj):
    """
    with_without_times is a list of pairs:
       (with_launcher_startup_time,
        without_launcher_startup_time)
    """
    def writeline(*msg):
        fileobj.write("%s\n" % ' '.join([str(s) for s in msg]))
    def fmtfloat(f):
        return "%.2f" % (f,)
    def filterstats(data, field):
        return tuple([d[field] for d in data])

    if with_without_times == []: return

    writeline("")
    rowformat = "%12s %12s %12s %12s"
    writeline('Startup times [s]:')
    writeline(rowformat % ('launcher-No', 'launcher-No', 'launcher-Yes', 'launcher-Yes'))
    writeline(rowformat % ('duihome-Yes', 'duihome-No', 'duihome-Yes', 'duihome-No'))
    
    t1,t2,t3,t4 = [], [], [], []
    for tnlwd,tnlnd,twlwd,twlnd in with_without_times:
        t1.append(tnlwd)
        t2.append(tnlnd)
        t3.append(twlwd)
        t4.append(twlnd)
        writeline(rowformat % (fmtfloat(tnlwd),fmtfloat(tnlnd),
                               fmtfloat(twlwd),fmtfloat(twlnd)))

    writeline('Average times:')
    writeline(rowformat % (fmtfloat(sum(t1)/len(t1)),fmtfloat(sum(t2)/len(t2)),
                           fmtfloat(sum(t3)/len(t3)),fmtfloat(sum(t4)/len(t4))))


def run_perf_test(TESTAPP):
    times = []
    for i in xrange(3):
        times.append(perftest_with_and_without_launcher(TESTAPP))
    print_test_report(times, sys.stdout)

# main
if __name__ == '__main__':
    run_perf_test(TESTAPP)
