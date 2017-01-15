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
** Definition of the FingerWidget class
** File name: fingerwidget.h
**
****************************************************************/

#ifndef FINGERWIDGET_H
#define FINGERWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QString>
#include <QStringList>
#include <QList>
#include <QGraphicsSceneMouseEvent>

#include "def/defines.h"

//! The FingerWidget class provides a finger error rate chart.
/*!
	@author Tom Thielicke, s712715
    @version 0.0.2
    @date 16.12.2010
*/
class FingerWidget : public QWidget {
	// Necessary to create own signals, slots and connections
    Q_OBJECT

	public:
        FingerWidget(QWidget *parent = 0);

	protected:

		//! Paintevent, draws current view of the chart.
		void paintEvent(QPaintEvent *event);
        void mouseMoveEvent(QMouseEvent *event);

	private:
		void getChartValues();
        void drawHeadline();
        void drawFingers();
        void drawFinger(int currentFinger, double currentPos, double widgetLeft,
                        double widgetTop, double currentTop, double currentWidth,
                        double widgetHeight, double xUnit);
        void drawTooltip(QPainter *painter, double x, double y, QString message);

        QList<int> fingerOccurs;
        QList<int> fingerErrors;
        QList<int> fingerRanks;
        QList<int> fingerRates;
        QList<double> fingerX1;
        QList<double> fingerX2;
        QList<double> fingerY1;
        QList<double> fingerY2;
        QString languageGui;
        QString languageLayout;
        int fingerSelected;
        QStringList fingerNames;
};

#endif // FINGERWIDGET_H
