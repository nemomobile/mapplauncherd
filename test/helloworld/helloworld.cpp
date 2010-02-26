#include <DuiApplication>
#include <DuiApplicationPage>
#include <DuiApplicationWindow>
//#include <DuiComponentCache>
#include <QDebug>


int main(int argc, char ** argv)
{
    //DuiApplication *app = DuiComponentCache::duiApplication(argc, argv);
    DuiApplication *app = new DuiApplication(argc, argv);
    DuiApplicationPage mainPage;
    DuiApplicationWindow window;

    window.show();

    mainPage.setTitle("Hello World! (Now supports Launcher)");
    mainPage.appearNow();
  
    return app->exec();
}
