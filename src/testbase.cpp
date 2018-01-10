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
#include "testdata.h"

#include <QDebug>
#include <wbxml.h>
#include <QBuffer>
#include <stdint.h>
#include <QUrlQuery>
#include <QMetaEnum>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QNetworkRequest>
#include <QCoreApplication>
#include <QProcessEnvironment>
#include <QNetworkProxy>

/*!
    \class TestBase
    \inmodule QDoc Examples
    \brief The TestBase class represents a simple attempt to see anything from qdoc.

    Vectors are one of the main building blocks of 3D representation and
    drawing.  They consist of three coordinates, traditionally called
    x, y, and z.

    The QVector3D class can also be used to represent vertices in 3D space.
    We therefore do not need to provide a separate vertex class.

    \b{Note:} By design values in the QVector3D instance are stored as \c float.
    This means that on platforms where the \c qreal arguments to QVector3D
    functions are represented by \c double values, it is possible to
    lose precision.
*/

namespace {
const char *const PROTOCOL_VERSIONS_HEADER = "MS-ASProtocolVersions";
const char *const PROTOCOL_COMMANDS_HEADER = "MS-ASProtocolCommands";
const QLatin1String sendMailSuffix("/Microsoft-Server-ActiveSync?Cmd=SendMail&User=fakeuser&DeviceId=v140Device&DeviceType=SmartPhone");
const QLatin1String folderSyncSuffix("/Microsoft-Server-ActiveSync?Cmd=FolderSync&User=fakeuser&DeviceId=v140Device&DeviceType=SmartPhone");
}

/*!
    Constructs a test object with zero value.
*/
TestBase::TestBase() : m_value(0), m_index(TestBase::IndexIdNone), m_reply(NULL)
{
  QCoreApplication::quit();

  mTags.push(0x1020);
  mTags.push(0x2030);
  mTags.push(0x3040);
  mTags.push(0x4050);
  mTags.push(0x5061);

  connect(this, SIGNAL(testignal(IndexId)), this, SLOT(start(IndexId)));

  m_timer.setSingleShot(false);
  m_timer.setInterval(500);
  connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
  m_timer.start();

  connect(&m_manager, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(onRequestReady(QNetworkReply*)));
  connect(&m_manager, SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)),
          this, SLOT(authentication(QNetworkReply*, QAuthenticator*)));

  // send the OPTIONS request
  sendRequest();
}

/*!
    Destructs a test object.
*/
TestBase::~TestBase()
{
  m_timer.stop();
}

/*!
    Returns the current value stored in the test object.
*/
int TestBase::getValue() const
{
  return m_value;
}

/*!
    Updates the current value stored in the test object to the value specified in \a newValue.
*/
void TestBase::setValue(int newValue)
{
  m_value = newValue;
}

/*!
 * \brief TestBase::check Args checks with \a length number of arguments
 * \param length The number of arguments to be parsed
 */
bool TestBase::check(int length, ...) const
{
  bool result = (length == mTags.length());

  va_list vl;
  va_start(vl, length);
  if (result) {
    for (int i = 0; i < length; ++i) {
      const uint tagId = va_arg(vl, uint);
      if (tagId != mTags[i]) {
        result = false;
        break;
      }
    }
  }

  va_end(vl);
  return result;
}

void TestBase::onTimer()
{
  emit testignal(static_cast<IndexId>(m_index));

  if (IndexIdLast == m_index) {
    m_index = IndexIdNone;
#if 0
    QCoreApplication::exit();
#endif /* 0 */
  } else {
    ++m_index;
  }
}

void TestBase::start(IndexId indexId)
{
  qDebug() << "[TestBase::start]: " << indexId;
}

void TestBase::authentication(QNetworkReply *reply, QAuthenticator *authenticator)
{
  qDebug() << "[TestBase::authentication]: " << reply << ", " << authenticator;
  const QProcessEnvironment &env = QProcessEnvironment::systemEnvironment();
  const QString &userId = env.value("MY_USER", "<user>");
  const QString &passwd = env.value("MY_PASS", "<password>");
  authenticator->setUser(userId);
  authenticator->setPassword(passwd);
}




void TestBase::sendEmail()
{
  const QProcessEnvironment &env = QProcessEnvironment::systemEnvironment();
  const QString &serverAddress = env.value("MY_ADDR", "exchange-server.com");
  const QString &serverPort = env.value("MY_PORT", "443");
  const QUrl &serverUrl = QUrl(QLatin1String("https://") + serverAddress + ":" + serverPort + sendMailSuffix);
  const QString &userName = env.value("MY_USER");
  const QString &passWord = env.value("MY_PASS");
  const QByteArray &mime = Data::testMime("\"Test1\" <w5292c@outlook.com>", "\"Alexander Chumakov\" <w5292c.ex2@gmail.com>").toUtf8();

  QByteArray wbxmlBuffer;

  /* WBXML version 1.3, charset: UTF-8, no string table */
  wbxmlBuffer.append("\x03\x01\x6A\x00", 4);
  /* Codepage switch: 'ComposeMail' - 21 (0x15) */
  wbxmlBuffer.append("\x00\x15", 2);
  /* <SendMail> */
  wbxmlBuffer.append("\x45", 1);
  /* <ClientId> */
  wbxmlBuffer.append("\x51\x03", 2);
  wbxmlBuffer.append("4677234947143296961493484255641840");
  /* </ClientId> */
  wbxmlBuffer.append("\x00\x01", 2);
  /* <SaveInSentItems /> */
  wbxmlBuffer.append("\x08", 1);
  /* <Mime> */
  wbxmlBuffer.append("\x50\xC3", 2);
  uint16_t length = mime.length();
  length = (length & 0x7FU) | ((length & 0x3F80) << 1);
  wbxmlBuffer.append(0x80u | (length >> 8));
  wbxmlBuffer.append(length);
//  wbxmlBuffer.append(mime.length());
  wbxmlBuffer.append(mime);
  /* </Mime> */
  wbxmlBuffer.append("\x01", 1);
  /* </SendMail> */
  wbxmlBuffer.append("\x01", 1);

  if (m_reply) {
    qDebug() << "Already have the outstanding network request";
    return;
  }
  qDebug() << "MIME length: " << mime.length();
  qDebug() << "Request dump:";
  Utils::hexDump((const unsigned char *)wbxmlBuffer.data(), wbxmlBuffer.size());

  static QBuffer buffer;
  buffer.setData(wbxmlBuffer);

/*  QUrlQuery query;
  query.addQueryItem("Cmd", "SendMail");
  query.addQueryItem("User", userName);
  query.addQueryItem("DeviceId", "");*/

  QNetworkRequest request(serverUrl);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/vnd.ms-sync.wbxml");
  request.setRawHeader("MS-ASProtocolVersion", "14.0");
  request.setRawHeader("User-Agent", "ASOM");
  request.setRawHeader("Host", serverAddress.toLatin1());
  request.setRawHeader("Authorization", "Basic " + QByteArray(QString("%1:%2").arg(userName + "123").arg(passWord + "321").toLatin1()).toBase64());
  m_reply = m_manager.post(request, &buffer);
  connect(m_reply, SIGNAL(finished()), this, SLOT(onSendMailReady()));
}

void TestBase::sendRequest()
{
  const QProcessEnvironment &env = QProcessEnvironment::systemEnvironment();
  const QString &serverAddress = env.value("MY_ADDR", "exchange-server.com");
  const QString &serverPort = env.value("MY_PORT", "443");
  const QUrl &serverUrl = QUrl(QLatin1String("https://") + serverAddress + ":" + serverPort + "/Microsoft-Server-ActiveSync");
  qDebug() << "[TestBase::sendRequest] URL: " << serverUrl;

  if (!m_reply) {
    QNetworkRequest request(serverUrl);
    request.setHeader(QNetworkRequest::UserAgentHeader, "curl/7.35.0");
//    request.setRawHeader("Host", "mobile.ur.ch:443");
    request.setRawHeader("Proxy-Connection", "Keep-Alive");
//    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
//    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
//    request.setSslConfiguration(sslConfig);

#if 0
    > CONNECT office.mailbox.org:443 HTTP/1.1
    > Host: office.mailbox.org:443
    > User-Agent: curl/7.35.0
    > Proxy-Connection: Keep-Alive
#endif

    m_reply = m_manager.sendCustomRequest(request, "OPTIONS");
    connect(m_reply, SIGNAL(finished()), this, SLOT(onOptionsRequestReady()));
    connect(m_reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(onSslErrors(QList<QSslError>)));
  }
}

void TestBase::onRequestReady(QNetworkReply *reply)
{
#if 0
  // This code works as well, but we will log the results in 'TestBase::onOptionsRequestReady'
  qDebug() << "[TestBase::onRequestReady] MS-ASProtocolVersions:" << reply->rawHeader(PROTOCOL_VERSIONS_HEADER);
  qDebug() << "[TestBase::onRequestReady] MS-ASProtocolCommands:" << reply->rawHeader(PROTOCOL_COMMANDS_HEADER);
#else
  qDebug() << "[TestBase::onRequestReady]: reply: " << reply;
#endif
}

void TestBase::onOptionsRequestReady()
{
  qDebug() << "Error:" << m_reply->error();
  qDebug() << "[TestBase::onOptionsRequestReady] MS-ASProtocolVersions:" << m_reply->rawHeader(PROTOCOL_VERSIONS_HEADER)/*.split(',')*/;
//  QByteArray emptyBytes = "14.1";
//  QList<QByteArray> emptyResult = emptyBytes.split(',');
//  qDebug() << "[TestBase::onOptionsRequestReady] empty:" << emptyResult.size() << ", " << emptyResult;
  qDebug() << "[TestBase::onOptionsRequestReady] MS-ASProtocolCommands:" << m_reply->rawHeader(PROTOCOL_COMMANDS_HEADER);
  qDebug() << "[TestBase::onOptionsRequestReady] Headers:" << m_reply->rawHeaderPairs();
  delete m_reply;
  m_reply = NULL;

  /*sendFolderSyncRequest();*/
  sendEmail();
  qDebug() << "****************************** SYNC ******************************";
}

void TestBase::onSslErrors(QList<QSslError> errors)
{
  qDebug() << "TestBase::onSslErrors: " << errors.count();
}

void TestBase::sendFolderSyncRequest()
{
  const QProcessEnvironment &env = QProcessEnvironment::systemEnvironment();
  const QString &serverAddress = env.value("MY_ADDR", "exchange-server.com");
  const QString &serverPort = env.value("MY_PORT", "443");
  const QUrl &serverUrl = QUrl(QLatin1String("https://") + serverAddress + ":" + serverPort + folderSyncSuffix);
  qDebug() << "[TestBase::sendFolderSyncRequest] URL: " << serverUrl;

  static const char TheData[] = {
    0x03, 0x01, 0x6A, 0x00, 0x00, 0x07, 0x56, 0x52,
    0x03, 0x30, 0x00, 0x01, 0x01
  };
  static QBuffer buffer;
  buffer.setData(TheData, sizeof (TheData));
  if (!m_reply) {
    QNetworkRequest request(serverUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/vnd.ms-sync.wbxml");
    request.setRawHeader("MS-ASProtocolVersion", "14.1");
    request.setRawHeader("User-Agent", "ASOM");
    request.setRawHeader("Host", serverAddress.toLatin1());
    const QString &userName = env.value("MY_USER");
    const QString &passWord = env.value("MY_PASS");
    request.setRawHeader("Authorization", "Basic " + QByteArray(QString("%1:%2").arg(userName + "123").arg(passWord + "321").toLatin1()).toBase64());
    m_reply = m_manager.post(request, &buffer);
    connect(m_reply, SIGNAL(finished()), this, SLOT(onFolderSyncReady()));
  }
}

void TestBase::onFolderSyncReady()
{
  qDebug() << "[TestBase::onFolderSyncReady]: error: " << m_reply->error();
  qDebug() << "[TestBase::onFolderSyncReady]: headers: " << m_reply->rawHeaderPairs();
  const QByteArray &bytes = m_reply->readAll();
  Utils::hexDump((const unsigned char *)bytes.data(), bytes.size());
  qDebug() << bytes;

  WBXMLTree *pTree = NULL;
  const WBXMLError wbxmlError = wbxml_tree_from_wbxml((unsigned char *)bytes.data(), bytes.size(), WBXML_LANG_AIRSYNC, &pTree);
  qDebug() << "[TestBase::onFolderSyncReady]: WBXML result: " << (const char *)wbxml_errors_string(wbxmlError);
  Utils::logNode(qDebug(), pTree->root);

  // Clean-up
  wbxml_tree_destroy(pTree);
  delete m_reply;
  m_reply = NULL;
}

void TestBase::onSendMailReady()
{
  qDebug() << "[TestBase::onSendMailReady]: error: " << m_reply->error();
  qDebug() << "[TestBase::onSendMailReady]: headers: " << m_reply->rawHeaderPairs();
  const QByteArray &bytes = m_reply->readAll();
  Utils::hexDump((const unsigned char *)bytes.data(), bytes.size());
  qDebug() << bytes;

/*  WBXMLTree *pTree = NULL;
  const WBXMLError wbxmlError = wbxml_tree_from_wbxml((unsigned char *)bytes.data(), bytes.size(), WBXML_LANG_AIRSYNC, &pTree);
  qDebug() << "[TestBase::onFolderSyncReady]: WBXML result: " << (const char *)wbxml_errors_string(wbxmlError);
  Utils::logNode(qDebug(), pTree->root);

  // Clean-up
  wbxml_tree_destroy(pTree);*/
  delete m_reply;
  m_reply = NULL;
}
