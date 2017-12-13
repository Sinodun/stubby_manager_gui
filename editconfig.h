/*
 * Copyright 2017 Sinodun Internet Technologies Ltd.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef EDITCONFIG_H
#define EDITCONFIG_H

#include <QDialog>
#include <QFile>

namespace Ui {
class EditConfig;
}

class EditConfig : public QDialog
{
    Q_OBJECT

public:
    explicit EditConfig(QWidget *parent = 0);
    ~EditConfig();

signals:
    void doneEditing(const bool changesExist);
    void newStatusMessage(const QString msg);

private slots:
    void on_rejected();

    void on_validateButton_clicked();
    void on_saveButton_clicked();

private:
    Ui::EditConfig *ui;
    QString m_fileName;
    QString m_defaultFileName;
};

#endif // EDITCONFIG_H
