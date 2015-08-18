#include "calendar.h"

#include <uuid.h>

#include <QtCore/qalgorithms.h>
#include <QDebug>
#include <icalformat.h>
#include <icaltimezones.h>
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

const char *const vCalInfo =
"BEGIN:VTIMEZONE\n"
"TZID:Russian Standard Time\n"
"BEGIN:STANDARD\n"
"DTSTART:16010101T000000\n"
"TZOFFSETFROM:+0300\n"
"TZOFFSETTO:+0300\n"
"END:STANDARD\n"
"BEGIN:DAYLIGHT\n"
"DTSTART:16010101T000000\n"
"TZOFFSETFROM:+0300\n"
"TZOFFSETTO:+0300\n"
"END:DAYLIGHT\n"
"END:VTIMEZONE\n";

void CalendarTest::test()
{
  const QString &original = QString::fromLatin1(vCalInfo);
  const QStringList &parts = original.split("\n", QString::SkipEmptyParts);

  ICalTimeZoneSource source;
  const ICalTimeZone &zone = source.parse("name", parts);

  qDebug() << "Info:" << parts;
  qDebug() << "Zone:" << zone.vtimezone();
}

/*
Russia:
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

void CalendarTest::testRule()
{
  printf("Out: %d\n", STDOUT_FILENO);
//  close(STDOUT_FILENO);
  fprintf(stderr, "Out: %d\n", STDOUT_FILENO);
  int fd = open("/dev/null", 0);
  fprintf(stderr, "Out: %d\n", fd);

  RecurrenceRule rule;
  rule.setRecurrenceType(RecurrenceRule::rDaily);
  rule.setFrequency(1);
  rule.setDuration(100);
  rule.setByDays(QList<RecurrenceRule::WDayPos>() << RecurrenceRule::WDayPos(0, 1) << RecurrenceRule::WDayPos(0, 2));

  ICalFormat format;
  const QString &ruleString = format.toString(&rule);

  qDebug() << "RRule:" << ruleString;
}

void CalendarTest::init()
{
  uuid_t out;
  uuid_generate_random(out);
}
