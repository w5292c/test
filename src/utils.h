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

#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <QtCore/qstring.h>
#include <QtCore/qdebug.h>

typedef struct WBXMLTag_s WBXMLTag;
typedef struct WBXMLTreeNode_s WBXMLTreeNode;
typedef struct WBXMLAttributeName_s WBXMLAttributeName;
class Utils
{
public:
  static void iconvTest();
  static void registerAccount();
  static void hexDump(const char *pData);
  static void hexDump(const unsigned char *pData, int length);
  static QString hexTreeNodeType(int treeNodeType);
  static QDebug logNodeName(QDebug debug, WBXMLTag *nodeName);
  static QDebug logNode(QDebug debug, WBXMLTreeNode *node, int level = 0);
};

#endif /* TEST_UTILS_H */
