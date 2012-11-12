#include <QtConcurrentRun>
#include <QApplication>

#include "coverage.h"
#include "eventhandler.h"
#include "connection.h"
#include "logger.h"
#include "booster.h"
#include <sys/socket.h>

int EventHandler::m_sighupFd[2];
struct sigaction EventHandler::m_oldSigAction;

EventHandler::EventHandler(Booster* parent)
    : m_parent(parent)
{
    m_sighupFd[0] = -1;
    m_sighupFd[1] = -1;
}

EventHandler::~EventHandler()
{
    if (m_sighupFd[0] != -1)
        ::close(m_sighupFd[0]);

    if (m_sighupFd[1] != -1)
        ::close(m_sighupFd[1]);
}

void EventHandler::runEventLoop()
{
    // Exit from event loop when invoker is ready to connect
    connect(this, SIGNAL(connectionAccepted()), QApplication::instance(), SLOT(quit()));
    connect(this, SIGNAL(connectionRejected()), QApplication::instance(), SLOT(quit()));

    // Start another thread to listen connection from invoker
    QtConcurrent::run(this, &EventHandler::accept);

    // Create socket pair for SIGHUP
    bool handlerIsSet = false;
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, m_sighupFd))
    {
        Logger::logError("EventHandler: Couldn't create HUP socketpair");
    }
    else
    {
        // Install signal handler e.g. to exit cleanly if launcher dies.
        // This is a problem because MBooster runs a Qt event loop.
        EventHandler::setupUnixSignalHandlers();

        // Install a socket notifier on the socket
        connect(new QSocketNotifier(m_sighupFd[1], QSocketNotifier::Read, this),
                SIGNAL(activated(int)), this, SLOT(handleSigHup()));

        handlerIsSet = true;
    }

    // Run event loop so application instance can receive notifications
    QApplication::exec();

    // Restore signal handlers to previous values
    if (handlerIsSet)
    {
        restoreUnixSignalHandlers();
    }
}

void EventHandler::accept()
{
    if (m_parent->connection()->accept(m_parent->appData()))
    {
        emit connectionAccepted();
    }
    else
    {
        emit connectionRejected();
    }
}

//
// All this signal handling code is taken from Qt's Best Practices:
// http://doc.qt.nokia.com/latest/unix-signals.html
//

void EventHandler::hupSignalHandler(int)
{
    char a = 1;
    ::write(m_sighupFd[0], &a, sizeof(a));
}

void EventHandler::handleSigHup()
{
#ifdef WITH_COVERAGE
    __gcov_flush();
#endif

    QApplication::quit();
    _exit(EXIT_SUCCESS);
}

bool EventHandler::setupUnixSignalHandlers()
{
    struct sigaction hup;

    hup.sa_handler = hupSignalHandler;
    sigemptyset(&hup.sa_mask);
    hup.sa_flags = SA_RESTART;

    if (sigaction(SIGHUP, &hup, &m_oldSigAction) > 0)
    {
        return false;
    }

    return true;
}

bool EventHandler::restoreUnixSignalHandlers()
{
    if (sigaction(SIGHUP, &m_oldSigAction, 0) > 0)
    {
        return false;
    }

    return true;
}
