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
** Definition of the TrainingPage, DatabasePage and
** OtherPage class
** File name: settingspages.h
**
****************************************************************/

#ifndef SETTINGPAGES_H
#define SETTINGPAGES_H

#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QSlider>
#include <QPushButton>
#include <QGroupBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QFont>
#include <QString>

class TrainingPage : public QWidget {
	Q_OBJECT

	public:
		TrainingPage(QWidget *parent = 0);

		//! Writes all user settings to hard disk.
		void writeSettings();

	public slots:

		//! Slot, shows the font setting window.
		/*!
			After the user clicked the font button, this function creates
			a QFontDialog and sets the new font if user has choosen one.
		*/
        void setFont();

        void setFontColor();

        void setBgColor();

        void setCursorColor();

	private:

    	//! Creates a ticker font button and the ticker speed spin box.
		void createGroupTickerFont();

    	//! Creates a group with the sound output selection.
		void createGroupSoundOutput();

		//! Creates the layout of the complete class.
		void createLayout();

		//! Sets the name of the font button.
		void setFontButtonLabel();

		//! Reads all user settings.
		void readSettings();

        QLabel *labelTickerFont;
        QLabel *labelTickerFontColor;
        QLabel *labelTickerBgColor;
        QLabel *labelTickerCursorColor;
        QLabel *labelTickerSpeed;
		QLabel *labelTickerSpeedMax;
		QLabel *labelTickerSpeedMin;
		QLabel *labelDatabasePath;
		QSlider *sliderTickerSpeed;
		QGroupBox *groupTickerFont;
        QGroupBox *groupSoundOutput;
        QPushButton *buttonSetFont;
        QPushButton *buttonSetFontColor;
        QPushButton *buttonSetBgColor;
        QPushButton *buttonSetCursorColor;
		QFont tickerFont;
		QRadioButton *radioSoundcard;
		QRadioButton *radioSpeaker;
        QCheckBox *checkMetronome;
        QSpinBox *spinMetronomeClock;
};

class DatabasePage : public QWidget {
	Q_OBJECT

	public:
		DatabasePage(QWidget *parent = 0);

		//! Writes all user settings to hard disk.
		void writeSettings();

	private slots:

		//! Slot, deletes the content of the user lessons table.
		/*!
			After the user clicked the reset user data button, this function
			deletes the content data of the user lesson table over
			StartSql class.

			@see StartSql
		*/
		void deleteUserLessonList();

		//! Slot, deletes the content of the user chars table.
		/*!
			After the user clicked the reset user data button, this function
			deletes the content data of the user chars table over
			StartSql class.

			@see StartSql
		*/
		void deleteUserChars();

		//! Slot, opens and cheks the database path file dialog.
		/*!
			After the user clicked the browse button, this function
			shows a file dialog. After that, the exist of the file
			is checked.
		*/
		void setDatabasePath();

	private:

    	//! Creates two user data reset buttons.
		void createGroupUserReset();

    	//! Creates a database path control.
		void createGroupDatabase();

		//! Creates the layout of the complete class.
		void createLayout();

		//! Reads all user settings.
		void readSettings();

		QLabel *labelDatabasePath;
		QLabel *labelDatabaseExplain;
		QLineEdit *lineDatabasePath;
		QGroupBox *groupUserReset;
		QGroupBox *groupDatabase;
		QPushButton *buttonLessonsReset;
		QPushButton *buttonCharsReset;
		QPushButton *buttonDatabasePath;
		QString currentDatabasePath;
};

class OtherPage : public QWidget {
	Q_OBJECT

	public:
		OtherPage(QWidget *parent = 0);

		//! Writes all user settings to hard disk.
		bool writeSettings();

	private:

    	//! Creates a group with check boxes to en-/disable info dialogs.
		void createGroupDialogCheck();

    	//! Creates a group with check boxes to en-/disable info dialogs.
		void createGroupAdaptation();

		//! Creates the layout of the complete class.
		void createLayout();

		//! Reads all user settings.
		void readSettings();

		QGroupBox *groupDialogCheck;
		QGroupBox *groupAdaptation;
		QCheckBox *checkIllustration;
		QCheckBox *checkIntelligence;
		QCheckBox *checkLimitLesson;
		QCheckBox *checkLessonPublish;
        QCheckBox *checkNativeStyle;
        QCheckBox *checkTxtMessage;
        QCheckBox *checkNewVersion;
};

class LanguagePage : public QWidget {
	Q_OBJECT

	public:
		LanguagePage(QWidget *parent = 0);

		//! Writes all user settings to hard disk.
		bool writeSettings();

    private slots:

        void checkLessonToLayout();

        void clearLayoutSetting();

        void showLayoutAdvanced();

	private:

    	//! Creates a group with language selection
		void createGroupLanguage();

		//! Creates the layout of the complete class.
		void createLayout();

		//! Reads all user settings.
        void readSettings();

		QGroupBox *groupLanguage;
		QLabel *labelLanguage;
		QLabel *labelLayout;
		QLabel *labelLesson;
        QLabel *labelLessonNotice;
		QComboBox *comboLanguages;
        QComboBox *comboLayouts;
        QComboBox *comboLessons;
        QPushButton *buttonLayoutRegEx;
};

#endif
