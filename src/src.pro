TEMPLATE = app
TARGET = $$OUT_PWD/test

CONFIG += link_pkgconfig
PKGCONFIG += \
  libwbxml2 \
  accounts-qt5 \
  libsignon-qt5 \
  glib-2.0 \
  libmkcal-qt5 \
  libkcalcoren-qt5 \
  uuid \
  libical

QT += network
QT -= gui

INCLUDEPATH += .

HEADERS += \
  utils.h \
  testbase.h \
  another.h \
  calendar.h

SOURCES += \
  main.cpp \
  utils.cpp \
  testbase.cpp \
  another.cpp \
  calendar.cpp
