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
** Definition of the LessonDialog class
** File name: lessondialog.h
**
****************************************************************/

#ifndef LESSONDIALOG_H
#define LESSONDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QProgressBar>
#include <QRadioButton>
#include <QStringList>

#include "helpbrowser.h"

//! The LessonDialog class provides a lesson edit dialog.
/*!
	The LessonDialog class shows three program settings. Setting the
	layout of the virtual keyboard, setting the ticker speed and font
	and doing a reset of the user tables.

	@author Tom Thielicke, s712715
	@version 0.0.3
	@date 18.06.2006
*/
class LessonDialog : public QDialog {
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
		LessonDialog(QString lessonid, QStringList *data,
			QWidget *parent = 0);

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
		void showHelp();

	private:

		//! Creates a cancel and a save button.
		void createButtons();

		//! Creates the layout of the complete class.
		void createLayout();

		//! Creates the controls.
		void createControls();

		//! Updates the content of the controls.
		void updateContent();

		QLabel *labelLessonName;
		QLabel *labelLessonDescription;
		QLabel *labelLessonContent;
		QLabel *labelLessonNotices;
		QLabel *labelLessonUnit;
		QLabel *labelLessonUnitRadio;
		QLineEdit *lineLessonName;
		QLineEdit *lineLessonDescription;
		QTextEdit *lineLessonContent;
		QPushButton *buttonSave;
		QPushButton *buttonCancel;
		QPushButton *buttonHelp;
		QRadioButton *radioUnitSentence;
		QRadioButton *radioUnitWord;

		//! Object of the help browser dialog
		HelpBrowser *helpBrowser;

		QString currentLessonId;

		QStringList *lessonData;
};

#endif //LESSONDIALOG_H
