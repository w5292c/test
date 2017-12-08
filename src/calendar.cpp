#include "calendar.h"

#include <uuid.h>

#include <QtCore/qalgorithms.h>
#include <QDebug>
#include <icalformat.h>
#include <icaltimezones.h>
#include <memorycalendar.h>
#include <extendedstorage.h>
#include <extendedcalendar.h>
#include <libical/ical.h>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#include <recurrencerule.h>
#include <ksystemtimezone.h>

using namespace mKCal;
using namespace KCalCore;

namespace {
void initTestMsTimezone(MSTimeZone &msTimezone, int variant);
}

void CalendarTest::test()
{
  QDateTime dt(QDate(2015, 1, 16), QTime(10, 15, 30), Qt::UTC);
  qDebug() << "Test:" << dt.toString(Qt::ISODate);
#if 1
  ICalTimeZones zones;

  MSTimeZone msTimezone;
  MSTimeZone msTimezone1;
  MSTimeZone msTimezone2;
  MSTimeZone msTimezone3;
  initTestMsTimezone(msTimezone, 0);
  initTestMsTimezone(msTimezone1, 1);
  initTestMsTimezone(msTimezone2, 2);
  initTestMsTimezone(msTimezone3, 3);

  ICalTimeZoneSource source;
  const ICalTimeZone &zone1 = source.parse(&msTimezone, zones);
  const ICalTimeZone &zone2 = source.parse(&msTimezone1, zones);
  const ICalTimeZone &zone3 = source.parse(&msTimezone2, zones);
  const ICalTimeZone &zone4 = source.parse(&msTimezone3, zones);

  qDebug() << "Zone 1:" << zone1.vtimezone();
  qDebug() << "Zone 2:" << zone2.vtimezone();
  qDebug() << "Zone 3:" << zone3.vtimezone();
  qDebug() << "Zone 4:" << zone4.vtimezone();

  const MemoryCalendar::Ptr &temp = MemoryCalendar::Ptr(new MemoryCalendar(KDateTime::Spec::UTC()));
  temp->setTimeZones(new ICalTimeZones(zones));

  ICalFormat ical;
  const QByteArray &data = ical.toString( temp , QString() ).toUtf8();
  qDebug() << "Calendar data:" << data;

  ICalFormat ical2;
  const MemoryCalendar::Ptr &temp2 = MemoryCalendar::Ptr(new MemoryCalendar(KDateTime::Spec::UTC()));
  if (ical2.fromString(temp2 , QString::fromUtf8(data))) {
    qDebug() << "Loaded timezone information";
    ICalTimeZones *zones = temp->timeZones();
    const int n = zones->count();
    qDebug() << "Number of loaded zones:" << n;
    const QMap<QString, ICalTimeZone> &loadedZones = zones->zones();
    QMapIterator<QString, ICalTimeZone> it(loadedZones);
    while (it.hasNext()) {
      it.next();
      qDebug() << "Key:" << it.key();
      const ICalTimeZone &zone = it.value();
      qDebug() << "Zone info:" << zone.vtimezone();
    }

    KDateTime original(QDate(2015, 7, 10), QTime(14, 33, 45), KDateTime::Spec::UTC());
    qDebug() << "Original time:" << original.dateTime();

    it = loadedZones;
    while (it.hasNext()) {
      it.next();
      qDebug() << "Time:" << original.toZone(it.value()).dateTime();
    }
  } else {
    qCritical() << "Failed to convert";
  }
#endif
}

void CalendarTest::init()
{
  uuid_t out;
  uuid_generate_random(out);
}

void CalendarTest::icalTest()
{
  MSTimeZone timezoneInfo;
  timezoneInfo.Bias = 180;
  timezoneInfo.StandardBias = 0;
  timezoneInfo.DaylightBias = 0;
  memset(&timezoneInfo.StandardDate, 0, sizeof (timezoneInfo.StandardDate));
  memset(&timezoneInfo.DaylightDate, 0, sizeof (timezoneInfo.DaylightDate));

  ICalTimeZoneSource source;
  const ICalTimeZone &timezone = source.parse(&timezoneInfo);
  KDateTime::Spec timezoneSpec;
  timezoneSpec.setType(timezone);


  KDateTime::Spec clockSpec(KDateTime::ClockTime);
  KDateTime::Spec localSpec(KDateTime::LocalZone);

  KCalCore::Event::Ptr event(new KCalCore::Event);
  event->setSchedulingID("ffffffff-ffff-1234-5678-22d38f61a45f");
  event->setLocation("Test location A1");
  event->setStatus(KCalCore::Incidence::StatusConfirmed);
  event->setDtStart(KDateTime::fromString("20171208T100000Z").toTimeSpec(localSpec));

  ICalFormat icf;
  const QString &ical = icf.createScheduleMessage(event, iTIPReply);
  qDebug() << "Here is the iCalendar data:" << endl
           << "********************************************************************************" << endl
           << (const char *)ical.toLatin1().data()
           << "\r********************************************************************************";
}

namespace {

void initTestMsTimezone(MSTimeZone &msTimezone, int variant)
{
  int bias = 0;
  QString name;
  switch (variant) {
  case 0:
    bias = -180;
    name = "MSK1";
    break;
  case 1:
    bias = -180;
    name = "MSK2";
    break;
  case 2:
    bias = -240;
    name = "MSK3";
    break;
  case 3:
    bias = -240;
    name = "MSK4";
    break;
  }

  msTimezone.Bias = bias;
  msTimezone.StandardName = name;
  memset(&msTimezone.StandardDate, 0, sizeof (MSSystemTime));
  msTimezone.StandardBias = 0;
  msTimezone.DaylightName = name;
  memset(&msTimezone.DaylightDate, 0, sizeof (MSSystemTime));
  msTimezone.DaylightBias = 0;
}

} // namespace {
