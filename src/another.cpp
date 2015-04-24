/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Alexander Chumakov
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

#include "another.h"

#include <QDebug>
#include <QVariant>

static QMap<uint, QVariant> TheMap;

QMap<uint, QVariant> testX()
{
  return TheMap;
}

static const QLatin1String KStr("Here is: %1, %2");

QByteArray qCleanupFuncinfo(QByteArray info);
QString qMessageFormatString(QtMsgType type, const QMessageLogContext &context, const QString &str);
void test()
{
  QMessageLogContext context;
  context.function = "void AClass::hello(void)";
  context.file = "../src/another.cpp";
  context.category = "[EAS]";
  context.line = __LINE__;
//  qDebug() << qCleanupFuncinfo("void hello(void)");
  qDebug() << qMessageFormatString(QtDebugMsg, context, "This is a debug string");
}
