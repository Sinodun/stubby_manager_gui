/*
 * Copyright 2017 Sinodun Internet Technologies Ltd.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "nativestatusbutton.h"
#include "msgdefs.h"

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#include <QDebug>

struct StatusItemWrapper
{
    NSStatusItem *statusItem;
};

NativeStatusButton::NativeStatusButton() :
    m_statusItemWrapper(new StatusItemWrapper)
{
    m_statusItemWrapper->statusItem = [[NSStatusBar systemStatusBar] statusItemWithLength:NSSquareStatusItemLength];
    m_statusItemWrapper->statusItem.button.image = [NSApp applicationIconImage];
    m_statusItemWrapper->statusItem.button.imageScaling = NSImageScaleProportionallyUpOrDown; //scales the icon so that it fits in menu bar
    m_statusItemWrapper->statusItem.button.alternateImage = [NSApp applicationIconImage]; //TODO - find another image and use this to indicate "strict"?
    m_statusItemWrapper->statusItem.button.accessibilityTitle = @"My Status Item";
    m_statusItemWrapper->statusItem.menu = 0; //"single click behaviour" if no menu. ?? What is single click behaviour?
    m_statusItemWrapper->statusItem.button.appearsDisabled = true;
    m_statusItemWrapper->statusItem.toolTip = @"Stubby v0.1.0-alpha";
}

NativeStatusButton::~NativeStatusButton()
{
    SMDebug(eDestructors,"");
    delete m_statusItemWrapper;
}

void NativeStatusButton::showDisabled(const bool disabled)
{
    m_statusItemWrapper->statusItem.button.appearsDisabled = disabled;
}
