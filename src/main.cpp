#include "testbase.h"

#include <QMap>
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

QMap<int, QString> TheMap;

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  TestBase test;
  test.setValue(123);
  qDebug() << "main: " << test.getValue();

  TheMap.insert(0, "This is the first element");
  TheMap.insert(1, "This one is to be updated");
  TheMap.insert(9, "Some more information is here, 9th element");
  TheMap.insert(5, "Some element in the middle");
  TheMap.insert(4, "The 4th element");
  TheMap.insert(1, "Here is the 1st updated element");
  qDebug() << "Here is the map:" << TheMap;

  return 0;
}
