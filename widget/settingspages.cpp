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
** Implementation of the TrainingPage, DatabasePage and
** OtherPage class
** File name: settingspages.cpp
**
****************************************************************/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSqlQuery>
#include <QFontDialog>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QCoreApplication>
#include <QVariant>
#include <QColor>
#include <QPalette>

#include "settingspages.h"
#include "def/defines.h"
#include "sql/startsql.h"
#include "def/errordefines.h"
#include "errormessage.h"
#include "regexpdialog.h"

TrainingPage::TrainingPage(QWidget *parent) : QWidget(parent) {
	// Default ticker font if there is no saved ticker font
	tickerFont = QFont(FONT_STANDARD, FONT_SIZE_TICKER);

	// Create group boxes with settings
    createGroupTickerFont();
	// Create group with sound output settings
    createGroupSoundOutput();

	// Set the layout of all widgets created above
	createLayout();

    // Widget connections
    connect(buttonSetFont, SIGNAL(clicked()), this, SLOT(setFont()));
    connect(buttonSetFontColor, SIGNAL(clicked()), this, SLOT(setFontColor()));
    connect(buttonSetBgColor, SIGNAL(clicked()), this, SLOT(setBgColor()));
    connect(buttonSetCursorColor, SIGNAL(clicked()), this, SLOT(setCursorColor()));
    connect(checkMetronome, SIGNAL(toggled(bool)), spinMetronomeClock, SLOT(setEnabled(bool)));

	// Read settings
	readSettings();
}

void TrainingPage::createGroupTickerFont() {
	// Group "Ticker"
    groupTickerFont = new QGroupBox(tr("Laufschrift"));

    // Button "Set ticker font"
    labelTickerFont = new QLabel(tr("Schriftart:"));
    buttonSetFont = new QPushButton(tr("&Schriftart aendern"));
    buttonSetFont->setToolTip(tr("Hier koennen Sie die Schriftart der "
        "Laufschrift veraendern\n(eine Schriftgroesse ueber 20 Punkte wird aus "
        "Formatierungs-\ngruenden nicht empfohlen)"));

    // Button "Font color"
    labelTickerFontColor = new QLabel(tr("Schriftfarbe:"));
    buttonSetFontColor = new QPushButton("");
    buttonSetFontColor->setToolTip(tr("Hier koennen Sie die Schriftfarbe der "
        "Laufschrift veraendern"));

    // Button "Background color"
    labelTickerBgColor = new QLabel(tr("Hintergrund:"));
    buttonSetBgColor = new QPushButton("");
    buttonSetBgColor->setToolTip(tr("Hier koennen Sie die Hintergrundfarbe der "
        "Laufschrift veraendern"));

    // Button "Cursor color"
    labelTickerCursorColor = new QLabel(tr("Cursor:"));
    buttonSetCursorColor = new QPushButton("");
    buttonSetCursorColor->setToolTip(tr("Hier koennen Sie die Farbe der Markierung "
                                        "fuer das aktuelle Zeichen veraendern"));

	// Spinbox "Set ticker speed"
	labelTickerSpeed = new QLabel(tr("Geschwindigkeit:"));
	labelTickerSpeedMax = new QLabel(tr("Schnell"));
	labelTickerSpeedMin = new QLabel(tr("Aus"));
	sliderTickerSpeed = new QSlider(Qt::Horizontal);
	sliderTickerSpeed->setMinimum(0);
	sliderTickerSpeed->setMaximum(4);
	sliderTickerSpeed->setToolTip(tr("Hier koennen Sie die "
		"Geschwindigkeit der Laufschrift veraendern\n(Der Regler ganz links bedeutet, das "
		"Laufband bewegt sich erst am Ende der Zeile.\n"
		"Der Regler ganz rechts entspricht einem schnellen Laufband.)"));

	// Layout of group box vertical
	QHBoxLayout *sliderlayout = new QHBoxLayout;
	sliderlayout->addWidget(labelTickerSpeedMin);
    sliderlayout->addSpacing(2);
	sliderlayout->addWidget(sliderTickerSpeed);
    sliderlayout->addSpacing(2);
    sliderlayout->addWidget(labelTickerSpeedMax);

    QVBoxLayout *fontlayout = new QVBoxLayout;
    fontlayout->addWidget(labelTickerFont);
    fontlayout->addWidget(buttonSetFont);

    QVBoxLayout *fontColorlayout = new QVBoxLayout;
    fontColorlayout->addWidget(labelTickerFontColor);
    fontColorlayout->addWidget(buttonSetFontColor);

    QVBoxLayout *bgColorlayout = new QVBoxLayout;
    bgColorlayout->addWidget(labelTickerBgColor);
    bgColorlayout->addWidget(buttonSetBgColor);

    QVBoxLayout *cursorColorlayout = new QVBoxLayout;
    cursorColorlayout->addWidget(labelTickerCursorColor);
    cursorColorlayout->addWidget(buttonSetCursorColor);

    QHBoxLayout *colorLayout = new QHBoxLayout;
    colorLayout->addLayout(fontlayout);
    colorLayout->addLayout(fontColorlayout);
    colorLayout->addLayout(bgColorlayout);
    colorLayout->addLayout(cursorColorlayout);

	QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(colorLayout);
    layout->addSpacing(10);
	layout->addWidget(labelTickerSpeed);
	layout->addLayout(sliderlayout);
	//layout->addStretch(1);
	layout->setMargin(16);
	groupTickerFont->setLayout(layout);
}

void TrainingPage::createGroupSoundOutput() {
	// Group "Ticker"
	groupSoundOutput = new QGroupBox(tr("Audioausgabe"));

	// Radiobutton Soundcard output
	radioSoundcard = new QRadioButton(tr("Soundkarte"));
	radioSoundcard->setToolTip(tr("Waehlen Sie diese Option, wenn Sie "
		"die Soundausgabe ueber die Soundkarte erfolgen soll"));

	// Radiobutton PC speaker output
	radioSpeaker = new QRadioButton(tr("PC-Lautsprecher"));
	radioSpeaker->setToolTip(tr("Waehlen Sie diese Option, wenn Sie "
		"die Soundausgabe ueber den PC-Lautsprecher erfolgen soll"));

    checkMetronome = new QCheckBox(tr("Metronom:"));
    checkMetronome->setToolTip(tr("Hier koennen Sie ein Metronom waehrend "
                                  "des Schreibtrainings aktivieren"));

    spinMetronomeClock = new QSpinBox();
    spinMetronomeClock->setMinimum(1);
    spinMetronomeClock->setMaximum(200);
    spinMetronomeClock->setSingleStep(1);
    spinMetronomeClock->setSuffix(tr(" A/min"));
    spinMetronomeClock->setToolTip(tr("Geben Sie hier an, wie oft pro "
                                      "Minute ein akustisches Signal ausgegeben "
                                      "werden soll"));

    // Layout of group box vertical
    QHBoxLayout *metronome = new QHBoxLayout;
    metronome->addWidget(checkMetronome);
    metronome->addWidget(spinMetronomeClock);
    metronome->addStretch(1);
    // Layout of group box vertical
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(radioSoundcard);
    layout->addSpacing(1);
	layout->addWidget(radioSpeaker);
    layout->addSpacing(10);
    layout->addLayout(metronome);
	layout->setMargin(16);
	groupSoundOutput->setLayout(layout);
}

void TrainingPage::createLayout() {
    // Group layout vertical
	QVBoxLayout *boxesLayout = new QVBoxLayout;
    boxesLayout->addWidget(groupTickerFont);
    //boxesLayout->addWidget(groupKeyboardLayout);
    boxesLayout->addWidget(groupSoundOutput);
	// Full layout of all widgets vertical
	QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(boxesLayout);
    //mainLayout->setMargin(5);
    mainLayout->setSpacing(15);
    // Pass layout to parent widget (this)
	this->setLayout(mainLayout);
}

void TrainingPage::setFont() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, tickerFont, this);
    if (ok) {
        tickerFont = font;
        setFontButtonLabel();
    }
}

void TrainingPage::setFontColor() {
    QColor color = QColorDialog::getColor(buttonSetFontColor->palette().background().color(), this);
    if (color.isValid()) {
        buttonSetFontColor->setPalette(QPalette(color));
    }
}

void TrainingPage::setBgColor() {
    QColor color = QColorDialog::getColor(buttonSetBgColor->palette().background().color(), this);
    if (color.isValid()) {
        buttonSetBgColor->setPalette(QPalette(color));
    }
}

void TrainingPage::setCursorColor() {
    QColor color = QColorDialog::getColor(buttonSetCursorColor->palette().background().color(), this);
    if (color.isValid()) {
        buttonSetCursorColor->setPalette(QPalette(color));
    }
}

void TrainingPage::setFontButtonLabel() {
    buttonSetFont->setText((tickerFont.family().length() > 8 ? tickerFont.family().left(6) : tickerFont.family()) +
                           (tickerFont.family().length() > 8 ? "..." : "") +
                           ", "
		+ QString::number(tickerFont.pointSize()));
}

void TrainingPage::readSettings() {
	QString dbNameUser = APP_USER_DB;
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
    settings.beginGroup("settings");
    buttonSetFontColor->setPalette(QPalette(QColor(settings.value("ticker_font_color",
                                                                  QString(TICKER_COLOR_FONT)).toString())));
    buttonSetBgColor->setPalette(QPalette(QColor(settings.value("ticker_bg_color",
                                                                  QString(TICKER_COLOR_BG)).toString())));
    buttonSetCursorColor->setPalette(QPalette(QColor(settings.value("ticker_cursor_color",
                                                                  QString(TICKER_COLOR_CURSOR)).toString())));
	tickerFont.fromString(settings.value("ticker_font",
		tickerFont.toString()).toString());
	sliderTickerSpeed->setValue(settings.value("ticker_speed",
		TICKERSPEED_STANDARD).toInt());
	settings.endGroup();
	settings.beginGroup("sound");
	#if APP_X11
	bool beepDefaultSoundcard = false;
	bool beepDefaultSpeaker = true;
	#else
	bool beepDefaultSoundcard = true;
	bool beepDefaultSpeaker = false;
	#endif
    radioSoundcard->setChecked(settings.value("radio_soundcard", beepDefaultSoundcard).toBool());
    radioSpeaker->setChecked(settings.value("radio_speaker", beepDefaultSpeaker).toBool());
    checkMetronome->setChecked(settings.value("check_metronome", false).toBool());
    spinMetronomeClock->setValue(settings.value("spin_metronome", METRONOM_STANDARD).toInt());
    spinMetronomeClock->setEnabled(checkMetronome->isChecked());
    settings.endGroup();
	setFontButtonLabel();
}

void TrainingPage::writeSettings() {
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
    settings.beginGroup("settings");
    settings.setValue("ticker_font_color", buttonSetFontColor->palette().background().color().name());
    settings.setValue("ticker_bg_color", buttonSetBgColor->palette().background().color().name());
    settings.setValue("ticker_cursor_color", buttonSetCursorColor->palette().background().color().name());
    settings.setValue("ticker_font", tickerFont.toString());
	settings.setValue("ticker_speed", sliderTickerSpeed->value());
	settings.endGroup();
	settings.beginGroup("sound");
    settings.setValue("radio_soundcard", radioSoundcard->isChecked());
    settings.setValue("radio_speaker", radioSpeaker->isChecked());
    settings.setValue("check_metronome", checkMetronome->isChecked());
    settings.setValue("spin_metronome", spinMetronomeClock->value());
	settings.endGroup();
}

//--------------------------------------------------------

DatabasePage::DatabasePage(QWidget *parent) : QWidget(parent) {

	// Create group boxes with settings
    createGroupUserReset();
    createGroupDatabase();

	// Set the layout of all widgets created above
	createLayout();

	// Widget connections
	connect(buttonLessonsReset, SIGNAL(clicked()), this, SLOT(deleteUserLessonList()));
	connect(buttonCharsReset, SIGNAL(clicked()), this, SLOT(deleteUserChars()));
	connect(buttonDatabasePath, SIGNAL(clicked()), this, SLOT(setDatabasePath()));

	// Read settings
	readSettings();
}

void DatabasePage::createGroupUserReset() {
	// Group "Keyboard layout"
	groupUserReset = new QGroupBox(tr("Benutzerdaten"));

	// Button "Reset user data"
	buttonLessonsReset = new QPushButton(tr("&Absolvierte Lektionen "
		"zuruecksetzen"));
	buttonLessonsReset->setToolTip(tr("Hier koennen Sie saemtliche "
        "gespeicherte Lektionendaten zuruecksetzen\n(die Lektionen werden so "
		"in den urspruenglichen Zustand, wie nach\nder Installation, "
		"versetzt)"));

	// Button "Reset user data"
	buttonCharsReset = new QPushButton(tr("&Aufgezeichnete "
		"Schriftzeichen zuruecksetzen"));
	buttonCharsReset->setToolTip(tr("Hier koennen Sie alle "
		"aufgezeichneten Tastendruecke und Tippfehler zuruecksetzen\n(die "
		"Schriftzeichen werden so in den urspruenglichen Zustand, "
		"wie nach der\nInstallation, versetzt)"));

	// Layout of group box
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(buttonLessonsReset);
	layout->addWidget(buttonCharsReset);
	//layout->addStretch(1);
	layout->setMargin(16);
	groupUserReset->setLayout(layout);
}

void DatabasePage::createGroupDatabase() {
	// Group "Keyboard layout"
	groupDatabase = new QGroupBox(tr("Speicherort"));

	labelDatabasePath = new QLabel(tr("Verzeichnispfad zur Datenbank") + " \"" +
		APP_USER_DB + "\":");
	lineDatabasePath = new QLineEdit();
	buttonDatabasePath = new QPushButton(tr("..."));
	buttonDatabasePath->setMaximumWidth(30);
    buttonDatabasePath->setFixedHeight(20);

	if (!APP_PORTABLE) {
		labelDatabaseExplain = new QLabel(tr("Wenn im angegebenen "
			"Verzeichnis keine Datei ") + "\"" +
			APP_USER_DB + "\"" + tr(" existiert, "
			"wird automatisch eine neue, leere Datenbank erzeugt."));
	} else {
		labelDatabaseExplain = new QLabel(tr("Der Pfad kann "
			"bei der portablen Version von ") +
			APP_NAME + tr(" nicht veraendert werden."));
		lineDatabasePath->setEnabled(false);
		buttonDatabasePath->setEnabled(false);
	}
    QFont h2;
    #if APP_MAC
    h2.setPointSize(11);
    #else
    h2.setPointSize(7);
    #endif
    labelDatabaseExplain->setFont(h2);
	labelDatabaseExplain->setWordWrap(true);

	// Layout of group box
	QHBoxLayout *hlayout = new QHBoxLayout;
	hlayout->addWidget(lineDatabasePath);
	hlayout->addWidget(buttonDatabasePath);
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(labelDatabasePath);
	layout->addLayout(hlayout);
	layout->addWidget(labelDatabaseExplain);
	//layout->addStretch(1);
	layout->setMargin(16);
	groupDatabase->setLayout(layout);
}

void DatabasePage::createLayout() {
	// Full layout of all widgets vertical
	QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(groupUserReset);
    mainLayout->addWidget(groupDatabase);
    mainLayout->setSpacing(15);
    // Pass layout to parent widget (this)
	this->setLayout(mainLayout);
}

void DatabasePage::setDatabasePath() {
	/*QFileDialog *fd = new QFileDialog(this);
	fd->setFileMode(QFileDialog::AnyFile);
	fd->setFilter("Datenbankdatei (*.db)");
	fd->setViewMode(QFileDialog::Detail);
	//fd->setLabelText("Bitte waehlen Sie eine Datenbankdatei aus...");
	fd->setDirectory(currentDatabasePath);
	if (fd->exec()) {
		//QString fileNames = fd->selectedFiles();
	}*/
	QString s = QFileDialog::getExistingDirectory(
		this,
		"Bitte waehlen Sie ein Verzeichnis aus...",
		lineDatabasePath->text());
		//QFileDialog::confirmOverwrite());
	if (s.size() != 0) {
		lineDatabasePath->setText(s);
	}
}

void DatabasePage::deleteUserLessonList() {
	QSqlQuery query;
	switch (QMessageBox::question(this, APP_NAME,
		tr("Es werden alle absolvierten Lektionen des aktuellen "
		"Benutzers geloescht\nund die Lektionenliste in den urpruenglichen "
		"Zustand versetzt!\n\nWollen Sie den Vorgang wirklich "
		"fortsetzen?\n\n"), tr("&Ja"), tr("&Abbrechen"),
		0, 1)) {
		case 0:
			StartSql *userSql = new StartSql();
			if (!userSql->deleteUserLessonList()) {
				// Error message
				ErrorMessage *errorMessage = new ErrorMessage(this);
				errorMessage->showMessage(ERR_USER_LESSONS_FLUSH,
					TYPE_WARNING, CANCEL_OPERATION);
				return;
			}
			QMessageBox::information(this, APP_NAME,
				tr("Die absolvierten Lektionen wurden erfolgreich "
				"zurueckgesetzt!\n"));
			break;
	}
}

void DatabasePage::deleteUserChars() {

	switch (QMessageBox::question(this, APP_NAME,
		tr("Es werden alle aufgezeichneten Schriftzeichen "
        "(Fehlerquoten) des "
        "aktuellen Benutzers geloescht und die Zeichenliste in den "
		"urpruenglichen "
		"Zustand versetzt!\n\nWollen Sie den Vorgang wirklich "
		"fortsetzen?\n\n"), tr("&Ja"), tr("&Abbrechen"),
		0, 1)) {
		case 0:
			StartSql *userSql = new StartSql();
			if (!userSql->deleteUserChars()) {
				// Error message
				ErrorMessage *errorMessage = new ErrorMessage(this);
				errorMessage->showMessage(ERR_USER_ERRORS_FLUSH,
					TYPE_WARNING, CANCEL_OPERATION);
				return;
			}
			QMessageBox::information(this, APP_NAME,
				tr("Die aufgezeichneten Schriftzeichen wurden erfolgreich "
				"zurueckgesetzt!\n"));
			break;
	}
}

void DatabasePage::readSettings() {
	QString dbNameUser = APP_USER_DB;
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
	settings.beginGroup("database");
	currentDatabasePath = settings.value("pathpro", "Fehler/" + dbNameUser).toString();
	if (currentDatabasePath.size() > dbNameUser.size() + 1) {
		lineDatabasePath->setText(currentDatabasePath.left(currentDatabasePath.size() - dbNameUser.size() - 1));
	}
	settings.endGroup();
}

void DatabasePage::writeSettings() {
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
	settings.beginGroup("database");
	settings.setValue("pathpro", lineDatabasePath->text() + "/" + QString::QString(APP_USER_DB));
	settings.endGroup();
}

//--------------------------------------------------------

OtherPage::OtherPage(QWidget *parent) : QWidget(parent) {

	// Create group with info window settings
    createGroupDialogCheck();

	// Create group with adaptation settings
    createGroupAdaptation();

	// Set the layout of all widgets created above
	createLayout();

	// Read settings
	readSettings();
}

void OtherPage::createGroupDialogCheck() {
	// Group "Ticker"
    groupDialogCheck = new QGroupBox(tr("Fenstereinstellungen"));

	// Check box "Show start window"
	checkIllustration = new QCheckBox(tr("Willkommenfenster beim "
		"Programmstart anzeigen"));
	checkIllustration->setToolTip(tr("Hier koennen Sie festlegen, "
		"ob ein Informationsfenster\nmit Ratschlaegen beim Start von ") +
		APP_NAME + tr(""
		"angezeigt werden soll"));

	// Check box "Show start window"
	checkTxtMessage = new QCheckBox(tr("Hinweis vor Im- und Export von Lektionen anzeigen"));
	checkTxtMessage->setToolTip(tr("Hier koennen Sie festlegen, "
		"ob ein Hinweis zu\nden Dateitypen vor dem Im- und Export "
		"angezeigt werden soll"));

	// Check box "Show intelligence warning"
	checkIntelligence = new QCheckBox(tr("Auf aktivierte Intelligenz "
		"beim Start einer freien oder\neigenen Lektion hinweisen"));
	checkIntelligence->setToolTip(tr("Hier koennen Sie festlegen, "
		"ob ein Hinweisfenster\nangezeigt werden soll, wenn eine freie "
		"oder eigene\nLektion mit aktivierter Intelligenz gestartet wird."));

	// Check box "Show info about publishing your own lesson"
	checkLessonPublish = new QCheckBox(tr("Vor dem Anlegen einer "
		"eigenen Lektion auf die\nVeroeffentlichungsmoeglichkeit als freie "
		"Lektion hinweisen"));
	//checkLessonPublish->setToolTip(tr("xxx")); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // Check box "Intelligence duration toggling"
    checkLimitLesson = new QCheckBox(tr("Dauer der Lektion "
        "automatisch auf \"Gesamte Lektion\"\numstellen, wenn die "
        "Intelligenz deaktiviert wird"));


    // Layout of group box vertical
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(checkIllustration);
    layout->addSpacing(8);
	layout->addWidget(checkIntelligence);
    layout->addSpacing(8);
    layout->addWidget(checkLimitLesson);
    layout->setMargin(16);
	groupDialogCheck->setLayout(layout);
}

void OtherPage::createGroupAdaptation() {
	// Group "Ticker"
    groupAdaptation = new QGroupBox(tr("Sonstiges"));

    checkNewVersion = new QCheckBox(tr("Einmal pro Woche beim Programmstart auf neue\nVersion pruefen"));

    checkNativeStyle = new QCheckBox(tr("Native Programmoberflaeche "
		"verwenden"));

	// Layout of group box vertical
	QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(checkNewVersion);
    layout->addSpacing(1);
    layout->addWidget(checkNativeStyle);
    layout->setMargin(16);
	groupAdaptation->setLayout(layout);
}

void OtherPage::createLayout() {
	// Full layout of all widgets vertical
	QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(groupDialogCheck);
    mainLayout->addWidget(groupAdaptation);
    mainLayout->setSpacing(15);
    // Pass layout to parent widget (this)
	this->setLayout(mainLayout);
}

void OtherPage::readSettings() {
    #if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
	settings.beginGroup("general");
	checkIllustration->setChecked(settings.value("check_illustration", true).toBool());
    checkIntelligence->setChecked(settings.value("check_toggle_intelligence", true).toBool());
	checkLimitLesson->setChecked(settings.value("check_limit_lesson", true).toBool());
	checkLessonPublish->setChecked(settings.value("check_lesson_publish", true).toBool());
    checkNewVersion->setChecked(settings.value("check_new_version", true).toBool());
    checkNativeStyle->setChecked(settings.value("check_native_style", false).toBool());
	settings.endGroup();
}

bool OtherPage::writeSettings() {

	bool requireRestart = false;

	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
	settings.beginGroup("general");
	if (checkNativeStyle->isChecked() !=
		settings.value("check_native_style", false).toBool()) {

		requireRestart = true;
	}
	settings.setValue("check_illustration", checkIllustration->isChecked());
	settings.setValue("check_toggle_intelligence", checkIntelligence->isChecked());
	settings.setValue("check_limit_lesson", checkLimitLesson->isChecked());
	settings.setValue("check_lesson_publish", checkLessonPublish->isChecked());
	settings.setValue("check_native_style", checkNativeStyle->isChecked());
    settings.setValue("check_new_version", checkNewVersion->isChecked());
    settings.endGroup();

	return requireRestart;
}


//--------------------------------------------------------

LanguagePage::LanguagePage(QWidget *parent) : QWidget(parent) {

	// Create group with language settings
    createGroupLanguage();

	// Set the layout of all widgets created above
	createLayout();

	// Read settings
	readSettings();

    checkLessonToLayout();

    connect(comboLayouts, SIGNAL(currentIndexChanged(int)), this, SLOT(checkLessonToLayout()));
    connect(comboLayouts, SIGNAL(currentIndexChanged(int)), this, SLOT(clearLayoutSetting()));
    connect(comboLessons, SIGNAL(currentIndexChanged(int)), this, SLOT(checkLessonToLayout()));
    connect(buttonLayoutRegEx, SIGNAL(clicked()), this, SLOT(showLayoutAdvanced()));
}

void LanguagePage::createGroupLanguage() {

    StartSql *startSql = new StartSql();

	groupLanguage = new QGroupBox(tr("Sprache"));

	labelLanguage = new QLabel(tr("Programmoberflaeche:"));

    comboLanguages = new QComboBox();

    startSql->fillLanguage(comboLanguages, "language_interfaces", "interface");

	labelLayout = new QLabel(tr("Tastaturlayout:"));

	comboLayouts = new QComboBox();

    startSql->fillLanguage(comboLayouts, "language_layouts", "layout");

    buttonLayoutRegEx = new QPushButton(tr("Erweitert"));
    buttonLayoutRegEx->setFixedHeight(20);

	labelLesson = new QLabel(tr("Uebungslektionen:"));

    comboLessons = new QComboBox();

    startSql->fillLanguage(comboLessons, "language_lessons", "lesson");

    labelLessonNotice =  new QLabel();
    QFont h2;
    #if APP_MAC
    h2.setPointSize(11);
    #else
    h2.setPointSize(7);
    #endif
    labelLessonNotice->setWordWrap(true);
    labelLessonNotice->setFont(h2);
    labelLessonNotice->setText(tr("Die gewaehlten Uebungslektionen sind nicht auf "
                                  "das Tastaturlayout abgestimmt. Sie koennen diese trotzdem "
                                  "trainieren, muessen aber unter Umstaenden von Beginn an "
                                  "Tastwege zuruecklegen.\n"));

    QHBoxLayout *layoutRegexp = new QHBoxLayout;
    layoutRegexp->addWidget(comboLayouts);
    layoutRegexp->addWidget(buttonLayoutRegEx);
	// Layout of group box
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(labelLanguage);
	layout->addWidget(comboLanguages);
    layout->addSpacing(12);
	layout->addWidget(labelLayout);
    layout->addLayout(layoutRegexp);
    layout->addSpacing(12);
    layout->addWidget(labelLesson);
    layout->addWidget(comboLessons);
    layout->addSpacing(10);
    layout->addWidget(labelLessonNotice);
    //layout->addSpacing(20);
    layout->addStretch(1);
	layout->setMargin(16);
	groupLanguage->setLayout(layout);
}

void LanguagePage::createLayout() {
	// Full layout of all widgets vertical
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(groupLanguage);
    mainLayout->setSpacing(15);
    // Pass layout to parent widget (this)
	this->setLayout(mainLayout);
}

void LanguagePage::checkLessonToLayout()
{
    QString layout = comboLayouts->itemData(comboLayouts->currentIndex()).toString();
    QString lessons = comboLessons->itemData(comboLessons->currentIndex()).toString();

    if (layout.contains(lessons.mid(3)) ||
        (lessons.mid(3) == "de_qwertz" && layout.contains("ch_qwertz"))) {
        labelLessonNotice->setVisible(false);
    } else {
        labelLessonNotice->setVisible(true);
    }
}

void LanguagePage::clearLayoutSetting()
{
    #if APP_PORTABLE
    QSettings settings(QCoreApplication::applicationDirPath() +
        "/portable/settings.ini", QSettings::IniFormat);
    #else
    QSettings settings;
    #endif
    settings.beginGroup("general");
    settings.setValue("layout_replace", "NULL");
    settings.setValue("layout_regexp", "NULL");
    settings.endGroup();
}

void LanguagePage::showLayoutAdvanced()
{
    RegExpDialog regExpDialog(comboLayouts->itemData(comboLayouts->currentIndex()).toString(), this);
    regExpDialog.exec();
}

void LanguagePage::readSettings() {
	int tempIndex;
	QString dbNameUser = APP_USER_DB;
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
	settings.beginGroup("general");
	tempIndex = comboLanguages->findData(settings.value("language_gui",
		 APP_STD_LANGUAGE_GUI).toString());
	if (!tempIndex) {
		tempIndex = 0;
	}
	comboLanguages->setCurrentIndex(tempIndex);
	tempIndex = comboLayouts->findData(settings.value("language_layout",
		 APP_STD_LANGUAGE_LAYOUT).toString());
	if (!tempIndex) {
		tempIndex = 0;
	}
	comboLayouts->setCurrentIndex(tempIndex);
	tempIndex = comboLessons->findData(settings.value("language_lesson",
		 APP_STD_LANGUAGE_LESSON).toString());
	if (!tempIndex) {
		tempIndex = 0;
	}
	comboLessons->setCurrentIndex(tempIndex);
	settings.endGroup();
}

bool LanguagePage::writeSettings() {

	bool requireRestart = false;

	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
	settings.beginGroup("general");
	if (comboLanguages->itemData(comboLanguages->currentIndex()) !=
		settings.value("language_gui", APP_STD_LANGUAGE_GUI).toString()) {

		requireRestart = true;
	}
    /*if (comboLessons->itemData(comboLessons->currentIndex()) !=
		settings.value("language_lesson", APP_STD_LANGUAGE_LESSON).toString()) {

		requireRestart = true;
    }*/
	settings.setValue("language_gui", comboLanguages->itemData(comboLanguages->currentIndex()));
	settings.setValue("language_layout", comboLayouts->itemData(comboLayouts->currentIndex()));
	settings.setValue("language_lesson", comboLessons->itemData(comboLessons->currentIndex()));
	settings.endGroup();

	return requireRestart;
}
