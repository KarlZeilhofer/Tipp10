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
** Definition of the LicenseDialog class
** File name: licensedialog.h
**
****************************************************************/

#ifndef LICENSEDIALOG_H
#define LICENSEDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QString>

//! The LicenseDialog class provides a license input widget.
/*!
	The LicenseDialog class shows a dialog to enter a license key.

	@author Tom Thielicke, s712715
	@version 0.0.1
	@date 09.09.2008
*/
class LicenseDialog : public QDialog {
	Q_OBJECT

	public:

		//! Constructor, creates two table objects and provide it in two tabs.
		LicenseDialog(QWidget *parent = 0);

		bool checkLicenseKey(QString licenseKey);

	public slots:

	private slots:

		//! Start button pressed
		void clickOk();

		//! Demo button pressed
		void clickDemo();

	private:

		//! Creates a cancel and a ok button.
		void createButtons();

		//! Creates a textbox.
		void createLineEdit();

		//! Creates the layout of the complete class.
		void createLayout();

		//! Writes user settings
		void writeSettings();

		QPushButton *buttonOk;
		QPushButton *buttonDemo;
		QLabel *labelLicensing;
		QLineEdit *lineLicensing;
};

#endif //LICENSEDIALOG_H
