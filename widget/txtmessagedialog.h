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
** Definition of the TxtMessageDialog class
** File name: txtmessagedialog.h
**
****************************************************************/

#ifndef TXTMESSAGEDIALOG_H
#define TXTMESSAGEDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QString>
#include <QLabel>

#include "txtmessagedialog.h"

//! The TxtMessageDialog class provides a program settings widget.
/*!
	The TxtMessageDialog class shows three program settings. Setting the
	layout of the virtual keyboard, setting the ticker speed and font
	and doing a reset of the user tables.

	@author Tom Thielicke, s712715
	@version 0.0.3
	@date 18.06.2006
*/
class TxtMessageDialog : public QDialog {
	Q_OBJECT

	public:

		//! Constructor, creates two table objects and provide it in two tabs.
		/*!
			In this contructor three groups are created over functions
			createGroupKeyboardLayout(), createGroupUserReset() and
			createGroupTickerFont(). In addition, standard settings are read,
			standard font is set and connections are set.

			@param parent The parent QWidget
			@see createGroupKeyboardLayout(), createGroupUserReset(),
				createGroupTickerFont(), readSettings(),
				tickerFont
		*/
		TxtMessageDialog(QString message, QString title, QString setting = "",
			QWidget *parent = 0);

	public slots:

	private slots:

		//! Ok button pressed
		void clickOk();

	private:

		//! Creates a cancel and a save button.
		void createButtons();

		//! Creates the layout of the complete class.
		void createLayout();

		//! Writes user settings
		void writeSettings();

		QPushButton *buttonOk;
		QCheckBox *showDialogCheck;
		QLabel *messageLabel;

		QString settingName;
};

#endif //TXTMESSAGEDIALOG_H
