/*
 * Copyright 2017 Sinodun Internet Technologies Ltd.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "nativenotification.h"
#import <Foundation/Foundation.h>

NativeNotification::NativeNotification()
{

}

void NativeNotification::postNotification(QString title, QString message)
{
    NSUserNotification* notification = [[NSUserNotification alloc] init];
    notification.title = title.toNSString();
    notification.informativeText = message.toNSString();
    notification.soundName = NSUserNotificationDefaultSoundName;
    [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification: notification];
    [notification autorelease];
}
