/*
 *  Manjaro Settings Manager
 *  Ramon Buldó <ramon@manjaro.org>
 *
 *  Copyright (C) 2007 Free Software Foundation, Inc.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TIMEDATEMODULE_H
#define TIMEDATEMODULE_H

#include "TimeDate.h"

#include <KAboutData>

#include <QtWidgets/QWidget>

namespace Ui
{
class TimeDateModule;
}

class TimeDateModule : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param parent Parent widget of the module
     */
    explicit TimeDateModule( QWidget* parent );
    /**
     * Destructor.
     */
    ~TimeDateModule();

    /**
     * Overloading the KCModule load() function.
     */
    void load();

    /**
     * Overloading the KCModule save() function.
     */
    void save();

    /**
     * Overloading the KCModule defaults() function.
     */
    void defaults();

    KAboutData* aboutData() const;

signals:
    void changed();

public slots:
    void updateFields();
    void updateTimeFields();
    void isNtpEnabledToggled();
    void timeZoneClicked();

private:
    Ui::TimeDateModule* ui;
    TimeDate* m_timeDate;
    KAboutData* m_aboutData;
    QTimer* m_timeFieldsTimer;

    bool m_isTimeEdited;
    bool m_isDateEdited;
    QString m_timeZone;
};

#endif // TIMEDATEMODULE_H
