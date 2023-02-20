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
** Implementation of the StartWidget class
** File name: startwidget.cpp
**
****************************************************************/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIcon>
#include <QSqlQuery>
#include <QSettings>
#include <QApplication>
#include <QMessageBox>
#include <QCoreApplication>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QTextStream>
#include <QRegExp>
#include <QFont>

#include "startwidget.h"
#include "sql/startsql.h"
#include "updatedialog.h"
#include "def/defines.h"
#include "def/errordefines.h"
#include "errormessage.h"
#include "lessondialog.h"
#include "downloaddialog.h"
#include "illustrationdialog.h"
#include "txtmessagedialog.h"

StartWidget::StartWidget(QWidget *parent) : QWidget(parent) {
	// Set the logo
	companyLogo = new CompanyLogo(this);

	readLicenseSettings();

	// Create group boxes with user settings
    createGroupLesson();
    createGroupLimit();
    createGroupError();
    createGroupSupport();

    // Create buttons
    createButtons();

	// Set the layout of all widgets created above
	createLayout();

	// Read user settings
	readSettings();

	// Widget connections
	createConnections();

    buttonTraining->setFocus();
}

StartWidget::~StartWidget() {
	// Save settings before closing the startwidget
	writeSettings();
}

void StartWidget::createGroupLesson() {

	tabLessons = new QTabWidget();
	//tabLessons->setUsesScrollButtons(false);
	tabLessons->setElideMode(Qt::ElideRight);

	//TIPP10
	//------
	// List widget with training lessons
	listLesson = new QListWidget();
	//fillLessonList();

	tabTrainingLessons = new QWidget();
	QVBoxLayout *tabVLayout = new QVBoxLayout;
	tabVLayout->addWidget(listLesson);
	tabTrainingLessons->setLayout(tabVLayout);

	tabLessons->addTab(tabTrainingLessons, QIcon(":/img/tab_training.png"), tr("Uebungslektionen"));

	// List widget with opem lessons
	listOpen = new QListWidget();

	comboTheme = new QComboBox();

	labelTheme = new QLabel(tr("Thema:"));

	tabOpenLessons = new QWidget();
	QHBoxLayout *tabPLayout = new QHBoxLayout;
	tabPLayout->addStretch(1);
	tabPLayout->addWidget(labelTheme);
	tabPLayout->addWidget(comboTheme);
	QVBoxLayout *tabOLayout = new QVBoxLayout;
	tabOLayout->addLayout(tabPLayout);
	tabOLayout->addWidget(listOpen);
	tabOpenLessons->setLayout(tabOLayout);

	tabLessons->addTab(tabOpenLessons, QIcon(":/img/tab_open.png"), "");
	//COMAK-Release
	//-------------
	/*
	// List widget with open lessons
	listOpen = new QListWidget();

	comboTheme = new QComboBox();

	labelTheme = new QLabel(tr("Thema:"));

	tabOpenLessons = new QWidget();
	QHBoxLayout *tabPLayout = new QHBoxLayout;
	tabPLayout->addStretch(1);
	tabPLayout->addWidget(labelTheme);
	tabPLayout->addWidget(comboTheme);
	QVBoxLayout *tabOLayout = new QVBoxLayout;
	tabOLayout->addLayout(tabPLayout);
	tabOLayout->addWidget(listOpen);
	tabOpenLessons->setLayout(tabOLayout);

	tabLessons->addTab(tabOpenLessons, QIcon(":/img/tab_open.png"), "");

	// List widget with training lessons
	listLesson = new QListWidget();
	//fillLessonList();

	tabTrainingLessons = new QWidget();
	QVBoxLayout *tabVLayout = new QVBoxLayout;
	tabVLayout->addWidget(listLesson);
	tabTrainingLessons->setLayout(tabVLayout);

	tabLessons->addTab(tabTrainingLessons, QIcon(":/img/tab_training.png"), tr("Uebungslektionen"));
	*/





	// List widget with own lessons
	listOwn = new QListWidget();

	lessonEditMenu = new QMenu();
	buttonEditLesson = new QPushButton(tr("&Bearbeiten"));
    buttonEditLesson->setFixedHeight(20);

	lessonNew = new QAction(QIcon(":/img/menu_new.png"),
		tr("&Neue Lektion"), this);
	lessonImport = new QAction(QIcon(":/img/menu_import.png"),
		tr("Lektion &importieren"), this);
	lessonExport = new QAction(QIcon(":/img/menu_export.png"),
		tr("Lektion &exportieren"), this);
	lessonDownload = new QAction(QIcon(":/img/menu_download.png"),
		tr("Lektion &downloaden"), this);
	lessonEdit = new QAction(QIcon(":/img/menu_edit.png"),
		tr("Lektion &editieren"), this);
	lessonDel = new QAction(QIcon(":/img/menu_del.png"),
		tr("Lektion &loeschen"), this);
	//lessonPublish = new QAction(QIcon(":/img/menu_help.png"),
	//	tr("Hilfe"), this);

	lessonEditMenu->addAction(lessonNew);
	lessonEditMenu->addAction(lessonEdit);
	lessonEditMenu->addAction(lessonDel);
	lessonEditMenu->addSeparator();
	lessonEditMenu->addAction(lessonImport);

    lessonEditMenu->addAction(lessonExport);
    //lessonEditMenu->addAction(lessonPublish);

	buttonEditLesson->setMenu(lessonEditMenu);
	tabOwnLessons = new QWidget();
	QHBoxLayout *tabHLayout = new QHBoxLayout;
	tabHLayout->addStretch(1);
	tabHLayout->addWidget(buttonEditLesson);
	QVBoxLayout *tabLayout = new QVBoxLayout;
	tabLayout->addLayout(tabHLayout);
	tabLayout->addWidget(listOwn);
	tabOwnLessons->setLayout(tabLayout);

	tabLessons->addTab(tabOwnLessons, QIcon(":/img/tab_own.png"), "");
}

void StartWidget::createGroupLimit() {
	// Group "Duration of lesson"
	groupLimit = new QGroupBox(tr("Dauer der Lektion"));

	// Radiobutton "Time limit"
	radioLimitTime = new QRadioButton(tr("Zeitlimit:"));
	radioLimitTime->setToolTip(tr("Das Diktat wird nach Ablauf "
		"einer\nfestgelegten Zeit beendet"));

	// Spin textbox "Time limit" with values from 2 to 20 minutes
	spinLimitTime = new QSpinBox();
	spinLimitTime->setMinimum(2);
	spinLimitTime->setMaximum(20);
	spinLimitTime->setSuffix(tr(" Minuten"));
	spinLimitTime->setToolTip(tr("Das Diktat wird nach Ablauf einer\n"
		"festgelegten Zeit beendet"));

	//Radiobutton "Number of tokens"
	radioLimitToken = new QRadioButton(tr("Zeichenlimit:"));
	radioLimitToken->setToolTip(tr("Das Diktat wird nach einer "
		"festgelegten Anzahl\nkorrekt getippter Zeichen beendet"));

	// Spin testbox "Number of tokens" with values from 200 to 2000 tokens
	spinLimitToken = new QSpinBox();
	spinLimitToken->setMinimum(200);
	spinLimitToken->setMaximum(2000);
	spinLimitToken->setSingleStep(100);
	spinLimitToken->setSuffix(tr(" Zeichen"));
	spinLimitToken->setToolTip(tr("Das Diktat wird nach einer "
		"bestimmten Anzahl\nkorrekt getippter Zeichen beendet"));

	//Radiobutton "To end of lesson"
	radioLimitLesson = new QRadioButton(tr("Gesamte\nLektion"));
	radioLimitLesson->setToolTip(tr("Es wird die gesamte (freie) Lektion\n"
		"von Anfang bis Ende diktiert"));
	labelLimitLesson = new QLabel(tr("(gesamte Lektion)"));


	// Layout of group box
	QVBoxLayout *timeLayout = new QVBoxLayout;
	timeLayout->addStretch(1);
	timeLayout->addWidget(radioLimitTime);
	timeLayout->addWidget(spinLimitTime);
	timeLayout->addStretch(1);
	QVBoxLayout *tokenLayout = new QVBoxLayout;
	tokenLayout->addStretch(1);
	tokenLayout->addWidget(radioLimitToken);
	tokenLayout->addWidget(spinLimitToken);
	tokenLayout->addStretch(1);
	QVBoxLayout *lessonLayout = new QVBoxLayout;
	lessonLayout->addStretch(1);
	lessonLayout->addWidget(radioLimitLesson);
    lessonLayout->addSpacing(24);
	lessonLayout->addStretch(1);
	QHBoxLayout *groupLayout = new QHBoxLayout;
    groupLayout->addLayout(timeLayout);
    groupLayout->addSpacing(20);
	groupLayout->addLayout(tokenLayout);
    groupLayout->addSpacing(20);
	groupLayout->addLayout(lessonLayout);
    groupLayout->setMargin(10);
	groupLimit->setLayout(groupLayout);
}

void StartWidget::createGroupError() {
	// Group "Type error handling"
	groupError = new QGroupBox(tr("Reaktion auf Tippfehler"));

	// Checkbox "Block type errors"
	checkErrorStop = new QCheckBox(tr("Diktat blockieren"));
	checkErrorStop->setToolTip(tr("Das Diktat wird erst fortgesetzt, "
		"wenn die\nrichtige Taste gedrueckt wurde"));

	// Checkbox "Correct type errors"
	checkErrorCorrect = new QCheckBox(tr("Fehler korrigieren"));
	checkErrorCorrect->setToolTip(tr("Tippfehler muessen zusaetzlich ueber die\n"
        "Ruecklauftaste entfernt werden"));

    // Checkbox "Beep on type errors"
    checkErrorBeep = new QCheckBox(tr("Akustisches Signal"));
    checkErrorBeep->setToolTip(tr("Bei jedem Tippfehler ertoent ein "
        "Beepton"));

    // Checkbox "Image on type errors"
    checkErrorImage = new QCheckBox(tr("Sinnbild einblenden"));
    checkErrorImage->setToolTip(tr("Bei jedem Tippfehler wird das "
        "zur Taste entsprechende Sinnbild angezeigt"));

    QFont h2;
	#if APP_MAC
    h2.setPointSize(11);
	labelIntelligence = new QLabel(tr("*Die Texte der Lektion "
		"werden nicht in ihrer vorgesehenen Reihenfolge diktiert, "
		"sondern in Echtzeit an die Tippfehler angepasst."));
    #else
    h2.setPointSize(7);
	labelIntelligence = new QLabel(tr("*Aktivieren Sie diese Option, "
		"wenn die Texte der Lektion nicht in ihrer vorgesehenen Reihenfolge diktiert, "
		"sondern in Echtzeit an die Tippfehler angepasst werden sollen."));
	#endif
	labelIntelligence->setWordWrap(true);
    labelIntelligence->setFont(h2);
	// Checkbox "Intelligence"
	checkIntelligence = new QCheckBox(tr("Intelligenz*"));
	checkIntelligence->setToolTip(tr("Anhand der aktuellen Fehlerquoten aller Schriftzeichen "
        "werden\ndie Worte und Saetze des Diktats in Echtzeit ausgewaehlt.\nIst die Intelligenz dagegen "
		"deaktiviert, werden die Texte der Lektion\nstets in der gleichen Reihenfolge diktiert."));

    // Layout of group box
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addSpacing(12);
    hlayout->addWidget(checkErrorCorrect);
    QHBoxLayout *h2layout = new QHBoxLayout;
    h2layout->addSpacing(12);
    h2layout->addWidget(checkErrorImage);
	QVBoxLayout *layout = new QVBoxLayout;
    layout->addStretch(1);
    layout->addWidget(checkErrorStop);
    layout->addLayout(hlayout);
    layout->addSpacing(10);
    layout->addStretch(1);
	layout->addWidget(checkErrorBeep);
    layout->addSpacing(10);
    layout->addStretch(1);
    layout->addWidget(checkIntelligence);
	layout->addWidget(labelIntelligence);
    layout->addStretch(1);
    //layout->addSpacing(10);
	layout->setMargin(10);
    // Pass layout to parent widget (group box)
	groupError->setLayout(layout);
}

void StartWidget::createGroupSupport() {
	// Group "Other user support"
	groupKeyboard = new QGroupBox(tr("Hilfestellungen"));

	checkHelpers = new QCheckBox(tr("Tastatur anzeigen"));
	checkHelpers->setToolTip(tr("Zur visuellen Unterstuetzung "
			"werden die virtuelle\nTastatur und Statusinformationen angezeigt"));

	// Checkbox "Enable color selection of keys"
	checkKeySelection = new QCheckBox(tr("Tasten farbig markieren"));
	checkKeySelection->setToolTip(tr("Zur visuellen Unterstuetzung "
		"werden die zu\ndrueckenden Tasten farbig markiert"));

	// Checkbox "Enable color selection of start keys"
	checkKeySelectionStart = new QCheckBox(tr("Grundstellung markieren"));
	checkKeySelectionStart->setToolTip(tr("Zur visuellen Unterstuetzung "
		"werden die\nverbleibenden Finger der Grundstellung\nfarbig markiert"));

	// Checkbox "Enable status information"
	checkKeyBorder = new QCheckBox(tr("Trennlinie anzeigen"));
	checkKeyBorder->setToolTip(tr("Zur visuellen Unterstuetzung "
		"wird eine Trennlinie\nfuer die zu Unterscheidung von linker und rechter Hand\n"
		"angezeigt"));

	// Checkbox "Enable status information"
	checkStatusInformation = new QCheckBox(tr("Hilfetext anzeigen"));
	checkStatusInformation->setToolTip(tr("Zur Unterstuetzung werden "
		"die zu verwendenden\nFinger in der Statusleiste angezeigt"));

	// Checkbox "Enable key path"
	checkKeyPath = new QCheckBox(tr("Tastpfade anzeigen"));
	checkKeyPath->setToolTip(tr("Zur Unterstuetzung werden "
		"die Tastpfade der\nFinger auf der Tastatur angezeigt"));

	// Layout of group box
	QHBoxLayout *helpLayout = new QHBoxLayout;
	helpLayout->addWidget(checkHelpers);
	QHBoxLayout *sLayout = new QHBoxLayout;
    sLayout->addSpacing(12);
	sLayout->addWidget(checkKeySelection);
	QHBoxLayout *ssLayout = new QHBoxLayout;
    ssLayout->addSpacing(12);
	ssLayout->addWidget(checkKeySelectionStart);
	QHBoxLayout *bLayout = new QHBoxLayout;
    bLayout->addSpacing(12);
	bLayout->addWidget(checkKeyBorder);
    QHBoxLayout *siLayout = new QHBoxLayout;
	siLayout->addWidget(checkStatusInformation);
	QHBoxLayout *paLayout = new QHBoxLayout;
    paLayout->addSpacing(12);
	paLayout->addWidget(checkKeyPath);
	QVBoxLayout *selectionLayout = new QVBoxLayout;
    selectionLayout->addStretch(1);
    selectionLayout->addSpacing(4);
	selectionLayout->addLayout(helpLayout);
    selectionLayout->addSpacing(4);
    selectionLayout->addStretch(1);
	selectionLayout->addLayout(sLayout);
    selectionLayout->addSpacing(4);
    selectionLayout->addStretch(1);
	selectionLayout->addLayout(ssLayout);
    selectionLayout->addSpacing(4);
    selectionLayout->addStretch(1);
	selectionLayout->addLayout(paLayout);
    selectionLayout->addSpacing(4);
    selectionLayout->addStretch(1);
	selectionLayout->addLayout(bLayout);
    selectionLayout->addSpacing(4);
    selectionLayout->addStretch(1);
	selectionLayout->addLayout(siLayout);
    selectionLayout->addSpacing(4);
    selectionLayout->addStretch(1);
	selectionLayout->setMargin(10);
    // Pass layout to parent widget (group box)
	groupKeyboard->setLayout(selectionLayout);
}

void StartWidget::createButtons() {
	//Button "Training starten"
	buttonHelp = new QPushButton(tr("&Hilfe"));
	buttonExit = new QPushButton(tr("&Beenden"));
	buttonTraining = new QPushButton(tr("&Schreibtraining starten"));
    buttonTraining->setDefault(true);
}

void StartWidget::createLayout() {
	// Button layout horizontal
	QHBoxLayout *buttonLayoutHorizontal = new QHBoxLayout;
    buttonLayoutHorizontal->addWidget(buttonExit);
    buttonLayoutHorizontal->addSpacing(10);
    buttonLayoutHorizontal->addWidget(buttonHelp);
    buttonLayoutHorizontal->addWidget(buttonTraining);
	// Button layout vertical
	QVBoxLayout *buttonLayoutVertical = new QVBoxLayout;
    buttonLayoutVertical->addSpacing(20);
    buttonLayoutVertical->addLayout(buttonLayoutHorizontal);
	// Logo layout vertical
	QVBoxLayout *logoLayout = new QVBoxLayout;
    logoLayout->addSpacing(10);
    logoLayout->addWidget(companyLogo);
	// Logo layout + button layout horizontal
	QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addLayout(logoLayout);
    bottomLayout->addStretch(1);
    bottomLayout->addLayout(buttonLayoutVertical);
    // Group error handling and other user support vertical
	/*QVBoxLayout *bottomboxesLayoutV = new QVBoxLayout;
    bottomboxesLayoutV->addWidget(groupError);
    bottomboxesLayoutV->addWidget(groupIntelligence);*/
    // Group error handling and other user support horizontal
	QHBoxLayout *bottomboxesLayout = new QHBoxLayout;
    bottomboxesLayout->addWidget(groupError);
    bottomboxesLayout->addWidget(groupKeyboard);
    // Group duration and layout above vertical
	QVBoxLayout *boxesLayout = new QVBoxLayout;
    boxesLayout->addWidget(groupLimit);
    boxesLayout->addLayout(bottomboxesLayout);
    // Layout all groups, buttons and logo horizontal
	QHBoxLayout *centerLayout = new QHBoxLayout;
    centerLayout->addWidget(tabLessons);
    centerLayout->addLayout(boxesLayout);
	// Full layout of all widgets vertical
	QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(centerLayout);
    mainLayout->addSpacing(1);
    mainLayout->addLayout(bottomLayout);
    mainLayout->setMargin(15);
    mainLayout->setSpacing(15);
    // Pass layout to parent widget (this)
	this->setLayout(mainLayout);
}

void StartWidget::createConnections() {
	// Widget connections
    connect(buttonTraining, SIGNAL(clicked()), this, SLOT(clickTraining()));
    connect(buttonExit, SIGNAL(clicked()), qApp, SLOT(quit()));
    connect(buttonHelp, SIGNAL(clicked()), this, SLOT(showHelp()));

	connect(radioLimitTime, SIGNAL(clicked()), this, SLOT(toggleLimit()));
	connect(radioLimitToken, SIGNAL(clicked()), this, SLOT(toggleLimit()));
	connect(radioLimitLesson, SIGNAL(clicked()), this, SLOT(toggleLimit()));
	connect(checkErrorStop, SIGNAL(clicked()), this, SLOT(toggleError()));
	connect(checkHelpers, SIGNAL(clicked()), this, SLOT(toggleHelpers()));
    connect(checkIntelligence, SIGNAL(clicked()), this, SLOT(toggleIntelligence()));

	connect(listLesson, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this,
		SLOT(doubleClickLesson(QListWidgetItem*)));
	connect(listOpen, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this,
		SLOT(doubleClickLesson(QListWidgetItem*)));
	connect(listOwn, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this,
		SLOT(doubleClickLesson(QListWidgetItem*)));

	connect(lessonNew, SIGNAL(triggered()), this, SLOT(clickNewLesson()));
	connect(lessonImport, SIGNAL(triggered()), this, SLOT(clickImportLesson()));
	connect(lessonExport, SIGNAL(triggered()), this, SLOT(clickExportLesson()));
#ifdef ONLINE
    connect(lessonDownload, SIGNAL(triggered()), this, SLOT(clickDownloadLesson()));
#endif
	connect(lessonDel, SIGNAL(triggered()), this, SLOT(clickDeleteLesson()));
	connect(lessonEdit, SIGNAL(triggered()), this, SLOT(clickEditLesson()));

	connect(tabLessons, SIGNAL(currentChanged(int)), this, SLOT(toggleTabs(int)));
	connect(comboTheme, SIGNAL(activated(int)), this, SLOT(toggleThemes(int)));
}

void StartWidget::fillLessonList(bool themeChanged = false) {
	int trainingNum = 0;
	int openNum = 0;
	int ownNum = 0;
	int themesNum = 0;

	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
    settings.beginGroup("general");
    QString languageLesson = settings.value("language_lesson",
        APP_STD_LANGUAGE_LESSON).toString();
    settings.endGroup();

	StartSql *lessonSql = new StartSql();
	// Training lesson list
	// --------------------
    if ((trainingNum = lessonSql->fillLessonList(listLesson, &arrayTraining, languageLesson)) == -1) {
		// Error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_LESSONS_EXIST, TYPE_CRITICAL,
			CANCEL_OPERATION);
		return;
	}
	// Preselection
	settings.beginGroup("lesson");
	int selectTraining = arrayTraining.indexOf(settings.value("list_training","").toString());
	if (selectTraining == -1) {
		listLesson->setCurrentRow(0);
	} else {
		listLesson->setCurrentRow(selectTraining);
	}
	settings.endGroup();
	// Open themes list
	// --------------------
    if ((themesNum = lessonSql->fillThemes(comboTheme, &arrayThemes,
                                           languageLesson, tr("Alle"))) == -1) {
		// Error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_LESSONS_EXIST, TYPE_CRITICAL,
			CANCEL_OPERATION);
		return;
	}
	// Preselection
	settings.beginGroup("lesson");
	int selectTheme = arrayThemes.indexOf(settings.value("combo_theme","").toString());
	if (selectTheme == -1) {
		comboTheme->setCurrentIndex(0);
	} else {
		comboTheme->setCurrentIndex(selectTheme);
	}
	// Hide Theme-Combo if only one ("all") theme exist
	if (comboTheme->count() <= 1) {
		labelTheme->setVisible(false);
		comboTheme->setVisible(false);
	} else {
		labelTheme->setVisible(true);
		comboTheme->setVisible(true);
	}
	settings.endGroup();
	// Open lesson list
	// --------------------
	if ((openNum = lessonSql->fillOpenList(listOpen, &arrayOpen,
        arrayThemes.at(comboTheme->currentIndex()), languageLesson)) == -1) {
		// Error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_LESSONS_EXIST, TYPE_CRITICAL,
			CANCEL_OPERATION);
		return;
	}
	// Preselection
	settings.beginGroup("lesson");
	int selectOpen = arrayOpen.indexOf(settings.value("list_open","").toString());
	if (selectOpen == -1) {
		listOpen->setCurrentRow(0);
	} else {
		listOpen->setCurrentRow(selectOpen);
	}
	settings.endGroup();
	// Own lesson list
	// --------------------
    if ((ownNum = lessonSql->fillOwnList(listOwn, &arrayOwn)) == -1) {
		// Error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_LESSONS_EXIST, TYPE_CRITICAL,
			CANCEL_OPERATION);
		return;
	}
	// Preselection
	settings.beginGroup("lesson");
	int selectOwn = arrayOwn.indexOf(settings.value("list_own","").toString());
	if (selectOwn == -1) {
		listOwn->setCurrentRow(0);
	} else {
		listOwn->setCurrentRow(selectOwn);
	}
	settings.endGroup();
	if (listOwn->count() == 0) {
        lessonEdit->setEnabled(false);
        lessonDel->setEnabled(false);
        lessonExport->setEnabled(false);
	} else {
		buttonTraining->setEnabled(true);
        lessonEdit->setEnabled(true);
        lessonDel->setEnabled(true);
        lessonExport->setEnabled(true);
	}
}

void StartWidget::toggleLimit() {
	// Check radio buttons in group "Duration of lesson"
	if (radioLimitTime->isChecked()) {
		// "Time limit" selected
		spinLimitTime->setEnabled(true);
		spinLimitToken->setEnabled(false);
	} else {
		if (radioLimitToken->isChecked()) {
			// "Token limit" selected
			spinLimitTime->setEnabled(false);
			spinLimitToken->setEnabled(true);
		} else {
			spinLimitTime->setEnabled(false);
			spinLimitToken->setEnabled(false);
		}
	}
}

void StartWidget::toggleError() {
	// Check check box in group "Type error handling"
	if (checkErrorStop->isChecked()) {
        checkErrorCorrect->setEnabled(true);
	} else {
		checkErrorCorrect->setChecked(false);
        checkErrorCorrect->setEnabled(false);
	}
}

void StartWidget::toggleHelpers() {
	// Check check box in group "Type error handling"
	if (checkHelpers->isChecked()) {
		checkKeySelection->setEnabled(true);
		checkKeySelection->setChecked(true);
		checkKeySelectionStart->setEnabled(true);
		checkKeySelectionStart->setChecked(true);
		checkKeyBorder->setEnabled(true);
		checkKeyBorder->setChecked(true);
        //checkStatusInformation->setEnabled(true);
        //checkStatusInformation->setChecked(true);
		checkKeyPath->setEnabled(true);
		checkKeyPath->setChecked(true);
	} else {
		checkKeySelection->setEnabled(false);
		checkKeySelection->setChecked(false);
		checkKeySelectionStart->setEnabled(false);
		checkKeySelectionStart->setChecked(false);
		checkKeyBorder->setEnabled(false);
		checkKeyBorder->setChecked(false);
        //checkStatusInformation->setEnabled(false);
        //checkStatusInformation->setChecked(false);
		checkKeyPath->setEnabled(false);
		checkKeyPath->setChecked(false);
	}
}

void StartWidget::toggleIntelligence() {
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
	// Check check box in group "Type error handling"
	if (checkIntelligence->isChecked()) {
		radioLimitLesson->setVisible(false);
		if (radioLimitLesson->isChecked()) {
			radioLimitTime->setChecked(true);
			spinLimitTime->setEnabled(true);
		}
	} else {
		radioLimitLesson->setVisible(true);
		if (toggleLimitLesson) {
			radioLimitLesson->setChecked(true);
			spinLimitTime->setEnabled(false);
			spinLimitToken->setEnabled(false);
		}
	}
	settings.beginGroup("intelligence");
	settings.setValue("check_intelligence", checkIntelligence->isChecked());
	settings.endGroup();
}

void StartWidget::toggleThemes(int index){
    #if APP_PORTABLE
    QSettings settings(QCoreApplication::applicationDirPath() +
        "/portable/settings.ini", QSettings::IniFormat);
    #else
    QSettings settings;
    #endif
    settings.beginGroup("general");
    QString languageLesson = settings.value("language_lesson",
        APP_STD_LANGUAGE_LESSON).toString();
    settings.endGroup();
	StartSql *lessonSql = new StartSql();
	// Open lesson list
	// --------------------
	if (lessonSql->fillOpenList(listOpen, &arrayOpen,
        arrayThemes.at(index), languageLesson) == -1) {
		// Error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_LESSONS_EXIST, TYPE_CRITICAL,
			CANCEL_OPERATION);
		return;
	}
	// Preselection
	listOpen->setCurrentRow(0);
}

void StartWidget::toggleTabs(int index) {
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
	tabLessons->setTabText(0, "");
	tabLessons->setTabText(1, "");
	tabLessons->setTabText(2, "");
	switch (index) {
		case 0:
			tabLessons->setTabText(0, tr("Uebungslektionen"));
			buttonTraining->setEnabled(true);
            checkIntelligence->setChecked(true);
			checkIntelligence->setEnabled(false);
			labelIntelligence->setEnabled(false);
			radioLimitLesson->setVisible(false);
			if (radioLimitLesson->isChecked()) {
				radioLimitTime->setChecked(true);
				spinLimitTime->setEnabled(true);
			}
            break;
		case 1:
			tabLessons->setTabText(1, tr("Freie Lektionen"));
			buttonTraining->setEnabled(true);
            checkIntelligence->setEnabled(true);
			labelIntelligence->setEnabled(true);
			settings.beginGroup("intelligence");
			checkIntelligence->setChecked(settings.value("check_intelligence", false).toBool());
			settings.endGroup();
			// Enable/disable radioLimitLesson
			if (checkIntelligence->isChecked() == true) {
				radioLimitLesson->setVisible(false);
				if (radioLimitLesson->isChecked()) {
					radioLimitTime->setChecked(true);
					spinLimitTime->setEnabled(true);
				}
			} else {
				radioLimitLesson->setVisible(true);
				if (toggleLimitLesson) {
					radioLimitLesson->setChecked(true);
					spinLimitTime->setEnabled(false);
					spinLimitToken->setEnabled(false);
				}
			}
            settings.beginGroup("general");
            if (openLessonWarning &&
                settings.value("language_lesson",
                         APP_STD_LANGUAGE_LESSON).toString() == "en_us_qwerty") {

                QMessageBox::information(this, APP_NAME,
                        tr("Derzeit gibt es die freien Lektionen nur in deutscher"
                                   " Sprache. Wir arbeiten daran, bald auch freie Lektionen "
                                   "in englischer Sprache anbieten zu koennen."
                        ));

                openLessonWarning = false;
                settings.setValue("check_open_lesson_warning", openLessonWarning);

            }
            settings.endGroup();
            break;
		case 2:
			tabLessons->setTabText(2, tr("Eigene Lektionen"));
			if (listOwn->count() == 0) {
				buttonTraining->setEnabled(false);
                lessonEdit->setEnabled(false);
                lessonDel->setEnabled(false);
                lessonExport->setEnabled(false);
			} else {
				buttonTraining->setEnabled(true);
                lessonEdit->setEnabled(true);
                lessonDel->setEnabled(true);
                lessonExport->setEnabled(true);
			}
            checkIntelligence->setEnabled(true);
			labelIntelligence->setEnabled(true);
			settings.beginGroup("intelligence");
			checkIntelligence->setChecked(settings.value("check_intelligence", false).toBool());
			settings.endGroup();
			// Enable/disable radioLimitLesson
			if (checkIntelligence->isChecked() == true) {
				radioLimitLesson->setVisible(false);
				if (radioLimitLesson->isChecked()) {
					radioLimitTime->setChecked(true);
					spinLimitTime->setEnabled(true);
				}
			} else {
				radioLimitLesson->setVisible(true);
				if (toggleLimitLesson) {
					radioLimitLesson->setChecked(true);
					spinLimitTime->setEnabled(false);
					spinLimitToken->setEnabled(false);
				}
			}
            break;
	}
}

void StartWidget::clickTraining() {
	// User finished setting the properties and wants to start the
	// training lesson
	int selectedLesson = -1;
	QString selectedName = "";
	int listSize = 0;
	int tabIndex = 0;
	switch (tabLessons->currentIndex()) {
		case 0:
			selectedLesson = arrayTraining.at(listLesson->currentRow()).toInt();
			listSize = arrayTraining.size();
			selectedName = listLesson->currentItem()->text();
			tabIndex = 0;
			break;
		case 1:
			selectedLesson = arrayOpen.at(listOpen->currentRow()).toInt();
			listSize = arrayOpen.size();
			selectedName = listOpen->currentItem()->text();
			tabIndex = 1;
			break;
		case 2:
			selectedLesson = arrayOwn.at(listOwn->currentRow()).toInt();
			listSize = arrayOwn.size();
			selectedName = listOwn->currentItem()->text();
			tabIndex = 2;
			break;
	}
	if (selectedLesson == -1 || listSize == 0) {
		// No selected lesson found in combo box
		// -> error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_LESSONS_SELECTED, TYPE_INFO,
			CANCEL_OPERATION);
		return;
    }
	// -> emit the signal that mainwindow knows user wants to start training
	emit trainingClicked(selectedLesson, tabIndex, selectedName);
}

void StartWidget::doubleClickLesson(QListWidgetItem *item) {
	clickTraining();
}

void StartWidget::clickNewLesson() {
	QStringList nullList;

	LessonDialog lessonDialog("-1", &nullList, this);
	if (lessonDialog.exec() != 0) {
		// Fill lesson list after changing lessons
		StartSql *lessonSql = new StartSql();
		// Own lesson list
		// ---------------
		if (lessonSql->fillOwnList(listOwn, &arrayOwn) == -1) {
			// Error message
			ErrorMessage *errorMessage = new ErrorMessage(this);
			errorMessage->showMessage(ERR_LESSONS_EXIST, TYPE_CRITICAL,
				CANCEL_OPERATION);
			return;
		}
		// Preselection
		listOwn->setCurrentRow(0);
		if (listOwn->count() == 0) {
			buttonTraining->setEnabled(false);
            lessonEdit->setEnabled(false);
            lessonDel->setEnabled(false);
            lessonExport->setEnabled(false);
		} else {
			buttonTraining->setEnabled(true);
            lessonEdit->setEnabled(true);
            lessonDel->setEnabled(true);
            lessonExport->setEnabled(true);
		}
	}
}

void StartWidget::clickImportLesson() {


	// Show file read dialog
	QFileDialog *fd = new QFileDialog(this);
	fd->setFileMode(QFileDialog::ExistingFile);
	fd->setViewMode(QFileDialog::Detail);

	QString path = fd->getOpenFileName(this,
		tr("Bitte waehlen Sie eine Textdatei aus"),
		QDir::homePath(), tr("Textdateien (*.txt)"));

	// Cancel pressed or no file selected
	if (path == "") {
		return;
	}

	QFile file(path);

	// Can't open file
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		// Error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_USER_IMPORT_READ, TYPE_INFO,
			CANCEL_OPERATION);
		return;
	}

	QStringList *lessonData = new QStringList;

	lessonData->clear();

	lessonData->append(QFileInfo(file).baseName());

	QTextStream in(&file);
	while (!in.atEnd()) {
		lessonData->append(in.readLine());
	}

	// Check if file is empty
	// (< 2 because first line is the file name)
	if (lessonData->size() < 2) {
		// Error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_USER_IMPORT_EMPTY, TYPE_INFO,
			CANCEL_OPERATION);
		return;
	}

	LessonDialog lessonDialog("-2", lessonData, this);
	if (lessonDialog.exec() != 0) {

		// Fill lesson list after changing lessons
		StartSql *lessonSql = new StartSql();
		// Own lesson list
		// ---------------
		if (lessonSql->fillOwnList(listOwn, &arrayOwn) == -1) {
			// Error message
			ErrorMessage *errorMessage = new ErrorMessage(this);
			errorMessage->showMessage(ERR_LESSONS_EXIST, TYPE_CRITICAL,
				CANCEL_OPERATION);
			return;
		}
		// Preselection
		listOwn->setCurrentRow(0);
		if (listOwn->count() == 0) {
			buttonTraining->setEnabled(false);
            lessonEdit->setEnabled(false);
            lessonDel->setEnabled(false);
            lessonExport->setEnabled(false);
		} else {
			buttonTraining->setEnabled(true);
            lessonEdit->setEnabled(true);
            lessonDel->setEnabled(true);
            lessonExport->setEnabled(true);
		}
	}
}

void StartWidget::clickExportLesson() {


	QSqlQuery query;
	QString currentLessonId = arrayOwn.at(listOwn->currentRow());
	QString lessonName = "";
	QString lessonDescription = "";
	QString lessonContent = "";

	if (!query.exec("SELECT own_name, own_description, own_unit "
		"FROM own_list "
		"WHERE own_id = " + currentLessonId + ";")) {
		return;
	}
	if (query.first()) {
		lessonName = query.value(0).toString();
		lessonDescription = query.value(1).toString();
	}

	if (!query.exec("SELECT content_text "
		"FROM own_content "
		"WHERE content_lesson = " + currentLessonId + " "
		"ORDER BY content_id;")) {
		return;
	}
	// Read all datasets to list items
	while (query.next()) {
		// ID of the lesson
		lessonContent.append(query.value(0).toString() + "\n");
	}

	// Show file read dialog
	QFileDialog *fd = new QFileDialog(this);
	fd->setFileMode(QFileDialog::AnyFile);
	fd->setViewMode(QFileDialog::Detail);

	QString fileNameEncoded = "";
	QRegExp regexp("[A-Za-z0-9_-]*");
	if (regexp.indexIn(lessonName) > -1) {
		QStringList list = regexp.capturedTexts();
		QStringList::iterator it = list.begin();
		while (it != list.end()) {
			fileNameEncoded.append(*it);
			++it;
		}
		fileNameEncoded.append(".txt");
	} else {
		fileNameEncoded = "export.txt";
	}

	QString path = fd->getSaveFileName(this,
		tr("Bitte geben Sie den Speicherort fuer eine Textdatei ein"), fileNameEncoded);

	// Cancel pressed or no file selected
	if (path == "") {
		return;
	}

	QFile file(path);

	// Can't open file
	if (!file.open(QFile::WriteOnly | QIODevice::Text)) {
		// Error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_USER_EXPORT_WRITE, TYPE_INFO,
			CANCEL_OPERATION);
		return;
	}

	QTextStream out(&file);

	out << lessonContent;
}

#ifdef ONLINE
void StartWidget::clickDownloadLesson() {

	QStringList lessonData;

	DownloadDialog downloadDialog(&lessonData, this);
	if (downloadDialog.exec() != 0) {

		LessonDialog lessonDialog("-2", &lessonData, this);
		if (lessonDialog.exec() != 0) {

			// Fill lesson list after changing lessons
			StartSql *lessonSql = new StartSql();
			// Own lesson list
			// ---------------
			if (lessonSql->fillOwnList(listOwn, &arrayOwn) == -1) {
				// Error message
				ErrorMessage *errorMessage = new ErrorMessage(this);
				errorMessage->showMessage(ERR_LESSONS_EXIST, TYPE_CRITICAL,
					CANCEL_OPERATION);
				return;
			}
			// Preselection
			listOwn->setCurrentRow(0);
			if (listOwn->count() == 0) {
				buttonTraining->setEnabled(false);
                lessonEdit->setEnabled(false);
                lessonDel->setEnabled(false);
                lessonExport->setEnabled(false);
			} else {
				buttonTraining->setEnabled(true);
                lessonEdit->setEnabled(true);
                lessonDel->setEnabled(true);
                lessonExport->setEnabled(true);
			}
		}
	}
}
#endif

void StartWidget::clickEditLesson() {

	int tempLesson = listOwn->currentRow();
	QStringList nullList;
	LessonDialog lessonDialog(arrayOwn.at(tempLesson),
		&nullList, this);
	if (lessonDialog.exec() != 0) {
		// Fill lesson list after changing lessons
		StartSql *lessonSql = new StartSql();
		// Own lesson list
		// ---------------
		if (lessonSql->fillOwnList(listOwn, &arrayOwn) == -1) {
			// Error message
			ErrorMessage *errorMessage = new ErrorMessage(this);
			errorMessage->showMessage(ERR_LESSONS_EXIST, TYPE_CRITICAL,
				CANCEL_OPERATION);
			return;
		}
		// Preselection
		listOwn->setCurrentRow(tempLesson);
	}
}

void StartWidget::clickDeleteLesson() {
	switch (QMessageBox::question(this, APP_NAME,
        tr("Wollen Sie die Lektion wirklich loeschen und damit auch "
        "alle aufgezeichneten Daten, die im Zusammenhang mit dieser Lektion stehen? "
		"\n\n"), tr("&Ja"), tr("&Abbrechen"),
		0, 1)) {
		case 0:
			StartSql *lessonSql = new StartSql();
			if (!lessonSql->deleteOwnLesson(arrayOwn.at(listOwn->currentRow()))) {
				// Error message
				ErrorMessage *errorMessage = new ErrorMessage(this);
				errorMessage->showMessage(ERR_LESSONS_EXIST, TYPE_CRITICAL,
					CANCEL_OPERATION);
				return;
			}
            if (!lessonSql->analyzeOwnLessons()) {
				// No selected lesson found in combo box
				// -> error message
				ErrorMessage *errorMessage = new ErrorMessage(this);
				errorMessage->showMessage(ERR_USER_LESSON_ANALYZE, TYPE_INFO,
					CANCEL_OPERATION);
				return;
			}
            // Fill lesson list after changing lessons
			if (lessonSql->fillOwnList(listOwn, &arrayOwn) == -1) {
				// Error message
				ErrorMessage *errorMessage = new ErrorMessage(this);
				errorMessage->showMessage(ERR_LESSONS_EXIST, TYPE_CRITICAL,
					CANCEL_OPERATION);
				return;
			}
			// Preselection
			listOwn->setCurrentRow(0);
			break;
	}
	if (listOwn->count() == 0) {
		buttonTraining->setEnabled(false);
        lessonEdit->setEnabled(false);
        lessonDel->setEnabled(false);
        lessonExport->setEnabled(false);
	} else {
		buttonTraining->setEnabled(true);
        lessonEdit->setEnabled(true);
        lessonDel->setEnabled(true);
        lessonExport->setEnabled(true);
	}
}

void StartWidget::showHelp() {
	helpBrowser = new HelpBrowser("", 0);
	helpBrowser->show();
}

void StartWidget::readLicenseSettings() {
	// Restores settings of the startwiget
	// (uses the default constructor of QSettings, passing
	// the application and company name see main function)
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif

	settings.beginGroup("general");

	lessonExportVisible = false;
    settings.endGroup();
}

void StartWidget::readSettings() {
	// Restores settings of the startwiget
	// (uses the default constructor of QSettings, passing
	// the application and company name see main function)
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif

	// Set current lesson tab
	settings.beginGroup("lesson");
	tabLessons->setCurrentIndex(settings.value("tab_current", 0).toInt());
	settings.endGroup();
	toggleTabs(tabLessons->currentIndex());

	settings.beginGroup("duration");
    radioLimitTime->setChecked(settings.value("radio_time", true).toBool());
	spinLimitTime->setEnabled(settings.value("radio_time", true).toBool());
	radioLimitLesson->setChecked(settings.value("radio_lesson", false).toBool());
    radioLimitToken->setChecked(settings.value("radio_token", false).toBool());
	spinLimitToken->setEnabled(settings.value("radio_token", false).toBool());
	spinLimitTime->setValue(settings.value("spin_time",
		LESSON_TIMELEN_STANDARD).toInt());
	spinLimitToken->setValue(settings.value("spin_token",
		LESSON_TOKENLEN_STANDARD).toInt());
	settings.endGroup();
	settings.beginGroup("error");
	checkErrorStop->setChecked(settings.value("check_stop", true).toBool());
	checkErrorCorrect->setChecked(settings.value("check_correct", false).toBool());
	checkErrorCorrect->setEnabled(settings.value("check_stop", true).toBool());
	checkErrorBeep->setChecked(settings.value("check_beep", false).toBool());
    settings.endGroup();
	settings.beginGroup("support");
	checkHelpers->setChecked(settings.value("check_helpers", true).toBool());
	checkKeySelection->setChecked(settings.value("check_selection", true).toBool());
	checkKeySelectionStart->setChecked(settings.value("check_selection_start", true).toBool());
	checkKeyBorder->setChecked(settings.value("check_border", true).toBool());
	checkStatusInformation->setChecked(settings.value("check_status", true).toBool());
	checkKeyPath->setChecked(settings.value("check_path", true).toBool());
	settings.endGroup();
	if (!checkHelpers->isChecked()) {
		toggleHelpers();
	}

	//!!!COMAK-Release
	if (tabLessons->currentIndex() != 0) {
		settings.beginGroup("intelligence");
		checkIntelligence->setChecked(settings.value("check_intelligence", false).toBool());
		settings.endGroup();
	} else {
        checkIntelligence->setChecked(true);
        checkIntelligence->setEnabled(false);
		labelIntelligence->setEnabled(false);
	}

    settings.beginGroup("general");
    openLessonWarning = settings.value("check_open_lesson_warning", true).toBool();
    toggleLimitLesson = settings.value("check_limit_lesson", true).toBool();
	checkLessonPublish = settings.value("check_lesson_publish", true).toBool();
	checkToggleIntelligence = settings.value("check_toggle_intelligence", true).toBool();
	checkTxtWarning = settings.value("check_txt_warning", true).toBool();
	settings.endGroup();

    // Enable/disable radioLimitLesson
	if (tabLessons->currentIndex() == 0) {
		radioLimitLesson->setVisible(false);
	} else {
		if (checkIntelligence->isChecked() == true) {
			radioLimitLesson->setVisible(false);
		} else {
			radioLimitLesson->setVisible(true);
		}
    }
}

void StartWidget::writeSettings() {
	// Saves settings of the startwiget
	// (uses the default constructor of QSettings, passing
	// the application and company name see main function)
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
	settings.beginGroup("lesson");
	settings.setValue("tab_current", tabLessons->currentIndex());
	if (listLesson->count() == 0) {
		settings.setValue("list_training", -1);
	} else {
		settings.setValue("list_training", arrayTraining.at(listLesson->currentRow()));
	}
	if (listOpen->count() == 0) {
		settings.setValue("list_open", -1);
	} else {
		settings.setValue("list_open", arrayOpen.at(listOpen->currentRow()));
	}
	if (comboTheme->count() == 0) {
		settings.setValue("combo_theme", -1);
	} else {
		settings.setValue("combo_theme", arrayThemes.at(comboTheme->currentIndex()));
	}
	if (listOwn->count() == 0) {
		settings.setValue("list_own", -1);
	} else {
		settings.setValue("list_own", arrayOwn.at(listOwn->currentRow()));
	}
	settings.endGroup();
	settings.beginGroup("duration");
	settings.setValue("radio_time", radioLimitTime->isChecked());
	settings.setValue("spin_time", spinLimitTime->value());
	settings.setValue("radio_token", radioLimitToken->isChecked());
	settings.setValue("spin_token", spinLimitToken->value());
	settings.setValue("radio_lesson", radioLimitLesson->isChecked());
	settings.endGroup();
	settings.beginGroup("error");
	settings.setValue("check_stop", checkErrorStop->isChecked());
	settings.setValue("check_correct", checkErrorCorrect->isChecked());
	settings.setValue("check_beep", checkErrorBeep->isChecked());
    settings.endGroup();
	settings.beginGroup("support");
	settings.setValue("check_helpers", checkHelpers->isChecked());
	settings.setValue("check_selection", checkKeySelection->isChecked());
	settings.setValue("check_selection_start", checkKeySelectionStart->isChecked());
	settings.setValue("check_border", checkKeyBorder->isChecked());
	settings.setValue("check_status", checkStatusInformation->isChecked());
	settings.setValue("check_path", checkKeyPath->isChecked());
	settings.endGroup();
    if (tabLessons->currentIndex() != 0) {
		settings.beginGroup("intelligence");
		settings.setValue("check_intelligence", checkIntelligence->isChecked());
		settings.endGroup();
	}
}
