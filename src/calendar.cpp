#include "calendar.h"

#include <uuid.h>

#include <QDebug>
#include <icalformat.h>
#include <icaltimezones.h>
#include <extendedstorage.h>
#include <extendedcalendar.h>

using namespace KCalCore;

static MSTimeZone createTimezone();

void CalendarTest::test()
{
  MSTimeZone tz = createTimezone();
  uuid_t out;
  uuid_generate_random(out);
//  qDebug() << (const char *)out;

  ICalTimeZones timezones;
  qDebug() << "Initial timezone count:" << timezones.count();
  ICalTimeZoneSource source;
  const ICalTimeZone &timezone1 = source.parse(&tz/*, timezones*/);
  qDebug() << "Transitions 1:" << timezone1.transitions().length();
  Q_UNUSED(timezone1);
  qDebug() << "Timezone count1:" << timezones.count() << ", " << tz.Bias;
  MSTimeZone tz2 = createTimezone();
  const ICalTimeZone &timezone2 = source.parse(&tz2/*, timezones*/);
  qDebug() << "Transitions 2:" << timezone2.transitions().length();
  qDebug() << "Timezone count2:" << timezones.count() << ", " << tz2.Bias;
  Q_UNUSED(timezone2);
  qDebug() << "Timezone test 1:" << timezone1.utcOffsets();
  qDebug() << "Timezone test 2:" << timezone2.utcOffsets();

  qDebug() << "ICAL 1:" << timezone1.vtimezone();
  qDebug() << "ICAL 2:" << timezone2.vtimezone();

  const Incidence::Ptr event(new Event());

  ICalFormat format;
  const QString &encoded = format.toICalString(event);
  qDebug() << "Encoded:[" << encoded << "]";
}

MSTimeZone createTimezone()
{
  MSTimeZone timezone;
  memset(&timezone.StandardDate, 0, sizeof (timezone.StandardDate));
  memset(&timezone.DaylightDate, 0, sizeof (timezone.DaylightDate));

  static int n = 0;
  QByteArray stdName = QByteArray::fromHex(
    "28 00 55 00  54 00 43 00 2b 00 30 00 34 00 3a 00 "
    "30 00 30 00  29 00 20 00 4d 00 6f 00 73 00 63 00 "
    "6f 00 77 00  2c 00 20 00 53 00 74 00 2e 00 20 00 "
    "50 00 65 00  74 00 65 00 72 00 73 00 62 00 75 00 ");
  stdName[2] = stdName[2] + n; ++n;
  QByteArray dayName = QByteArray::fromHex(
    "28 00 55 00  54 00 43 00 2b 00 30 00 34 00 3a 00"
    "30 00 30 00  29 00 20 00 4d 00 6f 00 73 00 63 00"
    "6f 00 77 00  2c 00 20 00 53 00 74 00 2e 00 20 00"
    "50 00 65 00  74 00 65 00 72 00 73 00 62 00 75 00");

  const QString &stdNameString = QString::fromUtf16(reinterpret_cast<const ushort*>(stdName.constData()));
  const QString &dayNameString = QString::fromUtf16(reinterpret_cast<const ushort*>(dayName.constData()));

  qDebug() << "Std buffer length:" << stdName.length() << ", name:" << stdNameString;
  qDebug() << "Day buffer length:" << dayName.length() << ", name:" << dayNameString;
  timezone.StandardName = stdNameString;
  timezone.DaylightName = dayNameString;
  timezone.Bias = (int)0xFFFFFF4CU - 60 * n;

  timezone.StandardBias = 0;
  timezone.DaylightBias = 0;
  qDebug() << "Bias" << timezone.Bias << ", std:" << timezone.StandardBias << ", day:" << timezone.DaylightBias;

  return timezone;
}

/*
00000000  4c ff ff ff                                         | Bias (4 bytes)
00000004  28 00 55 00  54 00 43 00  2b 00 30 00  34 00 3a 00  | StandardName (64 bytes)
00000014  30 00 30 00  29 00 20 00  4d 00 6f 00  73 00 63 00  |
00000024  6f 00 77 00  2c 00 20 00  53 00 74 00  2e 00 20 00  |
00000034  50 00 65 00  74 00 65 00  72 00 73 00  62 00 75 00  |
00000044  00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00  | StandardDate (16 bytes)
00000058  00 00 00 00                                         | StandardBias ( 4 bytes)
00000058  28 00 55 00  54 00 43 00  2b 00 30 00  34 00 3a 00  | DaylightName (64 bytes)
00000068  30 00 30 00  29 00 20 00  4d 00 6f 00  73 00 63 00  |
00000078  6f 00 77 00  2c 00 20 00  53 00 74 00  2e 00 20 00  |
00000088  50 00 65 00  74 00 65 00  72 00 73 00  62 00 75 00  |
00000098  00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00  | DaylightDate (16 bytes)
000000A8  00 00 00 00                                         | DaylightBias ( 4 bytes)

typedef struct _MSSystemTime {
  qint16 wYear;
  qint16 wMonth;
  qint16 wDayOfWeek;
  qint16 wDay;
  qint16 wHour;
  qint16 wMinute;
  qint16 wSecond;
  qint16 wMilliseconds;
} MSSystemTime;

typedef struct _MSTimeZone {
  long         Bias;
  QString      StandardName;
  MSSystemTime StandardDate;
  long         StandardBias;
  QString      DaylightName;
  MSSystemTime DaylightDate;
  long         DaylightBias;
} MSTimeZone;
*/
