#
# Copyright 2017 Sinodun Internet Technologies Ltd.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, you can obtain one at https://mozilla.org/MPL/2.0/.
#

#-------------------------------------------------
#
# Project created by QtCreator 2017-06-05T16:45:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StubbyManager
TEMPLATE = app

message($$QMAKESPEC)

macx {
  message(using macx to set scope)
  QT += macextras
  ICON = Stubby.icns
  LIBS += -framework Foundation
  LIBS += -framework AppKit
  LIBS += -framework Security
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        stubbymanager.cpp \
    daemoncontroller.cpp \
    runtask.cpp \
    logging.cpp \
    editconfig.cpp \
    stubbysetdns.cpp \
    nativenotification.mm \
    nativestatusbutton.mm \
    configfilemanager.cpp \
    msgdefs.cpp

HEADERS += \
        stubbymanager.h \
    daemoncontroller.h \
    runtask.h \
    logging.h \
    editconfig.h \
    stubbysetdns.h \
    nativenotification.h \
    nativestatusbutton.h \
    configfilemanager.h \
    msgdefs.h

FORMS += \
        stubbymanager.ui \
    logging.ui \
    editconfig.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    README.md \
    Changelog \
    Known_Issues \
    LICENSE
