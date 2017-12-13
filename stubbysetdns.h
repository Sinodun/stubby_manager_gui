/*
 * Copyright 2017 Sinodun Internet Technologies Ltd.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef STUBBYSETDNS_H
#define STUBBYSETDNS_H

#include <QObject>

#include "runtask.h"

class StubbySetdns : public QObject
{
    Q_OBJECT

public:
    StubbySetdns(StubbyManager *parent = 0);
    virtual ~StubbySetdns();

    typedef enum {
        NotStubby = 0,
        Stubby,
        Unknown
    } DnsServerState;

    int stubbyDns();
    int defaultDns();
    int getDnsServerState();
    DnsServerState dnsState() { return m_dnsState; }

public:
signals:
    void dnsServerStateChanged(int); //DnsServerState

private slots:
    void on_stubbyDns_finished(int exitCode, QProcess::ExitStatus);
    void on_stubbyDns_readyReadStderr();
    void on_stubbyDns_readyReadStdout();
    void on_stubbyDns_errorOccurred(QProcess::ProcessError);

    void on_defaultDns_finished(int exitCode, QProcess::ExitStatus);
    void on_defaultDns_readyReadStderr();
    void on_defaultDns_readyReadStdout();
    void on_defaultDns_errorOccurred(QProcess::ProcessError);

    void on_getDnsServerState_finished(int exitCode, QProcess::ExitStatus exitStatus);
    void on_getDnsServerState_readyReadStderr();
    void on_getDnsServerState_readyReadStdout();
    void on_getDnsServerState_errorOccurred(QProcess::ProcessError);

private:
    DnsServerState m_dnsState;
    StubbyManager *m_manager;
    RunHelperTask *m_stubbyDns;
    RunHelperTask *m_defaultDns;
    RunHelperTask *m_getDnsServerState;
    QString m_getDnsServerState_output;
};

#endif // STUBBYSETDNS_H
