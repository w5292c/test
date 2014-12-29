TEMPLATE = app
TARGET = $$OUT_PWD/test

CONFIG += link_pkgconfig
PKGCONFIG += libwbxml2 accounts-qt5 libsignon-qt5

MOBILITY = systeminfo
CONFIG += mobility

QT += network
QT -= gui

INCLUDEPATH += .

HEADERS += \
  utils.h \
  testbase.h

SOURCES += \
  main.cpp \
  utils.cpp \
  testbase.cpp
