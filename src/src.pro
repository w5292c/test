TEMPLATE = app
TARGET = $$OUT_PWD/test

CONFIG += link_pkgconfig
PKGCONFIG += libwbxml2

INCLUDEPATH += .

# Input
HEADERS += \
  testbase.h

SOURCES += \
  main.cpp \
  testbase.cpp
