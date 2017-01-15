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
** Definition of the LessonPrintDialog class
** File name: lessonprintdialog.h
**
****************************************************************/

#ifndef LESSONPRINTDIALOG_H
#define LESSONPRINTDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QString>

//! The LessonPrintDialog class provides a user input widget.
/*!
	The LessonPrintDialog class shows a user name input for printing.

	@author Tom Thielicke, s712715
	@version 0.0.1
	@date 09.09.2008
*/
class LessonPrintDialog : public QDialog {
	Q_OBJECT

	public:

		//! Constructor, creates two table objects and provide it in two tabs.
		LessonPrintDialog(QString *enteredName, QWidget *parent = 0);

	public slots:

	private slots:

		//! Start button pressed
		void clickOk();

	private:

		//! Creates a cancel and a ok button.
		void createButtons();

		//! Creates a textbox.
		void createLineEdit();

		//! Creates the layout of the complete class.
		void createLayout();

		QPushButton *buttonOk;
		QPushButton *buttonCancel;
		QLabel *labelName;
		QLineEdit *lineName;
		QString *userName;
};

#endif //LESSONPRINTDIALOG_H
