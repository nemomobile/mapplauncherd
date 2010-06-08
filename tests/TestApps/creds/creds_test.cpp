#include <MApplication>
#include <MApplicationWindow>
#include <MApplicationPage>
#include <MExport>
#include <MTextEdit>
#include <QStringList>

#ifdef HAVE_MCOMPONENTCACHE
#include <MComponentCache>
#endif

#ifdef HAVE_CREDS
#include <cstdlib>
#include <sys/creds.h>

/**
 * Returns a QStringList filled with credentials the
 * application currently has.
 */
QStringList credentials()
{
    QStringList credList;

    // credentials for the current pid
    creds_t creds;
    if ((creds = creds_gettask(0)) == NULL)
    {
        return credList;
    }

    creds_type_t creds_type;
    creds_value_t creds_value;
    int i = 0;

    // Retrieve all credentials
    while ((creds_type = creds_list(creds, i++, &creds_value)) != CREDS_BAD)
    {
        // Some magic to get the correct buffer length
        int size = creds_creds2str(creds_type, creds_value, NULL, 0);

        char *buf = new char [size+1];
        creds_creds2str(creds_type, creds_value, buf, size+1);

        credList << QString(buf);

        delete [] buf;
    }

    creds_free(creds);

    return credList;
}
#endif // HAVE_CREDS

M_EXPORT int main(int argc, char **argv)
{
#ifdef HAVE_MCOMPONENTCACHE
    MApplication *app = MComponentCache::mApplication(argc, argv);
    MApplicationWindow *win = MComponentCache::mApplicationWindow();
#else
    MApplication *app = new MApplication(argc, argv);
    MApplicationWindow *win = new MApplicationWindow;
#endif

    MApplicationPage page;

    win->show();

    page.setTitle("Applauncherd Aegis test");
    page.appear();

    MTextEdit *edit = new MTextEdit(MTextEditModel::MultiLine);
    edit->setReadOnly(true);

    page.setCentralWidget(edit);

#ifdef HAVE_CREDS
    edit->setText(credentials().join("\n"));
#else
    edit->setText("Credential support not compiled");
#endif

    return app->exec();
}


