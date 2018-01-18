/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Alexander Chumakov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <wbxml-utils.h>

QByteArray Wbxml::random64()
{
  union {
    qulonglong value;
    uint8_t buffer[8];
  } randomData;

  for (int i = 0; i < 8; ++i) {
    randomData.buffer[i] = (uint8_t)rand();
  }

  return QByteArray::number(randomData.value);
}

void Wbxml::appendInt(QByteArray &buffer, uint32_t value)
{
  // Bit fields:
  // 0000 0000    0000 0000    0000 0000    0000 0000
  //                                         ________ : Byte N
  //                             __ ____    _         : Byte N - 1
  //                 _ ____    __                     : Byte N - 2
  //      ____    ___                                 : Byte N - 3
  // ____                                             : Byte N - 4

  unsigned int i = 5;
  bool appended = false;
  do {
    --i;

    uint8_t byte = static_cast<uint8_t>((value >> (7*i)) & 0x7FU);
    if (byte || appended || !i) {
      if (i) {
        byte |= 0x80U;
      }
      buffer.append(byte);
      appended = true;
    }
  } while (i);
}
