#!/usr/bin/env python
"""
This program tests the startup time of the given application with and
without launcher.

Requirements:
1. DISPLAY environment variable must be set correctly.
2. DBus session bus must be running.
3. DBus session bus address must be stored in /tmp/session_bus_address.user.
4. Given application supports launcher with -launcher commandline argument.
5. waitforwindow application should be installed.

Usage:    test-perf-launcher <launcherable application>

Example:  test-perf-launcher /usr/bin/helloworld

Authors: Nimika Keshri ext-nimika.1.keshri@nokia.com 
"""
import os
import subprocess
import commands
import time
import sys

LAUNCHER_BINARY='/usr/bin/applauncherd.bin'
WAIT_FOR_WINDOW_BINARY   = '/usr/bin/waitforwindow'
DEV_NULL = file("/dev/null","w")

_timer_pipe = None
_start_time = 0
_memory_stats = []

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
    
def check_prerequisites(appname):
    if os.getenv('DISPLAY') == None:
        error("DISPLAY is not set. Check the requirements.")
        
    if os.getenv('DBUS_SESSION_BUS_ADDRESS') == None:
        error("DBUS_SESSION_BUS_ADDRESS is not set.\n" +
              "You probably want to source /tmp/session_bus_address.user")

    if not is_executable_file(appname):
        error("'%s' is not an executable file\n" % (appname,) +
              "(should be an application that supports launcher)")

    if not is_executable_file(WAIT_FOR_WINDOW_BINARY):
        error("'%s' is not an executable file\n" % (WAIT_FOR_WINDOW_BINARY,) +
              "(it should be an app that stops when a window is mapped)")

def start_launcher_daemon():
    temp = basename(LAUNCHER_BINARY)
    st, op = commands.getstatusoutput("pidof %s" %temp)
    if st == 0:
        debug("Launcher already started")
        return op
    else:
        p = subprocess.Popen(LAUNCHER_BINARY,
                shell=False,
                stdout=DEV_NULL, stderr=DEV_NULL)
        debug("Launcher has started")
        return p

def start_timer():
    global _timer_pipe
    global _start_time
    # call measure_time after calling this...
    p = subprocess.Popen(WAIT_FOR_WINDOW_BINARY,
                         shell=True,
                         stdout=subprocess.PIPE)
    _timer_pipe = p.stdout
    time.sleep(2)
    debug("starting timer")
    _start_time = time.time()

def measure_time():
    global _start_time
    debug("measuring time...")
    wait_for_window_output = None
    while wait_for_window_output != '':
        wait_for_window_output = _timer_pipe.readline()
    end_time = time.time()
    debug("...time measured")
    return end_time - _start_time


def run_without_launcher(appname):
    """returns process handle with pid attribute and terminate function"""
    start_timer()
    p = subprocess.Popen(appname,
                         shell=False,
                         stdout=DEV_NULL, stderr=DEV_NULL)
    debug("app", appname, "started")
    return p

def run_with_launcher(appname):
    """returns process handle with pid attribute and terminate function"""
    start_timer()
    p = subprocess.Popen(appname,
                         shell=False,
                         stdout=DEV_NULL, stderr=DEV_NULL)
    debug("app", appname, "started with launcher")
    return p

def kill_process(process_handle, appname):
    commands.getoutput("pkill %s" % (basename(appname),))
    os.wait()

def perftest_with_and_without_launcher(appname):
    debug("run app with launcher")
    p = run_with_launcher(appname)
    time_with = measure_time()
    time.sleep(2)
    kill_process(p, appname)
    debug("got time:", time_with)
    time.sleep(2)

    return time_with

def perftest_without_launcher(appname):
    debug("run app without launcher")
    p = run_without_launcher(appname)
    time_without = measure_time()
    time.sleep(2)
    kill_process(p, appname)
    debug("got time:", time_without)
    time.sleep(2)
    return time_without


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
    rowformat = "%12s %12s"
    writeline('Startup times [s]:')

def run_perf_test(appname):
    times = []
    for i in xrange(3):
        times.append(perftest_with_and_without_launcher(appname))
    print_test_report(times, sys.stdout)
    

# main
if __name__ == '__main__':
    try:
        appname = sys.argv[1]
    except:
        print __doc__
        error("Invalid parameters.")
    start_launcher_daemon()
    check_prerequisites(appname)
    run_perf_test(appname)
    
