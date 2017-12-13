/*
 * Copyright 2017 Sinodun Internet Technologies Ltd.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef LOGGING_H
#define LOGGING_H

#include <QWidget>
#include "runtask.h"

namespace Ui {
class Logging;
}

class Logging : public QWidget
{
    Q_OBJECT

public:
    explicit Logging(QWidget *parent = 0);
    ~Logging();

    void start();

signals:
    void alert(bool on);

private slots:
    void on_started();
    void on_finished(int exitCode);
    void on_readyRead();

private:
    Ui::Logging *ui;
    RunTask *m_logger;
    bool m_isRunning;
};

#endif // LOGGING_H
