/*
 * Copyright 2017 Sinodun Internet Technologies Ltd.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "logging.h"
#include "ui_logging.h"
#include "msgdefs.h"
#include <QDebug>

Logging::Logging(QWidget *parent) :
    QWidget(parent, Qt::Window), //Without Qt::Window this widget opens over the top of contents of the window that owns the object.
    ui(new Ui::Logging),
    m_logger(0),
    m_isRunning(false)
{
    ui->setupUi(this);
    m_logger = new RunTask("tail -f /var/log/stubby.log", this);
    connect(m_logger, SIGNAL(started()), this, SLOT(on_started()));
    connect(m_logger, SIGNAL(finished(int)), this, SLOT(on_finished(int)));
    connect(m_logger, SIGNAL(readyReadStandardOutput()), this, SLOT(on_readyRead()));
}

Logging::~Logging()
{
    SMDebug(eDestructors,"");
    delete ui;
}

void Logging::start()
{
    if (m_logger) m_logger->start();
}

void Logging::on_started()
{
    m_isRunning = true;
}

void Logging::on_finished(int exitCode)
{
    SMDebug(eLogging, "Exit code is %d", exitCode);
    m_isRunning = false;
}

void Logging::on_readyRead()
{
    QByteArray stdoutData;
    stdoutData = m_logger->readAllStandardOutput();

    static bool isAlert = false;
    static QByteArray incompleteLine;
    incompleteLine.append(stdoutData);
    QList<QByteArray> lines = incompleteLine.split('\n');
    int n = lines.count();
    for (int i = 0; i < n-1; i++) {
        //SMDebug(eLogging, "Line %d: %s", i, lines[i].data());
        /*now parse this line of data - for now just look for indication of failure or success*/
        if (lines[i].contains("*FAILURE* no valid transports or upstreams available!")) {
            if (!isAlert) emit alert(true);
            isAlert = true;
        }
        // TODO: This won't mean a successfull connection, just a retry.
        // There's no text that works for Strict and Oppo at the moment.....
        if (lines[i].contains("Conn opened: ")) {
            if (isAlert) emit alert(false);
            isAlert = false;
        }
    }
    incompleteLine = lines[n-1]; //save the remnant so it can be processed next time
    //SMDebug(eLogging, "Last line is %s", incompleteLine.data());

    ui->textEdit->moveCursor (QTextCursor::End);
    ui->textEdit->insertPlainText (QString::fromLatin1(stdoutData.data()));
    ui->textEdit->moveCursor (QTextCursor::End);

}
