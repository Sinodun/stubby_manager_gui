/*
 * Copyright 2017 Sinodun Internet Technologies Ltd.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "stubbymanager.h"
#include "ui_stubbymanager.h"
#include "nativenotification.h"
#include "nativestatusbutton.h"
#include "configfilemanager.h"
#include "msgdefs.h"
#include <QDebug>

StubbyManager::StubbyManager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StubbyManager),
    m_stubbyController(0),
    m_stubbyState(DaemonController::Unknown),
    m_dnsServerController(0),
    m_dnsServerState(StubbySetdns::Unknown),
    m_dnsServerIsStubby(false),
    m_dnsServerDoesNotMatchCheckbox(false),
    m_editConfig(0),
    m_configChanged(false),
    m_statusButton(0),
    m_logging(0),
    m_testerProcess(NULL)
{
    ui->setupUi(this);
    m_stubbyController = new DaemonController(this);
    if (m_stubbyController) {
        connect(m_stubbyController, SIGNAL(daemonStateChanged(DaemonController::DaemonState)), this, SLOT(on_stubbyStateChanged(DaemonController::DaemonState)));

        /* Check whether or not Stubby is running.
         * on_StubbyStateChanged() will be called on completion,
         * the "Running"/"Not running" widget will be updated,
         * and the stubby dns checkbox may be disabled.*/
        m_stubbyController->checkDaemon();
    }

    m_dnsServerController = new StubbySetdns(this);
    if (m_dnsServerController) {
        connect(m_dnsServerController, SIGNAL(dnsServerStateChanged(int)), this, SLOT(on_dnsServerStateChanged(int)));

        /* Find out if stubby dns is in use.
         * on_dnsServerStateChanged() will be called on completion.
         * the checkbox will be checked/unchecked depending on which servers are in use.
         * Apply and Revert buttons will be disabled (greyed out) */
        m_dnsServerController->getDnsServerState();
    }

    /* Button in the status bar
     * For now, the button is greyed out and updated in the Server check function */
    m_statusButton = new NativeStatusButton();
    m_statusButton->showDisabled(true);

    m_logging = new Logging(this);
    if (m_logging) {
        connect(m_logging, SIGNAL(alert(bool)), this, SLOT(on_alert(bool)));
        m_logging->start();
    }

}

StubbyManager::~StubbyManager()
{
    SMDebug(eDestructors,"");

    /* Discard configuration changes made, but not applied. */
    bool ok;
    ConfigFileManager(this).discardTempConfigFile(&ok);

    delete m_statusButton;
    delete ui;
}

void StubbyManager::on_startButton_clicked()
{
    ui->startButton->setEnabled(false);

    /* Run the launchctl command to load the stubby daemon.*/
    m_stubbyController->startDaemon();

}

void StubbyManager::on_stopButton_clicked()
{
    ui->stopButton->setEnabled(false);
    ui->privacyCheckBox->setEnabled(false);

    /* run the launchctl command to unload the stubby daemon,*/
    if (m_dnsServerState == StubbySetdns::NotStubby) {
        /* in this case, leave service settings as they are. There are No connections with private dns.*/
    } else {
        /* Does this need to be more sophisticated?
         *  a) Could there be a mix of private and non-private dns settings? We switch all to default anyway.
         *  b) Instead of assuming default (ie from DHCP) DNS setting is the alternative to
         *     Private, maybe we should save the setting before switching to Private,
         *     so that at this point we can return to it.
         */
        m_dnsServerController->defaultDns(); //TODO arrange for this to finish BEFORE stopping the daemon.
    }

    /* and also the script to restore default DNS servers */
    m_stubbyController->stopDaemon();

}

void StubbyManager::on_applyButton_clicked()
{
    /* changed configuration doesn't take effect until Stubby has been restarted... */
    if (m_configChanged) {
        bool ok;
        ConfigFileManager(this).applyConfig(&ok);
        if (ok) {
            m_configChanged = false;
        } else {
            SMDebug(eConfiguration, "Error applying configuration");
        }

        m_stubbyController->restartDaemon();

    } //TODO: make sure this finishes before changing the servers to private...or does it matter?

    /* if the privacy checkbox is true then run the script to switch to the private DNS servers
     * if the privacy checkbox is false then run the script to switch to the default DNS server
     */
    if (m_dnsServerDoesNotMatchCheckbox) {
        m_dnsServerDoesNotMatchCheckbox = false;
        if (ui->privacyCheckBox->isChecked()) {
            m_dnsServerController->stubbyDns();
        } else {
            m_dnsServerController->defaultDns();
        }
    }
    //TODO Check that the change has happened

    bool ok;
    bool strict = ConfigFileManager().strictAuthentication(&ok);
    m_statusButton->showDisabled(!strict || !m_dnsServerIsStubby);

    updateWidgetVisibility();

}

void StubbyManager::on_advancedButton_clicked()
{
    /* open the stubby.conf edit window */
    if (!m_editConfig) {
        m_editConfig = new EditConfig(this);
        connect(m_editConfig, SIGNAL(doneEditing(bool)), this, SLOT(on_doneEditing(bool)));
        connect(m_editConfig, SIGNAL(newStatusMessage(QString)), this, SLOT(on_newStatusMessage(QString)));
    }
    m_editConfig->show();
}

void StubbyManager::on_privacyCheckBox_clicked()
{
    m_dnsServerDoesNotMatchCheckbox = (m_dnsServerIsStubby != ui->privacyCheckBox->checkState());
    updateWidgetVisibility();
}

void StubbyManager::on_logButton_clicked()
{
    m_logging->show();
}

void StubbyManager::on_stubbyStateChanged(DaemonController::DaemonState state)
{
    m_stubbyState = state;

    ui->status->setText(stubbyState(state));

    updateWidgetVisibility();
}

void StubbyManager::on_doneEditing(const bool changesExist)
{
    SMDebug(eConfiguration, "");
    m_configChanged = changesExist;
    updateWidgetVisibility();
    delete m_editConfig;
    m_editConfig = 0;
}

void StubbyManager::on_newStatusMessage(const QString msg)
{
    statusBar()->showMessage(msg, 5000); //Message will displayed for 5s
}

void StubbyManager::on_dnsServerStateChanged(int state) //StubbySetdns::DnsServerState
{
    SMDebug(eDnsServer, "%s", ((state == 1) ? "Using stubby dns" : "Not using stubby dns"));

    m_dnsServerState = (StubbySetdns::DnsServerState)state;
    m_dnsServerIsStubby = m_dnsServerState == StubbySetdns::Stubby;

    updateWidgetVisibility();

    /*..ok to do this the first time we check status, but afterwards?....
     *
     * After changing to/from stubby dns? It's ok:
     *        the action was trigger by clicking apply button, so state should match checkbox.
     * After stop button clicked? It's ok:
     *        Automatically switches to default servers before stopping stubby and disabling the checkbox.
     *        As the checkbox will be disabled, it makes sense for its state to match the actual state.
     * During restart, which happens as a result of Apply if the configuration has changed?
     *      ??????
     */
    ui->privacyCheckBox->setChecked(state == (int)StubbySetdns::Stubby);
    bool ok;
    bool strict = ConfigFileManager().strictAuthentication(&ok);
    m_statusButton->showDisabled(!strict || !m_dnsServerIsStubby);

}

/* alerts come from logging:
 *   the logging object checks for errors indicating that stubby dns queries are failing.
 *   if they are, it emits an alert - which is picked up here.
 *   an alert is also emitted when the problem goes away.
 */
void StubbyManager::on_alert(bool on)
{
    if (on) {
        /* display alert */
        SMDebug(eGeneric, "!!!Stubby failing!!!");
        statusBar()->showMessage(tr("Stubby is failing - look at the log."));
#ifdef Q_OS_MACOS
        QtMac::setBadgeLabelText("alert!");

        NativeNotification notification;
        notification.postNotification("Stubby", "Alert!");

#endif
    } else {
        /* clear alert */
        SMDebug(eGeneric, "!!!Stubby has recovered!!!");
        statusBar()->clearMessage();
#ifdef Q_OS_MACOS
        QtMac::setBadgeLabelText(QString());

#endif
    }
}

void StubbyManager::on_revertToDefaultButton_clicked()
{
    bool ok;
    ConfigFileManager(this).saveDefaultToTempConfig(&ok);
    if (!ok)
        SMDebug(eConfiguration, "Error occurred reverting to default configuration");
    m_configChanged = true;
    updateWidgetVisibility();
}

void StubbyManager::on_revertButton_clicked()
{
    bool ok;
    ConfigFileManager(this).discardTempConfigFile(&ok);
    if (!ok)
        SMDebug(eConfiguration, "Error discarding changes");
    m_configChanged = false;

    if (m_dnsServerDoesNotMatchCheckbox) {
        m_dnsServerDoesNotMatchCheckbox = false;
        ui->privacyCheckBox->setChecked(m_dnsServerIsStubby);
    }

    updateWidgetVisibility();

}

/*
 * Helper functions
 */
QString StubbyManager::stubbyState(const DaemonController::DaemonState state)
{
    switch (state) {
    case DaemonController::NotRunning : return "Not running";
    case DaemonController::Starting   : return "Starting";
    case DaemonController::Stopping   : return "Stopping";
    case DaemonController::Running    : return "Running";
    case DaemonController::Unknown :
    default : return "unknown";
    }
}

void StubbyManager::updateWidgetVisibility()
{
    ui->privacyCheckBox->setEnabled(m_stubbyState == DaemonController::Running);
    ui->startButton->setEnabled(m_stubbyState == DaemonController::NotRunning);
    ui->stopButton->setEnabled(m_stubbyState == DaemonController::Running);
    ui->testButton->setEnabled(m_stubbyState == DaemonController::Running);
    ui->restartButton->setEnabled(m_stubbyState == DaemonController::Running);
    ui->testOutput->clear();

    ui->applyButton->setEnabled(m_configChanged || m_dnsServerDoesNotMatchCheckbox);
    ui->revertButton->setEnabled(m_configChanged || m_dnsServerDoesNotMatchCheckbox);

    if (m_configChanged || m_dnsServerDoesNotMatchCheckbox) {
        //ui->changesLabel->setText("There are changes that need Applying");
        statusBar()->showMessage("There are changes that need Applying");
    } else {
        ui->changesLabel->setText("");
        statusBar()->clearMessage();
    }

}

void StubbyManager::on_testButton_clicked()
{
    ui->testOutput->setText("Testing....");
    if (m_testerProcess == NULL)
        m_testerProcess = new RunTask("dig @127.0.0.1 getdnsapi.net", this);
    connect(m_testerProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(on_testerProcess_finished(int, QProcess::ExitStatus)));
    m_testerProcess->start();
}


void StubbyManager::on_testerProcess_finished(int, QProcess::ExitStatus)
{
    QByteArray stdoutData;
    stdoutData = m_testerProcess->readAllStandardOutput();
    if (stdoutData.isEmpty())
        return;
    if (stdoutData.contains("NOERROR")) {
        qDebug() << __FILE__ << ":" << __FUNCTION__ << "OK";
        ui->testOutput->setText("Test was OK!");
    }
    else {
        ui->testOutput->setText("Test failed.");
    }
}

void StubbyManager::on_restartButton_clicked()
{
    qDebug() << __FILE__ << ":" << __FUNCTION__ << "";
    m_stubbyController->restartDaemon();
}

