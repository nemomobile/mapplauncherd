#!/usr/bin/python

import subprocess
import sys

p = subprocess.Popen("(for p in $(pgrep applauncherd); do ls -l /proc/$p/fd | grep pipe ; done) | awk -F\: '{print $3}' | sort -u",
                     shell=True,
                     stdout=subprocess.PIPE)

result= p.stdout.read()
print type(result), result

if result[0]!='[':
    sys.exit(1)


p = subprocess.Popen("(for p in $(pgrep applauncherd); do ls -l /proc/$p/fd | grep pipe ; done) | awk -F\: '{print $3}' | sort -u | wc -l",
                     shell=True,
                     stdout=subprocess.PIPE)

result= int(p.stdout.read())

if result!=1:
    sys.exit(2)

p = subprocess.Popen("(for p in $(pgrep applauncherd); do ls -l /proc/$p/fd | grep pipe ; done) | awk -F\: '{print $3}' | wc -l",
                     shell=True,
                     stdout=subprocess.PIPE)

result= int(p.stdout.read())

if result!=4:
    sys.exit(4)
