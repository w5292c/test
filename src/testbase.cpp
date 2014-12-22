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

#include <QDebug>
#include <QMetaEnum>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QNetworkRequest>
#include <QProcessEnvironment>

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
}

/*!
    Constructs a test object with zero value.
*/
TestBase::TestBase() : m_value(0), m_index(TestBase::IndexIdNone), m_reply(NULL)
{
  connect(this, SIGNAL(testignal(IndexId)), this, SLOT(start(IndexId)));

  m_timer.setSingleShot(false);
  m_timer.setInterval(2000);
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

void TestBase::onTimer()
{
  emit testignal(static_cast<IndexId>(m_index));

  if (IndexIdLast == m_index) {
    m_index = IndexIdNone;
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

void TestBase::sendRequest()
{
  const QProcessEnvironment &env = QProcessEnvironment::systemEnvironment();
  const QString &serverAddress = env.value("MY_ADDR", "exchange-server.com");
  const QString &serverPort = env.value("MY_PORT", "443");
  const QUrl &serverUrl = QUrl(QLatin1String("https://") + serverAddress + ":" + serverPort + "/Microsoft-Server-ActiveSync");
  qDebug() << "[TestBase::sendRequest] URL: " << serverUrl;

  if (!m_reply) {
    QNetworkRequest request(serverUrl);
    m_reply = m_manager.sendCustomRequest(request, "OPTIONS");
    connect(m_reply, SIGNAL(finished()), this, SLOT(onOptionsRequestReady()));
  }
}

void TestBase::onRequestReady(QNetworkReply *reply)
{
  Q_UNUSED(reply);
#if 0
  // This code works as well, but we will log the results in 'TestBase::onOptionsRequestReady'
  qDebug() << "[TestBase::onRequestReady] MS-ASProtocolVersions:" << reply->rawHeader(PROTOCOL_VERSIONS_HEADER);
  qDebug() << "[TestBase::onRequestReady] MS-ASProtocolCommands:" << reply->rawHeader(PROTOCOL_COMMANDS_HEADER);
#endif
}

void TestBase::onOptionsRequestReady()
{
  qDebug() << "[TestBase::onOptionsRequestReady] MS-ASProtocolVersions:" << m_reply->rawHeader(PROTOCOL_VERSIONS_HEADER);
  qDebug() << "[TestBase::onOptionsRequestReady] MS-ASProtocolCommands:" << m_reply->rawHeader(PROTOCOL_COMMANDS_HEADER);
  delete m_reply;
  m_reply = NULL;
}
