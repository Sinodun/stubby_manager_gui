/*
 * Copyright 2017 Sinodun Internet Technologies Ltd.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef MSGDEFS_H
#define MSGDEFS_H
#include <QDebug>

typedef enum
{
    eGeneric = 0,
    eRunTask,
    eLogging,
    eConfiguration,
    eDestructors,
    eDaemon,
    eDnsServer,
    eNoOfDebugMsgTypes
} DebugMsgType;

extern bool debugFlags[eNoOfDebugMsgTypes];

#define DEBUG_MESSAGES_ON

#ifdef DEBUG_MESSAGES_ON
#define SMDebug(type, fmt, ...) qDebug("%s:%s(%d) " fmt, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define SMDebug(type, fmt, ...)
#endif

#define SMInfo(fmt, ...)     qDebug(fmt, ##__VA_ARGS__)
#define SMWarning(fmt, ...)  qWarning("%s:%s: " fmt, __FILE__, __FUNCTION__, ##__VA_ARGS__)
#define SMCritical(fmt, ...) qCritical("%s:%s: " fmt, __FILE__, __FUNCTION__, ##__VA_ARGS__)
#define SMFatal(fmt, ...)    qFatal(fmt, __VA_ARGS__)

class DebugTypeText
{
public:
    DebugTypeText() {}
    const char *text(DebugMsgType type);

private:
    static const char *s_textArray[eNoOfDebugMsgTypes];
    static const char *s_typeUnknown;
};

#endif // MSGDEFS_H
