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

static bool testTimezone(const QString &timezoneName);

class MyTimeZoneData : public ICalTimeZoneData
{
public:
  ICalTimeZone myparse(const MSTimeZone *tz);
};

static int weekOfMonth(const QDate &date, bool &isLast)
{
  qDebug() << "in:" << date;
  int result = 0;
  if (date.isValid()) {
    // Move 'day' to the start of the month/year
    QDate day(date.year(), date.month(), 1);
    while (true) {
      ++result;
      day = day.addDays(7);
      qDebug() << "Check:" << day << " >? " << date;
      if (day > date) {
        break;
      }
    }

    isLast = (day.month() != date.month());
  }

  qDebug() << "out" << result << isLast;
  return result;
}

void CalendarTest::test()
{
#if 0
  int n = 0;
  const QStringList &zones = KSystemTimeZones::zones().keys();
  qDebug() << "Number of zones:" << zones.count();
  foreach (const QString &zone, zones) {
    const bool res = testTimezone(zone);
    if (!res) {
      ++n;
//      qDebug() << "Zone:" << zone << ":" << (res ? "PASS" : "FAILED");
    }
  }
  qDebug() << "Failed " << n << " from " << zones.count() << " timezones.";
#else
  QString timezone = "Pacific/Fiji";
  bool res = testTimezone(timezone);
  qDebug() << "Zone: " << timezone << (res ? "PASS" : "FAILED");
#endif
}

bool testTimezone(const QString &timezoneName)
{
  qDebug() << "Testing:" << timezoneName;
  // BEGIN: this code is required for linking
  uuid_t out;
  uuid_generate_random(out);
  // END: this code is required for linking

#if 1
  const KTimeZone &timezone = KSystemTimeZones::readZone(timezoneName);
  KDateTime next = KDateTime::currentUtcDateTime();
  const QList<KTimeZone::Transition> &transitions = timezone.transitions(next.toUtc().dateTime());
//  qDebug() << "Number of transitions:" << transitions.count();
  if (transitions.isEmpty()) {
//    qDebug() << "1";
    return true;
  }
  if (transitions.length() < 2) {
    qDebug() << "Too few transitions";
    return false;
  }
  const KTimeZone::Transition &_transition1 = transitions.at(0);
  const KTimeZone::Transition &_transition2 = transitions.at(1);
  const KTimeZone::Phase &_phase1 = _transition1.phase();
  const KTimeZone::Phase &_phase2 = _transition2.phase();
  if ((_phase1.isDst() && _phase2.isDst()) || (!_phase1.isDst() && !_phase2.isDst())) {
    qDebug() << "2";
    return false;
  }
  // STD
  const KTimeZone::Phase &phase1 = (!_phase1.isDst()) ? _phase1 : _phase2;
  const QDateTime &time1 = (!_phase1.isDst()) ? _transition1.time() : _transition2.time();
  // DST
  const KTimeZone::Phase &phase2 = ( _phase2.isDst()) ? _phase2 : _phase1;
  const QDateTime &time2 = ( _phase2.isDst()) ? _transition2.time() : _transition1.time();

  KCalCore::RecurrenceRule recurrence;
  recurrence.setReadOnly(false);
  recurrence.setRecurrenceType(RecurrenceRule::rYearly);
  recurrence.setFrequency(1);
  recurrence.setStartDt(next);
  recurrence.setBySeconds(QList<int>() << 0);
  recurrence.setByMinutes(QList<int>() << time1.time().minute());
  recurrence.setByHours(QList<int>() << time1.time().hour());
  recurrence.setByMonths(QList<int>() << time1.date().month());
//  int weekNumber1 = time1.date().weekNumber();
  bool isLast1 = false;
  int weekNumber1 = weekOfMonth(time1.date(), isLast1);
//  if (weekNumber1 == 4 || weekNumber1 == 5) {
//  if (isLast1) {
//    weekNumber1 = -1;
//  }
//  }
  recurrence.setByDays(QList<RecurrenceRule::WDayPos>() << RecurrenceRule::WDayPos(weekNumber1, time1.date().dayOfWeek()));

  KDateTime next2 = next;
  KCalCore::RecurrenceRule recurrence2;
  recurrence2.setReadOnly(false);
  recurrence2.setRecurrenceType(RecurrenceRule::rYearly);
  recurrence2.setFrequency(1);
  recurrence2.setStartDt(next);
  recurrence2.setBySeconds(QList<int>() << 0);
  recurrence2.setByMinutes(QList<int>() << time2.time().minute());
  recurrence2.setByHours(QList<int>() << time2.time().hour());
  recurrence2.setByMonths(QList<int>() << time2.date().month());
//  int weekNumber2 = time2.date().weekNumber();
  bool isLast2 = false;
  int weekNumber2 = weekOfMonth(time2.date(), isLast2);
//  if (weekNumber2 == 4 || weekNumber2 == 5) {
//  if (isLast2) {
//    weekNumber2 = -1;
//  }
  recurrence2.setByDays(QList<RecurrenceRule::WDayPos>() << RecurrenceRule::WDayPos(weekNumber2, time2.date().dayOfWeek()));

  bool result1 = true;
  bool result2 = true;
  foreach (const KTimeZone::Transition &transition, transitions) {
    const KTimeZone::Phase &phase = transition.phase();
    if (phase.isDst()) {
      next2 = recurrence2.getNextDate(next2);
      if (transition.time() != next2.dateTime()) {
        qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX NO MATCH!!!";
        qDebug() << ">>> Time1:" << transition.time();
        qDebug() << ">>> Time2:" << next2.dateTime();
        qDebug() << ">>> Phase isDst:" << phase.isDst();
//        qDebug() << ">>> Phase offset:" << phase.utcOffset();
//        qDebug() << ">>> Phase abbreviations:" << phase.abbreviations();
//        qDebug() << ">>> Phase comment:" << phase.comment();
        result1 = false;
      } else {
//        qDebug() << "Match:" << next2.dateTime();
      }
    } else {
      next = recurrence.getNextDate(next);
      if (transition.time() != next.dateTime()) {
        qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX NO MATCH!!!";
        qDebug() << ">>> Time1:" << transition.time();
        qDebug() << ">>> Time2:" << next.dateTime();
        qDebug() << ">>> Phase isDst:" << phase.isDst();
//        qDebug() << ">>> Phase offset:" << phase.utcOffset();
//        qDebug() << ">>> Phase abbreviations:" << phase.abbreviations();
//        qDebug() << ">>> Phase comment:" << phase.comment();
        result2 = false;
      } else {
//        qDebug() << "Match:" << next.dateTime();
      }
    }
  }
  if (result1 && result2) {
    return true;
  }

  if (result2 && isLast1) {
    qDebug() << "Reset 1";
    weekNumber1 = -1;
    recurrence.setByDays(QList<RecurrenceRule::WDayPos>() << RecurrenceRule::WDayPos(weekNumber1, time1.date().dayOfWeek()));
  }
  if (result1 && isLast2) {
    qDebug() << "Reset 2";
    weekNumber2 = -1;
    recurrence2.setByDays(QList<RecurrenceRule::WDayPos>() << RecurrenceRule::WDayPos(weekNumber2, time2.date().dayOfWeek()));
  }

  result1 = true;
  result2 = true;
  next = KDateTime::currentUtcDateTime();
  next2 = KDateTime::currentUtcDateTime();
  foreach (const KTimeZone::Transition &transition, transitions) {
    const KTimeZone::Phase &phase = transition.phase();
    if (phase.isDst()) {
      next2 = recurrence2.getNextDate(next2);
      if (transition.time() != next2.dateTime()) {
        qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX NO MATCH!!!";
        qDebug() << ">>> Time1 (real):" << transition.time();
        qDebug() << ">>> Time2 (rule):" << next2.dateTime();
        qDebug() << ">>> Phase isDst:" << phase.isDst();
        qDebug() << ">>> Phase offset:" << phase.utcOffset();
        qDebug() << ">>> Phase abbreviations:" << phase.abbreviations();
        qDebug() << ">>> Phase comment:" << phase.comment();
        result1 = false;
      }
    } else {
      next = recurrence.getNextDate(next);
      if (transition.time() != next.dateTime()) {
        qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX NO MATCH!!!";
        qDebug() << ">>> Time1 (real):" << transition.time();
        qDebug() << ">>> Time2 (rule):" << next.dateTime();
        qDebug() << ">>> Phase isDst:" << phase.isDst();
        qDebug() << ">>> Phase offset:" << phase.utcOffset();
        qDebug() << ">>> Phase abbreviations:" << phase.abbreviations();
        qDebug() << ">>> Phase comment:" << phase.comment();
        result2 = false;
      }
    }
  }

  return result1 && result2;
#endif // Playing with transitions
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
