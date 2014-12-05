#include "testbase.h"

#include <QDebug>
#include <QCoreApplication>

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  TestBase test;
  test.setValue(123);
  qDebug() << "main: " << test.getValue();

  return 0;
}
