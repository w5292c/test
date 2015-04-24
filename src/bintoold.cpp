#include "bintoold.h"

#include <QFile>
#include <QDebug>

void Bin::test()
{
  qDebug() << "Hello!";

  QFile file("test.bin");
  bool res = file.open(QIODevice::WriteOnly);
  QDataStream stream(&file);
  stream.setByteOrder(QDataStream::BigEndian);
  quint16 data = 0;
  for (int i = 0; i < 8; ++i) {
    switch (i) {
    case 0:
      data = 0u;
      break;
    case 1:
      data = 0x001fu;
      break;
    case 2:
      data = 0x07e0u;
      break;
    case 3:
      data = 0x07ffu;
      break;
    case 4:
      data = 0xf800u;
      break;
    case 5:
      data = 0xf81fu;
      break;
    case 6:
      data = 0xffe0u;
      break;
    case 7:
      data = 0xffffu;
      break;
    }
    for (int x = 0; x < 40*240; ++x) {
      stream << data;
    }
  }

  qDebug() << "done." << res;
}
