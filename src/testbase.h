#ifndef TEST_BASE_H
#define TEST_BASE_H

#include <QTimer>
#include <QtCore/qmetaobject.h>
#include <QtCore/qdebug.h>

#define TEST_ENUMS_DEBUG(Class, Enum) \
inline QDebug operator<<(QDebug dbg, Class::Enum value) { \
  const int index = Class::staticMetaObject.indexOfEnumerator(#Enum); \
  dbg.nospace() << #Class << "::" << TestBase::staticMetaObject.enumerator(index).valueToKey(value); \
  return dbg.space(); \
}

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

  TestBase();
  virtual ~TestBase();

  int getValue() const;
  void setValue(int newValue);

public slots:
  void onTimer();
  void start(IndexId indexId);

Q_SIGNALS:
  void testignal(IndexId indexId);

private:
  int m_value;
  int m_index;
  QTimer m_timer;
};

Q_DECLARE_METATYPE(TestBase::IndexId)

TEST_ENUMS_DEBUG(TestBase, IndexId)

#endif /* TEST_BASE_H */
