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
  libical \
  sqlite3 \
  libcrypto

LIBS += -lmosquitto

QT += network
QT -= gui

INCLUDEPATH += .

HEADERS += \
  utils.h \
  testbase.h \
  another.h \
  calendar.h \
  bintoold.h \
  sqltest.h \
  masn1.h \
  mosq.h \
  testdata.h \
  wbxml-utils.h

SOURCES += \
  main.cpp \
  utils.cpp \
  testbase.cpp \
  another.cpp \
  calendar.cpp \
  bintoold.cpp \
  sqltest.cpp \
  masn1.c \
  mosq.c \
  testdata.cpp \
  wbxml-utils.cpp
