/*
 * Copyright 2017 Sinodun Internet Technologies Ltd.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "stubbymanager.h"
#include <QApplication>
#include "msgdefs.h"

bool debugFlags[eNoOfDebugMsgTypes];

static void InitMsgFlags(int argc, char * argv[]);
static void SetAllMsgFlags(const bool on);
static void SetMsgFlag(const DebugMsgType type, const bool on);
static bool EnableIfNumber(QString sArg);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    InitMsgFlags(argc, argv);

    StubbyManager w;
    w.show();

    return a.exec();
}

static void InitMsgFlags(int argc, char *argv[])
{
    SetAllMsgFlags(false);

    bool bFlagWithoutVals = false;
    int i = 0;
    while (i < argc) {
        if (strcmp(argv[i++], "-debug") == 0) {
            bFlagWithoutVals = true;
            while ((i < argc) && (EnableIfNumber(argv[i]))) {
                bFlagWithoutVals = false;
                i++;
            }
        }
    }
    if (bFlagWithoutVals) SetAllMsgFlags(true);
}

static void SetAllMsgFlags(const bool on)
{
    for (int i = 0; i < eNoOfDebugMsgTypes; i++) {
        SetMsgFlag((DebugMsgType)i, on);
    }
}

static void SetMsgFlag(const DebugMsgType type, const bool on)
{
    if ((type >= 0) && (type < eNoOfDebugMsgTypes)) {
        debugFlags[type] = on;
        if (on) {
            SMInfo("%s %s", "Debug messages enabled: ", DebugTypeText().text(type));
        }
    }
}

static bool EnableIfNumber(QString sArg)
{
    bool isNumber;
    int iFlag = sArg.toInt(&isNumber);
    if (isNumber && (iFlag < eNoOfDebugMsgTypes)) {
        SetMsgFlag((DebugMsgType)iFlag, true);
    }
    return isNumber;
}
