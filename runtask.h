/*
 * Copyright 2017 Sinodun Internet Technologies Ltd.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RUNTASK_H
#define RUNTASK_H

#include <Security/Authorization.h>

#include <QObject>
#include <QProcess>

class StubbyManager;

class RunTask : public QProcess
{
    Q_OBJECT

public:
    RunTask(const QString command, QObject *parent = 0);
    virtual ~RunTask();
    void start();

private:
    QString m_command;
};

class RunHelperTask : public QProcess
{
    Q_OBJECT

public:
    RunHelperTask(const QString command, QString need_right = QString(), const QString config = QString(), QObject *parent = 0, StubbyManager *manager = 0);
    virtual ~RunHelperTask();
    int execute();
    void start();

    static const char *RIGHT_DAEMON_RUN;
    static const char *RIGHT_DNS_LOCAL;

private:
    QString makeCommandLine();
    QString makeExternalAuth();

    QString m_command;
    QString m_need_right;
    QString m_config;
    StubbyManager *m_manager;
    AuthorizationRef m_auth_ref;
};

#endif // RUNTASK_H
