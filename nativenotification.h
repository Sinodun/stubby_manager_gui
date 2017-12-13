/*
 * Copyright 2017 Sinodun Internet Technologies Ltd.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef NATIVENOTIFICATION_H
#define NATIVENOTIFICATION_H
#include <QString>

class NativeNotification
{
public:
    NativeNotification();

    void postNotification(QString title, QString message);
};

#endif // NATIVENOTIFICATION_H
