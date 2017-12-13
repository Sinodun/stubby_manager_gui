/*
 * Copyright 2017 Sinodun Internet Technologies Ltd.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "msgdefs.h"

const char *DebugTypeText::s_typeUnknown = "UNKNOWN";
const char *DebugTypeText::s_textArray[eNoOfDebugMsgTypes] = {"GENERIC", "RUNTASK", "LOGGING", "CONFIGURATION", "DESTRUCTORS", "DAEMON", "DNSSERVER"};

const char* DebugTypeText::text(DebugMsgType type)
{
    const char *returnText = s_typeUnknown;
    if ((type < eNoOfDebugMsgTypes) && (s_textArray[type])) {
        returnText = s_textArray[type];
    }
    return returnText;
}
