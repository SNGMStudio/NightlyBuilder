#-------------------------------------------------
#
# Project created by QtCreator 2015-04-06T23:01:49
#
#-------------------------------------------------

QT       += core gui widgets

CONFIG += c++11

TARGET = NightlyBuilder
TEMPLATE = app


SOURCES += \
    src/main.cpp \
    src/dialogs/maindialog.cpp \
    src/dialogs/rundialog.cpp \
    src/core/statemanager.cpp \
    src/states/statepulling.cpp \
    src/states/stategeneratingbotversioncpp.cpp \
    src/states/state.cpp \
    src/states/stateqmaking.cpp \
    src/states/statemaking.cpp \
    src/states/statedeploying.cpp \
    src/states/statepackaging.cpp \
    src/states/stateuploading.cpp \
    src/core/globalconfig.cpp \
    src/states/statefactory.cpp \
    src/core/deploy.cpp \
    src/core/upload.cpp

HEADERS  += \
    src/dialogs/maindialog.h \
    src/dialogs/rundialog.h \
    src/core/statemanager.h \
    src/states/statepulling.h \
    src/states/stategeneratingbotversioncpp.h \
    src/states/state.h \
    src/states/stateqmaking.h \
    src/states/statemaking.h \
    src/states/statedeploying.h \
    src/states/statepackaging.h \
    src/states/stateuploading.h \
    src/core/globalconfig.h \
    src/states/statefactory.h \
    src/core/deploy.h \
    src/core/upload.h

INCLUDEPATH += \
    src/states \
    src/dialogs \
    src/core


