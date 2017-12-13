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

#include "stubbysetdns.h"

#include "stubbymanager.h"

StubbySetdns::StubbySetdns(StubbyManager *parent) :
    QObject(parent),
    m_dnsState(StubbySetdns::Unknown),
    //m_dnsState(false),
    m_manager(parent),
    m_stubbyDns(0),
    m_defaultDns(0),
    m_getDnsServerState(0),
    m_getDnsServerState_output("")
{
    m_stubbyDns = new RunHelperTask("dns_stubby", RunHelperTask::RIGHT_DNS_LOCAL, QString(), this, parent);
    m_defaultDns = new RunHelperTask("dns_default", RunHelperTask::RIGHT_DNS_LOCAL, QString(), this, parent);
    m_getDnsServerState = new RunHelperTask("dns_list", QString(), QString(), this, parent);

    connect(m_stubbyDns, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(on_stubbyDns_finished(int,QProcess::ExitStatus)));
    connect(m_stubbyDns, SIGNAL(readyReadStandardError()), this, SLOT(on_stubbyDns_readyReadStderr()));
    connect(m_stubbyDns, SIGNAL(readyReadStandardOutput()), this, SLOT(on_stubbyDns_readyReadStdout()));
    connect(m_stubbyDns, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(on_stubbyDns_errorOccurred(QProcess::ProcessError)));

    connect(m_defaultDns, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(on_defaultDns_finished(int, QProcess::ExitStatus)));
    connect(m_defaultDns, SIGNAL(readyReadStandardError()), this, SLOT(on_defaultDns_readyReadStderr()));
    connect(m_defaultDns, SIGNAL(readyReadStandardOutput()), this, SLOT(on_defaultDns_readyReadStdout()));
    connect(m_defaultDns, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(on_defaultDns_errorOccurred(QProcess::ProcessError)));

    connect(m_getDnsServerState, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(on_getDnsServerState_finished(int, QProcess::ExitStatus)));
    connect(m_getDnsServerState, SIGNAL(readyReadStandardError()), this, SLOT(on_getDnsServerState_readyReadStderr()));
    connect(m_getDnsServerState, SIGNAL(readyReadStandardOutput()), this, SLOT(on_getDnsServerState_readyReadStdout()));
    connect(m_getDnsServerState, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(on_getDnsServerState_errorOccurred(QProcess::ProcessError)));
}

StubbySetdns::~StubbySetdns()
{
    SMDebug(eDestructors,"");
}

int StubbySetdns::stubbyDns()
{
    m_stubbyDns->start();
    return 0;
}

int StubbySetdns::defaultDns()
{
    m_defaultDns->start();
    return 0;
}

int StubbySetdns::getDnsServerState()
{
    m_getDnsServerState_output = "";
    m_getDnsServerState->start();
    return 0;
}

void StubbySetdns::on_stubbyDns_finished(int exitCode, QProcess::ExitStatus)
{
    SMDebug(eDnsServer, "Exit code is %d", exitCode);
    getDnsServerState();
}

void StubbySetdns::on_stubbyDns_readyReadStderr()
{

}

void StubbySetdns::on_stubbyDns_readyReadStdout()
{

}

void StubbySetdns::on_stubbyDns_errorOccurred(QProcess::ProcessError)
{

}

void StubbySetdns::on_defaultDns_finished(int exitCode, QProcess::ExitStatus)
{
    SMDebug(eDnsServer, "Exit code is %d", exitCode);
    getDnsServerState();
}

void StubbySetdns::on_defaultDns_readyReadStderr()

{

}

void StubbySetdns::on_defaultDns_readyReadStdout()
{

}

void StubbySetdns::on_defaultDns_errorOccurred(QProcess::ProcessError)
{

}

void StubbySetdns::on_getDnsServerState_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    SMDebug(eDnsServer, "Exit code is %d", exitCode);
    /*
     * $ sudo ./stubby-setdns.sh -l
     * ** Current DNS settings **
     * Apple USB Ethernet Adapter:    127.0.0.1 ::1
     * Wi-Fi:                         127.0.0.1 ::1
     * Bluetooth PAN:                 127.0.0.1 ::1
     * $ sudo ./stubby-setdns.sh
     * ** Current DNS settings ** -l
     * Apple USB Ethernet Adapter:    There aren't any DNS Servers set on Apple USB Ethernet Adapter.
     * Wi-Fi:                         There aren't any DNS Servers set on Wi-Fi.
     * Bluetooth PAN:                 There aren't any DNS Servers set on Bluetooth PAN.
     * $
     */

     if (exitStatus == QProcess::NormalExit) {
        QStringList slist = m_getDnsServerState_output.split("\n");
        SMDebug(eDnsServer, "Output from stdout is\n%s", m_getDnsServerState_output.toLatin1().data());
        //SMDebug(eDnsServer, "Number of lines in dns server state output is %d", slist.length());
        int numberOfConnectors = slist.length() - 2;
        bool isStubby[numberOfConnectors];
        bool isNotStubby[numberOfConnectors];

        QString localPID("127.0.0.1 ::1");
        int i = 1;
        while (i <= numberOfConnectors) {
            if (slist[i].contains(&localPID)) {
                isStubby[i-1] = true;
                isNotStubby[i-1] = false;
            } else {
                isStubby[i-1] = false;
                isNotStubby[i-1] = true;
            }
            i++;
        }

        /* if all isStubby are true then stubbyDNS is place */
        i = 0;
        while ((i < numberOfConnectors) && isStubby[i]) { i++; }
        if (i==numberOfConnectors) {
            m_dnsState = Stubby;
            emit dnsServerStateChanged((int)Stubby);
            SMDebug(eDnsServer, "All connections are using Stubby");
            return;
        }

        /* if all isNotStubby are true then local IP is not being used */
        i = 0;
        while ((i < numberOfConnectors) && isNotStubby[i]) { i++; }
        if (i==numberOfConnectors) {
            m_dnsState = NotStubby;
            emit dnsServerStateChanged((int)NotStubby);
            SMDebug(eDnsServer, "No connections are using Stubby");
             return;
        }

        /* otherwhise, the true state is unknown.*/
     }
     m_dnsState = Unknown;
     emit dnsServerStateChanged((int)Unknown);
     SMDebug(eDnsServer, "Error - dns server use of Stubby is unknown");
}

void StubbySetdns::on_getDnsServerState_readyReadStderr()
{

}

void StubbySetdns::on_getDnsServerState_readyReadStdout()

{
    m_getDnsServerState_output = m_getDnsServerState_output + QString::fromLatin1(m_getDnsServerState->readAllStandardOutput().data());
    //SMDebug(eDnsServer, "Output from stdout is\n%s", m_getDnsServerState_output.toLatin1().data());
}

void StubbySetdns::on_getDnsServerState_errorOccurred(QProcess::ProcessError)
{

}
