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
static bool checkTransitions(const QList<KTimeZone::Transition> &transitions, MSTimeZone &msTimezone);

class MyTimeZoneData : public ICalTimeZoneData
{
public:
  ICalTimeZone myparse(const MSTimeZone *tz);
};

int weekOfMonth2(const QDate &date, bool &isLast)
{
    int result = 0;
    if (date.isValid()) {
        // Find the number of the week-day in the month
        result = (date.day() + 6) / 7;
        // Check if the date is the last week-day in the month
        isLast = (date.month() != date.addDays(7).month());
    }

    qDebug() << "Checked date:" << date
             << ", week-of-month:" << result << ", last:" << isLast;
    return result;
}

int weekOfMonth(const QDate &date, bool &isLast)
{
    int result = 0;
    if (date.isValid()) {
        // Move 'day' to the start of the month/year
        QDate day(date.year(), date.month(), 1);
        // Find the first day in month of the input week-day
        while (day.dayOfWeek() != date.dayOfWeek()) {
            day = day.addDays(1);
        }
        result = 1;
        while (day != date) {
            ++result;
            day = day.addDays(7);
        }
        // Check if the date is the last week-day in the month
        isLast = (date.month() != date.addDays(7).month());
    }

    qDebug() << "Checked date:" << date << ", week-of-month:" << result << ", last:" << isLast;
    return result;
}

bool checkTransitionsForRule(const QList<KTimeZone::Transition> &transitions,
                      const KCalCore::RecurrenceRule &rule, bool dst, bool required)
{
    if (transitions.isEmpty()) {
        return false;
    }

    const KTimeZone::Transition &firstTransition = transitions.at(0);
    KDateTime next = KDateTime(firstTransition.time().addSecs(-1), KDateTime::Spec::UTC());
    bool expectedDst = firstTransition.phase().isDst();
    foreach (const KTimeZone::Transition &transition, transitions) {
        const KTimeZone::Phase &phase = transition.phase();
        // Check the phase DST/STD
        const bool phaseDst = phase.isDst();
        if (phaseDst != expectedDst) {
            return false;
        }
        // Update the expected DST/STD
        expectedDst = !expectedDst;
        if (phaseDst == dst) {
            next = rule.getNextDate(next);
            if (next.dateTime() != transition.time()) {
                if (required) {
                    qWarning() << "Check failed (rule):" << next.dateTime()
                               << "does not match (expected):" << transition.time();
                }
                return false;
            }
        }
    }

    return true;
}

bool checkTransitions(const QList<KTimeZone::Transition> &transitions, MSTimeZone &msTimezone)
{
    if (transitions.isEmpty()) {
        return true;
    }
    if (transitions.length() < 2) {
        qCritical() << "Too few transitions";
        return false;
    }

    // Next, check if the next 2 transitions define STD and DST phases
    const KTimeZone::Transition &_transition1 = transitions.at(0);
    const KTimeZone::Transition &_transition2 = transitions.at(1);
    const KTimeZone::Phase &_phase1 = _transition1.phase();
    const KTimeZone::Phase &_phase2 = _transition2.phase();
    const KTimeZone::Phase &phaseStd = (!_phase1.isDst()) ? _phase1 : _phase2;
    const KTimeZone::Phase &phaseDst = ( _phase1.isDst()) ? _phase1 : _phase2;
    if (phaseStd.isDst() || !phaseDst.isDst()) {
        qCritical() << "Unexpected phases types";
        return false;
    }

    // Get the transitions to STD/DST phases
    const KTimeZone::Transition &transitionStd = (!_phase1.isDst()) ? _transition1 : _transition2;
    const KTimeZone::Transition &transitionDst = ( _phase1.isDst()) ? _transition1 : _transition2;
    const QDateTime &startStd = transitionStd.time();
    const QDate &startDateStd = startStd.date();
    const QTime &startTimeStd = startStd.time();
    const QDateTime &startDst = transitionDst.time();
    const QDate &startDateDst = startDst.date();
    const QTime &startTimeDst = startDst.time();

    KCalCore::RecurrenceRule ruleStd;
    ruleStd.setReadOnly(false);
    ruleStd.setRecurrenceType(RecurrenceRule::rYearly);
    ruleStd.setFrequency(1);
    ruleStd.setStartDt(KDateTime(startStd, KDateTime::Spec::UTC()));
    ruleStd.setBySeconds(QList<int>() << startTimeStd.second());
    ruleStd.setByMinutes(QList<int>() << startTimeStd.minute());
    ruleStd.setByHours(QList<int>() << startTimeStd.hour());
    ruleStd.setByMonths(QList<int>() << startDateStd.month());
    bool lastWeekStd = false;
    int monthWeekStd = weekOfMonth(startDateStd, lastWeekStd);
    bool lastWeekStd2 = false;
    int monthWeekStd2 = weekOfMonth2(startDateStd, lastWeekStd2);
    if (lastWeekStd2 != lastWeekStd || monthWeekStd2 != monthWeekStd) {
        qCritical() << "Does not match!!!!!!!!!!!!!";
        exit(1);
        return false;
    }
    if (monthWeekStd == 5 && lastWeekStd) {
        monthWeekStd = -1;
    }
    ruleStd.setByDays(QList<RecurrenceRule::WDayPos>() <<
                      RecurrenceRule::WDayPos(monthWeekStd, startDateStd.dayOfWeek()));
    bool success = true;
    if (!checkTransitionsForRule(transitions, ruleStd, false, monthWeekStd == -1 || !lastWeekStd)) {
        // Try last-week-of-month rule
        if (monthWeekStd != -1 && lastWeekStd) {
            monthWeekStd = -1;
            ruleStd.setByDays(QList<RecurrenceRule::WDayPos>() <<
                              RecurrenceRule::WDayPos(monthWeekStd, startDateStd.dayOfWeek()));
            if (!checkTransitionsForRule(transitions, ruleStd, false, true)) {
                qWarning() << "STD transitions check failed";
                success = false;
            }
        } else {
            qWarning() << "DST transitions check failed";
            success = false;
        }
    }

    int monthWeekDst = 0;
    KCalCore::RecurrenceRule ruleDst;
    if (success) {
        ruleDst.setReadOnly(false);
        ruleDst.setRecurrenceType(RecurrenceRule::rYearly);
        ruleDst.setFrequency(1);
        ruleDst.setStartDt(KDateTime(startDst, KDateTime::Spec::UTC()));
        ruleDst.setBySeconds(QList<int>() << startTimeDst.second());
        ruleDst.setByMinutes(QList<int>() << startTimeDst.minute());
        ruleDst.setByHours(QList<int>() << startTimeDst.hour());
        ruleDst.setByMonths(QList<int>() << startDateDst.month());
        bool lastWeekDst = false;
        monthWeekDst = weekOfMonth(startDateDst, lastWeekDst);
        bool lastWeekDst2 = false;
        int monthWeekDst2 = weekOfMonth2(startDateDst, lastWeekDst2);
        if (monthWeekDst != monthWeekDst2 || lastWeekDst2 != lastWeekDst) {
            qCritical() << "Does not match DST: !!!!!!!!!!!!!!!!!!!";
            exit(1);
            return false;
        }
        if (monthWeekDst == 5 && lastWeekDst) {
            monthWeekDst = -1;
        }
        ruleDst.setByDays(QList<RecurrenceRule::WDayPos>() <<
                          RecurrenceRule::WDayPos(monthWeekDst, startDateDst.dayOfWeek()));
        if (!checkTransitionsForRule(transitions, ruleDst, true,
                                     monthWeekStd == -1 || !lastWeekStd)) {
            // Try last-week-of-month rule
            if (monthWeekDst != -1 && lastWeekDst) {
                monthWeekDst = -1;
                ruleDst.setByDays(QList<RecurrenceRule::WDayPos>() <<
                                  RecurrenceRule::WDayPos(monthWeekDst, startDateDst.dayOfWeek()));
                if (!checkTransitionsForRule(transitions, ruleDst, true, true)) {
                    qWarning() << "DST transitions check failed";
                    success = false;
                }
            } else {
                qWarning() << "DST transitions check failed";
                success = false;
            }
        }
    }

    if (success) {
        const int biasStd = phaseStd.utcOffset()/(-60);
        const int biasDst = phaseDst.utcOffset()/(-60);

        msTimezone.Bias = biasStd;
        msTimezone.StandardBias = 0;
        msTimezone.DaylightBias = biasDst - biasStd;

        msTimezone.StandardDate.wYear = 0;
        msTimezone.StandardDate.wMonth = startDateStd.month();
        msTimezone.StandardDate.wMilliseconds = startTimeStd.msec();
        msTimezone.StandardDate.wSecond = startTimeStd.second();
        msTimezone.StandardDate.wMinute = startTimeStd.minute();
        msTimezone.StandardDate.wHour = startTimeStd.hour();
        msTimezone.StandardDate.wDayOfWeek =
            (startDateStd.dayOfWeek() == 7) ? 0 : startDateStd.dayOfWeek();
        msTimezone.StandardDate.wDay = (monthWeekStd == -1) ? 5 : monthWeekStd;

        msTimezone.DaylightDate.wYear = 0;
        msTimezone.DaylightDate.wMonth = startDateDst.month();
        msTimezone.DaylightDate.wMilliseconds = startTimeDst.msec();
        msTimezone.DaylightDate.wSecond = startTimeDst.second();
        msTimezone.DaylightDate.wMinute = startTimeDst.minute();
        msTimezone.DaylightDate.wHour = startTimeDst.hour();
        msTimezone.DaylightDate.wDayOfWeek =
            (startDateDst.dayOfWeek() == 7) ? 0 : startDateDst.dayOfWeek();
        msTimezone.DaylightDate.wDay = (monthWeekDst == -1) ? 5 : monthWeekDst;
    }

    return success;
}

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

#if 0
  qDebug() << "Time:" << KDateTime::currentUtcDateTime().toString();
#if 0
  const KDateTime &time = KDateTime::currentUtcDateTime();
  qDebug() << "Time2:" << time.isValid();

#if 1
  int n = 0;
  const QStringList &zones = KSystemTimeZones::zones().keys();
  qDebug() << "Number of zones:" << zones.count();
  foreach (const QString &zone, zones) {
    const bool res = testTimezone(zone);
    if (!res) {
      ++n;
    }
    qDebug() << "Zone:" << zone << ":" << (res ? "PASS" : "!!!!!!!!!!!!!!!!!! FAILED !!!!!!!!!!!!!!!!!!");
  }
  qDebug() << "Failed " << n << " from " << zones.count() << " timezones.";
#else
//  QString timezone = "Pacific/Fiji";
//  QString timezone = "Europe/Helsinki";
  QString timezone = "Europe/Moscow";
  bool res = testTimezone(timezone);
  qDebug() << "Zone: " << timezone << (res ? "PASS" : "!!!!!!!!!!!!!!!!!! FAILED !!!!!!!!!!!!!!!!!!");
#endif

#else
  //xxxx
  const KDateTime &time = KDateTime::currentUtcDateTime();
  qDebug() << "Time2:" << time.isValid();
  qDebug() << "Type:" << time.timeSpec().type();
#endif
#endif
}

bool testTimezone(const QString &timezoneName)
{
//  qDebug() << "Testing:" << timezoneName;

  const KTimeZone &timezone = KSystemTimeZones::readZone(timezoneName);
  KDateTime next = KDateTime::currentUtcDateTime();
  const QList<KTimeZone::Transition> &transitions = timezone.transitions(next.toUtc().dateTime());
//  transitions.removeAt(0);
//  transitions.removeAt(0);

  MSTimeZone msTimezone;
  const bool res = checkTransitions(transitions, msTimezone);
  return res;
#if 0
  if (transitions.isEmpty()) {
    // No transitions, this is ok
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
    qDebug() << "Missmatch phases";
    return false;
  }
  // STD
//  const KTimeZone::Phase &phase1 = (!_phase1.isDst()) ? _phase1 : _phase2;
  const QDateTime &timeStd = (!_phase1.isDst()) ? _transition1.time() : _transition2.time();
  // DST
//  const KTimeZone::Phase &phase2 = ( _phase2.isDst()) ? _phase2 : _phase1;
  const QDateTime &timeDst = ( _phase2.isDst()) ? _transition2.time() : _transition1.time();
  qDebug() << "Times:" << timeStd << timeDst;

  KCalCore::RecurrenceRule recurrence;
  recurrence.setReadOnly(false);
  recurrence.setRecurrenceType(RecurrenceRule::rYearly);
  recurrence.setFrequency(1);
  recurrence.setStartDt(next);
  recurrence.setBySeconds(QList<int>() << 0);
  recurrence.setByMinutes(QList<int>() << timeStd.time().minute());
  recurrence.setByHours(QList<int>() << timeStd.time().hour());
  recurrence.setByMonths(QList<int>() << timeStd.date().month());
//  int weekNumber1 = timeStd.date().weekNumber();
  bool isLastStd = false;
  int weekNumber1 = weekOfMonth(timeStd.date(), isLastStd);
  recurrence.setByDays(QList<RecurrenceRule::WDayPos>() << RecurrenceRule::WDayPos(weekNumber1, timeStd.date().dayOfWeek()));

  KCalCore::RecurrenceRule recurrenceDst;
  recurrenceDst.setReadOnly(false);
  recurrenceDst.setRecurrenceType(RecurrenceRule::rYearly);
  recurrenceDst.setFrequency(1);
  recurrenceDst.setStartDt(next);
  recurrenceDst.setBySeconds(QList<int>() << 0);
  recurrenceDst.setByMinutes(QList<int>() << timeDst.time().minute());
  recurrenceDst.setByHours(QList<int>() << timeDst.time().hour());
  recurrenceDst.setByMonths(QList<int>() << timeDst.date().month());
  bool isLastDst = false;
  int weekNumber2 = weekOfMonth(timeDst.date(), isLastDst);
  recurrenceDst.setByDays(QList<RecurrenceRule::WDayPos>() << RecurrenceRule::WDayPos(weekNumber2, timeDst.date().dayOfWeek()));

  bool resultStd = true;
  bool resultDst = true;
  KDateTime nextStd = KDateTime::currentUtcDateTime();
  KDateTime nextDst = nextStd;
  foreach (const KTimeZone::Transition &transition, transitions) {
    const KTimeZone::Phase &phase = transition.phase();
    qDebug() << "Time:" << transition.time();
    if (phase.isDst()) {
      nextDst = recurrenceDst.getNextDate(nextDst);
      if (transition.time() != nextDst.dateTime()) {
        qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX NO MATCH!!!";
        qDebug() << ">>> Time1:" << transition.time();
        qDebug() << ">>> Time2:" << nextDst.dateTime();
        qDebug() << ">>> Phase isDst:" << phase.isDst();
        resultDst = false;
      }
    } else {
      nextStd = recurrence.getNextDate(nextStd);
      if (transition.time() != nextStd.dateTime()) {
        qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX NO MATCH!!!";
        qDebug() << ">>> Time1:" << transition.time();
        qDebug() << ">>> Time2:" << nextStd.dateTime();
        qDebug() << ">>> Phase isDst:" << phase.isDst();
        resultStd = false;
      }
    }
  }
  if (resultStd && resultDst) {
    return true;
  }

  if (resultStd && isLastStd) {
    qDebug() << "Reset 1";
    weekNumber1 = -1;
    recurrence.setByDays(QList<RecurrenceRule::WDayPos>() << RecurrenceRule::WDayPos(weekNumber1, timeStd.date().dayOfWeek()));
  }
  if (resultDst && isLastDst) {
    qDebug() << "Reset 2";
    weekNumber2 = -1;
    recurrenceDst.setByDays(QList<RecurrenceRule::WDayPos>() << RecurrenceRule::WDayPos(weekNumber2, timeDst.date().dayOfWeek()));
  }

  resultStd = true;
  resultDst = true;
  nextStd = KDateTime::currentUtcDateTime();
  nextDst = KDateTime::currentUtcDateTime();
  foreach (const KTimeZone::Transition &transition, transitions) {
    const KTimeZone::Phase &phase = transition.phase();
    if (phase.isDst()) {
      nextDst = recurrenceDst.getNextDate(nextDst);
      if (transition.time() != nextDst.dateTime()) {
        qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX NO MATCH!!!";
        qDebug() << ">>> Time1 (real):" << transition.time();
        qDebug() << ">>> Time2 (rule):" << nextDst.dateTime();
        qDebug() << ">>> Phase isDst:" << phase.isDst();
        qDebug() << ">>> Phase offset:" << phase.utcOffset();
        qDebug() << ">>> Phase abbreviations:" << phase.abbreviations();
        qDebug() << ">>> Phase comment:" << phase.comment();
        resultDst = false;
      }
    } else {
      nextStd = recurrence.getNextDate(nextStd);
      if (transition.time() != nextStd.dateTime()) {
        qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX NO MATCH!!!";
        qDebug() << ">>> Time1 (real):" << transition.time();
        qDebug() << ">>> Time2 (rule):" << nextStd.dateTime();
        qDebug() << ">>> Phase isDst:" << phase.isDst();
        qDebug() << ">>> Phase offset:" << phase.utcOffset();
        qDebug() << ">>> Phase abbreviations:" << phase.abbreviations();
        qDebug() << ">>> Phase comment:" << phase.comment();
        resultStd = false;
      }
    }
  }

  return resultStd && resultDst;
#endif
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
