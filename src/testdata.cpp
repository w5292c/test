/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Alexander Chumakov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "testdata.h"

#include "utils.h"

#include <icalformat.h>
#include <QProcessEnvironment>

namespace {
QString testIcalData(const QString &from, const QString &to, bool response);
}

QString Data::testMime(const QString &from, const QString &to)
{
  return QString(
    "From: %1\r\n"
    "To: %2\r\n"
    "Subject: Test subject OA0.0002\r\n"
    "Content-Type: multipart/alternative;\r\n"
    "	boundary=\"_000_6386444f7f19453ba396a9ec3e14f76bHIMDWSMB02adharmancom_\"\r\n"
    "MIME-Version: 1.0\r\n"
    "\r\n"
    "--_000_6386444f7f19453ba396a9ec3e14f76bHIMDWSMB02adharmancom_\r\n"
    "Content-Type: text/plain; charset=\"iso-8859-1\"\r\n"
    "Content-Transfer-Encoding: quoted-printable\r\n"
    "\r\n"
    "Test body for OA0.0002\r\n"
    "\r\n"
    "\r\n"
    "--_000_6386444f7f19453ba396a9ec3e14f76bHIMDWSMB02adharmancom_\r\n"
    "Content-Type: text/html; charset=\"iso-8859-1\"\r\n"
    "Content-Transfer-Encoding: quoted-printable\r\n"
    "\r\n"
    "<html>\r\n"
    "<head>\r\n"
    "</head>\r\n"
    "<body>\r\n"
    "<div>Test body for OA0.0002</div>\r\n"
    "</body>\r\n"
    "</html>\r\n"
    "\r\n"
    "--_000_6386444f7f19453ba396a9ec3e14f76bHIMDWSMB02adharmancom_\r\n"
"Content-Type: text/calendar; charset=\"utf-8\"; method=REQUEST\r\n"
"Content-Transfer-Encoding: 7BIT\r\n"
"\r\n"
"BEGIN:VCALENDAR\r\n"
"METHOD:REQUEST\r\n"
"PRODID:Test ActiveSync client w5292c\r\n"
"VERSION:2.0\r\n"
"BEGIN:VTIMEZONE\r\n"
"TZID:Russian Standard Time\r\n"
"BEGIN:STANDARD\r\n"
"DTSTART:16010101T000000\r\n"
"TZOFFSETFROM:+0300\r\n"
"TZOFFSETTO:+0300\r\n"
"END:STANDARD\r\n"
"BEGIN:DAYLIGHT\r\n"
"DTSTART:16010101T000000\r\n"
"TZOFFSETFROM:+0300\r\n"
"TZOFFSETTO:+0300\r\n"
"END:DAYLIGHT\r\n"
"END:VTIMEZONE\r\n"
"BEGIN:VEVENT\r\n"
"ORGANIZER;CN=\"Chumakov, Alexander\":w5292c@outlook.com\r\n"
"ATTENDEE;ROLE=REQ-PARTICIPANT;PARTSTAT=NEEDS-ACTION;RSVP=TRUE:w5292c.ex2@gm\r\n"
" ail.com\r\n"
"DESCRIPTION;LANGUAGE=en-US:Test body for OA0.0001\\n\\n\r\n"
"SUMMARY;LANGUAGE=en-US:Test subject OA0.0001\r\n"
"DTSTART;TZID=Russian Standard Time:20180121T153000\r\n"
"DTEND;TZID=Russian Standard Time:20180121T160000\r\n"
"UID:040000008200E00074C5B7101A82E00800000000407986865D89D301000000000000000\r\n"
" 01000000022F019C41CE1724D8429E0D7E00CDBEF\r\n"
"CLASS:PUBLIC\r\n"
"PRIORITY:5\r\n"
"DTSTAMP:20180109T122131Z\r\n"
"TRANSP:OPAQUE\r\n"
"STATUS:CONFIRMED\r\n"
"SEQUENCE:0\r\n"
"LOCATION;LANGUAGE=en-US:Test location OA0.0001\r\n"
"X-MICROSOFT-CDO-APPT-SEQUENCE:0\r\n"
"X-MICROSOFT-CDO-OWNERAPPTID:992548834\r\n"
"X-MICROSOFT-CDO-BUSYSTATUS:TENTATIVE\r\n"
"X-MICROSOFT-CDO-INTENDEDSTATUS:BUSY\r\n"
"X-MICROSOFT-CDO-ALLDAYEVENT:FALSE\r\n"
"X-MICROSOFT-CDO-IMPORTANCE:1\r\n"
"X-MICROSOFT-CDO-INSTTYPE:0\r\n"
"X-MICROSOFT-DISALLOW-COUNTER:FALSE\r\n"
"BEGIN:VALARM\r\n"
"DESCRIPTION:REMINDER\r\n"
"TRIGGER;RELATED=START:-PT15M\r\n"
"ACTION:DISPLAY\r\n"
"END:VALARM\r\n"
"END:VEVENT\r\n"
"END:VCALENDAR\r\n"
"\r\n"
"--_000_6386444f7f19453ba396a9ec3e14f76bHIMDWSMB02adharmancom_--\r\n").arg(from).arg(to);
}

QString Data::testMime2(const QString &from, const QString &to)
{
  return QString(
    "From: %1\r\n"
    "To: %2\r\n"
    "Subject: Test subject OA0.0002 response\r\n"
    "Content-Type: multipart/alternative;\r\n"
    "   boundary=\"_000_6386444f7f19453ba396a9ec3e14f76bHIMDWSMB02adharmancom_\"\r\n"
    "MIME-Version: 1.0\r\n"
    "\r\n"
    "--_000_6386444f7f19453ba396a9ec3e14f76bHIMDWSMB02adharmancom_\r\n"
    "Content-Type: text/plain; charset=\"iso-8859-1\"\r\n"
    "Content-Transfer-Encoding: quoted-printable\r\n"
    "\r\n"
    "Test body for OA0.0002 response\r\n"
    "\r\n"
    "\r\n"
    "--_000_6386444f7f19453ba396a9ec3e14f76bHIMDWSMB02adharmancom_\r\n"
    "Content-Type: text/html; charset=\"iso-8859-1\"\r\n"
    "Content-Transfer-Encoding: quoted-printable\r\n"
    "\r\n"
    "<html>\r\n"
    "<head>\r\n"
    "</head>\r\n"
    "<body>\r\n"
    "<div>Test body for OA0.0002 response</div>\r\n"
    "</body>\r\n"
    "</html>\r\n"
    "\r\n"
    "--_000_6386444f7f19453ba396a9ec3e14f76bHIMDWSMB02adharmancom_\r\n"
"Content-Type: text/calendar; charset=\"utf-8\"; method=REPLY\r\n"
"Content-Transfer-Encoding: 7BIT\r\n"
"\r\n"
"BEGIN:VCALENDAR\r\n"
"METHOD:REPLY\r\n"
"PRODID:Test ActiveSync client w5292c\r\n"
"VERSION:2.0\r\n"
"BEGIN:VEVENT\r\n"
"ORGANIZER;CN=\"Chumakov, Alexander\":w5292c.ex2@gmail.com\r\n"
"ATTENDEE;ROLE=REQ-PARTICIPANT;PARTSTAT=ACCEPTED:w5292c@outlook.com\r\n"
"SUMMARY:Test subject OA0.0001\r\n"
"DTSTART:20180121T153000Z\r\n"
"DTEND:20180121T160000Z\r\n"
"UID:040000008200E00074C5B7101A82E00800000000407986865D89D301000000000000000\r\n"
" 01000000022F019C41CE1724D8429E0D7E00CDBF1\r\n"
"PRIORITY:5\r\n"
"DTSTAMP:20180109T122132Z\r\n"
"TRANSP:OPAQUE\r\n"
"STATUS:CONFIRMED\r\n"
"LOCATION:Test location OA0.0001\r\n"
"X-MICROSOFT-CDO-APPT-SEQUENCE:1\r\n"
"X-MICROSOFT-CDO-BUSYSTATUS:TENTATIVE\r\n"
"X-MICROSOFT-CDO-ALLDAYEVENT:FALSE\r\n"
"X-MICROSOFT-CDO-IMPORTANCE:1\r\n"
"X-MICROSOFT-CDO-INSTTYPE:0\r\n"
"END:VEVENT\r\n"
"END:VCALENDAR\r\n"
"\r\n"
"--_000_6386444f7f19453ba396a9ec3e14f76bHIMDWSMB02adharmancom_--\r\n").arg(from).arg(to);
}

QString Data::testMime3(const QString &from, const QString &to)
{
#if 0
  QString ical = QString(
    "BEGIN:VCALENDAR\r\n"
    "METHOD:REPLY\r\n"
    "PRODID:Test ActiveSync client w5292c\r\n"
    "VERSION:2.0\r\n"
    "BEGIN:VEVENT\r\n"
    "ORGANIZER;CN=\"Chumakov, Alexander\":w5292c.ex2@gmail.com\r\n"
    "ATTENDEE;ROLE=REQ-PARTICIPANT;PARTSTAT=ACCEPTED:w5292c@outlook.com\r\n"
    "SUMMARY:Test subject OA0.0001\r\n"
    "DTSTART:20180121T153000Z\r\n"
    "DTEND:20180121T160000Z\r\n"
    "UID:040000008200E00074C5B7101A82E00800000000407986865D89D301000000000000000\r\n"
    " 01000000022F019C41CE1724D%1\r\n"
    "PRIORITY:5\r\n"
    "DTSTAMP:20180109T122132Z\r\n"
    "TRANSP:OPAQUE\r\n"
    "STATUS:CONFIRMED\r\n"
    "LOCATION:Test location OA0.0001\r\n"
    "X-MICROSOFT-CDO-APPT-SEQUENCE:1\r\n"
    "X-MICROSOFT-CDO-BUSYSTATUS:TENTATIVE\r\n"
    "X-MICROSOFT-CDO-ALLDAYEVENT:FALSE\r\n"
    "X-MICROSOFT-CDO-IMPORTANCE:1\r\n"
    "X-MICROSOFT-CDO-INSTTYPE:0\r\n"
    "END:VEVENT\r\n"
    "END:VCALENDAR\r\n"
    ).arg(QString(Utils::hexRandom64()));
#endif /* 0 */

  const QString &ical = testIcalData(from, to, true);

  QString mime = QString(
    "From: %1\r\n"
    "To: %2\r\n"
    "Subject: Test subject OA0.0002 response\r\n"
    "Content-Type: multipart/alternative;\r\n"
    "   boundary=\"_000_6386444f7f19453ba396a9ec3e14f76bHIMDWSMB02adharmancom_\"\r\n"
    "MIME-Version: 1.0\r\n"
    "\r\n"
    "--_000_6386444f7f19453ba396a9ec3e14f76bHIMDWSMB02adharmancom_\r\n"
    "Content-Type: text/plain; charset=\"iso-8859-1\"\r\n"
    "Content-Transfer-Encoding: quoted-printable\r\n"
    "\r\n"
    "Test body for OA0.0002 response\r\n"
    "\r\n"
    "\r\n"
    "--_000_6386444f7f19453ba396a9ec3e14f76bHIMDWSMB02adharmancom_\r\n"
    "Content-Type: text/html; charset=\"iso-8859-1\"\r\n"
    "Content-Transfer-Encoding: quoted-printable\r\n"
    "\r\n"
    "<html>\r\n"
    "<head>\r\n"
    "</head>\r\n"
    "<body>\r\n"
    "<div>Test body for OA0.0002 response</div>\r\n"
    "</body>\r\n"
    "</html>\r\n"
    "\r\n"
    "--_000_6386444f7f19453ba396a9ec3e14f76bHIMDWSMB02adharmancom_\r\n"
    "Content-Type: text/calendar; charset=\"utf-8\"; method=REPLY\r\n"
    "Content-Transfer-Encoding: 7BIT\r\n"
    "\r\n").arg(from).arg(to);

  mime.append(ical);

  mime.append(
    "\r\n"
    "--_000_6386444f7f19453ba396a9ec3e14f76bHIMDWSMB02adharmancom_--\r\n"
  );

  return mime;
}

namespace {
QString testIcalData(const QString &from, const QString &to, bool response)
{
  const QString &defaultUid = QString("040000008200E00074C5B7101A82E00800000000407986865D89D30100000000000000001000000022F019C41CE1724D%1").arg(QString(Utils::hexRandom64()));
  const QProcessEnvironment &env = QProcessEnvironment::systemEnvironment();
  const QString &eventUid = env.value("MY_UID", defaultUid);

  KCalCore::Event::Ptr event(new KCalCore::Event);
  event->setSchedulingID(eventUid);
  event->setLocation("Test room AB10.12");
  event->setStatus(KCalCore::Incidence::StatusConfirmed);
  event->setDtStart(KDateTime::fromString("20180121T180000Z"));
  event->setDtEnd(KDateTime::fromString(  "20180121T190000Z"));
  event->setSummary("Test event N2.0001", false);
  event->setPriority(5);
  event->setTransparency(KCalCore::Event::Opaque);
  event->setRevision(0);
  event->setNonKDECustomProperty("X-MICROSOFT-CDO-APPT-SEQUENCE", "0");
  event->setNonKDECustomProperty("X-MICROSOFT-CDO-ALLDAYEVENT", "FALSE");

  KCalCore::Attendee::Ptr attendee(new KCalCore::Attendee(
    "Attendee",
    response ? from : to,
    true,
    KCalCore::Attendee::Accepted,
    KCalCore::Attendee::ReqParticipant));
  event->addAttendee(attendee);

  KCalCore::ICalFormat icf;
  const QString &ical = icf.createScheduleMessage(event, KCalCore::iTIPReply);
  qDebug() << "Here is the iCalendar data:" << endl
           << "********************************************************************************" << endl
           << (const char *)ical.toLatin1().data()
           << "\r********************************************************************************";

  return ical;
}
}

/*
# Correct response:
#ATTENDEE;PARTSTAT=ACCEPTED;CN="Chumakov, Alexander":MAILTO:Alexander.Chumakov
# @harman.com
#COMMENT;LANGUAGE=en-US:\n\n
#SUMMARY;LANGUAGE=en-US:Accepted: [EXTERNAL] Invitation: Test event N2.0001
# @ Sun 21 Jan 2018   18:00 - 19:00 (MSK) (alexander.chumakov@harman.com)
DTSTART;TZID=Russian Standard Time:20180121T180000
DTEND;TZID=Russian Standard Time:20180121T190000
UID:08fqr3afnmdmrssh1vff3sd316@google.com
CLASS:PUBLIC
PRIORITY:5
DTSTAMP:20180119T113855Z
TRANSP:OPAQUE
STATUS:CONFIRMED
SEQUENCE:0
LOCATION;LANGUAGE=en-US:Test room AB10.12
X-MICROSOFT-CDO-APPT-SEQUENCE:0
X-MICROSOFT-CDO-BUSYSTATUS:BUSY
X-MICROSOFT-CDO-INTENDEDSTATUS:BUSY
X-MICROSOFT-CDO-ALLDAYEVENT:FALSE
X-MICROSOFT-CDO-IMPORTANCE:1
X-MICROSOFT-CDO-INSTTYPE:0
X-MICROSOFT-DISALLOW-COUNTER:FALSE
*/
