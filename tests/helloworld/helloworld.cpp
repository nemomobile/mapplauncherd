/*
 * helloworld.cpp
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

#include <MApplication>
#include <MApplicationPage>
#include <MApplicationWindow>
#include <QDebug>

#ifdef HAVE_MCOMPONENTCACHE
#include <mcomponentcache.h>
#endif

extern "C" __attribute__((visibility("default"))) int main(int, char**);

int main(int argc, char ** argv)
{
    #ifdef HAVE_MCOMPONENTCACHE
    MApplication *app = MComponentCache::mApplication(argc, argv);
    #else
    MApplication *app = new MApplication(argc, argv);
    #endif
    MApplicationPage mainPage;
    MApplicationWindow window;

    window.show();

    mainPage.setTitle("Hello World! (Now supports Launcher)");
    mainPage.appear();
  
    return app->exec();
}
