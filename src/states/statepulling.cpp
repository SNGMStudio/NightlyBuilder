#include "statepulling.h"
#include "global.h"
#include "log.h"


#include <QTimer>


NBStatePulling::NBStatePulling(QObject *parent) : NBState(parent), m_git(NULL), m_waitTimer(NULL), m_running(false), m_isError(false)
{

}

NBStatePulling::~NBStatePulling()
{
    if (m_waitTimer != NULL) {
        m_waitTimer->stop();
        delete m_waitTimer;
    }

    if (m_git != NULL) {
        disconnect(m_git, (void (QProcess::*)(int, QProcess::ExitStatus))(&QProcess::finished), this, 0);
        disconnect(m_git, (void (QProcess::*)(QProcess::ProcessError))(&QProcess::error), this, 0);
        if (m_git->state() != QProcess::NotRunning)
            m_git->kill();
        if (m_git->state() == QProcess::NotRunning || m_git->waitForFinished()) {

        } else
            GlobalMethod::crash();

        m_git->deleteLater();
    }
}


void NBStatePulling::run()
{
    if (m_running) {
        emit error();
        return;
    }

    m_running = true;
    m_isError = false;

    if (m_waitTimer != NULL)
        m_waitTimer->stop();

    if (m_git != NULL) {
        disconnect(m_git, (void (QProcess::*)(int, QProcess::ExitStatus))(&QProcess::finished), this, 0);
        disconnect(m_git, (void (QProcess::*)(QProcess::ProcessError))(&QProcess::error), this, 0);
        if (m_git->state() != QProcess::NotRunning)
            m_git->kill();
        if (m_git->state() == QProcess::NotRunning || m_git->waitForFinished()) {

        } else {
            // log
            m_isError = true;
            emit fatal();
            return;
        }
        delete m_git;
        m_git = NULL;
    }

    m_git = new QProcess;
    m_git->setWorkingDirectory(NBSettings::ProjectPath);
    m_git->setProgram("git");
    m_git->setArguments(QStringList() << "pull");

    connect(m_git, (void (QProcess::*)(int, QProcess::ExitStatus))(&QProcess::finished), this, &NBStatePulling::processFinished);
    connect(m_git, (void (QProcess::*)(QProcess::ProcessError))(&QProcess::error), this, &NBStatePulling::processError);

    // start timer before the process start

    if (m_waitTimer == NULL)
        m_waitTimer = new QTimer;
    m_waitTimer->setSingleShot(true);
    m_waitTimer->setInterval(3600000);
    connect(m_waitTimer, &QTimer::timeout, this, &NBStatePulling::timeout);
    m_waitTimer->start();

    m_git->start();
}

void NBStatePulling::processError(QProcess::ProcessError)  // designed to handle error, temporily no effect
{
    m_isError = true;
}

void NBStatePulling::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QProcess *p = qobject_cast<QProcess *>(sender());

    if (p == NULL) {
        // log
        m_isError = true;
        emit fatal();
        return;
    }

    if (m_waitTimer != NULL)
        m_waitTimer->stop();

    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        QString stdErr = QString::fromLocal8Bit(p->readAllStandardError());
        Q_UNUSED(stdErr);
        m_isError = true;
        m_running = false;
        //log
        emit error();
        return;
    }

    QString stdOut = QString::fromLocal8Bit(p->readAllStandardOutput());
    QString stdErr = QString::fromLocal8Bit(p->readAllStandardError());

    NBLog logstdOut;
    logstdOut.openLogFile("git-StdOut");
    logstdOut.insertLog(stdOut);
    logstdOut.closeLogFile();
    NBLog logstdErr;
    logstdErr.openLogFile("git-StdErr");
    logstdErr.insertLog(stdErr);
    logstdErr.closeLogFile();

    m_running = false;

    if (!m_isError)
        emit finished();
}

void NBStatePulling::timeout()
{
    m_isError = true;
    //m_running = false;

    if (m_git == NULL) {
        // what the hell?
        return;
    }

    if (m_git->state() != QProcess::NotRunning)
        m_git->kill();

    emit error();
}

void NBStatePulling::shutUp()
{
    if (m_waitTimer != NULL)
        m_waitTimer->stop();

    if (m_git == NULL) {
        // what the fuck??
        return;
    }

    disconnect(m_git, (void (QProcess::*)(int, QProcess::ExitStatus))(&QProcess::finished), this, 0);
    disconnect(m_git, (void (QProcess::*)(QProcess::ProcessError))(&QProcess::error), this, 0);
    if (m_git->state() != QProcess::NotRunning)
        m_git->kill();
    if (m_git->state() == QProcess::NotRunning || m_git->waitForFinished()) {

    } else {
        /*
        // log
        m_isError = true;
        emit fatal();
        return;
        */
    }
    m_git->deleteLater();
    m_git = NULL;

    emit stopped();
}
