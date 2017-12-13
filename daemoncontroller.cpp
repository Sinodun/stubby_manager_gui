/*
 * Copyright 2017 Sinodun Internet Technologies Ltd.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "msgdefs.h"
#include <QDebug>
#include <QRegularExpression>

#include "daemoncontroller.h"
#include "stubbymanager.h"

DaemonController::DaemonController(StubbyManager *parent) :
    QObject(parent),
    m_daemonState(DaemonController::Unknown),
    m_manager(parent),
    m_starterProcess(0),
    m_stopperProcess(0),
    m_checkerProcess(0),
    m_checkerProcess_output("")
{
    m_starterProcess = new RunHelperTask("start", RunHelperTask::RIGHT_DAEMON_RUN, QString(), this, parent);
    m_stopperProcess = new RunHelperTask("stop", RunHelperTask::RIGHT_DAEMON_RUN, QString(), this, parent);
    m_checkerProcess = new RunHelperTask("list", QString(), QString(), this, parent);

    connect(m_starterProcess, SIGNAL(readyReadStandardError()), this, SLOT(on_starterProcess_readyReadStderr()));
    connect(m_starterProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(on_starterProcess_readyReadStdout()));
    connect(m_starterProcess, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(on_starterProcess_errorOccurred(QProcess::ProcessError)));

    connect(m_stopperProcess, SIGNAL(readyReadStandardError()), this, SLOT(on_stopperProcess_readyReadStderr()));
    connect(m_stopperProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(on_stopperProcess_readyReadStdout()));
    connect(m_stopperProcess, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(on_stopperProcess_errorOccurred(QProcess::ProcessError)));

    connect(m_checkerProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(on_checkerProcess_finished(int, QProcess::ExitStatus)));
    connect(m_checkerProcess, SIGNAL(readyReadStandardError()), this, SLOT(on_checkerProcess_readyReadStderr()));
    connect(m_checkerProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(on_checkerProcess_readyReadStdout()));
    connect(m_checkerProcess, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(on_checkerProcess_errorOccurred(QProcess::ProcessError)));

}

DaemonController::~DaemonController()
{
    SMDebug(eDestructors,"");

}

int DaemonController::startDaemon()
{
    if (daemonState() == Unknown || daemonState() == NotRunning) {
        connect(m_starterProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(on_starterProcess_finished(int, QProcess::ExitStatus)));
        setDaemonState(Starting);
        m_starterProcess->start();
    }
    return 0;
}

int DaemonController::stopDaemon()
{
    if (daemonState() == Unknown || daemonState() == Running) {
        connect(m_stopperProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(on_stopperProcess_finished(int, QProcess::ExitStatus)));
        setDaemonState(Stopping);
        m_stopperProcess->start();
    }
    return 0;
}

int DaemonController::restartDaemon()
{
    if (daemonState() == Unknown || daemonState() == Running) {
        setDaemonState(Stopping);
        connect(m_stopperProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(on_restartStopperProcess_finished(int, QProcess::ExitStatus)));
        m_stopperProcess->start();
    }
    return 0;
}

int DaemonController::checkDaemon()
{
    m_checkerProcess_output = "";
    m_checkerProcess->start();
    return 0;
}

/* private slots to handle QProcess signals */

void DaemonController::on_starterProcess_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    SMDebug(eDaemon, "Exit status %d, launchstl exit code %d", exitStatus, exitCode);
    disconnect(m_starterProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(on_starterProcess_finished(int, QProcess::ExitStatus)));

    checkDaemon();
}

void DaemonController::on_starterProcess_readyReadStderr()
{

}

void DaemonController::on_starterProcess_readyReadStdout()
{

}

void DaemonController::on_starterProcess_errorOccurred(QProcess::ProcessError)
{

}

void DaemonController::on_restartStarterProcess_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    SMDebug(eDaemon, "Exit status %d, launchstl exit code %d", exitStatus, exitCode);
    disconnect(m_starterProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(on_restartStarterProcess_finished(int, QProcess::ExitStatus)));

    checkDaemon();
}

void DaemonController::on_stopperProcess_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    SMDebug(eDaemon, "Exit status %d, launchstl exit code %d", exitStatus, exitCode);
    disconnect(m_stopperProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(on_stopperProcess_finished(int, QProcess::ExitStatus)));

    checkDaemon();
}

void DaemonController::on_stopperProcess_readyReadStderr()
{

}

void DaemonController::on_stopperProcess_readyReadStdout()
{

}

void DaemonController::on_stopperProcess_errorOccurred(QProcess::ProcessError)
{

}

void DaemonController::on_restartStopperProcess_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    SMDebug(eDaemon, "Exit status %d, launchstl exit code %d", exitStatus, exitCode);

    disconnect(m_stopperProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(on_restartStopperProcess_finished(int, QProcess::ExitStatus)));

    connect(m_starterProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(on_restartStarterProcess_finished(int, QProcess::ExitStatus)));
    m_starterProcess->start();

}

void DaemonController::on_checkerProcess_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    SMDebug(eDaemon, "Exit status %d, launchstl exit code %d", exitStatus, exitCode);

    if (exitStatus == QProcess::NormalExit) {
        /* search for "PID" = integer in the text */
        QRegularExpression pidRegEx("\"PID\" = \\d+;");
        QRegularExpressionMatch match;
        match = pidRegEx.match(m_checkerProcess_output);
        if (match.hasMatch()) {
            SMDebug(eDaemon, "matched string is %s", match.captured().toLatin1().data());
            m_daemonState = Running;
            emit daemonStateChanged(m_daemonState);
        } else {
            m_daemonState = NotRunning;
            emit daemonStateChanged(m_daemonState);
        }
    } else {
        m_daemonState = Unknown;
        emit daemonStateChanged(m_daemonState);
    }
}

void DaemonController::on_checkerProcess_readyReadStderr()
{

}

void DaemonController::on_checkerProcess_readyReadStdout()
{
    m_checkerProcess_output = m_checkerProcess_output + QString::fromLatin1(m_checkerProcess->readAllStandardOutput().data());
    //SMDebug(eDaemon, "Output from launchctl list - %s", m_checkerProcess_output.toLatin1().data());
}

void DaemonController::on_checkerProcess_errorOccurred(QProcess::ProcessError)
{

}

