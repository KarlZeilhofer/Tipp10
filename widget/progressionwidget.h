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
** Definition of the ProgressionWidget class
** File name: progressionwidget.h
**
****************************************************************/

#ifndef PROGRESSIONWIDGET_H
#define PROGRESSIONWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPaintEvent>
#include <QString>
#include <QList>
#include <QLabel>
#include <QComboBox>
#include <QGraphicsSceneMouseEvent>

#include "def/defines.h"

//! The ProgressionWidget class provides a progression chart.
/*!
	@author Tom Thielicke, s712715
	@version 0.1.6
	@date 16.06.2006
*/
class ProgressionWidget : public QWidget {
	// Necessary to create own signals, slots and connections
    Q_OBJECT

	public:
		ProgressionWidget(QWidget *parent = 0);

	signals:

	private slots:
		void changeFilter(int rowindex);
		void changeOrder(int rowindex);

	protected:

		//! Paintevent, draws current view of the chart.
		void paintEvent(QPaintEvent *event);
        void mouseMoveEvent(QMouseEvent *event);

	private:
		void getChartValues();
		void drawGrid();
		void drawGraph();
		void drawNothing();
        void drawTooltip(QPainter *painter, double x, double y, QString message);

		QList<QString> lessonsNumbers;
        QList<QString> lessonsTimestamps;
        QList<QString> lessonsNames;
        QList<QString> lessonsAxis;
        QList<int> lessonsGrades;
        QList<int> lessonsCpms;
        QList<int> lessonsType;
        QList<double> lessonsX;
        QList<double> lessonsY;
		QLabel *labelFilter;
		QComboBox *comboFilter;
		QLabel *labelOrder;
		QComboBox *comboOrder;
		int lessonCounter;
		int lessonGradeMax;
		int lessonAv;
		QString whereClausel;
		QString orderClausel;
		QString xAxis;
		int xAxisColumn;
		QString language;
        int lessonSelected;
};

#endif // PROGRESSIONWIDGET_H
