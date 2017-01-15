/****************************************************************
**
** Implementation of the SettingDialog class
** File name: settingsdialog.cpp
**
****************************************************************/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSqlQuery>
#include <QSettings>
#include <QMessageBox>
#include <QFontDialog>
#include <QFileDialog>
#include <QCoreApplication>

#include "settingsdialog.h"
#include "sql/startsql.h"
#include "sql/connection.h"
#include "def/errordefines.h"
#include "errormessage.h"
#include "def/defines.h"

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
	// Default ticker font if there is no saved ticker font
	tickerFont = QFont(FONT_STANDARD, 18);

	// Create group boxes with settings
    createGroupKeyboardLayout();
    createGroupTickerFont();
    createGroupUserReset();
    createGroupDatabase();
    createGroupDialogCheck();

    // Create buttons
    createButtons();

	// Set the layout of all widgets created above
	createLayout();

	// Widget connections
    connect(buttonSave, SIGNAL(clicked()), this, SLOT(clickSave()));
    connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(buttonSetFont, SIGNAL(clicked()), this, SLOT(setFont()));
	connect(buttonLessonsReset, SIGNAL(clicked()), this, SLOT(deleteUserData()));
	connect(buttonCharsReset, SIGNAL(clicked()), this, SLOT(deleteUserData()));
	connect(buttonDatabasePath, SIGNAL(clicked()), this, SLOT(setDatabasePath()));

	// Read settings
	readSettings();

    setWindowTitle(tr("Grundeinstellungen"));
	setWindowIcon(QIcon(":/img/icon.ico"));

    setMinimumSize(400, 470);

    buttonSave->setFocus();
}

void SettingsDialog::createGroupKeyboardLayout() {
	// Group "Keyboard layout"
	groupKeyboardLayout = new QGroupBox(tr("Tastaturlayout"));

	// Radiobutton "Windows layout"
	radioLayoutWin = new QRadioButton(tr("QWERTZ (Windows)"));
	radioLayoutWin->setToolTip(tr("Waehlen Sie diese Option, wenn Sie "
		"eine standardmaessige Windows-Tastatur verwenden\n(Windows-Tasten vorhanden und "
		"@-Zeichen auf der Q-Taste)"));

	//Radiobutton "Macintosh layout"
	radioLayoutMac = new QRadioButton(tr("QWERTZ (Apple Macintosh)"));
	radioLayoutMac->setToolTip(tr("Waehlen Sie diese Option, wenn Sie "
		"eine standardmaessige Apple-Tastatur verwenden\n(Apfel-Tasten vorhanden und @-Zeichen "
		"auf der L-Taste)"));

	//Radiobutton "NEO layout"
	radioLayoutNeo = new QRadioButton(tr("NEO"));
	radioLayoutNeo->setToolTip(tr("Waehlen Sie diese Option, wenn Sie "
		"eine Tastatur mit NEO-Layout verwenden"));

	//Radiobutton "Dvorak layout"
	radioLayoutDvorak = new QRadioButton(tr("Dvorak"));
	radioLayoutDvorak->setToolTip(tr("Waehlen Sie diese Option, wenn Sie "
		"eine Tastatur mit Dvorak-Layout verwenden"));

	//Radiobutton "NEO layout"
	radioLayoutRistome = new QRadioButton(tr("RISTOME"));
	radioLayoutRistome->setToolTip(tr("Waehlen Sie diese Option, wenn Sie "
		"eine Tastatur mit RISTOME-Layout verwenden"));

	// Layout of group box
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(radioLayoutWin);
	layout->addWidget(radioLayoutMac);
	layout->addWidget(radioLayoutNeo);
	layout->addWidget(radioLayoutDvorak);
	layout->addWidget(radioLayoutRistome);
	layout->setMargin(16);
	groupKeyboardLayout->setLayout(layout);
}

void SettingsDialog::createGroupTickerFont() {
	// Group "Ticker"
	groupTickerFont = new QGroupBox(tr("Laufschrift"));

	// Button "Set ticker font"
	labelTickerFont = new QLabel(tr("Schriftart:"));
	buttonSetFont = new QPushButton(tr("&Schriftart aendern"));
	buttonSetFont->setToolTip(tr("Hier koennen Sie die Schriftart der "
		"Laufschrift veraendern\n(eine Schriftgroesse ueber 20 Punkte wird aus "
		"Formatierungs-\ngruenden nicht empfohlen)"));

	// Spinbox "Set ticker speed"
	labelTickerSpeed = new QLabel(tr("Anfangs-\ngeschwindigkeit:"));
	spinTickerSpeed = new QSpinBox();
	spinTickerSpeed->setMinimum(1);
	spinTickerSpeed->setMaximum(4);
	spinTickerSpeed->setToolTip(tr("Hier koennen Sie die "
		"Geschwindigkeit der Laufschrift veraendern\n(der Wert 0 entspricht "
		"einem langsamen und der Wert 10\neinem schnellen Laufband)"));

	// Layout of group box vertical
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(labelTickerFont);
	layout->addWidget(buttonSetFont);
    layout->addSpacing(10);
	layout->addWidget(labelTickerSpeed);
	layout->addWidget(spinTickerSpeed);
	//layout->addStretch(1);
	layout->setMargin(16);
	groupTickerFont->setLayout(layout);
}

void SettingsDialog::createGroupUserReset() {
	// Group "Keyboard layout"
	groupUserReset = new QGroupBox(tr("Benutzerdaten"));

	// Button "Reset user data"
	buttonLessonsReset = new QPushButton(tr("&Gespeicherte Lektionen zuruecksetzen"));
	buttonLessonsReset->setToolTip(tr("Hier koennen Sie saemtliche "
		"gespeicherte Lektionendaten\n(die Lektionen werden so in den urspruenglichen Zustand,\n"
		"wie nach der Installation, versetzt)"));

	// Button "Reset user data"
	buttonCharsReset = new QPushButton(tr("&Gespeicherte Schriftzeichen zuruecksetzen"));
	buttonCharsReset->setToolTip(tr("Hier koennen Sie saemtliche "
		"gespeicherte Schriftzeichen (Tippfehler)\n(die Schriftzeichen werden so in den urspruenglichen Zustand,\n"
		"wie nach der Installation, versetzt)"));

	// Layout of group box
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(buttonLessonsReset);
	layout->addWidget(buttonCharsReset);
	//layout->addStretch(1);
	layout->setMargin(16);
	groupUserReset->setLayout(layout);
}

void SettingsDialog::createGroupDatabase() {
	// Group "Keyboard layout"
	groupDatabase = new QGroupBox(tr("Datenbank"));

	labelDatabasePath = new QLabel(tr("Verzeichnispfad zur Datenbank \"tipp10v2.db\":"));
	lineDatabasePath = new QLineEdit();
	buttonDatabasePath = new QPushButton(tr("..."));
	buttonDatabasePath->setMaximumWidth(30);
	labelDatabaseExplain = new QLabel(tr("Wenn im angegebenen Verzeichnis keine Datei \"tipp10v2.db\" existiert, "
		"wird automatisch eine neue Datenbank erzeugt."));
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

void SettingsDialog::createGroupDialogCheck() {
	// Group "Ticker"
	groupDialogCheck = new QGroupBox(tr("Programmstart"));

	// Check box "Show start window"
	checkIllustration = new QCheckBox(tr("&Willkommenfenster anzeigen"));
	checkIllustration->setToolTip(tr("Hier koennen Sie festlegen, "
		"ob ein Informationsfenster\nmit Ratschlaegen beim Start von TIPP10"
		"angezeigt werden soll"));

	// Layout of group box vertical
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(checkIllustration);
    //layout->addSpacing(1);
	layout->setMargin(16);
	groupDialogCheck->setLayout(layout);
}

void SettingsDialog::createButtons() {
	//Buttons
	buttonCancel = new QPushButton(tr("&Abbrechen"));
	buttonSave = new QPushButton(tr("&Speichern"));
	buttonSave->setDefault(true);
}

void SettingsDialog::createLayout() {
	// Button layout horizontal
	QHBoxLayout *buttonLayoutHorizontal = new QHBoxLayout;
    buttonLayoutHorizontal->addStretch(1);
    buttonLayoutHorizontal->addWidget(buttonCancel);
    buttonLayoutHorizontal->addWidget(buttonSave);
	// Button layout horizontal
	QHBoxLayout *buttonLayoutHorizontalTop = new QHBoxLayout;
    buttonLayoutHorizontalTop->addWidget(groupKeyboardLayout);
    buttonLayoutHorizontalTop->addWidget(groupTickerFont);
    // Group layout vertical
	QVBoxLayout *boxesLayout = new QVBoxLayout;
    boxesLayout->addLayout(buttonLayoutHorizontalTop);
    boxesLayout->addWidget(groupUserReset);
    boxesLayout->addWidget(groupDatabase);
    boxesLayout->addWidget(groupDialogCheck);
	// Full layout of all widgets vertical
	QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(boxesLayout);
    mainLayout->addSpacing(1);
    mainLayout->addLayout(buttonLayoutHorizontal);
    mainLayout->setMargin(15);
    mainLayout->setSpacing(15);
    // Pass layout to parent widget (this)
	this->setLayout(mainLayout);
}

void SettingsDialog::setFont() {
	bool ok;
	QFont font = QFontDialog::getFont(&ok, tickerFont, this);
	if (ok) {
		tickerFont = font;
		setFontButtonLabel();
	}
}

void SettingsDialog::setFontButtonLabel() {
	buttonSetFont->setText(tickerFont.family() + ", "
		+ QString::number(tickerFont.pointSize()));
}

void SettingsDialog::setDatabasePath() {
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

void SettingsDialog::deleteUserData() {
	QSqlQuery query;
		switch (QMessageBox::question(this, APP_NAME,
			tr("Es werden alle aufgezeichneten Werte des aktuellen "
			"Benutzers geloescht\nund das Programm in den urpruenglichen "
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
				if (!userSql->deleteUserChars()) {
					// Error message
					ErrorMessage *errorMessage = new ErrorMessage(this);
					errorMessage->showMessage(ERR_USER_ERRORS_FLUSH,
						TYPE_WARNING, CANCEL_OPERATION);
					return;
				}
				QMessageBox::information(this, APP_NAME,
					tr("Die Benutzerdaten wurden erfolgreich "
					"zurueckgesetzt!\n"));
				break;
		}
}

void SettingsDialog::clickSave() {
	writeSettings();
	if (createConnection()) {
		this->accept();
	}
}

void SettingsDialog::readSettings() {
	QString dbNameUser = APP_USER_DB;
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "portable", "settings");
    settings.setPath(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::applicationDirPath());
	settings.beginGroup("settings");
	switch (KEYBOARD_LAYOUT_STANDARD) {
		case 0:
			radioLayoutWin->setChecked(settings.value("radio_win", true).toBool());
			radioLayoutMac->setChecked(settings.value("radio_mac", false).toBool());
			radioLayoutNeo->setChecked(settings.value("radio_neo", false).toBool());
			radioLayoutDvorak->setChecked(settings.value("radio_dvorak", false).toBool());
			radioLayoutRistome->setChecked(settings.value("radio_ristome", false).toBool());
			break;
		case 1:
			radioLayoutWin->setChecked(settings.value("radio_win", false).toBool());
			radioLayoutMac->setChecked(settings.value("radio_mac", true).toBool());
			radioLayoutNeo->setChecked(settings.value("radio_neo", false).toBool());
			radioLayoutDvorak->setChecked(settings.value("radio_dvorak", false).toBool());
			radioLayoutRistome->setChecked(settings.value("radio_ristome", false).toBool());
			break;
		case 2:
			radioLayoutWin->setChecked(settings.value("radio_win", false).toBool());
			radioLayoutMac->setChecked(settings.value("radio_mac", false).toBool());
			radioLayoutNeo->setChecked(settings.value("radio_neo", true).toBool());
			radioLayoutDvorak->setChecked(settings.value("radio_dvorak", false).toBool());
			radioLayoutRistome->setChecked(settings.value("radio_ristome", false).toBool());
			break;
		case 3:
			radioLayoutWin->setChecked(settings.value("radio_win", false).toBool());
			radioLayoutMac->setChecked(settings.value("radio_mac", false).toBool());
			radioLayoutNeo->setChecked(settings.value("radio_neo", false).toBool());
			radioLayoutDvorak->setChecked(settings.value("radio_dvorak", true).toBool());
			radioLayoutRistome->setChecked(settings.value("radio_ristome", false).toBool());
			break;
		case 4:
			radioLayoutWin->setChecked(settings.value("radio_win", false).toBool());
			radioLayoutMac->setChecked(settings.value("radio_mac", false).toBool());
			radioLayoutNeo->setChecked(settings.value("radio_neo", false).toBool());
			radioLayoutDvorak->setChecked(settings.value("radio_dvorak", false).toBool());
			radioLayoutRistome->setChecked(settings.value("radio_ristome", true).toBool());
			break;
		default:
			radioLayoutWin->setChecked(settings.value("radio_win", true).toBool());
			radioLayoutMac->setChecked(settings.value("radio_mac", false).toBool());
			radioLayoutNeo->setChecked(settings.value("radio_neo", false).toBool());
			radioLayoutDvorak->setChecked(settings.value("radio_dvorak", false).toBool());
			radioLayoutRistome->setChecked(settings.value("radio_ristome", false).toBool());
			break;
	}
	tickerFont.fromString(settings.value("ticker_font",
		tickerFont.toString()).toString());
	spinTickerSpeed->setValue(settings.value("ticker_speed",
		TICKERSPEED_STANDARD).toInt());
	settings.endGroup();
	settings.beginGroup("general");
	checkIllustration->setChecked(!settings.value("check_illustration", true).toBool());
	settings.endGroup();
	settings.beginGroup("database");
	currentDatabasePath = settings.value("path", "Fehler/" + dbNameUser).toString();
	if (currentDatabasePath.size() > dbNameUser.size() + 1) {
		lineDatabasePath->setText(currentDatabasePath.left(currentDatabasePath.size() - dbNameUser.size() - 1));
	}
	settings.endGroup();
	setFontButtonLabel();
}

void SettingsDialog::writeSettings() {
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "portable", "settings");
    settings.setPath(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::applicationDirPath());
	settings.beginGroup("settings");
	settings.setValue("radio_win", radioLayoutWin->isChecked());
	settings.setValue("radio_mac", radioLayoutMac->isChecked());
	settings.setValue("radio_neo", radioLayoutNeo->isChecked());
	settings.setValue("radio_dvorak", radioLayoutDvorak->isChecked());
	settings.setValue("radio_ristome", radioLayoutRistome->isChecked());
	settings.setValue("ticker_font", tickerFont.toString());
	settings.setValue("ticker_speed", spinTickerSpeed->value());
	settings.endGroup();
	settings.beginGroup("general");
	settings.setValue("check_illustration", !checkIllustration->isChecked());
	settings.endGroup();
	settings.beginGroup("database");
	settings.setValue("path", lineDatabasePath->text() + "/" + QString::QString(APP_USER_DB));
	settings.endGroup();
}
