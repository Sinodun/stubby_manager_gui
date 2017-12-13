/*
 * Copyright 2017 Sinodun Internet Technologies Ltd.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <QObject>
#include <QString>


class ConfigFileManager
{
public:
    ConfigFileManager(QObject *parent = 0);

    /*
     * check the current config file for GETDNS_AUTHENTICATION_REQUIRED.
     */
    bool strictAuthentication(bool *ok);

    /*
     * set strict authentication
     */
    void setStrictAuthentication(bool *ok);

    /*
     * set opportunistic authentication
     */
    void setOpportunisticAuthentication(bool *ok);

    /*
     * get the contents of the config (from the current file)
     */
    QString getRuntimeConfig(bool *ok);

    /*
     * save config text (in a temporary file)
     */
    void saveTempConfig(const QString text, bool *ok);

    /*
     * Most recent saved config - ie,
     * if temporary file exists, read from that, otherwise read from the current file.
     */
    QString getLastSavedConfig(bool *ok);

    /*
     * revert Config to default
     */
    void saveDefaultToTempConfig(bool *ok);

    /*
     * discard config changes, i.e. delete the temporary file.
     */
    void discardTempConfigFile(bool *ok);

    /*
     * changes exist? ie temporary file exists?
     */
    bool tempConfigFileExists();

    /*
     * validate the configuration. Write the config to a temp file and parse with getdns_query. Delete the file afterwards.
     */
    bool isValidConfigText(QString text, bool *ok);

    /*
     * apply the configuration. ie copy stubby.temp to stubby.conf
     */
    void applyConfig(bool *ok);

private:
    QString getConfigFromFile(const QString filename, bool *ok);
    void saveConfigTextToFile(const QString filename, const QString text, bool *ok);
    void deleteFile(const QString filename, bool *ok);

    QObject *m_parent;
};

#endif // CONFIGFILE_H
