#!/usr/bin/env python
#
# test-func-launcher.py
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
4. Given application supports launcher with .launcher binary in /usr/bin/.
5. launcher application should be installed.

Usage:    test-func-launcher <launcherable application>

Example:  test-func-launcher /usr/bin/fala_ft_hello

Authors:   ext-nimika.1.keshri@nokia.com
           ext-oskari.timperi@nokia.com
"""

import os
import subprocess
import commands
import time
import sys
import unittest

LAUNCHER_BINARY='/usr/bin/applauncherd'
DEV_NULL = file("/dev/null","w")
LAUNCHABLE_APPS = ['/usr/bin/fala_ft_hello','/usr/bin/fala_ft_hello1', '/usr/bin/fala_ft_hello2']
PREFERED_APP = '/usr/bin/fala_ft_hello'

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

def basename(filepath):
    """
    return base name of a file
    """
    return os.path.basename(filepath) 

def start_launcher_daemon():
    temp = basename(LAUNCHER_BINARY)
    st, op = commands.getstatusoutput("pgrep %s" %temp)
    if st == 0:
        debug("Launcher already started")
        return op
    else:
        p = subprocess.Popen(LAUNCHER_BINARY,
                shell=False,
                stdout=DEV_NULL, stderr=DEV_NULL)

        debug("Launcher has started")
        return p

def check_prerequisites():
    if os.getenv('DISPLAY') == None:
        error("DISPLAY is not set. Check the requirements.")
        
    if os.getenv('DBUS_SESSION_BUS_ADDRESS') == None:
        error("DBUS_SESSION_BUS_ADDRESS is not set.\n" +
              "You probably want to source /tmp/session_bus_address.user")


class launcher_tests (unittest.TestCase):
    def setUp(self):
        #setup here
        self.globalpidlist = []

    def tearDown(self):
        #teardown here
        if len(self.globalpidlist) > 0:
            for pid in self.globalpidlist:
                commands.getstatusoutput("kill -9 %s" % pid) 

    #Other functions
    def run_app_with_launcher(self, appname):
        p = subprocess.Popen(appname, 
                shell=False, 
                stdout=DEV_NULL, stderr=DEV_NULL)
        return p

    #get_pid = lambda appname: commands.getstatusoutput("pidof %s" % appname)[-1]
    
    def get_pid(self, appname):
        temp = basename(appname)[:14]
        st, op = commands.getstatusoutput("pgrep %s" % temp)
        if st == 0:
            return op
        else:
            return None
    
    def kill_process(self, appname):
        temp = basename(appname)[:14]
        st, op = commands.getstatusoutput("pkill -9 %s" % temp)
        os.wait()

    def process_state(self, processid):
        st, op = commands.getstatusoutput('cat /proc/%s/stat' %processid)
        if st == 0:
            return op
        else:
            debug(op)
            return None

    def get_creds(self, path):
        """
        Tries to launch an application and if successful, returns the
        credentials the application has as a list. 
        """

        # try launch the specified application
        handle = self.run_app_with_launcher(path)

        # sleep for a moment to allow applauncherd to start the process
        time.sleep(5)

        # with luck, the process should have correct name by now
        pid = self.get_pid(path)

        debug("%s has PID %s" % (basename(path), pid,))

        self.assert_(pid != None, "Couldn't launch %s" % basename(path))

        # get the status and output (needs creds-get from libcreds2-tools
        # package)
        st, op = commands.getstatusoutput("/usr/bin/creds-get -p %s" % pid)

        self.kill_process(path)

        return op.split("\n")

    #Testcases
    def test_001(self):
        """
        To test if the launcher exists and is executable or not
        """
        self.assert_(os.path.isfile(LAUNCHER_BINARY), "Launcher file does not exist")
        self.assert_(os.access(LAUNCHER_BINARY, os.X_OK), "Launcher exists, but is not executable")

    def test_002(self):
        """
        test_launchable_application_exists
        """
        failed_apps = []
        for app in LAUNCHABLE_APPS: 
            temp = "%s.launch" % app
            if not (os.path.isfile(temp) and os.access(temp, os.X_OK)): 
                failed_apps.append(temp)
        if len(failed_apps) > 0:
            raise AssertionError, "Some applcations do not have the launch files, list: %s" % str(failed_apps)
        #assert for method 2, type one
        self.assert_(failed_apps == [], "Some applcations do not have the launch files, list: %s" % str(failed_apps))
        #assert for method 2, type two

    def test_003(self):
        """
        test if len(filename) is less than 20
        """
        for app in LAUNCHABLE_APPS: 
            self.assert_(len(basename(app)) <= 20, "For app: %s, base name !<= 20" % app)

    def test_004(self):
        """
        To test that no Zombie process exist after the application is killed
        """
        #launch application with launcher
        #check if the application is running
        #kill the application (pid = p.pid)
        #check if pidof appname should be nothing
        #self.kill_process(LAUNCHER_BINARY)
        process_handle = self.run_app_with_launcher(PREFERED_APP)
        process_id = self.get_pid(PREFERED_APP)
        self.kill_process(PREFERED_APP)
        time.sleep(4)
        process_id = self.get_pid(PREFERED_APP)
        self.assert_(process_id == None  , "Process still running")
    
    def test_005(self):
        """
        To test that more than one applications are launched by the launcher 
        """
        for app in LAUNCHABLE_APPS: 
            #launch application with launcher
            #check if the application is running
            #check if p.pid is same as pidof appname
            #in a global dictionary, append the pid
            process_handle = self.run_app_with_launcher(app)
            time.sleep(5)
            process_id = self.get_pid(app)
            self.assert_(not (process_id == None), "process id is None")
            self.globalpidlist.append(process_id)
        self.assert_(len(self.globalpidlist) == len(LAUNCHABLE_APPS), "All Applications were not launched using launcher")
     
     
    
    def test_006(self):
        """
        To test that only one instance of a application exist 
        """
        #launch application
        #self.run_app_with_launcher(appname)
        #get pid of application
        #launch applicatoin again
        #check pidof application
        #y = commands.getstatusoutput(pidof appname)
        #len(y[-1].split(' ')) == 1
        process_handle = self.run_app_with_launcher(PREFERED_APP)
        process_id = self.get_pid(PREFERED_APP)
        debug("PID of first %s" % process_id)
        process_handle1 = self.run_app_with_launcher(PREFERED_APP)
        process_id1 = self.get_pid(PREFERED_APP)
        debug("PID of 2nd %s" % process_id1)
        self.assert_( len(process_id1.split(' ')) == 1, "Only one instance of app not running")


    def test_008(self):
        """
        Test that the fala_ft_creds* applications have the correct
        credentials set (check aegis file included in the debian package)
        """
        op1 = self.get_creds('/usr/bin/fala_ft_creds1')
        op2 = self.get_creds('/usr/bin/fala_ft_creds2')

        debug("fala_ft_creds1 has %s" % ', '.join(op1))
        debug("fala_ft_creds2 has %s" % ', '.join(op2))

        # required common caps
        caps = ['UID::user', 'GID::users', 'SRC::com.nokia.maemo',
                'applauncherd-functional-tests::applauncherd-functional-tests']

        # required caps for fala_ft_creds1
        cap1 = ['tcb', 'drm', 'Telephony', 'CAP::setuid', 'CAP::setgid',
                'CAP::setfcap'] + caps

        # required caps for fala_ft_creds2
        cap2 = ['Cellular'] + caps

        # check that all required creds are there
        for cap in cap1:
            self.assert_(cap in op1, "%s not set for fala_ft_creds1" % cap)

        for cap in cap2:
            self.assert_(cap in op2, "%s not set for fala_ft_creds2" % cap)

        # check that no other creds are set
        op1.sort()
        cap1.sort()

        self.assert_(op1 == cap1, "fala_ft_creds1 has non-requested creds!")

        op2.sort()
        cap2.sort()

        self.assert_(op2 == cap2, "fala_ft_creds2 has non-requested creds!")

    # uncomment this test when cred_confine starts working correctly
    # def test_009(self):
    #     """
    #     Check that an application that doesn't have aegis file doesn't
    #     get any funny credentials.
    #     """

    #     creds = self.get_creds('/usr/bin/fala_ft_hello')
    #     debug("fala_ft_hello has %s" % ', '.join(creds))

    #     req_creds = ['UID::nobody', 'GID::nogroup']

    #     creds.sort()
    #     req_creds.sort()

    #     self.assert_(creds == req_creds, "fala_ft_hello has too many creds!")

    def test_010(self):
        """
        Test that the launcher registered customized credentials 
        and invoker has proper credentials to access launcher
        """

        INVOKER_BINARY='/usr/bin/invoker'
        FAKE_INVOKER_BINARY='/usr/bin/invoker2'

        op1 = self.get_creds(INVOKER_BINARY)
        debug("/usr/bin/invoker has %s" % ', '.join(op1))

        # required custom caps
        caps = ['applauncherd-launcher::access']

        for cap in caps:
            self.assert_(cap in op1, "%s not set for invoker" % cap)

        st, op = commands.getstatusoutput("cp %s %s" % (INVOKER_BINARY, FAKE_INVOKER_BINARY))
        self.assert_(st == 0, "can't make copy of invoker")

        op2 = self.get_creds(FAKE_INVOKER_BINARY)
        debug("fake invoker has %s" % ', '.join(op2))

        for cap in caps:
            self.assert_(not (cap in op2), "%s is set for fake invoker" % cap)

 

# main
if __name__ == '__main__':
    check_prerequisites()
    start_launcher_daemon()
    tests = sys.argv[1:]
    mysuite = unittest.TestSuite(map(launcher_tests, tests))
    result = unittest.TextTestRunner(verbosity=2).run(mysuite)
    if not result.wasSuccessful():
        sys.exit(1)
    sys.exit(0)
