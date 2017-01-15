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
** Definition of the RegExpDialog class
** File name: regexpdialog.h
**
****************************************************************/

#ifndef REGEXPDIALOG_H
#define REGEXPDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

#include "helpbrowser.h"

//! The RegExpDialog class provides a keyboard layout regular expression dialog.

class RegExpDialog : public QDialog {
	Q_OBJECT

    public:

        RegExpDialog(QString layout, QWidget *parent = 0);

	public slots:

	private slots:

		//! Slot, saves the setting data.
		/*!
			After the user clicked the save button, this function saves the
			settings over function writeSettings() and closes the current
			dialog.

			@see writeSettings()
		*/
		void clickSave();

		//! Slot, shows the help dialog.
		/*!
			This slot shows the help dialog. It creates an object of
			the QDialog class with an QTextbrowser.
		*/
        void getDefault();

	private:

        //! Reads all user settings.
        void readSettings();

        void writeSettings();

		//! Creates a cancel and a save button.
		void createButtons();

		//! Creates the layout of the complete class.
		void createLayout();

		//! Creates the controls.
		void createControls();

        QLabel *labelRegExp;
        QLabel *labelReplace;
        QLabel *labelRegExpHelp;
        QLabel *labelReplaceHelp;
        QLineEdit *lineRegExp;
        QLineEdit *lineReplace;
		QPushButton *buttonSave;
		QPushButton *buttonCancel;
        QPushButton *buttonDefault;

        QString currentLayout;

		//! Object of the help browser dialog
        HelpBrowser *helpBrowser;
};

#endif //REGEXPDIALOG_H
