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
** Definition of the SettingDialog class
** File name: settingsdialog.h
**
****************************************************************/

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QListWidgetItem>
#include <QListWidget>
#include <QFont>
#include <QString>

#include "settingspages.h"
#include "helpbrowser.h"

//! The SettingsDialog class provides a program settings widget.
/*!
	The SettingsDialog class shows three program settings. Setting the
	layout of the virtual keyboard, setting the ticker speed and font
	and doing a reset of the user tables.

	@author Tom Thielicke, s712715
	@version 0.0.3
	@date 18.06.2006
*/
class SettingsDialog : public QDialog {
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
		SettingsDialog(QWidget *parent = 0);

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
		void showHelp();

	public slots:
		void changePage(QListWidgetItem *current, QListWidgetItem *previous);

	private:
		void createIcons();
        void readSettings();
        void writeSettings();

		//! Object of the help browser dialog
		HelpBrowser *helpBrowser;

		QPushButton *buttonSave;
		QPushButton *buttonCancel;
		QPushButton *buttonHelp;
		QListWidget *contentsWidget;
		QStackedWidget *pagesWidget;
		TrainingPage *trainingPage;
		DatabasePage *databasePage;
		LanguagePage *languagePage;
		OtherPage *otherPage;
		QFont tickerFont;
};

#endif
