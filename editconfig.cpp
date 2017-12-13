/*
 * Copyright 2017 Sinodun Internet Technologies Ltd.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "editconfig.h"
#include "ui_editconfig.h"
#include "configfilemanager.h"
#include "msgdefs.h"
#include <QDebug>

EditConfig::EditConfig(QWidget *parent) :
    QDialog(parent, Qt::Sheet),
    ui(new Ui::EditConfig),
    m_fileName(""),
    m_defaultFileName("")
{
    ui->setupUi(this);
    setSizeGripEnabled((true));
    SMDebug(eConfiguration, "sizeGripEnabled is %s", (isSizeGripEnabled() ? "true" : "false"));

    connect(this, SIGNAL(rejected()), this, SLOT(on_rejected()));

    bool ok;

    /* Open the most recently edited configuration*/
    QString configText = ConfigFileManager().getLastSavedConfig(&ok);
    if (ok) {
        ui->plainTextEdit->setPlainText(configText);
    } else {
        ui->plainTextEdit->setPlainText("Error opening config file");
    }
}

EditConfig::~EditConfig()
{
    SMDebug(eDestructors,"");
    delete ui;
}

void EditConfig:: on_rejected()
{
    SMDebug(eConfiguration, "");

    /*there may be changes from earlier*/
    emit doneEditing(ConfigFileManager().tempConfigFileExists());
}

void EditConfig::on_saveButton_clicked()
{
    SMDebug(eConfiguration, "");

    bool ok;

    QString text = ui->plainTextEdit->toPlainText();
    if (ConfigFileManager().isValidConfigText(text, &ok) && ok) {
        ConfigFileManager().saveTempConfig(text, &ok); //saves to temporary file, overwriting if one already exists.
        qDebug() << __FILE__ << ":" << __FUNCTION__ << ": config saved to temp file";
        if (ConfigFileManager().getRuntimeConfig(&ok) == text) {
            ConfigFileManager().discardTempConfigFile(&ok);
            emit doneEditing(false);
        } else {
            emit doneEditing(true);
        }
    } else {
        SMWarning("Edited configuration could not be validated - it was not saved");
        emit newStatusMessage("Edited configuration could not be validated - it was not saved");

        ui->validateLabel->setText("Not valid - config cannot be saved.");
    }
}

void EditConfig::on_validateButton_clicked()
{
    bool ok;
    if (ConfigFileManager().isValidConfigText(ui->plainTextEdit->toPlainText(), &ok) && ok) {
        ui->validateLabel->setText("Valid!");
    } else {
        if (ok) {
            ui->validateLabel->setText("Not valid");
        } else {
            ui->validateLabel->setText("Error");
            SMDebug(eConfiguration, "Error during validation");
        }
    }
}
