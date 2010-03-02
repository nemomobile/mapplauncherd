#include <DuiApplication>
#include <DuiApplicationPage>
#include <DuiApplicationWindow>
#include <QDebug>

#ifdef HAVE_DUI
#include <duicomponentcache.h>
#endif


int main(int argc, char ** argv)
{
    #ifdef HAVE_DUI
    DuiApplication *app = DuiComponentCache::duiApplication(argc, argv);
    #else
    DuiApplication *app = new DuiApplication(argc, argv);
    #endif
    DuiApplicationPage mainPage;
    DuiApplicationWindow window;

    window.show();

    mainPage.setTitle("Hello World! (Now supports Launcher)");
    mainPage.appearNow();
  
    return app->exec();
}
