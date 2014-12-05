#ifndef TEST_BASE_H
#define TEST_BASE_H

#include <QObject>

class TestBase : public QObject
{
  Q_OBJECT
public:
  TestBase();
  virtual ~TestBase();

  int getValue() const;
  void setValue(int newValue);

private:
  int m_value;
};

#endif /* TEST_BASE_H */
