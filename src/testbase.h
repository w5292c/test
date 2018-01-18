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

#ifndef TEST_BASE_H
#define TEST_BASE_H

#include <QStack>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QtCore/qmetaobject.h>
#include <QtCore/qdebug.h>

#define TEST_ENUMS_DEBUG(Class, Enum) \
inline QDebug operator<<(QDebug dbg, Class::Enum value) { \
  const int index = Class::staticMetaObject.indexOfEnumerator(#Enum); \
  dbg.nospace() << #Class << "::" << TestBase::staticMetaObject.enumerator(index).valueToKey(value); \
  return dbg.space(); \
}

#define TEST_COUNT_ARGS(...)  (sizeof((int[]){__VA_ARGS__})/sizeof (int))
#define checkTags(...) check(TEST_COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

class TestBase : public QObject
{
  Q_OBJECT
  Q_ENUMS(IndexId)

public:
  enum IndexId {
    IndexIdNone,
    IndexIdOne,
    IndexIdTwo,
    IndexIdThree,
    IndexIdTest,

    IndexIdLast
  };

  TestBase(bool calendarResponse);
  virtual ~TestBase();

  int getValue() const;
  void setValue(int newValue);
  bool check(int length, ...) const;

public slots:
  void onTimer();
  void onSendMailReady();
  void onFolderSyncReady();
  void start(IndexId indexId);
  void onOptionsRequestReady();
  void onRequestReady(QNetworkReply *reply);
  void onSslErrors(QList<QSslError> errors);
  void authentication(QNetworkReply *reply, QAuthenticator *authenticator);

Q_SIGNALS:
  void testignal(IndexId indexId);

private:
  void sendEmail();
  void sendRequest();
  void sendFolderSyncRequest();

private:
  int m_value;
  int m_index;
  QTimer m_timer;
  QStack<uint> mTags;
  QNetworkReply *m_reply;
  QNetworkAccessManager m_manager;
  bool m_calendarResponse;
};

Q_DECLARE_METATYPE(TestBase::IndexId)

TEST_ENUMS_DEBUG(TestBase, IndexId)

#endif /* TEST_BASE_H */
