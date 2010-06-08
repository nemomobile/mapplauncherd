#!/usr/bin/ruby1.8
#
# This file is part of applifed
#
# Copyright (C) 2009 Nokia Corporation. All rights reserved.
#
#  * Description: Testcases for the prestart functionality 
#   
#  * Objectives: Test that a prestarted application can be 
#    launched as well    
#
#  Author: Nimika Keshri(mailto: ext-nimika.1.keshri@nokia.com) 
#
# This software, including documentation, is protected by copyright
# controlled by Nokia Corporation. All rights are reserved.
# Copying, including reproducing, storing, adapting or translating,
# any or all of this material requires the prior written consent of
# Nokia Corporation. This material also contains confidential
# information which may not be disclosed to others without the prior
# written consent of Nokia.
#

require 'matti'
require 'date'
require 'test/unit'
include MattiVerify


class TC_PRESTARTLAUNCHTESTS < Test::Unit::TestCase

	# method called before any test case
	def setup
		@sut = MATTI.sut(:Id=>ARGV[0] || 'sut_qt_maemo')    
	end
	
	# method called after any test case for cleanup purposes
	def teardown
	end

	def test_launch_prestarted_app
	#Test that a prestarted application can be launched

        @appname = 'fala_testapp'
        verify_equal(false,2,"Application is Prestarted"){
                system "pidof #{@appname}"}
        sleep 2

        string = `export DISPLAY=:0; source /tmp/session_bus_address.user;dbus-send --dest=com.nokia.#{@appname} --type="method_call" /org/maemo/m com.nokia.MApplicationIf.ping`
        sleep 1

        verify_equal(true,2,"Application is not Prestarted"){
                system "pidof #{@appname}"}
        pid = string = `pidof #{@appname}`
        sleep 1

        string = `export DISPLAY=:0; source /tmp/session_bus_address.user;dbus-send --dest=com.nokia.#{@appname} --type="method_call" /org/maemo/m com.nokia.MApplicationIf.launch`
        @app = @sut.application( :name => 'applauncherd.bin' ) 
        @app.MButton( :name => 'CloseButton' ).tap
        newid = string = `pidof #{@appname}`
        verify_true(30,"The application is not prestarted"){pid == newid}
        sleep 1
        system "kill -9 `pidof #{@appname}`"
    end

end
