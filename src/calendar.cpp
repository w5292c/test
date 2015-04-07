#include "calendar.h"

#include <uuid.h>

#include <QtCore/qalgorithms.h>
#include <QDebug>
#include <icalformat.h>
#include <icaltimezones.h>
#include <extendedstorage.h>
#include <extendedcalendar.h>
#include <libical/ical.h>

using namespace mKCal;
using namespace KCalCore;

static MSTimeZone createTimezone();
static ICalTimeZone myparse(const MSTimeZone *tz);

class MyTimeZoneData : public ICalTimeZoneData
{
public:
  ICalTimeZone myparse(const MSTimeZone *tz);
};

void CalendarTest::test()
{
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

#if 0
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
