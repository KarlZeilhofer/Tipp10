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
** Definition of the TrainingWidget class
** File name: trainingwidget.h
**
****************************************************************/

#ifndef TRAININGWIDGET_H
#define TRAININGWIDGET_H

#include <QWidget>
#include <QList>
#include <QChar>
#include <QString>
#include <QTimer>
#include <QPushButton>
#include <QDateTime>
#include <QSound>

#include "tickerboard.h"
#include "keyboard.h"
#include "numpad.h"
#include "statusbar.h"
#include "sql/trainingsql.h"
#include "sql/keyboardsql.h"
#include "companylogo.h"
#include "helpbrowser.h"

//! The TrainingWidget class provides the training widgets and manages them.
/*!
	The TrainingWidget class provides the training widgets ticker, virtual
	keyboard and a status bar. It manages the training by communicating
	with all widgets.

	@author Tom Thielicke, s712715
	@version 0.3.6
	@date 04.07.2006
*/
class TrainingWidget : public QWidget {
	Q_OBJECT

	public:

		//! Constructor, creates all necessary objects for training.
		/*!
			In this contructor the following variables are initialized:
				- isStarted = false;
				- isPaused = false;
				- currentChar = ' ';
				- currentStrokes = 0;
				- currentChars = 0;
				- currentErrors = 0;
				- currentSeconds = 0;
				- currentLesson = lesson;
				- errorCorrectFlag = false;
				- counterToNewLine = 0;

			In addition, it creates a ticker, a keyboard and a status bar.

			@param lesson Lesson number
			@param parent The parent QWidget
			@see createButtons(), createLayout(), createConnections(),
				readSettings(), startSession(), isStarted, isPaused
				currentChar, currentStrokes, currentErrors, currentSeconds,
				currentLesson, errorCorrectFlag, counterToNewLine, startTime
				startFlag, colorSelection, tickerFont, background
		*/
		TrainingWidget(int lesson, int type, QString name, QWidget *parent = 0);

		StatusBar *statusBar;

		KeyBoard *keyBoard;

		NumPad *numPad;

		TickerBoard *tickerBoard;

	signals:

		//! Signal, lesson is ready.
		/*!
			@param row Id of the saved user lesson results
		*/
		void lessonReady(int row, int type, QList<QChar> charList,
			QList<int> mistakeList);

		//! Signal, lesson was canceled.
		void lessonCanceled();

	public slots:

		//! Slot, starts the training session.
		void startSession();

		//! Slot, pauses the training session.
		void pauseSession();

		//! Slot, cancels the training session.
		void cancelSession();

		//! Slot, sets a new char from ticker.
		void setChar(QChar key);

		//! Slot, sets a key from virtual keyboard.
		void setKey(QChar key);

		//! Slot, updates the lesson text over the database class.
		void updateLesson();

		//! Slot, updates the text of the status bar over the database class.
		void updateStatusText(QString statustext);

    private slots:

        //! Slot, updates the time counter.
        void secondsUpdate();

        //! Slot, updates the time counter.
        void metronomeOutput();

		//! Saves lesson results and exit training.
		void exitTraining();

		//! Slot, shows the help dialog.
		/*!
			This slot shows the help dialog. It creates an object of
			the QDialog class with an QTextbrowser.
		*/
		void showHelp();

	private:

		//! Creates the buttons cancel and pause.
		void createButtons();

		//! Creates full layout of the class.
		void createLayout();

		//! Creates the connections between all widgets.
		void createConnections();

		//! Creates the first lesson text.
		void createLesson();

		//! Updates the status values.
		void updateStatusValues();

		//! Reads all user settings.
		void readSettings();

		//! Object of the help browser dialog
		HelpBrowser *helpBrowser;

		QChar currentChar;
		int currentLesson;
		int currentType;
		QString currentName;
		int currentStrokes;
		int currentChars;
		int currentErrors;
		int currentSeconds;
		int counterToNewLine;
		int counterChars;
		QDateTime startTime;
		bool isStarted;
		bool isPaused;
		int limitType; //0=Time, 1=Token, 2=Lesson
		int limitValue;
		bool stopOnError;
		bool correctOnError;
		bool beepOnError;
		bool beepSoundcard;
		bool errorCorrectFlag;
		int uncorrectedErrors;
		bool showStatusInformation;
		bool showHelpers;
		bool useIntelligence;
		bool useEszett;
		bool useUmlaut;
        int lessonUnit;
        QTimer *timer;
        QTimer *metronomeTimer;
        int metronomeClock;
		CompanyLogo *companyLogo;
		TrainingSql *trainingSql;
		KeyboardSql *keyboardSql;
		QPushButton *buttonPause;
		QPushButton *buttonCancel;
		QPushButton *buttonHelp;
        QString opSystem;
        QSound *bells;
        QSound *metronomeSound;

		QList<QChar> charList;
		QList<int> mistakeList;

        QString layoutSetting;
        QString replaceSetting;
        QString regexpSetting;

        bool checkErrorImage;
};

#endif // TRAININGWIDGET_H
