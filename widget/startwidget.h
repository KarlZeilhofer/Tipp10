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
** Definition of the StartWidget class
** File name: startwidget.h
**
****************************************************************/

#ifndef STARTWIDGET_H
#define STARTWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QListWidget>
#include <QRadioButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QListWidgetItem>
#include <QList>
#include <QString>
#include <QTabWidget>
#include <QMenu>

#include "companylogo.h"
#include "helpbrowser.h"

//! The StartWidget class provides a lesson list and lesson settings.
/*!
	The StartWidget class provides a widget with a lesson list, buttons to exit
	program and to start training, and a three lesson settings groups.

	@author Tom Thielicke, s712715
	@version 0.1.1
	@date 23.06.2006
*/
class StartWidget : public QWidget {
	Q_OBJECT

	public:

		//! Constructor, creates lesson list, setting groups and buttons.
		/*!
			In this contructor four groups are created over functions
			createGroupLesson(), createGroupLimit(), createGroupError() and
			createGroupSupport(). In addition, user settings are read,
			and connections are set.

			@param parent The parent QWidget
			@see createGroupLesson(), createGroupLimit(), createGroupError(),
				createGroupSupport(), createButtons(), createLayout(),
				createConnections(), readSettings()
		*/
		StartWidget(QWidget *parent = 0);

		//! Deconstructor, saves the current settings to hard disk.
		/*!
			In this contructor four groups are created over functions
			createGroupLesson(), createGroupLimit(), createGroupError() and
			createGroupSupport(). In addition, user settings are read,
			and connections are set.

			@see writeSettings()
		*/
		~StartWidget();

		//! Fills the lesson list over database class StartSql.
		/*!
			@see listLesson
		*/
		void fillLessonList(bool themeChanged);

		//! Writes all user settings to hard disk.
		void writeSettings();

		//! Reads all user settings.
		void readSettings();

		//! Arrays for all lesson lists with data id
		QList<QString> arrayTraining;
		QList<QString> arrayOpen;
		QList<QString> arrayOwn;
		QList<QString> arrayThemes;

	protected:
		//! Lesson list.
		QListWidget *listLesson;
		QListWidget *listOpen;
		QListWidget *listOwn;
		QComboBox *comboTheme;

	signals:
		//! Signal, starts the training.
		/*!
			This signal is emitted in function clickTraining().

			@param lesson Number of the selected lesson
			@see clickTraining()
		*/
		void trainingClicked(int lesson, int type, QString name);

	private slots:
		//! Toggles between time limit and token limit.
		void toggleLimit();

		//! Sets and disables the error handling checkboxes.
		void toggleError();

		void toggleHelpers();

		void toggleIntelligence();

		void toggleTabs(int index);

		void toggleThemes(int index);

		//! Slot, starts the training.
		/*!
			This slot receives a signal when a user pushs the "start training"
			button. It checks the selected lesson and emits the
			trainingClicked signal to inform the parent class about.

			@see trainingClicked()
		*/
		void clickTraining();

		void clickNewLesson();

		void clickEditLesson();

		void clickImportLesson();

		void clickExportLesson();

		void clickDownloadLesson();

		void clickDeleteLesson();

		void doubleClickLesson(QListWidgetItem *item);

		//! Slot, shows the help dialog.
		/*!
			This slot shows the help dialog. It creates an object of
			the QDialog class with an QTextbrowser.
		*/
		void showHelp();

	private:
		void readLicenseSettings();
    	//! Creates a radiobutton with time and token limit items.
		void createGroupLimit();

    	//! Creates checkboxes with different error handlings while training.
		void createGroupError();

		//! Creates checkboxes with different supports while training.
		void createGroupSupport();

    	//! Creates a lesson list widget.
		void createGroupLesson();

		//! Creates buttons "exit" and "start training".
		void createButtons();

		//! Creates the layout of the complete class.
		void createLayout();

		//! Creates connections for this class.
		void createConnections();

		QPushButton *buttonTraining;
		QPushButton *buttonExit;
		QPushButton *buttonHelp;
		QGroupBox *groupLesson;
		QGroupBox *groupLimit;
		QGroupBox *groupError;
		QGroupBox *groupKeyboard;
		QLabel *labelIntelligence;
		QLabel *labelLimitLesson;
		QLabel *labelTheme;
		QRadioButton *radioLimitTime;
		QRadioButton *radioLimitToken;
		QRadioButton *radioLimitLesson;
		QSpinBox *spinLimitTime;
		QSpinBox *spinLimitToken;
		QCheckBox *checkIntelligence;
		QCheckBox *checkErrorBeep;
        QCheckBox *checkErrorStop;
        QCheckBox *checkErrorCorrect;
        QCheckBox *checkErrorImage;
		QCheckBox *checkHelpers;
		QCheckBox *checkKeySelection;
		QCheckBox *checkKeySelectionStart;
		QCheckBox *checkKeyBorder;
		QCheckBox *checkStatusInformation;
		QCheckBox *checkKeyPath;
		CompanyLogo *companyLogo;
		QPushButton *buttonEditLesson;
		QWidget *tabTrainingLessons;
		QWidget *tabOpenLessons;
		QWidget *tabOwnLessons;
		bool toggleLimitLesson;
		bool checkLessonPublish;
		bool checkToggleIntelligence;
		bool checkTxtWarning;
		bool lessonExportVisible;
        bool openLessonWarning;

		//! Object of the help browser dialog
		HelpBrowser *helpBrowser;

		//! Menu entry for the file menu.
		QMenu *lessonEditMenu;

		//! Menu item for lesson edit menu.
		QAction *lessonEdit;

		//! Menu item for lesson edit menu.
		QAction *lessonDel;

		//! Menu item for lesson edit menu.
		QAction *lessonNew;

		//! Menu item for lesson import menu.
		QAction *lessonImport;

		//! Menu item for lesson export menu.
		QAction *lessonExport;

		//! Menu item for lesson download menu.
		QAction *lessonDownload;

		//! Menu item for lesson edit menu.
		//QAction *lessonPublish;

		//! Tab widget
		QTabWidget *tabLessons;
};

#endif // STARTWIDGET_H
