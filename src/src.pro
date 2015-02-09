TEMPLATE = app
TARGET = $$OUT_PWD/test

CONFIG += link_pkgconfig
PKGCONFIG += \
    libwbxml2 \
    accounts-qt5 \
    libsignon-qt5 \
    glib-2.0

QT += network
QT -= gui

INCLUDEPATH += .

HEADERS += \
  utils.h \
  testbase.h \
  another.h

SOURCES += \
  main.cpp \
  utils.cpp \
  testbase.cpp \
  another.cpp
