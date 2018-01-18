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

#include "utils.h"

#include "wbxml-utils.h"

#include <glib.h>
#include <QDebug>
#include <wbxml.h>
#include <assert.h>
#include <string.h>
#include <QTextCodec>
#include <SignOn/Identity>
#include <Accounts/Account>
#include <Accounts/Manager>
#include <QProcessEnvironment>

namespace {
const QLatin1String SsoMethod("auth/method");
const QLatin1String AsProviderName("activesync");
const QLatin1String AccountCredId("CredentialsId");
const QLatin1String ExchangeServerPort("connection/port");
const QLatin1String ExchangeServerHost("connection/exchange_server");
const QLatin1String NwSecureConnection("connection/secure_connection");
}

void Utils::registerAccount()
{
  Accounts::Manager manager;
  Accounts::Account *account = manager.account(1);
  if (!account) {
    account = manager.createAccount(AsProviderName);
  }
  account->setEnabled(true);
  account->setDisplayName("Main AS Account");
  const QProcessEnvironment &env = QProcessEnvironment::systemEnvironment();
  const QString &userId = env.value("MY_USER", "<user>");
  const QString &serverAddress = env.value("MY_ADDR", "exchange-server.com");
  const QString &serverPort = env.value("MY_PORT", "443");
  account->setValue("default_credentials_username", userId);
  account->beginGroup("connection");
  account->setValue("exchange_server", serverAddress);
  account->setValue("port", serverPort);
  account->endGroup();
  account->setValue(SsoMethod, "password");
  account->setValue(AccountCredId, "1");
  account->setValue(NwSecureConnection, true);
  account->setValue("sync/mail", true);

  account->sync();

  // SignOn handling
  const QString &passwd = env.value("MY_PASS", "<password>");
  SignOn::IdentityInfo identityInfo;
  identityInfo.setId(1);
  identityInfo.setUserName(userId);
  identityInfo.setSecret(passwd, true);
  identityInfo.setStoreSecret(true);
  SignOn::Identity *identity = SignOn::Identity::existingIdentity(1);
  if (!identity) {
    qWarning() << "New identity";
    identity = SignOn::Identity::newIdentity(identityInfo);
  }

  if (!identity) {
    qDebug() << "[Utils::registerAccount] Cannot create 'identity'";
  } else {
    identity->storeCredentials(identityInfo);
  }

  qDebug() << "[Utils::registerAccount]: account, ID: " << account->id();
}

void Utils::hexDump(const char *pData)
{
  const int length = strlen (pData);
  hexDump (reinterpret_cast<const unsigned char *>(pData), length);
}

void Utils::hexDump(const QByteArray &data)
{
  hexDump(reinterpret_cast<const unsigned char *>(data.data()), data.length());
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
      // 0x0A => 'A', etc...
      lowByte += 'A' - ('9' + 1);
    }
    if (highByte > '9') {
      // 0x0A => 'A', etc...
      highByte += 'A' - ('9' + 1);
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

QString Utils::hexTreeNodeType(int treeNodeType)
{
  QString name;
  switch (treeNodeType) {
  case WBXML_TREE_ELEMENT_NODE:
    name = "WBXML_TREE_ELEMENT_NODE";
    break;
  case WBXML_TREE_TEXT_NODE:
    name = "WBXML_TREE_TEXT_NODE";
    break;
  case WBXML_TREE_CDATA_NODE:
    name = "WBXML_TREE_CDATA_NODE";
    break;
  case WBXML_TREE_PI_NODE:
    name = "WBXML_TREE_PI_NODE";
    break;
  case WBXML_TREE_TREE_NODE:
    name = "WBXML_TREE_TREE_NODE";
    break;
  default:
    name = "WBXML_TREE_UNDEFINED";
    break;
  }
  return name;
}

QDebug Utils::logNodeName(QDebug debug, WBXMLTag *nodeName)
{
  if (!nodeName) return debug;

  if (WBXML_VALUE_TOKEN == nodeName->type) {
    debug << "[WBXML_VALUE_TOKEN: ";
    const WBXMLTagEntry *const token = nodeName->u.token;
    if (token) {
      const WB_TINY *const xmlName = token->xmlName;
      debug << "ENTRY: ";
      if (xmlName) {
        debug << "\"" << xmlName << "\", ";
      } else {
        debug << "<null>, ";
      }
      debug << "PAGE: " << token->wbxmlCodePage << ", TOKEN: " << token->wbxmlToken;
    } else {
      debug << "<null>";
    }
    debug << "]";
  } else if (WBXML_VALUE_LITERAL  == nodeName->type) {
    debug << "[WBXML_VALUE_LITERAL: \"" << (const char *)wbxml_buffer_get_cstr(nodeName->u.literal) << "\"]";
  } else {
    debug << "[WBXML_VALUE_UNKNOWN]";
  }

  return debug;
}

QDebug Utils::logNode(QDebug debug, WBXMLTreeNode *node, int level)
{
  // check if the 'node' exists
  if (!node) return debug;

  debug.nospace();
  for (int i = 0; i < level; ++i) {
    debug << "  ";
  }
  const char *content = (const char *)wbxml_buffer_get_cstr(node->content);
  if (!strlen(content)) {
    debug << "Tree node type: " << hexTreeNodeType(node->type);
  } else {
    debug << "Tree node type: " << hexTreeNodeType(node->type) << ", content: \"" << content << "\"";
  }

  if (node->name) {
    debug << ", name: \"";
    Utils::logNodeName(debug, node->name);
    debug << "\"";
  }
  debug << "\n";
  debug.space();

  WBXMLTreeNode *children = node->children;
  while (children) {
    logNode(debug, children, level + 1);

    children = children->next;
  }

  return debug;
}

void Utils::iconvTest()
{
  GError *error = NULL;
  gsize bytes_read = 0;
  gsize bytes_written = 0;
  gchar *const result = g_convert(
    // Data to be converted
    "Str: \xF4", -1,
    // TO <= FROM
    "utf-8", "ISO8859-1",
    &bytes_read, &bytes_written, &error);
  QByteArray outBuffer;
  outBuffer.append(result, bytes_written);
  const QString &resultString = QString::fromUtf8(outBuffer);

  qDebug() << "Utils::iconvTest, Result: " << resultString
           << ", bytes read: " << bytes_read << ", bytes_written: " << bytes_written
           << ", error: " << error;
  Utils::hexDump(outBuffer);
}

void Utils::testTextCodec()
{
  const QList<QByteArray> &availableCodecs = QTextCodec::availableCodecs();
  qDebug() << availableCodecs;
}

const char *const wbxmlHexData =
"03016a00 0007564c 03310001 52033100 014e5703 31310001 4f480331 00014903 30000147 0343616c "
"656e6461 7200014a 03380001 014f4803 32000149 03300001 4703436f 6e746163 74730001 4a033900 "
"01014f48 03330001 49033000 01470344 656c6574 65642049 74656d73 00014a03 34000101 4f480334 "
"00014903 30000147 03447261 66747300 014a0333 0001014f 48033500 01490330 00014703 496e626f "
"7800014a 03320001 014f4803 36000149 03300001 47034a6f 75726e61 6c00014a 03313100 01014f48 "
"03370001 49033000 0147034a 756e6b20 456d6169 6c00014a 03313200 01014f48 03380001 49033000 "
"0147034e 6f746573 00014a03 31300001 014f4803 39000149 03300001 47034f75 74626f78 00014a03 "
"36000101 4f480331 30000149 03300001 47035365 6e742049 74656d73 00014a03 35000101 4f480331 "
"31000149 03300001 47035461 736b7300 014a0337 00010101 01";

void Utils::wbxmlTest()
{
  const QByteArray &data = QByteArray::fromHex(wbxmlHexData);

  // Create the WBXML tree
  WBXMLTree *tree = NULL;
  WBXMLError result = wbxml_tree_from_wbxml((WB_UTINY *)data.constData(),
                                            data.length(), WBXML_LANG_AIRSYNC, &tree);
  Q_ASSERT(result == WBXML_OK);
  WB_UTINY *xml = NULL;
  WB_ULONG xml_len = 0;
  WBXMLGenXMLParams params;
  params.gen_type = WBXML_GEN_XML_INDENT;
  params.lang = WBXML_LANG_AIRSYNC;
  params.indent = 2;
  params.keep_ignorable_ws = TRUE;
  // Encode the WBXML tree in XML format
  result = wbxml_tree_to_xml(tree, &xml, &xml_len, &params);
  Q_ASSERT(result == WBXML_OK);
  Q_UNUSED(result);
  qDebug() << "Length: " << xml_len << "Data:\r\n" << (const char *)xml;
}

void Utils::wbxmlIntTest()
{
  QByteArray buffer;

  qDebug() << "Encoding single-byte value (0x75)";
  Wbxml::appendInt(buffer, 0x75);
  assert(buffer.length() == 1);
  assert(buffer[0] == '\x75');

  qDebug() << "Encoding single-byte value (0x00)";
  buffer.clear();
  Wbxml::appendInt(buffer, 0x00);
  assert(buffer.length() == 1);
  assert(buffer[0] == '\x00');

  qDebug() << "Encoding two-byte value (0xA0)";
  buffer.clear();
  Wbxml::appendInt(buffer, 0xA0);
  assert(buffer.length() == 2);
  assert(buffer[0] == '\x81');
  assert(buffer[1] == '\x20');

  qDebug() << "Encoding two-byte value (0x3FFF)";
  buffer.clear();
  Wbxml::appendInt(buffer, 0x3FFFU);
  assert(buffer.length() == 2);
  assert(buffer[0] == '\xFF');
  assert(buffer[1] == '\x7F');

  qDebug() << "Encoding three-byte value (0x4000)";
  buffer.clear();
  Wbxml::appendInt(buffer, 0x4000U);
  assert(buffer.length() == 3);
  assert(buffer[0] == '\x81');
  assert(buffer[1] == '\x80');
  assert(buffer[2] == '\x00');

  qDebug() << "Encoding five-byte value (0xFFFFFFFFU)";
  buffer.clear();
  Wbxml::appendInt(buffer, 0xFFFFFFFFU);
  assert(buffer.length() == 5);
  assert(buffer[0] == '\x8F');
  assert(buffer[1] == '\xFF');
  assert(buffer[2] == '\xFF');
  assert(buffer[3] == '\xFF');
  assert(buffer[4] == '\x7F');

  qDebug() << "Encoding five-byte value (0xFEDCBA98U)";
  buffer.clear();
  Wbxml::appendInt(buffer, 0xFEDCBA98U);
  assert(buffer.length() == 5);
  assert(buffer[0] == '\x8F');
  assert(buffer[1] == '\xF6');
  assert(buffer[2] == '\xF2');
  assert(buffer[3] == '\xF5');
  assert(buffer[4] == '\x18');

  qDebug() << "Encoding five-byte value (0xFFFFFFFFU) plus two-byte value (0x3FFF)";
  buffer.clear();
  Wbxml::appendInt(buffer, 0xFEDCBA98U);
  Wbxml::appendInt(buffer, 0x3FFF);
  assert(buffer.length() == 7);
  assert(buffer[0] == '\x8F');
  assert(buffer[1] == '\xF6');
  assert(buffer[2] == '\xF2');
  assert(buffer[3] == '\xF5');
  assert(buffer[4] == '\x18');
  assert(buffer[5] == '\xFF');
  assert(buffer[6] == '\x7F');
}

uint Utils::getUint32(const QByteArray &buffer, uint startIndex)
{
    int i = 0;
    uint result = 0;
    const uint length = buffer.length();
    while (length > startIndex + i && i < 4) {
        result = result | ((static_cast<uint>(buffer[startIndex + i])) << (8*i)); ++i;
    }

    return result;
}

QByteArray Utils::random64()
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

QByteArray Utils::hexRandom64()
{
  union {
    qulonglong value;
    uint8_t buffer[8];
  } randomData;

  for (int i = 0; i < 8; ++i) {
    randomData.buffer[i] = (uint8_t)rand();
  }

  return QByteArray::number(randomData.value, 16);
}
