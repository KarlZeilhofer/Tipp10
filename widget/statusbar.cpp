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
** Implementation of the StatusBar class
** File name: statusbar.cpp
**
****************************************************************/

#include <QPainter>
#include <QFont>
#include <QColor>

#include "statusbar.h"
#include "def/errordefines.h"
#include "errormessage.h"
#include "def/defines.h"

StatusBar::StatusBar(QWidget *parent) : QWidget(parent) {
	// Init values
	txtLeftLeft = "";
	txtLeft = "";
	txtCenter = "";
	txtRightRight = "";
	txtRight = "";

	// Load background image
    if (!background.load(":/img/statusbg.png")) {
		// Error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_STATUS_PIC, TYPE_WARNING,
			CANCEL_OPERATION);
	}

    setFixedSize(610, 30);

    setAttribute(Qt::WA_NoSystemBackground);
}

void StatusBar::setText(QString leftleft, QString left, QString center, QString right, QString rightright) {
	txtLeftLeft = leftleft;
	txtLeft = left;
	txtCenter = center;
	txtRight = right;
	txtRightRight = rightright;
	repaint();
}

void StatusBar::setLeftLeftText(QString txt) {
	txtLeftLeft = txt;
	repaint();
}

void StatusBar::setLeftText(QString txt) {
	txtLeft = txt;
	repaint();
}

void StatusBar::setCenterText(QString txt) {
	txtCenter = txt;
	repaint();
}

void StatusBar::setRightText(QString txt) {
	txtRight = txt;
	repaint();
}

void StatusBar::setRightRightText(QString txt) {
	txtRightRight = txt;
	repaint();
}

void StatusBar::paintEvent(QPaintEvent *pevent) {
	QPainter painter(this);
	// Draw backgorund image
	painter.drawPixmap(0, 0, background);
	painter.setFont(QFont(FONT_STANDARD, FONT_SIZE_STATUS));
	painter.setPen(QColor(40, 40, 40));
	painter.drawText(190, 7, 230, 14, Qt::AlignCenter | Qt::AlignVCenter,
		txtCenter);
	painter.setPen(QColor(120, 120, 120));
	painter.drawText(10, 7, 80, 14, Qt::AlignCenter | Qt::AlignVCenter,
		txtLeftLeft);
	painter.drawText(100, 7, 80, 14, Qt::AlignCenter | Qt::AlignVCenter,
		txtLeft);
	painter.drawText(430, 7, 80, 14, Qt::AlignCenter | Qt::AlignVCenter,
		txtRight);
	painter.drawText(520, 7, 80, 14, Qt::AlignCenter | Qt::AlignVCenter,
		txtRightRight);
}
