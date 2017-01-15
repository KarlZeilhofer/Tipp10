/*
Copyright (c) 2006-2009, Tom Thielicke IT Solutions

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.
*/

/****************************************************************
**
** Implementation of the KeyBoard class
** File name: progressionwidget.cpp
**
****************************************************************/

#include <QPainter>
#include <QSettings>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QVariant>
#include <QSqlDatabase>
#include <QLineF>
#include <QPen>
#include <QDateTime>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRectF>
#include <QCoreApplication>

#include "progressionwidget.h"

ProgressionWidget::ProgressionWidget(QWidget *parent) : QWidget(parent) {
	// Get language
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
		"/portable/settings.ini", QSettings::IniFormat);
	#else
	QSettings settings;
	#endif
	settings.beginGroup("general");
    language = settings.value("language_gui", APP_STD_LANGUAGE_GUI).toString();
	settings.endGroup();

    // Fix the size of this class because of using fix sized images
    //setFixedSize(600, 310);
	whereClausel = "";
	orderClausel = "ORDER BY user_lesson_timestamp";


	xAxis = tr("Zeitpunkt");
	xAxisColumn = 1;

    lessonSelected = -1;

	// Create filter headline
	labelFilter = new QLabel(tr("Zeige: "));
	comboFilter = new QComboBox();
	comboFilter->insertItem(0, tr("Alle Lektionen"));
	comboFilter->insertItem(1, tr("Uebungslektionen"));
	comboFilter->insertItem(2, tr("Freie Lektionen"));
	comboFilter->insertItem(3, tr("Eigene Lektionen"));
	labelOrder = new QLabel(tr("X-Achse sortieren nach: "));
	comboOrder = new QComboBox();
	comboOrder->insertItem(0, tr("Zeitpunkt"));
	comboOrder->insertItem(1, tr("Lektion"));
	//comboOrder->insertItem(2, tr("A/min"));
	//comboOrder->insertItem(3, tr("Bewertung"));

	// Set a horizonal layout
	QHBoxLayout *filterLayout = new QHBoxLayout;
	filterLayout->addStretch(1);
	filterLayout->addWidget(labelOrder);
	filterLayout->addWidget(comboOrder);
	filterLayout->addSpacing(20);
	filterLayout->addWidget(labelFilter);
	filterLayout->addWidget(comboFilter);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(filterLayout);
	mainLayout->addStretch(1);
	// Pass layout to parent widget (this)
	this->setLayout(mainLayout);

	connect(comboFilter, SIGNAL(activated(int)), this, SLOT(changeFilter(int)));
	connect(comboOrder, SIGNAL(activated(int)), this, SLOT(changeOrder(int)));
	getChartValues();
    setMouseTracking(true);
}

void ProgressionWidget::getChartValues() {
	QSqlQuery query;
    QString lessonNumber;
    QDateTime timeStamp;
    QString timeStampShort;
    QString timeStampLong;
    double gradeTemp;
    QString gradeTempString;
    double cpmTemp;
    QString cpmTempString;
	lessonCounter = 0;
	lessonAv = 0;
	lessonsNumbers.clear();
    lessonsAxis.clear();
    lessonsGrades.clear();
    lessonsCpms.clear();
	lessonsType.clear();
	// SQL: all lessons sorted by id and a left joint to the number of
	// lessons done by the user
	if (!query.exec("SELECT user_lesson_lesson, user_lesson_timestamp, "
            "(((user_lesson_strokesnum - (20.0 * user_lesson_errornum)) / "
            "(user_lesson_timelen / 60.0)) * 0.4) AS user_lesson_grade, "
            "user_lesson_id, (user_lesson_strokesnum / "
            "(user_lesson_timelen / 60.0)), user_lesson_type, "
			"user_lesson_name AS user_lesson_cpm FROM user_lesson_list " +
			whereClausel + orderClausel + ";")) {
		return;
	}
	lessonGradeMax = 0;
	// Read all datasets to list items
	while (query.next()) {
		// Number of the lesson
        lessonsNumbers.append(QString::number(query.value(0).toInt() % 100));
		// Timestamp of the lesson
        timeStamp = QDateTime::fromString(query.value(1).toString(), "yyyyMMddhhmmss");
        timeStampShort =
            QDateTime::fromString(query.value(xAxisColumn).toString(),
            "yyyyMMddhhmmss").toString((language == "de" ? "dd.MM.yyyy" : "MMM d, yyyy"));
        timeStampLong = timeStamp.toString((language == "de" ? "dd.MM.yyyy hh:mm" : "MMM d, yyyy hh:mm ap")) +
                (language == "de" ? tr(" Uhr") : "");

        lessonsTimestamps.append(timeStampLong);

        // X-Axis of the chart
		if (xAxisColumn == 1) {
            lessonsAxis.append(timeStampShort);
		} else {
			if (xAxisColumn == 2 && query.value(2).toInt() < 0) {
                lessonsAxis.append("0");
			} else {
                lessonsAxis.append(query.value(xAxisColumn).toString());
			}
		}
		// Type of lesson
		lessonsType.append(query.value(5).toInt());
        // Name of lesson
        lessonsNames.append(query.value(6).toString());
		// Grade of the lesson
        gradeTemp = (query.value(2).toDouble() > 0 ? query.value(2).toDouble() : 0);
        gradeTempString.sprintf("%.0f", gradeTemp);
        lessonsGrades.append(gradeTempString.toInt());
        // CPM of the lesson
        cpmTemp = query.value(4).toDouble();
        cpmTempString.sprintf("%.0f", cpmTemp);
        lessonsCpms.append(cpmTempString.toInt());

		// Maximum
		if (query.value(2).toInt() > lessonGradeMax) {
			lessonGradeMax = query.value(2).toInt();
		}

		// Average
		if (query.value(2).toInt() > 0) {
			lessonAv += query.value(2).toInt() ;
        }
        lessonsX.append(0.0);
        lessonsY.append(0.0);
		lessonCounter++;
	}
	if (lessonGradeMax > 120) {
		lessonGradeMax += 10;
	} else {
		lessonGradeMax = 120;
	}
	if (lessonCounter != 0) {
		lessonAv = lessonAv / lessonCounter;
	}
	update();
}

void ProgressionWidget::paintEvent(QPaintEvent *event) {
	if (lessonCounter > 1) {
		drawGrid();
		drawGraph();
	} else {
		drawNothing();
    }
}

void ProgressionWidget::mouseMoveEvent(QMouseEvent *event) {
    lessonSelected = -1;

    for (int x = 0; x < lessonCounter; x++) {
        if ((double) event->pos().x() > lessonsX[x] - 6.0 &&
            (double) event->pos().x() < lessonsX[x] + 6.0 &&
            (double) event->pos().y() > lessonsY[x] - 6.0 &&
            (double) event->pos().y() < lessonsY[x] + 6.0) {

            lessonSelected = x;
        }

    }

    repaint();

}

void ProgressionWidget::drawTooltip(QPainter *painter, double x, double y, QString message)
{

    double yOffset = -66.0;
    if (y < ((double) this->height() * (2.0/5.0))) {
        yOffset = 6.0;
    }

    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setFont(QFont(FONT_STANDARD, FONT_SIZE_PROGRESS));
    painter->setPen(QColor(0, 0, 0));

    painter->setBrush(QColor(255, 255, 255));
    painter->drawRect(QRectF(x - 60.0,
        y  + yOffset,
        138.0,
        60.0));

    painter->drawText(QRectF(x - 60.0,
        y + yOffset,
        138.0,
        60.0),
        Qt::AlignCenter | Qt::AlignTop,
        message);
}

void ProgressionWidget::drawGrid() {
	QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, false);
    double widgetLeft = 20.0;
    double widgetTop = 40.0;
    double widgetWidth = (double) this->width() - 20.0;
    double widgetHeight = (double) this->height();
	int lessonCounterTemp;
	if (lessonCounter > 1) {
		lessonCounterTemp = lessonCounter;
	} else {
		lessonCounterTemp = 2;
	}
    double xUnit = (widgetWidth - 120.0) / ((double) (lessonCounterTemp - 1));
    double yUnit = (widgetHeight - 110.0) / (double) lessonGradeMax; //120;
	QPen penDashLine;

	painter.setFont(QFont(FONT_STANDARD, FONT_SIZE_PROGRESS));
	painter.setPen(QColor(0, 0, 0));

	// Text y axis
    painter.drawText(QRectF(widgetLeft,
        (widgetTop + widgetHeight - 113.0 - ((double) lessonGradeMax * yUnit)),
        50.0,
        10.0),
		Qt::AlignCenter | Qt::AlignVCenter,
		tr("Punkte"));
	// Text x axis
    painter.drawText(QRectF((widgetLeft + 60.0 + ((double) (lessonCounterTemp - 1) * xUnit)),
        (widgetTop + widgetHeight - 91.0),
        60.0,
        20.0),
		Qt::AlignLeft | Qt::AlignVCenter,
		xAxis);
	// y axis
    painter.drawLine(QLineF(widgetLeft + 30.0,
        widgetTop + widgetHeight - 95.0 - (lessonGradeMax * yUnit),
        widgetLeft + 30.0,
        widgetTop + widgetHeight - 80.0));
	// y axis arrow
    painter.drawLine(QLineF(widgetLeft + 28.0,
        widgetTop + widgetHeight - 93.0 - (lessonGradeMax * yUnit),
        widgetLeft + 30.0,
        widgetTop + widgetHeight - 95.0 - (lessonGradeMax * yUnit)));
    painter.drawLine(QLineF(widgetLeft + 32.0,
        widgetTop + widgetHeight - 93.0 - (lessonGradeMax * yUnit),
        widgetLeft + 30.0,
        widgetTop + widgetHeight - 95.0 - (lessonGradeMax * yUnit)));
	// x axis
    painter.drawLine(QLineF(widgetLeft + 28.0,
        widgetTop + widgetHeight - 80.0,
        widgetLeft + 50.0 + ((double)(lessonCounterTemp - 1) * xUnit),
        widgetTop + widgetHeight - 80.0));
	// x axis arrow
    painter.drawLine(QLineF(widgetLeft+ 48.0 + ((double) (lessonCounterTemp - 1) * xUnit),
        widgetTop + widgetHeight - 82.0,
        widgetLeft + 50.0 + ((double) (lessonCounterTemp - 1) * xUnit),
        widgetTop + widgetHeight - 80.0));
    painter.drawLine(QLineF(widgetLeft + 48.0 + ((double) (lessonCounterTemp - 1) * xUnit),
        widgetTop + widgetHeight - 78.0,
        widgetLeft + 50.0 + ((double) (lessonCounterTemp - 1) * xUnit),
        widgetTop + widgetHeight - 80.0));
	// Bottom unit line y axis
    painter.drawText(QRectF(widgetLeft + 4.0,
        widgetTop + widgetHeight - 85.0,
        20.0,
        10.0),
		Qt::AlignCenter | Qt::AlignVCenter,
		"0");
	// Unit lines and text y axis
	for (int x = 1; x <= (lessonGradeMax / 10); x++) {
        painter.drawLine(QLineF(widgetLeft + 28.0,
            widgetTop + widgetHeight - 80.0 - ((double) x * yUnit * 10.0),
            widgetLeft + 50.0 + ((double) (lessonCounterTemp - 1) * xUnit),
            widgetTop + widgetHeight - 80.0 - ((double) x * yUnit * 10.0)));
        painter.drawText(QRectF(widgetLeft + 4.0,
            widgetTop + widgetHeight - 85.0 - ((double) x * yUnit * 10.0),
            20.0,
            10.0),
			Qt::AlignCenter | Qt::AlignVCenter,
			QString::number(x * 10));
	}
	// Text x axis
	if (lessonCounter > 0) {
        painter.drawText(QRectF(widgetLeft - 20.0,
            widgetTop + widgetHeight - 74.0,
            120.0,
            30.0),
			Qt::AlignCenter | Qt::AlignVCenter | Qt::TextWordWrap,
            lessonsAxis[0]);
	}
	if (lessonCounter > 1) {
        painter.drawText(QRectF(widgetLeft - 20.0 + ((double) (lessonCounter - 1) * xUnit),
            widgetTop + widgetHeight - 74.0,
            120.0,
            30.0),
			Qt::AlignCenter | Qt::AlignVCenter | Qt::TextWordWrap,
            lessonsAxis[lessonCounter - 1]);
	}
	// Average
    penDashLine.setBrush(QColor(249, 126, 50));
    penDashLine.setStyle(Qt::DashLine);
    painter.setPen(penDashLine);
    painter.drawLine(QLineF(widgetLeft + 28.0,
        widgetTop + widgetHeight - 80.0 - ((double) lessonAv * yUnit),
        widgetLeft + 50.0 + ((double) (lessonCounterTemp - 1) * xUnit),
        widgetTop + widgetHeight - 80.0 - ((double) lessonAv * yUnit)));
	penDashLine.setStyle(Qt::SolidLine);
    painter.setPen(penDashLine);
    painter.drawEllipse(QRectF(widgetLeft - 10.0,
        widgetTop + (widgetHeight - 80.0) - ((double) lessonAv * yUnit) - 3.0,
        6.0,
        6.0));
    painter.drawLine(QLineF(widgetLeft - 5.0,
        widgetTop + widgetHeight - 80.0 - ((double) lessonAv * yUnit) - 5.0,
        widgetLeft - 10.0,
        widgetTop + widgetHeight - 80.0 - ((double) lessonAv * yUnit) + 5.0));


}

void ProgressionWidget::drawGraph() {
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	QPen penOrange;
	QPen penBlack;
    QPen penBlue;
    QPen penGreen;
    QPen penWhite;
	QLineF line;
    double currentValue;
    double nextValue;
    double widgetLeft = 20.0;
    double widgetTop = 40.0;
    double widgetWidth = this->width() - 20.0;
    double widgetHeight = this->height();
	int lessonCounterTemp;
	if (lessonCounter > 1) {
		lessonCounterTemp = lessonCounter;
	} else {
		lessonCounterTemp = 2;
	}
    double xUnit = (widgetWidth - 120.0) / ((double) (lessonCounterTemp - 1));
    double yUnit = (widgetHeight - 110.0) / (double) lessonGradeMax;

	painter.setFont(QFont(FONT_STANDARD, FONT_SIZE_PROGRESS));

    penOrange.setStyle(Qt::SolidLine);
    penOrange.setBrush(QColor(249, 126, 50));
    penOrange.setCapStyle(Qt::RoundCap);
    penOrange.setJoinStyle(Qt::RoundJoin);
	penOrange.setWidth(2);

	penBlack.setStyle(Qt::SolidLine);
    penBlack.setBrush(QColor(0, 0, 0));
    penBlack.setCapStyle(Qt::RoundCap);
    penBlack.setJoinStyle(Qt::RoundJoin);
	penBlack.setWidth(2);

    penBlue.setStyle(Qt::SolidLine);
    penBlue.setBrush(QColor(60, 60, 180));
    penBlue.setCapStyle(Qt::RoundCap);
    penBlue.setJoinStyle(Qt::RoundJoin);
    penBlue.setWidth(2);

    penGreen.setStyle(Qt::SolidLine);
    penGreen.setBrush(QColor(60, 180, 60));
    penGreen.setCapStyle(Qt::RoundCap);
    penGreen.setJoinStyle(Qt::RoundJoin);
    penGreen.setWidth(2);

    penWhite.setStyle(Qt::SolidLine);
    penWhite.setBrush(QColor(255, 255, 255));
    penWhite.setCapStyle(Qt::RoundCap);
    penWhite.setJoinStyle(Qt::RoundJoin);
    penWhite.setWidth(2);

	// Legend
	if (whereClausel == "") {
		#if APP_MAC
		//Mac Version:
		//-----------
		painter.setPen(penBlack);
        painter.drawEllipse(QRectF(widgetLeft + 60.0,
            widgetTop - 20.0,
            4.0,
            4.0));
		painter.setFont(QFont(FONT_STANDARD, FONT_SIZE_PROGRESS));
        painter.drawText(QRectF(widgetLeft + 70.0,
            widgetTop - 24.0,
            90.0,
            12.0),
			Qt::AlignLeft | Qt::AlignVCenter,
			tr("Uebungslektion"));

        painter.setPen(penBlue);
        painter.drawEllipse(QRectF(widgetLeft + 60.0,
            widgetTop - 5.0,
            4.0,
            4.0));
		painter.setFont(QFont(FONT_STANDARD, FONT_SIZE_PROGRESS));
        painter.drawText(QRectF(widgetLeft + 70.0,
            widgetTop - 9.0,
            90.0,
            12.0),
			Qt::AlignLeft | Qt::AlignVCenter,
			tr("Freie Lektion"));

        painter.setPen(penGreen);
        painter.drawEllipse (QRectF(widgetLeft + 60.0,
            widgetTop + 10.0,
            4.0,
            4.0));
		painter.setFont(QFont(FONT_STANDARD, FONT_SIZE_PROGRESS));
        painter.drawText(QRectF(widgetLeft + 70.0,
            widgetTop + 6.0,
            90.0,
            12.0),
			Qt::AlignLeft | Qt::AlignVCenter,
			tr("Eigene Lektion"));
		#else
		//Win Version:
		//-----------
		painter.setPen(penBlack);
        painter.drawEllipse(QRectF(widgetLeft + 70.0,
            widgetTop - 20.0,
            4.0,
            4.0));
		painter.setFont(QFont(FONT_STANDARD, FONT_SIZE_PROGRESS));
        painter.drawText(QRectF(widgetLeft + 80.0,
            widgetTop - 24.0,
            90.0,
            12.0),
			Qt::AlignLeft | Qt::AlignVCenter,
			tr("Uebungslektion"));

        painter.setPen(penBlue);
        painter.drawEllipse(QRectF(widgetLeft + 70.0,
            widgetTop - 5.0,
            4.0,
            4.0));
		painter.setFont(QFont(FONT_STANDARD, FONT_SIZE_PROGRESS));
        painter.drawText(QRectF(widgetLeft + 80.0,
            widgetTop - 9.0,
            90.0,
            12.0),
			Qt::AlignLeft | Qt::AlignVCenter,
			tr("Freie Lektion"));

        painter.setPen(penGreen);
        painter.drawEllipse(QRectF(widgetLeft + 70.0,
            widgetTop + 10.0,
            4.0,
            4.0));
		painter.setFont(QFont(FONT_STANDARD, FONT_SIZE_PROGRESS));
        painter.drawText(QRectF(widgetLeft + 80.0,
            widgetTop + 6.0,
            90.0,
            12.0),
			Qt::AlignLeft | Qt::AlignVCenter,
			tr("Eigene Lektion"));
		#endif
	}

	// Number of lines
	for (int x = 0; x < lessonCounter; x++) {
		painter.setPen(penOrange);
        currentValue = (double) lessonsGrades[x];
		if (currentValue < 0) {
            currentValue = 0.0;
		}
		if (x < lessonCounter - 1) {
            nextValue = (double) lessonsGrades[x + 1];
			if (nextValue < 0) {
                nextValue = 0.0;
			}
            painter.drawLine(QLineF(widgetLeft + 40.0 + ((double) x * xUnit),
				widgetTop + (widgetHeight - 80) - (currentValue * yUnit),
                widgetLeft + 40.0 + ((double) (x + 1) * xUnit),
                widgetTop + (widgetHeight - 80.0) - (nextValue * yUnit)));
		}
		switch (lessonsType[x]) {
			case 0:
			default:
				painter.setPen(penBlack);
				break;
			case 1:
                painter.setPen(penBlue);
				break;
			case 2:
                painter.setPen(penGreen);
				break;
		}
        if (lessonSelected == x) {
            painter.setPen(penWhite);
        }
        lessonsX[x] = widgetLeft + 40.0 + ((double) x * xUnit);
        lessonsY[x] = widgetTop + (widgetHeight - 80.0) - (currentValue * yUnit);
        painter.drawEllipse(QRectF(lessonsX[x] - 2.0,
            lessonsY[x] - 2.0,
            4.0,
            4.0));
		//painter.setPen(QColor(255, 255, 255));
    	//painter.drawLine(60 + (x * xUnit), widgetHeight - 80,
		//	60 + (x * xUnit), (widgetHeight - 80) - (currentValue * yUnit));
        if (lessonsType[x] == 0) {
			painter.setFont(QFont(FONT_STANDARD, FONT_SIZE_PROGRESS_LESSON));
            painter.drawText(QRectF(widgetLeft + 30.0 + ((double) x * xUnit),
                widgetTop + (widgetHeight - 96.0) - (currentValue * yUnit),
                20.0,
                10.0),
				Qt::AlignCenter | Qt::AlignVCenter,
				lessonsNumbers[x]);
        }
	}

    if (lessonSelected != -1) {

        drawTooltip(&painter,
            lessonsX[lessonSelected],
            lessonsY[lessonSelected],
            lessonsNames[lessonSelected] + "\n" +
            QString::number(lessonsGrades[lessonSelected]) +
            (lessonsGrades[lessonSelected] ==  1 ? tr(" Punkt") : tr(" Punkte")) +
            " / " + QString::number(lessonsCpms[lessonSelected]) + tr(" A/min") + "\n" +
            lessonsTimestamps[lessonSelected]);

    }
}

void ProgressionWidget::drawNothing() {
	QPainter painter(this);
	//painter.setRenderHint(QPainter::Antialiasing);
	int widgetWidth = this->width();
	int widgetHeight = this->height();

	painter.setFont(QFont(FONT_STANDARD, FONT_SIZE_PROGRESS));
	painter.setPen(QColor(0, 0, 0));
	// Text y axis
    painter.drawText(QRectF(0.0,
        0.0,
		widgetWidth,
        widgetHeight),
		Qt::AlignCenter | Qt::AlignVCenter,
		tr("Der Verlauf wird erst nach der zweiten absolvierten Lektion sichtbar."));
}

void ProgressionWidget::changeFilter(int rowindex) {
	// Select columnname from columnindex
	switch (rowindex) {
		case 0:
			whereClausel = "";
			break;
		case 1:
		default:
			whereClausel = "WHERE user_lesson_type = 0 ";
			break;
		case 2:
			whereClausel = "WHERE user_lesson_type = 1 ";
			break;
		case 3:
			whereClausel = "WHERE user_lesson_type = 2 ";
			break;
	}
	// Call new query
	getChartValues();
}

void ProgressionWidget::changeOrder(int rowindex) {
	// Select columnname from columnindex
	switch (rowindex) {
		case 0:
			orderClausel = "ORDER BY user_lesson_timestamp";
			xAxis = tr("Zeitpunkt");
			xAxisColumn = 1;
			break;
		case 1:
		default:
			orderClausel = "ORDER BY user_lesson_type, user_lesson_lesson";
			xAxis = tr("Lektion");
			xAxisColumn = 6;
			break;
		case 2:
			orderClausel = "ORDER BY user_lesson_cpm";
			xAxis = tr("A/min");
			xAxisColumn = 4;
			break;
		case 3:
			orderClausel = "ORDER BY user_lesson_grade";
			xAxis = tr("Bewertung");
			xAxisColumn = 2;
			break;
	}
	// Call new query
	getChartValues();
}
