#!/usr/bin/python
#
# check_pipes.py
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
