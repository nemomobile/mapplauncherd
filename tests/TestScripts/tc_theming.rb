#!/usr/bin/ruby1.8
#
# tc_theming.rb
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
This script tests that system theming works for launched applications.

The method used is simple:

  * launch application
  * take a screenshot (1)
  * change theme
  * take a screenshot (2)
  * change theme back to original theme
  * take a screenshot (3)
  * make sure that 1 and 2 differ and that 1 and 3 are the same
"""

require 'matti'
require 'date'
require 'test/unit'
include MattiVerify

if ENV.has_key?('_SBOX_DIR')
    ENV['PATH'] += ":" + ENV['_SBOX_DIR'] + "/tools/bin"
end

class TC_Theming < Test::Unit::TestCase
    def change_theme(theme_name)
        puts "Changing theme from #{get_theme()} to #{theme_name}"

        cmd = "gconftool-2 -s /meegotouch/theme/name -t string " + theme_name
        
        verify_equal(true, 5, "Could not change theme") {
            system(cmd)
        }

        sleep(5)
    end

    def get_theme()
        `gconftool-2 -g /meegotouch/theme/name`.strip()
    end

    def setup
        system('mcetool --set-tklock-mode=unlocked')
        @sut = MATTI.sut(:Id => ARGV[0] || 'sut_qt_maemo') 
    end

    def test_theming
        app_name = 'fala_ft_hello'

        system("pkill #{app_name}")

        system(app_name)
        sleep(5)

        #pid = `pgrep -n #{app_name}`

        app = @sut.application(:name => app_name)

        original_theme = get_theme()
        alternative_theme = 'plankton'

        if original_theme == alternative_theme
            alternative_theme = 'dev'
        end

        app.capture_screen('PNG', '/home/testshot1.png', true)

        change_theme(alternative_theme)

        app.capture_screen('PNG', '/home/testshot2.png', true)

        change_theme(original_theme)

        app.capture_screen('PNG', '/home/testshot3.png', true)

        system("pkill #{app_name}")
    end
end
