#include "testbase.h"

#include <QMetaEnum>
#include <QDebug>

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

/*!
    Constructs a test object with zero value.
*/
TestBase::TestBase() : m_value(0), m_index(TestBase::IndexIdNone)
{
  connect(this, SIGNAL(testignal(IndexId)), this, SLOT(start(IndexId)));

  m_timer.setSingleShot(false);
  m_timer.setInterval(2000);
  connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
  m_timer.start();
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
  }
  ++m_index;
}

void TestBase::start(IndexId indexId)
{
  qDebug() << "[TestBase::start]: " << indexId;
}
