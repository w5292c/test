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

#include "testbase.h"

#include "utils.h"

#include "mosq.h"
#include "masn1.h"
#include "another.h"
#include "sqltest.h"
#include "calendar.h"
#include "bintoold.h"

#include <QDir>
#include <QMap>
#include <QDebug>
#include <wbxml.h>
#include <string.h>
#include <wbxml_conv.h>

#include <QDateTime>
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
  CalendarTest::init();

  if (2 == argc && !strcmp(argv[1], "reg")) {
    Utils::registerAccount();
    qDebug() << "MAIN: registration done.";
    return 0;
  } else if (2 == argc && !strcmp(argv[1], "iconv")) {
    Utils::iconvTest();
    return 0;
  } else if (2 == argc && !strcmp(argv[1], "check-test")) {
    TestBase testBase;
    qDebug() << "MAIN result (expected: false): " << testBase.checkTags(0x1020u, 0x2030u, 0x3040, 0x4051, 0x5061);
    qDebug() << "MAIN result (expected:  true): " << testBase.checkTags(0x1020u, 0x2030u, 0x3040, 0x4050, 0x5061);
    return 0;
  } else if (2 == argc && !strcmp(argv[1], "time")) {
    QDateTime dateTime(QDateTime(QDate(2015, 2, 15), QTime(10, 33, 50, 775), Qt::UTC));
    qDebug() << "ISO     Date: " << dateTime.toString(Qt::ISODate);
    qDebug() << "RFC2822 Date: " << dateTime.toString(Qt::RFC2822Date);
    qDebug() << "Custom  Date: " << dateTime.toString("yyyy-MM-ddTHH:mm:ss.zzzZ");
    const QDateTime &parsed = QDateTime::fromString("2016-06-21T21:20:31.235Z", Qt::ISODate);
    qDebug() << "Parsed: " << parsed;
    return 0;
  } else if (2 == argc && !strcmp(argv[1], "text")) {
    Utils::testTextCodec();
    return 0;
  } else if (2 == argc && !strcmp(argv[1], "test")) {
    test();
    return 0;
  } else if (2 == argc && !strcmp(argv[1], "wbxml")) {
    Utils::wbxmlTest();
    return 0;
  } else if (2 == argc && !strcmp(argv[1], "endian")) {
    QByteArray buffer;
    buffer.append("\x00\x01\x02\x03\x04\x05", 6);

    qCritical() << "Length:" << buffer.length();
    qCritical() << "0:" << QByteArray::number(Utils::getUint32(buffer, 0), 16);
    qCritical() << "1:" << QByteArray::number(Utils::getUint32(buffer, 1), 16);
    qCritical() << "2:" << QByteArray::number(Utils::getUint32(buffer, 2), 16);
    qCritical() << "3:" << QByteArray::number(Utils::getUint32(buffer, 3), 16);
    qCritical() << "4:" << QByteArray::number(Utils::getUint32(buffer, 4), 16);
    qCritical() << "5:" << QByteArray::number(Utils::getUint32(buffer, 5), 16);
    qCritical() << "6:" << QByteArray::number(Utils::getUint32(buffer, 6), 16);
    qCritical() << "7:" << QByteArray::number(Utils::getUint32(buffer, 7), 16);
    return 0;
  } else if (2 == argc && !strcmp(argv[1], "cal")) {
    qDebug() << "Home:" << QDir::home().path();
    CalendarTest::test();
    return 0;
  } else if (2 == argc && !strcmp(argv[1], "bin")) {
    Bin::test();
    return 0;
  } else if (2 == argc && !strcmp(argv[1], "sql")) {
    test_sql();
    return 0;
  } else if (2 == argc && !strcmp(argv[1], "sql")) {
    TestBase testObject;

    return 0;
  } else if (2 == argc && !strcmp(argv[1], "asn1")) {
    test_asn1();
    return 0;
  } else if (2 == argc && !strcmp(argv[1], "mosq")) {
    test_mosq();
    return 0;
  }


  TestBase *test = new TestBase();

#if 0

#if 0 // Base64 encoding test
  const WB_UTINY TheData[] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  };
  WB_UTINY *const encoded = wbxml_base64_encode (TheData, sizeof (TheData));
  qDebug() << "Encoded: " << (const char *)encoded;

  wbxml_free (encoded);
#endif // Base64 encoding test

  WB_UTINY xmlBlock[] =
    "<?xml version=\"1.0\"?>"
    "<!DOCTYPE AirSync PUBLIC \"-//AIRSYNC//DTD AirSync//EN\" \"http://www.microsoft.com/\">"
    "<FolderSync xmlns=\"http://synce.org/formats/airsync_wm5/airsync\">"
    "<SyncKey>0</SyncKey>"
    "</FolderSync>";

  WBXMLGenWBXMLParams gparams;
  gparams.wbxml_version = WBXML_VERSION_13;
  gparams.keep_ignorable_ws = FALSE;
  gparams.use_strtbl = TRUE;
  gparams.produce_anonymous = TRUE;

  WB_UTINY *wbxml = NULL;
  WB_ULONG wbxml_len = 0;
  const WBXMLError result = wbxml_conv_xml2wbxml_withlen(
    xmlBlock, strlen ((const char *)xmlBlock),
    &wbxml, &wbxml_len,
    &gparams);
  qDebug() << "********************************************************************************************";
  qDebug() << "BWXml result: " << (const char *)wbxml_errors_string (result) << ", block: " << wbxml << ", Length: " << wbxml_len;
  qDebug() << "XML Data:" << (const char *)xmlBlock << ", Land ID: " << WBXML_LANG_AIRSYNC;
  Utils::hexDump(wbxml, wbxml_len);
  qDebug() << "********************************************************************************************";
  WBXMLTree *wbxml_tree = NULL;
  const WBXMLError treeRes = wbxml_tree_from_xml(xmlBlock, strlen ((const char *)xmlBlock), &wbxml_tree);
  qDebug() << "Converted to a tree, result: " << (const char *)wbxml_errors_string(treeRes) << ", " << wbxml_tree;
  qDebug() << "Charset: " << wbxml_tree->orig_charset << ", codePage: " << wbxml_tree->cur_code_page;
  WBXMLTreeNode *item = wbxml_tree->root;
  Utils::logNode(qDebug(), item);

  qDebug() << "********************************************************************************************";
  /* Example from [MS-ASWBXML].pdf */
  WB_UTINY testWbxml[] = {
    0x03, 0x01, 0x6A, 0x00, 0x45, 0x5C, 0x4F, 0x50,
    0x03, 0x43, 0x6F, 0x6E, 0x74, 0x61, 0x63, 0x74,
    0x73, 0x00, 0x01, 0x4B, 0x03, 0x32, 0x00, 0x01,
    0x52, 0x03, 0x32, 0x00, 0x01, 0x4E, 0x03, 0x31,
    0x00, 0x01, 0x56, 0x47, 0x4D, 0x03, 0x32, 0x3A,
    0x31, 0x00, 0x01, 0x5D, 0x00, 0x11, 0x4A, 0x46,
    0x03, 0x31, 0x00, 0x01, 0x4C, 0x03, 0x30, 0x00,
    0x01, 0x4D, 0x03, 0x31, 0x00, 0x01, 0x01, 0x00,
    0x01, 0x5E, 0x03, 0x46, 0x75, 0x6E, 0x6B, 0x2C,
    0x20, 0x44, 0x6F, 0x6E, 0x00, 0x01, 0x5F, 0x03,
    0x44, 0x6F, 0x6E, 0x00, 0x01, 0x69, 0x03, 0x46,
    0x75, 0x6E, 0x6B, 0x00, 0x01, 0x00, 0x11, 0x56,
    0x03, 0x31, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01
  };

  WBXMLGenXMLParams params;
  params.gen_type = WBXML_GEN_XML_INDENT;
  params.lang = WBXML_LANG_AIRSYNC;
  params.indent = 0;
  params.keep_ignorable_ws = TRUE;
  WB_ULONG xml_len = 2;
  WB_UTINY *pXml = NULL;
  const WBXMLError wbres = wbxml_conv_wbxml2xml_withlen(
    testWbxml, sizeof (testWbxml),
    &pXml, &xml_len,
    &params);
  qDebug() << "XML: result: " << (const char *)wbxml_errors_string(wbres) << ", XML length: " << xml_len;
  qDebug() << (const char *)pXml;

  qDebug() << "********************************************************************************************";
  WBXMLTree *resTree = NULL;
  const WBXMLError lastRes = wbxml_tree_from_wbxml(testWbxml, sizeof (testWbxml), WBXML_LANG_AIRSYNC, &resTree);
  qDebug() << "Tree from WBXML result: " << (const char *)wbxml_errors_string(lastRes);
  Utils::logNode(qDebug(), resTree->root);
  qDebug() << "********************************************************************************************";
  // Manually creating WBXML tree for SyncFolder ActiveSync command.
  // Tree node type: "WBXML_TREE_ELEMENT_NODE", name: "[WBXML_VALUE_TOKEN: ENTRY: "FolderSync", PAGE: 7, TOKEN: 22]"
  //    Tree node type: "WBXML_TREE_ELEMENT_NODE", name: "[WBXML_VALUE_TOKEN: ENTRY: "SyncKey", PAGE: 0, TOKEN: 11]"
  //      Tree node type: "WBXML_TREE_TEXT_NODE", content: "0"

  // Create the tree object
  WBXMLTree *createdTree = wbxml_tree_create(WBXML_LANG_AIRSYNC, WBXML_CHARSET_UTF_8);
  WBXMLTreeNode *folderSyncNode = wbxml_tree_node_create(WBXML_TREE_ELEMENT_NODE);
  WBXMLTreeNode *syncKeyNode = wbxml_tree_node_create(WBXML_TREE_ELEMENT_NODE);

  // Creare/prepare the value TREE-NODE:
  WBXMLTreeNode *const keyNode = wbxml_tree_node_create_text((const unsigned char *)("0"), 1);

  // Create the 'SyncKey' TREE-TAG
  WBXMLTagEntry tagEntry;
  tagEntry.xmlName = NULL;
  tagEntry.wbxmlCodePage = 7;
  tagEntry.wbxmlToken = 18;
  syncKeyNode->name = wbxml_tag_create_token(&tagEntry);
  // Create the 'FolderSync' tag
  WBXMLTagEntry tagEntry1;
  tagEntry1.xmlName = NULL;
  tagEntry1.wbxmlCodePage = 7;
  tagEntry1.wbxmlToken = 22;
  folderSyncNode->name = wbxml_tag_create_token(&tagEntry1);

  // Configure links
  createdTree->root = folderSyncNode;
  folderSyncNode->children = syncKeyNode;
  syncKeyNode->parent = folderSyncNode;
  syncKeyNode->children = keyNode;
  keyNode->parent = syncKeyNode;

  qDebug() << "Created tree:";
  Utils::logNode(qDebug(), createdTree->root);
  WB_UTINY *createdWbxml = NULL;
  WB_ULONG  createdWbxmlLen = 0;
  const WBXMLError createdWbxmlResult = wbxml_tree_to_wbxml(createdTree, &createdWbxml, &createdWbxmlLen, &gparams);
  qDebug() << "Result: " << (const char *)wbxml_errors_string(createdWbxmlResult) << ", length: " << createdWbxmlLen;
  Utils::hexDump(createdWbxml, createdWbxmlLen);
#endif

  qDebug() << "********************************************************************************************";

  int rrr = app.exec();
  delete test;
  return rrr;
}
