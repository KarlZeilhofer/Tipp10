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
** Definition of the MainWindow class
** File name: mainwindow.h
**
****************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QStackedWidget>
#include <QString>
#include <QList>
#include <QChar>

#include "startwidget.h"
#include "trainingwidget.h"
#include "evaluationwidget.h"
#include "illustrationdialog.h"
#include "licensedialog.h"
#include "games/abcrainwidget.h"
#include "helpbrowser.h"

//! The MainWindow class provides the main window of the application.
/*!
	The MainWindow class shows the main window. At startup it shows the widget
	StartWidget. Over functions the widget can be changed also to TrainingWidget
	or Evaluatonwidget. In addition it has an own menu bar.

	@author Tom Thielicke, s712715
	@version 0.1.9
	@date 01.07.2006
*/
class MainWindow : public QMainWindow {
	Q_OBJECT

	public:

		//! Constructor, initializes the window and shows the start widget.
		/*!
			This contructor initializes the following variable:
				- trainingStarted = false

			In addition, it sets menu actions and the menu bar over functions
			createActions() and createMenu(). After that, it creates the start
			widget over function createStart() and reads the settings over
			function readSettings().
			At the end it sets the window title, icon and window size.

			@see trainingStarted, createActions(), createMenu(), createStart(),
				readSettings()
		*/
		MainWindow();

		//! Deconstructor, writes the current settings to hard disk.
		/*!
			This decontructor writes the settings over function
			writeSettings().

			@see writeSettings()
		*/
		~MainWindow();

	public slots:

		//! Slot, deletes the start widget and shows the training widget.
		/*!
			This slot deletes the start widget over function deleteStart() and
			shows the training widget over function createTraining().

			@see deleteStart(), createTraining()
		*/
		void toggleStartToTraining(int lesson, int type, QString name);

		//! Slot, deletes the training widget and shows the start widget.
		/*!
			This slot deletes the training widget over function deleteTraining()
			and shows the start widget over function createStart().

			@see deleteTraining(), createStart()
		*/
		void toggleTrainingToStart();

		//! Slot, deletes the training widget and shows the evaluation widget.
		/*!
			This slot deletes the training widget over function deleteTraining()
			and shows the evaluation widget over function createEvaluation().

			@see deleteTraining(), createEvaluation()
		*/
		void toggleTrainingToEvaluation(int row, int type, QList<QChar> charList,
			QList<int> mistakeList);

		//! Slot, deletes the evaluation widget and shows the start widget.
		/*!
			This slot deletes the evaluation widget over function
			deleteEvaluation() and shows the start widget over
			function createStart().

			@see deleteEvaluation(), createStart()
		*/
		void toggleEvaluationToStart();

		//! Slot, deletes the start widget and shows the evaluation widget.
		/*!
			This slot deletes the start widget over function deleteStart() and
			shows the evaluation widget over function createEvaluation().

			@see deleteStart(), createEvaluation()
		*/
		void toggleStartToEvaluation();

		//! Slot, deletes the abcrain widget and shows the start widget.
		/*!
			This slot deletes the abcrain widget over function
			deleteAbcrain() and shows the start widget over
			function createStart().

			@see deleteAbcrain(), createStart()
		*/
		void toggleAbcrainToStart();

		//! Slot, deletes the start widget and shows the abcrain widget.
		/*!
			This slot deletes the start widget over function deleteStart() and
			shows the evaluation widget over function createAbcrain().

			@see deleteStart(), createAbcrain()
		*/
		void toggleStartToAbcrain();

		//! Slot, shows the online update dialog.
		/*!
			This slot shows the online update dialog. It creates an object of
			the UpdateDialog class.

			@attention It updates the lesson list after the dialog was closed
			@see UpdateDialog, fillLessonList()
		*/
		void showUpdate();

        void newVersionAvailable();

	private slots:

		//! Slot, shows the settings dialog.
		/*!
			This slot shows the settings dialog. It creates an object of
			the SettingsDialog class.

			@attention It updates the lesson list after the dialog was closed
			@see SettingsDialog, fillLessonList()
		*/
		void showSettings();

		//! Slot, shows the help dialog.
		/*!
			This slot shows the help dialog. It creates an object of
			the QDialog class with an QTextbrowser.
		*/
		void showHelp();

		//! Slot, opens a website in the default browser.
		/*!
			This slot opens a website (url) in the default browser.
        */
        void openWebsite();

        void openDonate();

		//! Slot, shows the about dialog.
		/*!
			This slot shows the about dialog. It creates a QMessageBox with
			program informations. To show the database version the version
			is read over a sql-command.
		*/
		void about();

	protected:

		//! Window close event, shows a request if training is started.
		/*!
			The window close event detect is user want to close the window. If
			training is started, the user is asked, wether he really wants to
			quit.

			@see trainingStarted
		*/
		void closeEvent(QCloseEvent *event);

		//! Writes the postition and size of the main window to hard disk.
		void writeSettings();

		//! Reads the postition and size of the main window.
		void readSettings();

    private:

		bool checkLicenseKey(QString licenseKey);

		//! Creates all necessary actions for the menu bar.
		/*!
			This function creates all menu bar actions and connects them
			to different slots.

			@see settingsAction, updateAction, exitAction, evalAction,
				helpAction, aboutAction, about(), close(), showSettings(),
				showUpdate(), toggleStartToEvaluation()
		*/
		void createActions();

		//! Creates the menu bar.
		/*!
			This function creates the menu bar an its top menu entries.

			@see fileMenu, evaluationMenu, helpMenu
		*/
		void createMenu();

		//! Creates all Widgets.
		/*!
			This function creates all widgets in a stacked widget.
		*/
		void createWidgets();

		//! Shows the start widget and enables the menu bar.
		/*!
			This function creates an object of the StartWidget class, shows
			it in the main window and enables the menu bar.

			@see StartWidget
		*/
		void createStart();

		//! Shows the training widget and disables the menu bar.
		/*!
			This function creates an object of the TrainingWidget class, shows
			it in the main window and disables (hides) the menu bar.

			@see TrainingWidget
		*/
		void createTraining(int lesson, int type, QString name);

		//! Shows the evaluation widget and disables the menu bar.
		/*!
			This function creates an object of the EvaluationWidget class, shows
			it in the main window and disables (hides) the menu bar. In
			addition, the variable trainingStarted is turned true.

			@see EvaluationWidget, trainingStarted
		*/
		void createEvaluation(int row, int type, QList<QChar> charList,
			QList<int> mistakeList);

		//! Shows the abcrain widget and disables the menu bar.
		/*!
			This function creates an object of the AbcRainWidget class, shows
			it in the main window and disables (hides) the menu bar.

			@see AbcRainWidget
		*/
		void createAbcrain();

		//! Deletes the start widget.
		/*!
			This function deletes the object startWidget.

			@see StartWidget
		*/
		void deleteStart();

		//! Deletes the training widget.
		/*!
			This function deletes the object trainingWidget and turnes the
			variable trainingStarted to false.

			@see TrainingWidget, trainingStarted
		*/
		void deleteTraining();

		//! Deletes the evaluation widget.
		/*!
			This function deletes the object evaluationWidget.

			@see EvaluationWidget
		*/
		void deleteEvaluation();

		//! Deletes the abcrain widget.
		/*!
			This function deletes the object abcrainWidget.

			@see AbcRainWidget
		*/
		void deleteAbcrain();

		//! Flag, training is started.
		bool trainingStarted;

		//! Object of the stacked widget
		QStackedWidget *stackedWidget;

		//! Object of the start widget.
		StartWidget *startWidget;

		//! Object of the training widget.
		TrainingWidget *trainingWidget;

		//! Object of the evaluation widget.
		EvaluationWidget *evaluationWidget;

		//! Object of the abcrain widget.
		AbcRainWidget *abcRainWidget;

		//! Object of the help browser dialog
		HelpBrowser *helpBrowser;

		//! Menu entry for the file menu.
		QMenu *fileMenu;

		//! Menu entry for the evaluation menu.
		QMenu *evaluationMenu;

		//! Menu entry for the help menu.
		QMenu *helpMenu;

		//! Menu item for settings.
		QAction *settingsAction;

		//! Menu item for online update.
		QAction *updateAction;

		//! Menu item for user admin.
		QAction *usersAction;

		//! Menu item for exit.
		QAction *exitAction;

		//! Menu item for evaluation.
		QAction *evalAction;

		//! Menu item for opening the website.
		QAction *websiteAction;

		//! Menu item for help.
		QAction *helpAction;

		//! Menu item for about.
        QAction *aboutAction;

        //! Menu item for game.
        QAction *gameAction;

        //! Menu item for donate.
        QAction *donateAction;

		int selectedLesson;

		int selectedType;

		QString selectedName;
};

#endif //MAINWINDOW_H
