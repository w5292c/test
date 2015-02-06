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

#include <glib.h>
#include <QDebug>
#include <wbxml.h>
#include <string.h>
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

  account->sync();

  // SignOn handling
  const QString &passwd = env.value("MY_PASS", "<password>");
  SignOn::IdentityInfo identityInfo;
  identityInfo.setUserName(userId);
  identityInfo.setSecret(passwd, true);
  SignOn::Identity *const identity = SignOn::Identity::newIdentity(identityInfo);
  if (!identity) {
    qDebug() << "[Utils::registerAccount] Cannot create 'identity'";
  } else {
    identity->storeCredentials();
  }

  qDebug() << "[Utils::registerAccount]: account, ID: " << account->id();
}

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
  GIConv conv = g_iconv_open("utf-8", "ISO8859-1");
}
