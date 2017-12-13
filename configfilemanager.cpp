/*
 * Copyright 2017 Sinodun Internet Technologies Ltd.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "configfilemanager.h"
#include <QFile>
#include "QTextStream"
#include "runtask.h"
#include "msgdefs.h"
#include <QDebug>


const QString CONFIG_RUNTIME_FILE_NAME = "/usr/local/etc/stubby/stubby.yml";         // Runtime file used by stubby when it runs
const QString CONFIG_DEFAULT_FILE_NAME = "/usr/local/etc/stubby/stubby.yml.example"; // Example file used to revert to defaults
const QString CONFIG_TEMP_FILE_NAME =    "/tmp/stubby.temp";                         // File used to hold changes made in the edit window.
                                                                                     // Only applied when user hits 'Apply' in the main window.
const QString CONFIG_VAL_FILE_NAME =     "/tmp/stubby.yml.validation";               // File used purely to check if the config if valid using check_config

/*
 * public member functions
 */
ConfigFileManager::ConfigFileManager(QObject *parent) :
    m_parent(parent)
{
}

/*
 * check the current config file for GETDNS_AUTHENTICATION_REQUIRED.
 */
bool ConfigFileManager::strictAuthentication(bool *ok)
{
    QString fileText = getRuntimeConfig(ok);
    if (!*ok) return false;

    //TODO: Add function to ui helper to determine this
    if (fileText.contains(QRegExp("\\ntls_authentication:\\s*GETDNS_AUTHENTICATION_REQUIRED"))) {
        return true;
    } else {
        return false;
    }
}

/*
 * set strict authentication
 */
void ConfigFileManager::setStrictAuthentication(bool *ok)
{
    QString fileText = getRuntimeConfig(ok);
    if (!*ok) return;

    if (fileText.contains(QRegExp("\\ntls_authentication:\\s*GETDNS_AUTHENTICATION_REQUIRED"))) {
        *ok = true;
        return; /* nothing to do */
    } else {
        fileText.insert(fileText.lastIndexOf("}"), ", tls_authentication: GETDNS_AUTHENTICATION_NONE\n");
        saveTempConfig(fileText, ok);
    }
    *ok = false;
}

/*
 * set opportunistic authentication
 */
void ConfigFileManager::setOpportunisticAuthentication(bool *ok)
{
    QString fileText = getRuntimeConfig(ok);
    if (!*ok) return;
    *ok = false;

}

/*
 * get the contents of the config (from the current file)
 */
QString ConfigFileManager::getRuntimeConfig(bool *ok)
{
    return getConfigFromFile(CONFIG_RUNTIME_FILE_NAME, ok);
}

/*
 * save config text (in a temporary file)
 */
void ConfigFileManager::saveTempConfig(const QString text, bool *ok)
{
    /*if a temp config file already exists, delete it*/
    deleteFile(CONFIG_TEMP_FILE_NAME, ok);
    if (!*ok)
        SMDebug(eConfiguration, "Error deleting %s", CONFIG_TEMP_FILE_NAME.toLatin1().data());

    saveConfigTextToFile(CONFIG_TEMP_FILE_NAME, text, ok);
}

/*
 * Most recent saved config - ie,
 * if temporary file exists, read from that, otherwise read from the current file.
 */
QString ConfigFileManager::getLastSavedConfig(bool *ok)
{
    QFile file(CONFIG_TEMP_FILE_NAME);

    if (file.exists()) {
        return getConfigFromFile(CONFIG_TEMP_FILE_NAME, ok);
    } else {
        return getRuntimeConfig(ok);
    }
}

/*
 * revert Config to default
 */
void ConfigFileManager::saveDefaultToTempConfig(bool *ok)
{
    QString text = getConfigFromFile(CONFIG_DEFAULT_FILE_NAME, ok);
    if (*ok) {
        saveConfigTextToFile(CONFIG_TEMP_FILE_NAME, text, ok);
    }
}

/*
 * discard latest changes, i.e. delete the temporary file.
 * (After doing this, will also need to restore edit text to the current config)
 */
void ConfigFileManager::discardTempConfigFile(bool *ok)
{
    deleteFile(CONFIG_TEMP_FILE_NAME, ok);
}

/*
 * changes exist? ie temporary file exists?
 */
bool ConfigFileManager::tempConfigFileExists()
{
    return QFile(CONFIG_TEMP_FILE_NAME).exists();
}

/*
 * validate the configuration. Write the config to a temp file and parse with getdns_query. Delete the file afterwards.
 * Return code is 0 if file validated without problem, ok is true if validation occured (no error in process).
 */
bool ConfigFileManager::isValidConfigText(QString text, bool *ok)
{
    saveConfigTextToFile(CONFIG_VAL_FILE_NAME, text, ok);
    if (!*ok) {
        SMDebug(eConfiguration, "Error saving configuration to %s before validation", CONFIG_VAL_FILE_NAME.toLatin1().data());
        return false;
    }

    RunHelperTask validationProcess("check_config", QString(), CONFIG_VAL_FILE_NAME, m_parent);
    int exit_code = validationProcess.execute();

    if (!QFile(CONFIG_VAL_FILE_NAME).remove())
        qDebug() << __FILE__ << ":" << __FUNCTION__ << ": Error clearing up after parse";

    *ok = ((exit_code == 0) || exit_code == 1);
    return (exit_code == 0);
}

/*
 * apply the configuration - i.e. overwrite the current file with the temporary file.
 */
void ConfigFileManager::applyConfig(bool *ok)
{
    if (QFile(CONFIG_TEMP_FILE_NAME).exists()) {
        QString text = getConfigFromFile(CONFIG_TEMP_FILE_NAME, ok);
        if (*ok) {
            //saveConfigTextToFile(CONFIG_RUNTIME_FILE_NAME, text, ok);
            RunHelperTask applyconfigProcess("write_config", RunHelperTask::RIGHT_DAEMON_RUN, CONFIG_TEMP_FILE_NAME, m_parent);
            int exit_code = applyconfigProcess.execute();
            if (*ok && exit_code) deleteFile(CONFIG_TEMP_FILE_NAME, ok);
        }
    } else {
        /*nothing to do*/
        *ok = true;
    }
}

/*
 *  Private member functions
 */
QString ConfigFileManager::getConfigFromFile(const QString filename, bool *ok)
{
    QFile file(filename);

    if (!file.exists()) {
        SMDebug(eConfiguration, "%s does not exist", filename.toLatin1().data());
        *ok = false;
        return QString();
    }

    if (!file.open(QFile::ReadOnly|QFile::Text)) {
        SMDebug(eConfiguration, "Error opening %s", filename.toLatin1().data());
        *ok = false;
        return QString();
    }

    QTextStream in(&file);
    *ok = true;
    return in.readAll();
}

void ConfigFileManager::saveConfigTextToFile(const QString filename, const QString text, bool *ok)
{
    QFile file(filename);
    if (file.open(QFile::WriteOnly|QFile::Text)) {
        QTextStream out(&file);
        out << text;
        out.flush();
        *ok = true;
    } else {
        *ok = false;
    }
}

void ConfigFileManager::deleteFile(const QString filename, bool *ok)
{
    QFile file(filename);

    if (!file.exists()) {
        *ok = true; //it's ok if there's no file - nothing to discard
        return;
    }

    *ok = file.remove();
}
