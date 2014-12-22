#include "testbase.h"

#include "utils.h"

#include <QMap>
#include <QDebug>
#include <wbxml.h>
#include <string.h>
#include <wbxml_conv.h>

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

  const WB_UTINY TheData[] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  };
  WB_UTINY *const encoded = wbxml_base64_encode (TheData, sizeof (TheData));
  qDebug() << "Encoded: " << (const char *)encoded;

  wbxml_free (encoded);

  WB_UTINY xmlBlock[] =
    "<?xml version=\"1.0\"?>\n"
    "<!DOCTYPE AirSync PUBLIC \"-//AIRSYNC//DTD AirSync//EN\" \"http://www.microsoft.com/\">\n"
    "<FolderSync xmlns=\"http://synce.org/formats/airsync_wm5/airsync\">\n"
    "  <SyncKey>0</SyncKey>\n"
    "</FolderSync>\n";

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
  qDebug() << "XML: result: " << (const char *)wbxml_errors_string (wbres) << ", XML length: " << xml_len;
  qDebug() << (const char *)pXml;

  qDebug() << "********************************************************************************************";

  return app.exec();
}
