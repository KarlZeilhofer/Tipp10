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
** Implementation of the MainWindow class
** File name: mainwindow.cpp
**
****************************************************************/

#include <QIcon>
#include <QMessageBox>
#include <QMenuBar>
#include <QSize>
#include <QPoint>
#include <QSqlQuery>
#include <QSettings>
#include <QProcess>
#include <QDir>
#include <QRect>
#include <QCoreApplication>
#include <QByteArray>
#include <QDesktopServices>
#include <QDate>

#include "mainwindow.h"
#include "settingsdialog.h"
#include "updatedialog.h"
#include "def/defines.h"
#include "def/errordefines.h"
#include "errormessage.h"
#include "checkversion.h"

MainWindow::MainWindow() {
	trainingStarted = false;

	createActions();
	createMenu();

	createWidgets();

	createStart();
	startWidget->fillLessonList(false);
	selectedLesson = -1;
	selectedType = -1;

	setWindowTitle(APP_NAME);
	setWindowIcon(QIcon(":/img/" + QString(ICON_FILENAME)));
    setMinimumSize(APP_WIDTH_STANDARD, APP_HEIGHT_STANDARD);

    readSettings();
}

MainWindow::~MainWindow() {
	writeSettings();
}

void MainWindow::newVersionAvailable() {

    if (QMessageBox::information(this, APP_NAME,
        QString(tr("Ihre Version der Software ist "
        "veraltet.\nDie "
        "neue Version erhalten Sie im Internet unter %1\n\n"
        "Moechten Sie die neue Version jetzt herunterladen?"
        )).arg(APP_URL), QMessageBox::Yes | QMessageBox::No) ==
        QMessageBox::Yes) {

        QDesktopServices::openUrl(QString(APP_URL) + "/download/");
    }
    #if APP_PORTABLE
    QSettings settings(QCoreApplication::applicationDirPath() +
        "/portable/settings.ini", QSettings::IniFormat);
    #else
    QSettings settings;
    #endif
    settings.beginGroup("general");
    QDate today = QDate::currentDate();
    settings.setValue("last_version_check", today.addDays(-8));
    settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent *event) {

	if (trainingStarted) {

		// Ask, if training is already started
		switch (QMessageBox::question(this, APP_NAME,
			tr("Es gehen alle Werte der laufenden Lektion verloren!"
			"\n\nWollen Sie die Anwendung wirklich beenden?\n\n"),
			tr("&Ja"), tr("&Abbrechen"), 0, 1)) {

			case 0:
				// Ok button pushed
				break;
			case 1:
				// Cancel button pushed
				event->ignore();
				break;
		}
	}
}

bool MainWindow::checkLicenseKey(QString licenseKey) {

    return false;
}

void MainWindow::createMenu() {
	//Mac-Version:
	//-----------
	#if APP_MAC
	evaluationMenu = menuBar()->addMenu(tr("&Gehe zu"));
	evaluationMenu->addAction(exitAction);
	evaluationMenu->addAction(settingsAction);
    evaluationMenu->addAction(aboutAction);
    //evaluationMenu->addAction(updateAction);
    evaluationMenu->addAction(evalAction);
    evaluationMenu->addAction(gameAction);
	helpMenu = menuBar()->addMenu(tr("&Hilfe"));
    helpMenu->addAction(helpAction);
    helpMenu->addSeparator();
    helpMenu->addAction(websiteAction);
    helpMenu->addAction(donateAction);
    #else
	// Win/X11-Version:
	// ---------------
	// Menu bar items
    #if APP_MAC
    fileMenu = menuBar()->addMenu(APP_NAME_INTERN);
    #else
    fileMenu = menuBar()->addMenu(tr("&Datei"));
    #endif
    fileMenu->addAction(settingsAction);
    //fileMenu->addAction(updateAction);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAction);
	evaluationMenu = menuBar()->addMenu(tr("&Gehe zu"));
	evaluationMenu->addAction(evalAction);
    //evaluationMenu->addSeparator();
    evaluationMenu->addAction(gameAction);
	helpMenu = menuBar()->addMenu(tr("&Hilfe"));
    helpMenu->addAction(helpAction);
    helpMenu->addAction(websiteAction);
    helpMenu->addAction(donateAction);
    helpMenu->addSeparator();
	helpMenu->addAction(aboutAction);
	#endif
}

void MainWindow::createActions() {
	// Mac-Version:
	// -----------
	#if APP_MAC
	settingsAction = new QAction(QIcon(":/img/menu_settings.png"),
        tr("&Einstellungen"), this);
	#else
	// Win/X11-Version:
	// ---------------
	// Menu bar actions
	settingsAction = new QAction(QIcon(":/img/menu_settings.png"),
        tr("&Grundeinstellungen"), this);
	#endif
    updateAction = new QAction(QIcon(":/img/menu_update.png"),
        tr("Aktualisierung"), this);
    exitAction = new QAction(tr("&Beenden"), this);
    evalAction = new QAction(QIcon(":/img/menu_evaluation.png"),
        tr("&Lernstatistik"), this);
    helpAction = new QAction(QIcon(":/img/menu_help.png"),
        tr("&Bedienungsanleitung"), this);
    websiteAction = new QAction(QIcon(":/img/menu_website.png"),
        APP_NAME_INTERN + tr(" im Internet"), this);
    #if APP_MAC
    aboutAction = new QAction(tr("Info"), this);
    #else
    aboutAction = new QAction(tr("&Ueber ") + APP_NAME_INTERN, this);
    #endif
    donateAction = new QAction(QIcon(":/img/menu_donate.png"),
        tr("Spenden"), this);
    gameAction = new QAction(QIcon(":/img/menu_game.png"),
		tr("ABC-Spiel"), this);


	connect(gameAction, SIGNAL(triggered()), this, SLOT(toggleStartToAbcrain()));

	// Connect bar actions
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
	connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
	connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));
	connect(updateAction, SIGNAL(triggered()), this, SLOT(showUpdate()));
	connect(evalAction, SIGNAL(triggered()), this,
        SLOT(toggleStartToEvaluation()));
    connect(websiteAction, SIGNAL(triggered()), this, SLOT(openWebsite()));
    connect(donateAction, SIGNAL(triggered()), this, SLOT(openDonate()));
	connect(helpAction, SIGNAL(triggered()), this, SLOT(showHelp()));
}

void MainWindow::showSettings() {
	SettingsDialog settingsDialog(this);
	settingsDialog.exec();
	// Fill lesson list after changing program settings
	startWidget->fillLessonList(false);
	startWidget->readSettings();
}

void MainWindow::showUpdate() {
	UpdateDialog updateDialog(this);
	updateDialog.exec();
	// Fill lesson list after online update
	startWidget->fillLessonList(false);
}

void MainWindow::showHelp()
{
    helpBrowser = new HelpBrowser("", 0);
	helpBrowser->show();
}

void MainWindow::openWebsite()
{
    QDesktopServices::openUrl(QString(APP_URL));
}

void MainWindow::openDonate()
{
    QDesktopServices::openUrl(QString(APP_URL) + "/donate/");
}

void MainWindow::about()
{

	QSqlQuery query;
	QString dbVersion = "?";
	QString softwareVersion = "?";
	QString versionText = "";


    // Get database version info
    if (!query.exec("SELECT * FROM db_version ORDER BY version DESC;")) {
        ErrorMessage *errorMessage = new ErrorMessage(this);
        errorMessage->showMessage(ERR_DB_VERSION_READABLE,
            TYPE_WARNING, CANCEL_NO);
        // return is not necessary here - "about message" anyway
    } else {
        if (query.first()) {
            // One number represents the DB version
            dbVersion = query.value(0).toString();
            softwareVersion = query.value(1).toString();
        }
    }
    //Show about window with image
    versionText = tr("Programmversion ") + softwareVersion +
        "<br>" + tr("Datenbankversion ") + dbVersion;
    if (APP_PORTABLE) {
        versionText.append("<br>" + tr("Portable Version"));
    }

    QString infoHeadline = tr("Der intelligente 10-Finger-Schreibtrainer");
    QString infoSubHeadline = tr("Im Internet: ") +
            "<a href=\"" + QString(APP_URL) + "\">" +
            QString(APP_URL) +
            "</a>";
    QString infoText = tr(""
        "TIPP10 wird unter den Bedingungen der GNU General Public License veroeffentlicht und "
        "ist kostenlos. Sie muessen dafuer nichts bezahlen, egal wo Sie die Software herunter"
        "laden!");
    QString supportHeadline = tr("TIPP10 unterstuetzen");
    QString supportText = tr(""
        "Sollte Ihnen TIPP10 gut gefallen, wuerden wir uns sehr freuen, wenn Sie das "
        "Projekt mit einem Geldbetrag unterstuetzen wuerden. Nur so kann "
        "TIPP10 auch weiterhin kostenlos angeboten, gepflegt und weiterentwickelt werden. Vielen Dank!") +
        " <a href=\"http://www.tipp10.de/index.php?action=spend\">" +
        tr("Spenden") +
        "</a>";
    QString licenseHeadline = tr("GNU General Public License");
    QString licenseSubHeadline = "TIPP10 Version 2, Copyright (c) 2006-2011, Tom Thielicke IT Solutions";
    QString licenseText = tr(""
        "TIPP10 comes with ABSOLUTELY NO WARRANTY; This is free software, and you are "
        "welcome to redistribute it under certain conditions;") +
        " <a href=\"http://www.tipp10.de/index.php?action=licence\">" +
        tr("details") +
        "</a>";
    QString companyHeadline = tr("TIPP10 wird herausgegeben von");
    QString companyText = QString(APP_ORGANIZATION) + ", " +
            "<a href=\"" + "http://www.thielicke.org" + "\">" +
            "http://www.thielicke.org" +
            "</a>";


    QMessageBox::about(this, QString(tr("Ueber ") + APP_NAME),
                       QString::fromLatin1("<center><img src=\":/img/logo_orange.png\">"
                                                           "<h3>%1</h3>"
                                                           "<p>%2</p>"
                                                           "<p>%3</p></center>"
                                                           "<p>%4</p>"
                                                           "<p><b>%5</b><br>"
                                                           "%6</p>"
                                                           "<p><b>%7</b><br>"
                                                           "%8<br>"
                                                           "%9</p>"
                                                           "<p><b>%10</b><br>"
                                                           "%11</p>")
                                          .arg(infoHeadline)
                                          .arg(infoSubHeadline)
                                          .arg(versionText)
                                          .arg(infoText)
                                          .arg(supportHeadline)
                                          .arg(supportText)
                                          .arg(licenseHeadline)
                                          .arg(licenseSubHeadline)
                                          .arg(licenseText)
                                          .arg(companyHeadline)
                                          .arg(companyText));

}

void MainWindow::createWidgets() {
	stackedWidget = new QStackedWidget(this);
	setCentralWidget(stackedWidget);
	startWidget = new StartWidget(this);

}

void MainWindow::createStart() {
	menuBar()->show();
    //setCentralWidget(startWidget);
    stackedWidget->addWidget(startWidget);
    stackedWidget->setCurrentWidget(startWidget);
	connect(startWidget, SIGNAL(trainingClicked(int, int, QString)),
		this, SLOT(toggleStartToTraining(int, int, QString)));
	if (!isMaximized() && height() < APP_HEIGHT_STANDARD) {
		resize(APP_WIDTH_STANDARD, APP_HEIGHT_STANDARD);
	}
	setMinimumSize(APP_WIDTH_STANDARD, APP_HEIGHT_STANDARD);
}

void MainWindow::deleteStart() {
    stackedWidget->removeWidget(startWidget);
	disconnect(startWidget, SIGNAL(trainingClicked(int, int, QString)),
		this, SLOT(toggleStartToTraining(int, int, QString)));
	startWidget->writeSettings();
	//delete startWidget;
}

void MainWindow::createTraining(int lesson, int type, QString name) {
	trainingStarted = true;
	menuBar()->hide();
	trainingWidget = new TrainingWidget(lesson, type, name, this);
	//setCentralWidget(trainingWidget);
    stackedWidget->addWidget(trainingWidget);
    stackedWidget->setCurrentWidget(trainingWidget);
    trainingWidget->tickerBoard->setFocus();
    connect(trainingWidget, SIGNAL(lessonReady(int, int, QList<QChar>, QList<int>)),
		this, SLOT(toggleTrainingToEvaluation(int, int, QList<QChar>, QList<int>)));
	connect(trainingWidget, SIGNAL(lessonCanceled()),
		this, SLOT(toggleTrainingToStart()));
}

void MainWindow::deleteTraining() {
    stackedWidget->removeWidget(trainingWidget);
	trainingStarted = false;
	disconnect(trainingWidget, SIGNAL(lessonReady(int, int, QList<QChar>, QList<int>)),
		this, SLOT(toggleTrainingToEvaluation(int, int, QList<QChar>, QList<int>)));
	disconnect(trainingWidget, SIGNAL(lessonCanceled()),
		this, SLOT(toggleTrainingToStart()));
	delete trainingWidget;
}

void MainWindow::createEvaluation(int row, int type, QList<QChar> charList,
	QList<int> mistakeList) {

	menuBar()->hide();
	evaluationWidget = new EvaluationWidget(row, type, charList, mistakeList, this);
    //setCentralWidget(evaluationWidget);
    stackedWidget->addWidget(evaluationWidget);
    stackedWidget->setCurrentWidget(evaluationWidget);
	connect(evaluationWidget, SIGNAL(readyClicked()),
		this, SLOT(toggleEvaluationToStart()));
	if (!isMaximized() && height() < APP_HEIGHT_STANDARD) {
		resize(APP_WIDTH_STANDARD, APP_HEIGHT_STANDARD);
	}
	setMinimumSize(APP_WIDTH_STANDARD, APP_HEIGHT_STANDARD);
}

void MainWindow::deleteEvaluation() {
    stackedWidget->removeWidget(evaluationWidget);
	disconnect(evaluationWidget, SIGNAL(readyClicked()),
		this, SLOT(toggleEvaluationToStart()));
	delete evaluationWidget;
}

void MainWindow::createAbcrain() {
	menuBar()->hide();
	abcRainWidget = new AbcRainWidget(this);
    //setCentralWidget(evaluationWidget);
    stackedWidget->addWidget(abcRainWidget);
    stackedWidget->setCurrentWidget(abcRainWidget);
	connect(abcRainWidget, SIGNAL(readyClicked()),
		this, SLOT(toggleAbcrainToStart()));
}

void MainWindow::deleteAbcrain() {
    stackedWidget->removeWidget(abcRainWidget);
	disconnect(abcRainWidget, SIGNAL(readyClicked()),
		this, SLOT(toggleAbcrainToStart()));
	delete abcRainWidget;
}

void MainWindow::toggleStartToTraining(int lesson, int type, QString name) {
	selectedLesson = lesson;
	selectedType = type;
	selectedName = name;
	deleteStart();
	startWidget->fillLessonList(false);
	createTraining(lesson, type, name);
}

void MainWindow::toggleTrainingToStart() {
	deleteTraining();
	createStart();
}

void MainWindow::toggleTrainingToEvaluation(int row, int type, QList<QChar> charList,
	QList<int> mistakeList) {

	deleteTraining();
	createEvaluation(row, type, charList, mistakeList);
}

void MainWindow::toggleEvaluationToStart() {
	deleteEvaluation();
	createStart();
	startWidget->fillLessonList(false);
}

void MainWindow::toggleStartToEvaluation() {
	deleteStart();
	QList<QChar> charList;
	QList<int> mistakeList;
	createEvaluation(0, 0, charList, mistakeList);
}

void MainWindow::toggleAbcrainToStart() {
	deleteAbcrain();
	createStart();
	startWidget->fillLessonList(false);
}

void MainWindow::toggleStartToAbcrain() {
	deleteStart();
	createAbcrain();
}

void MainWindow::readSettings() {
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif

	settings.beginGroup("mainwindow");
	#if APP_WIN
	QByteArray storedGeometry;
	storedGeometry = settings.value("geometry").toByteArray();
	if (storedGeometry.isEmpty() || storedGeometry.isNull()) {
		resize(QSize(APP_WIDTH_STANDARD, APP_HEIGHT_STANDARD));
		move(QPoint(100, 100));
	} else {
		restoreGeometry(storedGeometry);
	}
	#else
	resize(settings.value("size", QSize(APP_WIDTH_STANDARD, APP_HEIGHT_STANDARD)).toSize());
	move(settings.value("pos", QPoint(100, 100)).toPoint());
	#endif
	settings.endGroup();

    settings.beginGroup("general");
    if (settings.value("check_new_version", true).toBool()) {

        QDate lastVersionCheck = settings.value("last_version_check").toDate();
        QDate today = QDate::currentDate();

        if (!lastVersionCheck.isValid() ||
            lastVersionCheck.addDays(7) < today) {
            CheckVersion *checkVersion = new CheckVersion();
            connect(checkVersion, SIGNAL(newVersionAvailable()), this, SLOT(newVersionAvailable()));
            checkVersion->checkVersion();
        }
        settings.setValue("last_version_check", today);
    }
    settings.endGroup();
}

void MainWindow::writeSettings() {
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif

    settings.beginGroup("mainwindow");
	#if APP_WIN
	settings.setValue("geometry", saveGeometry());
	#else
	settings.setValue("size", size());
	settings.setValue("pos", pos());
	#endif
	settings.endGroup();
}
