/*
 * Copyright 2017 Sinodun Internet Technologies Ltd.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef STUBBYMANAGER_H
#define STUBBYMANAGER_H

#include <QMainWindow>

#ifdef Q_OS_MACOS
#include <QtMac>
#endif
#include "nativestatusbutton.h"

class StubbyManager;

#include "daemoncontroller.h"
#include "stubbysetdns.h"
#include "logging.h"
#include "editconfig.h"

namespace Ui {
class StubbyManager;
}

class StubbyManager : public QMainWindow
{
    Q_OBJECT

public:
    explicit StubbyManager(QWidget *parent = 0);
    ~StubbyManager();

private slots:
    /* automatic slots for ui controls */
    void on_startButton_clicked();

    void on_stopButton_clicked();

    void on_applyButton_clicked();

    void on_advancedButton_clicked();

    void on_privacyCheckBox_clicked();

    void on_logButton_clicked();

    void on_revertToDefaultButton_clicked();

    void on_revertButton_clicked();

    void on_testerProcess_finished(int, QProcess::ExitStatus);

    /* slots for handling signals from daemon controller and config editor */
    void on_stubbyStateChanged(DaemonController::DaemonState state);

    void on_doneEditing(const bool changesExist);

    void on_dnsServerStateChanged(int state); //StubbySetdns::DnsServerState

    void on_alert(bool on);

    void on_testButton_clicked();

    void on_restartButton_clicked();

    void on_newStatusMessage(const QString msg);

private:
    QString stubbyState(const DaemonController::DaemonState state);
    void updateWidgetVisibility();

private:
    Ui::StubbyManager *ui;

    DaemonController *m_stubbyController;
    DaemonController::DaemonState m_stubbyState;

    StubbySetdns * m_dnsServerController;
    StubbySetdns::DnsServerState m_dnsServerState;
    bool m_dnsServerIsStubby;
    bool m_dnsServerDoesNotMatchCheckbox;

    EditConfig *m_editConfig;
    bool m_configChanged;
    NativeStatusButton *m_statusButton;

    Logging *m_logging;
    RunTask *m_testerProcess;

};

#endif // STUBBYMANAGER_H
