/*
 * Copyright 2017 Sinodun Internet Technologies Ltd.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "runtask.h"
#include "msgdefs.h"

#include <assert.h>

#include <Security/AuthorizationDB.h>

#include <QDebug>
#include <QRegularExpression>

RunTask::RunTask(const QString command, QObject *parent) :
    QProcess(parent),
    m_command(command)
{
    setProgram(m_command);
}

RunTask::~RunTask()
{
    //SMDebug(eDestructors,"");
    if (state() == Running) {
        terminate();
        waitForFinished();
        SMDebug(eRunTask,"waited for finish");
    }
}

void RunTask::start()
{
    QProcess::start(m_command, ReadWrite);
}

static const char STUBBY_UI_HELPER[] = "/usr/local/sbin/stubby-ui-helper";

const char *RunHelperTask::RIGHT_DAEMON_RUN = "net.getdnsapi.stubby.daemon.run";
const char *RunHelperTask::RIGHT_DNS_LOCAL = "net.getdnsapi.stubby.dns.local";

RunHelperTask::RunHelperTask(const QString command, QString need_right, const QString config, QObject *parent, StubbyManager *manager) :
    QProcess(parent),
    m_command(command),
    m_need_right(need_right),
    m_config(config),
    m_manager(manager)
{
    setProgram(STUBBY_UI_HELPER);

    OSStatus oss = AuthorizationCreate(NULL, NULL, 0, &m_auth_ref);
    assert(oss == errAuthorizationSuccess);
    qDebug() << __FILE__ << ":" << __FUNCTION__ << "Auth create " << (oss == errAuthorizationSuccess);

    // Ensure rights have been created.
    oss = AuthorizationRightGet(RIGHT_DAEMON_RUN, NULL);
    assert(oss == errAuthorizationSuccess);
    qDebug() << __FILE__ << ":" << __FUNCTION__ << "Auth daemon " << (oss == errAuthorizationSuccess);
    oss = AuthorizationRightGet(RIGHT_DNS_LOCAL, NULL);
    assert(oss == errAuthorizationSuccess);
    qDebug() << __FILE__ << ":" << __FUNCTION__ << "Auth dns " << (oss == errAuthorizationSuccess);
}

RunHelperTask::~RunHelperTask()
{
    AuthorizationFree(m_auth_ref, kAuthorizationFlagDefaults);

    qDebug() << __FILE__ << ":" << __FUNCTION__;
    if (state() == Running) {
        terminate();
        waitForFinished();
        qDebug() << __FILE__ << ":" << __FUNCTION__ << "waited for finish";
    }
}

int RunHelperTask::execute()
{
    QString cmd = makeCommandLine();
    if (cmd.isNull()) {
        qDebug() << __FILE__ << ":" << __FUNCTION__ << "auth failed";
        return 1;
    }
    else {
        //qDebug() << __FILE__ << ":" << __FUNCTION__ << "executing " << cmd;
        QProcess::start(cmd);
        if (!waitForStarted()) {
            qDebug() << __FILE__ << ":" << __FUNCTION__ << "Command could not be executed, process not found " << cmd;
            return -2;
        }
        QString ext_auth = makeExternalAuth();
        if (!ext_auth.isEmpty())
            write(ext_auth.toUtf8());
        closeWriteChannel();
        if (!waitForFinished())
            qDebug() << __FILE__ << ":" << __FUNCTION__ << "Waiting for finish failed " << cmd;
        return exitStatus() == NormalExit ? exitCode() : -1;
    }
}

void RunHelperTask::start()
{
    QString cmd = makeCommandLine();
    if (cmd.isNull())
        qDebug() << __FILE__ << ":" << __FUNCTION__ << "auth failed";
    else {
        //qDebug() << __FILE__ << ":" << __FUNCTION__ << "starting " << cmd;

        QProcess::start(cmd);
        QString ext_auth = makeExternalAuth();
        if (!ext_auth.isEmpty())
            write(ext_auth.toUtf8());
        closeWriteChannel();
    }
}

QString RunHelperTask::makeCommandLine()
{
    QString cmd(STUBBY_UI_HELPER);

    if ( !m_need_right.isEmpty() ) {
        AuthorizationItem right_detail[] = {
            { NULL, 0, NULL, 0 },
        };
        AuthorizationRights the_right = { 1, &right_detail[0] };
        OSStatus oss;
        QByteArray needed_right = m_need_right.toUtf8();
        right_detail[0].name = needed_right.constData();

        oss = AuthorizationCopyRights(
                m_auth_ref,
                &the_right,
                kAuthorizationEmptyEnvironment,
                kAuthorizationFlagExtendRights | kAuthorizationFlagInteractionAllowed | kAuthorizationFlagPreAuthorize,
                NULL);
        if (oss != errAuthorizationSuccess)
            return NULL;

        cmd += " -auth - ";
    }

    if ( !m_config.isEmpty() ) {
        cmd += " -config ";
        cmd += m_config;
    }

    cmd += " " + m_command;
    return cmd;
}

QString RunHelperTask::makeExternalAuth()
{
    QString res;
    AuthorizationExternalForm auth_ext_form;

    OSStatus oss = AuthorizationMakeExternalForm(m_auth_ref, &auth_ext_form);
    if ( oss == errAuthorizationSuccess)
        for (size_t i = 0; i < kAuthorizationExternalFormLength; ++i) {
            // Turn into a printable string of hex digits.
            char b = auth_ext_form.bytes[i];
            char c;

            c = (b >> 4) & 0xf;
            res.append(c >= 10 ? 'a' + c - 10 : '0' + c);
            c = b & 0xf;
            res.append(c >= 10 ? 'a' + c - 10 : '0' + c);
        }
    return res;
}
