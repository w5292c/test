#include "testbase.h"

TestBase::TestBase() : m_value(0)
{
}

TestBase::~TestBase()
{
}

int TestBase::getValue() const
{
  return m_value;
}

void TestBase::setValue(int newValue)
{
  m_value = newValue;
}
