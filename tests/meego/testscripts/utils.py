import os, os.path, glob
import subprocess
import commands
import time
import sys
import re
from subprocess import Popen
from os.path import basename

DEV_NULL = file("/dev/null","w")

def debug(*msg):
    """
    Debug function
    """
    sys.stderr.write('[DEBUG %s] %s\n' % (time.ctime(), \
            ' '.join([str(s) for s in msg]),))

def error(*msg):
    """
    exit when error, give proper log 
    """
    sys.stderr.write('ERROR %s\n' % (' '.join([str(s) for s in msg]),))
    sys.exit(1)

def remove_applauncherd_runtime_files():
    """
    Removes files that applauncherd leaves behind after it has been stopped
    """

    files = ["%s/applauncherd.lock" % (os.environ['XDG_RUNTIME_DIR'])]
    files += glob.glob('/tmp/boost*')

    for f in files:
        print "removing %s" % f

        try:
            os.remove(f)
        except:
            pass

def start_applauncherd():
    handle = Popen(['initctl', 'start', 'xsession/applauncherd'],
                   stdout = DEV_NULL, stderr = DEV_NULL,
                   shell = False)

    return handle.wait() == 0

def kill_applauncherd():
    handle = Popen(['initctl', 'stop', 'xsession/applauncherd'],
                   stdout = DEV_NULL, stderr = DEV_NULL,
                   shell = False)

    time.sleep(1)

    remove_applauncherd_runtime_files()

    return handle.wait()

def restart_applauncherd():
    stop_applauncherd()
    start_applauncherd()

def run_app_as_user(appname, out = DEV_NULL, err = DEV_NULL):
    """
    Runs the specified command as a user.
    """

    cmd = ['su', '-', 'meego', '-c']

    if type(appname) == list:
        cmd += appname
    elif type(appname) == str:
        cmd.append(appname)
    else:
        raise TypeError("List or string expected")

    p = subprocess.Popen(cmd, shell = False, 
                         stdout = out, stderr = err)
    return p

def get_pid(appname):
    temp = basename(appname)[:14]
    st, op = commands.getstatusoutput("pgrep %s" % temp)
    if st == 0:
        return op
    else:
        return None

def get_newest_pid(appname):
    temp = basename(appname)[:14]
    st, op = commands.getstatusoutput("pgrep -n %s" % temp)
    time.sleep(5)
    if st == 0:
        return op
    else:
        return None

def wait_for_app(app = None, timeout = 5, sleep = 0.5):
    """
    Waits for an application to start. Checks periodically if
    the app is running for a maximum wait set in timeout.

    Returns the pid of the application if it was running before
    the timeout finished, otherwise None is returned.
    """

    pid = None
    start = time.time()

    while pid == None and time.time() < start + timeout:
        pid = get_newest_pid(app)

        if pid != None:
            break

        print "waiting %s secs for %s" % (sleep, app)

        time.sleep(sleep)

    return pid


def kill_process(appname=None, apppid=None, signum=9):
    if apppid and appname: 
        return None
    else:
        if apppid: 
            st, op = commands.getstatusoutput("kill -%s %s" % (str(signum), str(apppid)))
        if appname: 
            temp = basename(appname)[:14]
            st, op = commands.getstatusoutput("pkill -%s %s" % (str(signum), temp))

            try:
                os.wait()
            except Exception as e:
                print e

def process_state(processid):
    st, op = commands.getstatusoutput('cat /proc/%s/stat' %processid)
    if st == 0:
        return op
    else:
        debug(op)
        return None

def get_file_descriptor(booster, type):
    """
    To test that file descriptors are closed before calling application main
    """
    #get fd of booster before launching application
    pid = commands.getoutput("pgrep '%s$'" %booster)
    fd_info = commands.getoutput('ls -l /proc/%s/fd/' % str(pid))
    fd_info = fd_info.split('\n')
    init = {}
    final = {}

    for fd in fd_info:
        if "->" in fd:
            init[fd.split(" -> ")[0].split(' ')[-1]] = fd.split(" -> ")[-1]
    print init

    #launch application using booster
    st = os.system('invoker --type=%s --no-wait /usr/bin/fala_ft_hello.launch' %type)
    time.sleep(2)

    #get fd of booster after launching the application
    if st == 0:
        fd_info = commands.getoutput('ls -l /proc/%s/fd/' % str(pid))
        fd_info = fd_info.split('\n')
        for fd in fd_info:
            if "->" in fd:
                final[fd.split(" -> ")[0].split(' ')[-1]] = fd.split(" -> ")[-1]
    print final
    pid = commands.getoutput('pgrep fala_ft_hello')    

    mykeys = init.keys()
    count = 0

    for key in mykeys:
        try:
            if init[key] != final[key]:
                count = count + 1
        except KeyError:
            print "some key in init is not in final" 
    time.sleep(2)
    print "The number of changed file descriptors %d" %count
    kill_process(apppid=pid) 
    return count

def get_groups_for_user():
    # get supplementary groups user belongs to (doesn't return
    # the gid group)
    p = run_app_as_user('id -Gn', out = subprocess.PIPE)
    groups = p.communicate()[0].split()
    p.wait()
    
    return groups
