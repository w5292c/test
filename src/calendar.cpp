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

namespace {
void initTestMsTimezone(MSTimeZone &msTimezone);
}

void CalendarTest::test()
{
  ICalTimeZones zones;

  MSTimeZone msTimezone;
  initTestMsTimezone(msTimezone);

  ICalTimeZoneSource source;
  const ICalTimeZone &zone1 = source.parse(&msTimezone, zones);
  const ICalTimeZone &zone2 = source.parse(&msTimezone, zones);

  qDebug() << "Zone 1:" << zone1.vtimezone();
  qDebug() << "Zone 2:" << zone2.vtimezone();
}

void CalendarTest::init()
{
  uuid_t out;
  uuid_generate_random(out);
}

namespace {

void initTestMsTimezone(MSTimeZone &msTimezone)
{
  msTimezone.Bias = -180;
  msTimezone.StandardName = "MSK";
  memset(&msTimezone.StandardDate, 0, sizeof (MSSystemTime));
  msTimezone.StandardBias = 0;
  msTimezone.DaylightName = "MSK";
  memset(&msTimezone.DaylightDate, 0, sizeof (MSSystemTime));
  msTimezone.DaylightBias = 0;
}

} // namespace {
