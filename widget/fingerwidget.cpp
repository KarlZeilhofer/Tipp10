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
** Implementation of the FingerWidget class
** File name: fingerwidget.cpp
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
#include <QRectF>
#include <QCoreApplication>

#include "fingerwidget.h"

FingerWidget::FingerWidget(QWidget *parent) : QWidget(parent) {
	// Get language
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
		"/portable/settings.ini", QSettings::IniFormat);
	#else
	QSettings settings;
	#endif
    settings.beginGroup("general");
    languageGui = settings.value("language_gui", APP_STD_LANGUAGE_GUI).toString();
    languageLayout = settings.value("language_layout", APP_STD_LANGUAGE_LAYOUT).toString();
    settings.endGroup();

	getChartValues();

    fingerSelected = -1;
    setMouseTracking(true);
}

void FingerWidget::getChartValues() {
    QSqlQuery query;
    double ratePrevious;
    int currentRank;
    double rateTemp;
    QString rateTempString;
    fingerRates.clear();
    fingerOccurs.clear();
    fingerErrors.clear();
    fingerRanks.clear();
    fingerX1.clear();
    fingerY1.clear();
    fingerX2.clear();
    fingerY2.clear();
    for (int i = 0; i < 9; i++) {
        fingerRates.append(0);
        fingerOccurs.append(0);
        fingerErrors.append(0);
        fingerRanks.append(0);
        fingerX1.append(0);
        fingerY1.append(0);
        fingerX2.append(0);
        fingerY2.append(0);
    }
	// SQL: all lessons sorted by id and a left joint to the number of
	// lessons done by the user
    if (!query.exec("SELECT keyboard_grids.finger,  "
                    "SUM(user_chars.user_char_target_errornum), "
                    "SUM(user_chars.user_char_occur_num), "
                    "((SUM(user_chars.user_char_target_errornum) * 1.0) / "
                        "(SUM(user_chars.user_char_occur_num) * 1.0)) * 100.0 "
                    "FROM user_chars "
                    "LEFT JOIN keyboard_layouts ON "
                        "keyboard_layouts.unicode = user_chars.user_char_unicode AND "
                        "keyboard_layouts.layout = '" + languageLayout + "' "
                    "LEFT JOIN keyboard_grids ON "
                        "keyboard_grids.key =  keyboard_layouts.grid AND "
                        "keyboard_grids.layout_country = '" + languageLayout.left(2) + "' "
                    "WHERE user_char_occur_num > 0 "
                    "GROUP BY keyboard_grids.finger "
                    "ORDER BY "
                    "((SUM(user_chars.user_char_target_errornum) * 1.0) / "
                        "(SUM(user_chars.user_char_occur_num) * 1.0)) * 100.0 DESC "
                    ";")) {
        return;
	}
    // Read all datasets to list items
    ratePrevious = -1;
    currentRank = 9; // Real: 0-8
    while (query.next()) {

        if (query.value(0).toInt() < 0 || query.value(0).toInt() > 8) {
            break;
        }
        if (ratePrevious != query.value(3).toDouble()) {
            currentRank--;
        }
        if (query.value(3).toDouble() == 0) {
            fingerRanks[query.value(0).toInt()] = 0;
        } else {
            fingerRanks[query.value(0).toInt()] = currentRank;
        }
        ratePrevious = query.value(3).toDouble();

        fingerOccurs[query.value(0).toInt()] = (query.value(2).toInt());

        fingerErrors[query.value(0).toInt()] = (query.value(1).toInt());


        rateTemp = query.value(3).toDouble();
        rateTempString.sprintf("%.0f", rateTemp);

        fingerRates[query.value(0).toInt()] = (rateTempString.toInt());

    }

    fingerNames << QObject::tr("Kleiner Finger links")
            << QObject::tr("Ringfinger links")
            << QObject::tr("Mittelfinger links")
            << QObject::tr("Zeigefinger links")
            << QObject::tr("Zeigefinger rechts")
            << QObject::tr("Mittelfinger rechts")
            << QObject::tr("Ringfinger rechts")
            << QObject::tr("Kleiner Finger rechts")
            << QObject::tr("Daumen");

	update();
}

void FingerWidget::paintEvent(QPaintEvent *event)
{
    double currentPos;
    double additionalWidth;
    for (int i = 0; i < 9; i++) {
        if (i == 8) {
            currentPos = 4.0;
            additionalWidth = (((double) this->width() - 40.0) / 10.0);
        } else {
            if (i > 3) {
                currentPos = (double) i + 2.0;
            } else {
                currentPos = (double) i;
            }
            additionalWidth = 0.0;
        }
        fingerX1[i] = 20.0 + (currentPos * (((double) this->width() - 40.0) / 10.0));
        fingerX2[i] = 20.0 + (currentPos * (((double) this->width() - 40.0) / 10.0)) +
                                           (((double) this->width() - 40.0) / 10.0) +
                                           additionalWidth - 4.0;
        fingerY1[i] = ((double) this->height() / 2.0) - 96.0;
        fingerY2[i] = (double) this->height() - 50.0;
    }
    drawHeadline();
    drawFingers();
}

void FingerWidget::mouseMoveEvent(QMouseEvent *event) {
    fingerSelected = -1;

    for (int x = 0; x < 9; x++) {
        if ((double) event->pos().x() > fingerX1[x] &&
            (double) event->pos().x() < fingerX2[x] &&
            (double) event->pos().y() > fingerY1[x] &&
            (double) event->pos().y() < fingerY2[x]) {

            fingerSelected = x;
        }

    }

    repaint();

}

void FingerWidget::drawFinger(int currentFinger, double currentPos, double widgetLeft,
                              double widgetTop, double currentTop, double currentWidth,
                              double widgetHeight, double xUnit)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setFont(QFont(FONT_STANDARD, FONT_SIZE_FINGER));

    QPen pen((fingerSelected == currentFinger ? QColor(255, 255, 255) : QColor(249, 126, 50)),
             5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    painter.setPen(pen);

    int startAngle = 30 * 16;
    int spanAngle = 120 * 16;

    painter.drawArc(QRectF(widgetLeft + (xUnit * currentPos) + 10.0,
                           currentTop,
                           currentWidth, 2.0 * currentWidth),
                    startAngle, spanAngle);

    painter.drawLine(QLineF(widgetLeft + (xUnit * currentPos) + 10.0 + (currentWidth / 15.0),
        currentTop + (currentWidth / 2.0),
        widgetLeft + (xUnit * currentPos) + 10.0 + (currentWidth / 15.0),
        widgetTop + widgetHeight - 80.0));

    painter.drawLine(QLineF(widgetLeft + (xUnit * currentPos) + currentWidth + 10.0 - (currentWidth / 15.0),
        currentTop + (currentWidth / 2.0),
        widgetLeft + (xUnit * currentPos) + currentWidth + 10.0 - (currentWidth / 15.0),
        widgetTop + widgetHeight - 80.0));

    painter.setPen(QColor(fingerRanks.at(currentFinger) * 27, fingerRanks.at(currentFinger) * 14,
                          fingerRanks.at(currentFinger) * 5));

    painter.drawText(QRectF(widgetLeft + (xUnit * currentPos) + 10.0,
        currentTop - 26.0,
        currentWidth,
        20.0), Qt::AlignCenter | Qt::AlignBottom,
        QString::number(fingerRates.at(currentFinger)) + " %");

}

void FingerWidget::drawHeadline()
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(QColor(0, 0, 0));

    painter.setFont(QFont(FONT_STANDARD, FONT_SIZE_FINGER, 75));

    painter.drawText(QRectF(20.0,
        30.0,
        (double) this->width() - 40.0,
        20.0), Qt::AlignCenter | Qt::AlignBottom,
                     tr("Fehlerquoten der Finger"));

    painter.setFont(QFont(FONT_STANDARD, FONT_SIZE_PROGRESS));

    painter.drawText(QRectF(20.0,
        (double) this->height() - 34.0,
        (double) this->width() - 40.0,
        16.0), Qt::AlignCenter | Qt::AlignBottom,
                     tr("Die Fehlerquoten werden aus den "
                        "Schriftzeichen und dem aktuell gewaehlten "
                        "Tastaturlayout berechnet."));



    if (fingerSelected != -1) {

        drawTooltip(&painter,
            fingerX1[fingerSelected] + ((fingerX2[fingerSelected] - fingerX1[fingerSelected]) / 2.0) - 4.0,
            fingerY1[fingerSelected] - 74.0,
            fingerNames[fingerSelected] + "\n" +
            tr("Fehlerquote:") + " " + QString::number(fingerRates[fingerSelected]) + tr(" %") + "\n" +
            tr("Vorkommen:") + " " + QString::number(fingerOccurs[fingerSelected]) + "\n" +
            tr("Fehler:") + " " + QString::number(fingerErrors[fingerSelected]));

    }
}

void FingerWidget::drawTooltip(QPainter *painter, double x, double y, QString message)
{

    double yOffset = -4.0;
    double newX = x;
    if (x < 72) {
        newX = 72;
    }
    if (x > ((double) this->width() - 74.0)) {
        newX = (double) this->width() - 74.0;
    }

    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setFont(QFont(FONT_STANDARD, FONT_SIZE_PROGRESS));
    painter->setPen(QColor(0, 0, 0));

    painter->setBrush(QColor(255, 255, 255));
    painter->drawRect(QRectF(newX - 70.0,
        y  + yOffset,
        140.0,
        66.0));

    painter->drawText(QRectF(newX - 70.0,
        y + yOffset,
        140.0,
        66.0),
        Qt::AlignCenter | Qt::AlignVCenter,
        message);
}

void FingerWidget::drawFingers()
{
    int currentFinger;
    double currentPos;
    double currentTop;
    double currentWidth;
    double widgetLeft = 10.0;
    double widgetTop = 30.0;
    double widgetWidth = (double) this->width() - 40.0;
    double widgetHeight = (double) this->height();
    double xUnit = widgetWidth / 10.0;



    currentFinger = 0;
    currentPos = 0.0;
    currentTop = widgetTop + (widgetHeight / 2.0) - 20.0;
    currentWidth = xUnit - 2.0;
    drawFinger(currentFinger, currentPos, widgetLeft, widgetTop, currentTop, currentWidth,
               widgetHeight, xUnit);


    currentFinger = 1;
    currentPos = 1.0;
    currentTop = widgetTop + (widgetHeight / 2.0) - 90.0;
    currentWidth = xUnit - 2.0;
    drawFinger(currentFinger, currentPos, widgetLeft, widgetTop, currentTop, currentWidth,
               widgetHeight, xUnit);


    currentFinger = 2;
    currentPos = 2.0;
    currentTop = widgetTop + (widgetHeight / 2.0) - 110.0;
    currentWidth = xUnit - 2.0;
    drawFinger(currentFinger, currentPos, widgetLeft, widgetTop, currentTop, currentWidth,
               widgetHeight, xUnit);


    currentFinger = 3;
    currentPos = 3.0;
    currentTop = widgetTop + (widgetHeight / 2.0) - 70.0;
    currentWidth = xUnit - 2.0;
    drawFinger(currentFinger, currentPos, widgetLeft, widgetTop, currentTop, currentWidth,
               widgetHeight, xUnit);


    currentFinger = 8;
    currentPos = 4.0;
    currentTop = widgetTop + (widgetHeight / 2.0) + 70.0;
    currentWidth = xUnit - 2.0;
    drawFinger(currentFinger, currentPos, widgetLeft, widgetTop, currentTop, currentWidth,
               widgetHeight, xUnit);


    currentFinger = 8;
    currentPos = 5.0;
    currentTop = widgetTop + (widgetHeight / 2.0) + 70.0;
    currentWidth = xUnit - 2.0;
    drawFinger(currentFinger, currentPos, widgetLeft, widgetTop, currentTop, currentWidth,
               widgetHeight, xUnit);


    currentFinger = 4;
    currentPos = 6.0;
    currentTop = widgetTop + (widgetHeight / 2.0) - 70.0;
    currentWidth = xUnit - 2.0;
    drawFinger(currentFinger, currentPos, widgetLeft, widgetTop, currentTop, currentWidth,
               widgetHeight, xUnit);


    currentFinger = 5;
    currentPos = 7.0;
    currentTop = widgetTop + (widgetHeight / 2.0) - 110.0;
    currentWidth = xUnit - 2.0;
    drawFinger(currentFinger, currentPos, widgetLeft, widgetTop, currentTop, currentWidth,
               widgetHeight, xUnit);


    currentFinger = 6;
    currentPos = 8.0;
    currentTop = widgetTop + (widgetHeight / 2.0) - 90.0;
    currentWidth = xUnit - 2.0;
    drawFinger(currentFinger, currentPos, widgetLeft, widgetTop, currentTop, currentWidth,
               widgetHeight, xUnit);


    currentFinger = 7;
    currentPos = 9.0;
    currentTop = widgetTop + (widgetHeight / 2.0) - 20.0;
    currentWidth = xUnit - 2.0;
    drawFinger(currentFinger, currentPos, widgetLeft, widgetTop, currentTop, currentWidth,
               widgetHeight, xUnit);
}
