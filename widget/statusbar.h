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
** Definition of the StatusBar class
** File name: statusbar.h
**
****************************************************************/

#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QWidget>
#include <QString>
#include <QPaintEvent>
#include <QLabel>
#include <QPixmap>

class QLabel;

//! The StatusBar class provides a status bar with three text parts.
/*!
	@author Tom Thielicke, s712715
	@version 0.0.3
	@date 08.06.2006
*/
class StatusBar : public QWidget {
    Q_OBJECT

	public:

		//! Constructor, creates a status bar with three text parts.
		/*!
			In this contructor the following variables are initialized:
				- txtLeft = ""
				- txtCenter = ""
				- txtRight = ""

			In addition, the image "statusbg.png" is loaded to variable
			background.

			@param parent The parent QWidget
			@see txtLeft, txtCenter, txtRight, background
		*/
		StatusBar(QWidget *parent = 0);

	public slots:

		//! Slot, sets the all three text items of the status bar.
		/*!
			@param left Text on the left side
			@param center Text in the middle
			@param right Text on the right side
			@see txtLeft, txtCenter, txtRight
		*/
		void setText(QString leftleft, QString left, QString center, QString right, QString rightright);

		//! Slot, sets the first left text item of the status bar.
		/*!
			@param txt Text
			@see txtLeft
		*/
		void setLeftLeftText(QString txt);

		//! Slot, sets the second left text item of the status bar.
		/*!
			@param txt Text
			@see txtLeft
		*/
		void setLeftText(QString txt);

		//! Slot, sets the center text item of the status bar.
		/*!
			@param txt Text
			@see txtCenter
		*/
		void setCenterText(QString txt);

		//! Slot, sets the second right text item of the status bar.
		/*!
			@param txt Text
			@see txtRight
		*/
		void setRightText(QString txt);

		//! Slot, sets the first right text item of the status bar.
		/*!
			@param txt Text
			@see txtRight
		*/
		void setRightRightText(QString txt);

	protected:

		//! Paint event, drwas the backgroung image an the three text itmes.
		/*!
			@param event Paint event
			@see txtLeft, txtCenter, txtRight, background
		*/
		void paintEvent(QPaintEvent *event);

	private:

		//! First left text of the status bar.
		QString txtLeftLeft;

		//! Second left text of the status bar.
		QString txtLeft;

		//! Center text of the status bar.
		QString txtCenter;

		//! Second right text of the status bar.
		QString txtRight;

		//! First right text of the status bar.
		QString txtRightRight;

		//! Backgound image.
		QPixmap background;

};

#endif // STATUSBAR_H
