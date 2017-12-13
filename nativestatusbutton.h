/*
 * Copyright 2017 Sinodun Internet Technologies Ltd.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef NATIVESTATUSBUTTON_H
#define NATIVESTATUSBUTTON_H

struct StatusItemWrapper;

class NativeStatusButton
{
public:
    NativeStatusButton();
    ~NativeStatusButton();

    void showDisabled(const bool disabled);

private:
    StatusItemWrapper *m_statusItemWrapper;
};

#endif // NATIVESTATUSBUTTON_H
