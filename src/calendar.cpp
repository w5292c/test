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

static MSTimeZone createTimezone();
//static ICalTimeZone myparse(const MSTimeZone *tz);

class MyTimeZoneData : public ICalTimeZoneData
{
public:
  ICalTimeZone myparse(const MSTimeZone *tz);
};

void CalendarTest::test()
{
#if 0
  // Get the current time
  KDateTime next = KDateTime::currentUtcDateTime();
  qDebug() << "Date:" << next.date() << ", time:" << next.time();
  // Initialize the recurrence object
  KCalCore::RecurrenceRule recurrence;
  recurrence.setReadOnly(false);
  recurrence.setRecurrenceType(RecurrenceRule::rYearly);
  recurrence.setFrequency(1);
  recurrence.setStartDt(next);
  recurrence.setByMonths(QList<int>() << 10);
  recurrence.setByDays(QList<RecurrenceRule::WDayPos>() << RecurrenceRule::WDayPos(-1, 6));
/*  recurrence.setYearly(1);*/
/*  recurrence.addYearlyDay(2);*/
/*  recurrence.addYearlyMonth(5);*/
  // Get the next 20 items
  for (int i = 0; i < 20; ++i) {
    next = recurrence.getNextDate(next);
    qDebug() << "Date:" << next.date() << ", time:" << next.time();
  }
#endif

#if 0
  qDebug() << "Expected:" << "20150428T111942Z";
  const KDateTime &currentLocalDateTime = KDateTime::currentLocalDateTime();
  qDebug() << "Date-time string:" << currentLocalDateTime.toString();
  const KDateTime &currentLocalDateTimeUtc =
      currentLocalDateTime.addMSecs(-currentLocalDateTime.time().msec()).toUtc();
  qDebug() << "Date-time string (UTC):" << currentLocalDateTimeUtc.toString();
#endif

#if 0
    // 1) NO daylight saving time: wMonth MUST be 0;
    // 2) Single transition: wYear is NOT 0;
    // 3) Yearly transitions:
    //    - wYear is 0;
    //    - ;
    MSTimeZone tz;
    tz.Bias = -180;
    tz.StandardBias = 0;
    tz.DaylightBias = -60;
    memset(&tz.StandardDate, 0, sizeof (MSSystemTime));
    memset(&tz.DaylightDate, 0, sizeof (MSSystemTime));
    tz.StandardDate.wYear = 0;
    tz.StandardDate.wMinute = 0;
    tz.StandardDate.wSecond = 0;
    tz.StandardDate.wMilliseconds = 0;
    tz.DaylightDate.wYear = 0;
    tz.DaylightDate.wMinute = 0;
    tz.DaylightDate.wSecond = 0;
    tz.DaylightDate.wMilliseconds = 0;
    // StandardDate
    tz.StandardDate.wMonth = 10;
    tz.StandardDate.wDayOfWeek = 6;
    tz.StandardDate.wDay = 5; // LAST SUN
    tz.StandardDate.wHour = 4;
    // DaylightDate
    tz.DaylightDate.wMonth = 3;
    tz.DaylightDate.wDayOfWeek = 6;
    tz.DaylightDate.wDay = 5; // LAST SUN
    tz.DaylightDate.wHour = 3;

    ICalTimeZoneSource source;
    const ICalTimeZone &timezone = source.parse(&tz);
    qDebug() << "*******************************************************************";
    qDebug() << "ICAL timezone:" << timezone.vtimezone();
    qDebug() << "*******************************************************************";
#endif

#if 1
  ExtendedCalendar::Ptr mCalendar = ExtendedCalendar::Ptr(new ExtendedCalendar(KDateTime::Spec::LocalZone()));
  ExtendedStorage::Ptr mStorage = ExtendedCalendar::defaultStorage(mCalendar);
  mStorage->open();

  MSTimeZone tz = createTimezone();
  const KCalCore::ICalTimeZone &iz = mCalendar->parseZone(&tz);
  const KDateTime::Spec spec(iz);
  qDebug() << "Valid:" << spec.isValid();
  qDebug() << "Type:" << spec.type();
  qDebug() << "Offset:" << spec.utcOffset();
  qDebug() << "TZ Valid:" << spec.timeZone().isValid();
  qDebug() << "TZ Offsets:" << spec.timeZone().utcOffsets();

  const KDateTime &time = KDateTime::currentLocalDateTime();
  qDebug() << "Local:" << time.isLocalZone();
  qDebug() << "Time:" << time.time();
  qDebug() << "Date:" << time.date();

  const KTimeZone &timezone1 = time.timeZone();
  qDebug() << "Timezone name:" << timezone1.name();

  /*timezone1.name()*/
//  const KTimeZone &timezone = KSystemTimeZones::zone("Europe/Moscow");
//  const KTimeZone &timezone = KSystemTimeZones::readZone("Europe/Moscow");
//  const KTimeZone &timezone = KSystemTimeZones::zone("Europe/Tallinn");
  const KTimeZone &timezone = KSystemTimeZones::readZone("Europe/Tallinn");
  const QStringList &zones = KSystemTimeZones::zones().keys();
  qDebug() << "Number of zones:" << zones.count();
  QString collectedZones;
  foreach (const QString &zone, zones) {
      collectedZones.append(zone + ":");
  }
  qDebug() << "Zones: [" << collectedZones << "]";

  const QList<KTimeZone::Phase> &phases = timezone.phases();
  qDebug() << "Number of phases:" << phases.count();
  foreach (const KTimeZone::Phase &phase, phases) {
    qDebug() << "***********************************************";
    qDebug() << ">>> isDst:" << phase.isDst();
    qDebug() << ">>> Offset:" << phase.utcOffset();
    qDebug() << ">>> Abbreviations:" << phase.abbreviations();
    qDebug() << ">>> comment:" << phase.comment();
  }
  qDebug() << "***********************************************";

  KDateTime next = KDateTime::currentUtcDateTime();
  const QList<KTimeZone::Transition> &transitions = timezone.transitions(next.toUtc().dateTime());
  qDebug() << "Number of transitions:" << transitions.count();

  KCalCore::RecurrenceRule recurrence;
  recurrence.setReadOnly(false);
  recurrence.setRecurrenceType(RecurrenceRule::rYearly);
  recurrence.setFrequency(1);
  recurrence.setStartDt(next);
  recurrence.setBySeconds(QList<int>() << 0);
  recurrence.setByMinutes(QList<int>() << 0);
  recurrence.setByHours(QList<int>() << 1);
  recurrence.setByMonths(QList<int>() << 10);
  recurrence.setByDays(QList<RecurrenceRule::WDayPos>() << RecurrenceRule::WDayPos(-1, 7));
  foreach (const KTimeZone::Transition &transition, transitions) {
    const KTimeZone::Phase &phase = transition.phase();
    if (phase.isDst()) {
    } else {
      next = recurrence.getNextDate(next);
      if (transition.time() != next.dateTime()) {
        qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX NO MATCH!!!";
        qDebug() << ">>> Time1:" << transition.time();
        qDebug() << ">>> Phase isDst:" << phase.isDst();
        qDebug() << ">>> Phase offset:" << phase.utcOffset();
        qDebug() << ">>> Phase abbreviations:" << phase.abbreviations();
        qDebug() << ">>> Phase comment:" << phase.comment();
      } else {
        qDebug() << "Match:" << next.dateTime();
      }
    }
  }

  qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";

  qDebug() << "Timezone offset:" << timezone.currentOffset();

  const QString &countryCode = timezone.countryCode();
  qDebug() << "Timezone country code:" << countryCode;

  qDebug() << "Abbreviations:" << timezone.abbreviations();
  qDebug() << "Timezone type:" << timezone.type();
  const QDateTime &currTime = QDateTime::currentDateTimeUtc();
  const int offset = timezone.offsetAtUtc(currTime);
  qDebug() << "Curr time:" << currTime << ", offset:" << offset << ", bias:" << offset / -60;
#endif // Playing with transitions

#if 0
  const KDateTime &time = KDateTime::currentUtcDateTime();
  qDebug() << "Now:" << time.date() << ", time:" << time.time() << ", time_t:" << time.toTime_t();
//  qDebug() << "Timezone XXX:" << iz.vtimezone();

  const QString &uid = KCalCore::CalFormat::createUniqueId();
  icalcomponent *const tzcomp = icalcomponent_new(ICAL_VTIMEZONE_COMPONENT);
  icalcomponent_add_property(tzcomp, icalproperty_new_tzid(uid.toUtf8()));
  icalcomponent *phaseCompStd = icalcomponent_new(ICAL_XSTANDARD_COMPONENT);
  icalcomponent *phaseCompDst = icalcomponent_new(ICAL_XDAYLIGHT_COMPONENT);

  icalcomponent_add_component(tzcomp, phaseCompStd);
  icalcomponent_add_component(tzcomp, phaseCompDst);

  ICalTimeZoneSource source;
  const ICalTimeZone &timezone1 = source.parse(tzcomp);
  qDebug() << "Timezone YYY:" << timezone1.vtimezone();

  uuid_t out;
  uuid_generate_random(out);
//  qDebug() << (const char *)out;

  ICalTimeZones timezones;
  qDebug() << "Initial timezone count:" << timezones.count();
  ICalTimeZoneSource sourceA;
  const ICalTimeZone &timezoneB = sourceA.parse(&tz/*, timezones*/);
  qDebug() << "Transitions 1:" << timezoneB.transitions().length();
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
//  event->

  ICalFormat format;
  const QString &encoded = format.toICalString(event);
  qDebug() << "Encoded:[" << encoded << "]";

  QLatin1String testTime("20150330T121005Z");
  KDateTime dataTime = KDateTime::fromString(testTime, KDateTime::ISODate); //, TimeFormat format = ISODate, bool *negZero = 0);
  qDebug() << "DateTime, date:" << dataTime.date() << ", time:" << dataTime.time();
#endif
  uuid_t out;
  uuid_generate_random(out);
}

MSTimeZone createTimezone()
{
  MSTimeZone timezone;
  memset(&timezone.StandardDate, 0, sizeof (timezone.StandardDate));
  memset(&timezone.DaylightDate, 0, sizeof (timezone.DaylightDate));

  timezone.StandardDate.wYear = 0;
  timezone.StandardDate.wMonth = 0;
  timezone.StandardDate.wDay = 0;
//  qint16 wHour;
//  qint16 wMinute;
//  qint16 wSecond;
//  qint16 wMilliseconds;
  timezone.DaylightDate.wYear = 0;
  timezone.DaylightDate.wMonth = 0;
  timezone.DaylightDate.wDay = 0;

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
  timezone.Bias = (int)0xFFFFFF4CU;

  timezone.StandardBias = 0;
  timezone.DaylightBias = 0;
  qDebug() << "Bias" << timezone.Bias << ", std:" << timezone.StandardBias << ", day:" << timezone.DaylightBias;

//  QByteArray buffer;
//  buffer.append("\x01\x02\x03\x04\x05");
//  QDataStream stream(buffer);
//  stream.setByteOrder(QDataStream::LittleEndian);
//  qint32 partsCount = 0;
//  stream >> partsCount;
//  qDebug() << "Here is XXXXXXXXXXXXXXX:" << hex << partsCount;

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
#if 0
ICalTimeZone MyTimeZoneData::myparse(const MSTimeZone *tz)
{
//  ICalTimeZoneData kdata;

  // General properties.
  uuid_t uuid;
  char suuid[64];
  uuid_generate_random( uuid );
  uuid_unparse( uuid, suuid );
  QString name = QString( suuid );

  // Create phases.
  QList<KTimeZone::Phase> phases;

  QList<QByteArray> standardAbbrevs;
  standardAbbrevs += tz->StandardName.toLatin1();
  const KTimeZone::Phase standardPhase(
    ( tz->Bias + tz->StandardBias ) * -60,
    standardAbbrevs, false,
    "Microsoft TIME_ZONE_INFORMATION" );
  phases += standardPhase;

  QList<QByteArray> daylightAbbrevs;
  daylightAbbrevs += tz->DaylightName.toLatin1();
  const KTimeZone::Phase daylightPhase(
    ( tz->Bias + tz->DaylightBias ) * -60,
    daylightAbbrevs, true,
    "Microsoft TIME_ZONE_INFORMATION" );
  phases += daylightPhase;

  // Set phases used by the time zone, but note that previous time zone
  // abbreviation is not known.
  const int prevOffset = tz->Bias * -60;
  setPhases( phases, prevOffset );

  // Create transitions
  QList<KTimeZone::Transition> transitions;
//  ICalTimeZoneSourcePrivate::parseTransitions(
//    tz->StandardDate, standardPhase, prevOffset, transitions );
//  ICalTimeZoneSourcePrivate::parseTransitions(
//    tz->DaylightDate, daylightPhase, prevOffset, transitions );

  qSort( transitions );
  setTransitions( transitions );

  ICalTimeZoneData *idata = new ICalTimeZoneData( *this, KTimeZone( name ), QDate() );

  return ICalTimeZone( *this, name, idata );
}
#endif

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
