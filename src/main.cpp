#include "testbase.h"

#include <QDebug>
#include <QCoreApplication>

/*!
    \page example.html
    \title Generic QDoc Guide
    There are three essential materials for generating documentation with qdoc:

    \list
        \li \c qdoc binary
        \li \c qdocconf configuration files
        \li \c Documentation in \c C++, \c QML, and \c .qdoc files
    \endlist
*/

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  TestBase test;
  test.setValue(123);
  qDebug() << "main: " << test.getValue();

  return 0;
}
