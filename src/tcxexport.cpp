#include "tcxexport.h"


TCXExport::TCXExport()
{
}

void TCXExport::save(QIODevice *dev, ActivityPtr activity)
{
    int cadenceTotal = 0;
    QDateTime cadenceStart;
    double totaldistance = 0.0;

    QXmlStreamWriter stream(dev);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    stream.writeStartElement("TrainingCenterDatabase");
    stream.writeAttribute("xsi:schemaLocation", "http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2 http://www.garmin.com/xmlschemas/TrainingCenterDatabasev2.xsd");
    stream.writeAttribute("xmlns", "http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2");
    stream.writeAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    stream.writeAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
    stream.writeAttribute("xmlns:ns2", "http://www.garmin.com/xmlschemas/ActivityExtension/v2");

    stream.writeStartElement("Activities");

    stream.writeStartElement("Activity");
    stream.writeAttribute("Sport", activity->sportString());

    stream.writeTextElement("Id", activity->date().toUTC().toString(Qt::ISODate));
    foreach( LapPtr lap, activity->laps() )
    {
        if ( lap->points().count() == 0 )
        {
            continue;
        }


        TrackPointPtr p = lap->points().first();

        stream.writeStartElement("Lap");
        stream.writeAttribute("StartTime", p->time().toUTC().toString(Qt::ISODate));

        stream.writeTextElement("TotalTimeSeconds", QString::number(lap->totalSeconds()) );
        stream.writeTextElement("DistanceMeters", QString::number( lap->length() ) );
        stream.writeTextElement("Calories", QString::number( lap->calories() ));
        stream.writeTextElement("Intensity", "Active");
        stream.writeTextElement("TriggerMethod", "Manual");

        stream.writeStartElement("Track");



        foreach (TrackPointPtr tp, lap->points())
        {
            if ( tp->latitude() == 0 && tp->longitude() == 0 )
            {
                continue;
            }

            stream.writeStartElement("Trackpoint");


            stream.writeTextElement("Time", tp->time().toUTC().toString(Qt::ISODate));
            stream.writeStartElement("Position");
            stream.writeTextElement("LatitudeDegrees", QString::number(tp->latitude()) );
            stream.writeTextElement("LongitudeDegrees", QString::number(tp->longitude()) );

            stream.writeTextElement("DistanceMeters", QString::number(tp->cummulativeDistance() - totaldistance));
            totaldistance = tp->cummulativeDistance();

            stream.writeEndElement(); // Position.

            if ( tp->heartRate() >= 0 )
            {
                stream.writeStartElement("HeartRateBpm");
                stream.writeTextElement("Value", QString::number(tp->heartRate()));
                stream.writeEndElement();
            }

            if ( activity->sport() == Activity::RUNNING )
            {
                if ( cadenceTotal == 0 )
                {
                    cadenceStart = tp->time();
                    if ( tp->cadence() > 0 )
                    {
                        cadenceTotal += tp->cadence();
                    }
                }
                else
                {
                    if ( tp->cadence() >0 )
                    {
                        cadenceTotal += tp->cadence();
                    }

                    int secs = cadenceStart.secsTo(tp->time());
                    if ( secs >= 60 )
                    {
                        stream.writeTextElement("Cadence", QString::number(  cadenceTotal * 60 / secs  ));
                        cadenceTotal = 0;
                    }
                }
            }
            if ( activity->sport() == Activity::BIKING )
            {
                stream.writeTextElement("Cadence", QString::number(tp->cadence()));
            }


            stream.writeEndElement(); // TrackPoint;
        }

        stream.writeEndElement(); // track.

        stream.writeEndElement(); // lap.
    }


    stream.writeEndElement(); // activity
    stream.writeEndElement(); // activities
    stream.writeEndElement(); // TrainingCenterDatabase

    stream.writeEndDocument();
}