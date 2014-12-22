#include "utils.h"

#include <QDebug>
#include <string.h>

void Utils::hexDump(const char *pData)
{
  const int length = strlen (pData);
  hexDump (reinterpret_cast<const unsigned char *>(pData), length);
}

void Utils::hexDump(const unsigned char *pData, int length)
{
  char buffer[20];
  QDebug debug = qDebug();
  debug.nospace();
  for (int i = 0; i < length; ++i) {
    if (!(i % 16)) {
      snprintf(buffer, 20, "%4.4x: ", i);
      debug << buffer;
    }
    char byte = pData[i];
    char lowByte = (0xFU&byte) + '0';
    char highByte = (0xFU&(byte>>4)) + '0';
    if (lowByte > '9') {
      lowByte += 'A' - '9';
    }
    if (highByte > '9') {
      highByte += 'A' - '9';
    }
    if (byte < 32) {
      byte = '.';
    }
    debug << highByte << lowByte << "(" << byte << ") ";
    if (i%16 == 15) {
      debug << "\n";
    }
  }
  debug << "\n";
  debug.space();
}
