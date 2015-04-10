#include "statedeploying.h"
#include "deploy.h"

#include <QTimer>

NBStateDeploying::NBStateDeploying(QObject *parent) : NBState(parent), m_deploy(NULL)
{

}

NBStateDeploying::~NBStateDeploying()
{

}

void NBStateDeploying::run()
{
    if (m_running) {
        emit error();
        return;
    }

    m_running = true;
    m_isError = false;

    if (m_waitTimer != NULL) {
        delete m_waitTimer;
        m_waitTimer = NULL;
    }

    if (m_deploy != NULL) {
        if (m_deploy->isRunning())
            m_deploy->terminate();
        if (!m_deploy->isRunning() || m_deploy->wait()) {

        } else {
            m_isError = true;
            emit fatal();
            return;
        }
        delete m_deploy;
        m_deploy = NULL;
    }

    m_deploy = new NBDeployThread;

    connect(m_deploy, &QThread::finished, this, &NBStateDeploying::deployFinished);

    // start timer before the thread start

    m_waitTimer = new QTimer;
    m_waitTimer->setSingleShot(true);
    m_waitTimer->setInterval(60000);
    connect(m_waitTimer, &QTimer::timeout, this, &NBStateDeploying::timeout);

    m_deploy->start();
}

void NBStateDeploying::deployFinished()
{
    if (m_deploy->succeed) {
        m_running = false;
        emit finished();
    } else {
        m_isError = true;
        emit error();
    }

    m_deploy->deleteLater();
    m_deploy = NULL;
}

void NBStateDeploying::timeout()
{
    m_isError = true;
    //m_running = false;

    if (m_deploy->isRunning())
        m_deploy->terminate();

    emit error();
}
