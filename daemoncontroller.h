/*
 * Copyright 2017 Sinodun Internet Technologies Ltd.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef DAEMONCONTROLLER_H
#define DAEMONCONTROLLER_H

#include <QObject>

#include "runtask.h"

class DaemonController : public QObject
{
    Q_OBJECT

public:
    DaemonController(StubbyManager *parent);
    virtual ~DaemonController();

    typedef enum {
        NotRunning,
        Starting,
        Stopping,
        Running,
        Unknown
    } DaemonState;

    int startDaemon();
    int stopDaemon();
    int restartDaemon();
    int checkDaemon();
    DaemonState daemonState() { return m_daemonState; }

private:
    void setDaemonState(const DaemonState state) { m_daemonState = state; }

public:
signals:
    void daemonStateChanged(DaemonController::DaemonState);

private slots:
    void on_starterProcess_finished(int, QProcess::ExitStatus);
    void on_starterProcess_readyReadStderr();
    void on_starterProcess_readyReadStdout();
    void on_starterProcess_errorOccurred(QProcess::ProcessError);
    void on_restartStarterProcess_finished(int exitCode, QProcess::ExitStatus exitStatus);

    void on_stopperProcess_finished(int, QProcess::ExitStatus);
    void on_stopperProcess_readyReadStderr();
    void on_stopperProcess_readyReadStdout();
    void on_stopperProcess_errorOccurred(QProcess::ProcessError);
    void on_restartStopperProcess_finished(int exitCode, QProcess::ExitStatus exitStatus);

    void on_checkerProcess_finished(int, QProcess::ExitStatus);
    void on_checkerProcess_readyReadStderr();
    void on_checkerProcess_readyReadStdout();
    void on_checkerProcess_errorOccurred(QProcess::ProcessError);

private:
    DaemonState m_daemonState;
    StubbyManager *m_manager;
    RunHelperTask *m_starterProcess;
    RunHelperTask *m_stopperProcess;
    RunHelperTask *m_checkerProcess;
    QString m_checkerProcess_output;
};

#endif // DAEMONCONTROLLER_H
