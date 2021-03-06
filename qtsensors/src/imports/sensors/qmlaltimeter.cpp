/****************************************************************************
**
** Copyright (C) 2016 Research In Motion
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSensors module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmlaltimeter.h"
#include <QtSensors/QAltimeter>

/*!
    \qmltype Altimeter
    \instantiates QmlAltimeter
    \ingroup qml-sensors_type
    \inqmlmodule QtSensors
    \since QtSensors 5.1
    \inherits Sensor
    \brief The Altimeter element reports on altitude.

    The Altimeter element reports on altitude.

    This element wraps the QAltimeter class. Please see the documentation for
    QAltimeter for details.

    \sa AltimeterReading
*/

QmlAltimeter::QmlAltimeter(QObject *parent)
    : QmlSensor(parent)
    , m_sensor(new QAltimeter(this))
{
}

QmlAltimeter::~QmlAltimeter()
{
}

QmlSensorReading *QmlAltimeter::createReading() const
{
    return new QmlAltimeterReading(m_sensor);
}

QSensor *QmlAltimeter::sensor() const
{
    return m_sensor;
}

/*!
    \qmltype AltimeterReading
    \instantiates QmlAltimeterReading
    \ingroup qml-sensors_reading
    \inqmlmodule QtSensors
    \since QtSensors 5.1
    \inherits SensorReading
    \brief The AltimeterReading element holds the most recent Altimeter reading.

    The AltimeterReading element holds the most recent Altimeter reading.

    This element wraps the QAltimeterReading class. Please see the documentation for
    QAltimeterReading for details.

    This element cannot be directly created.
*/

QmlAltimeterReading::QmlAltimeterReading(QAltimeter *sensor)
    : QmlSensorReading(sensor)
    , m_sensor(sensor)
    , m_altitude(0)
{
}

QmlAltimeterReading::~QmlAltimeterReading()
{
}

/*!
    \qmlproperty qreal AltimeterReading::altitude
    This property holds the altitude of the device.

    Please see QAltimeterReading::altitude for information about this property.
*/

qreal QmlAltimeterReading::altitude() const
{
    return m_altitude;
}

QSensorReading *QmlAltimeterReading::reading() const
{
    return m_sensor->reading();
}

void QmlAltimeterReading::readingUpdate()
{
    qreal altitude = m_sensor->reading()->altitude();
    if (m_altitude != altitude) {
        m_altitude = altitude;
        Q_EMIT altitudeChanged();
    }
}
