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
** Implementation of the TrainingWidget class
** File name: trainingwidget.cpp
**
****************************************************************/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSqlQuery>
#include <QApplication>
#include <QSettings>
#include <QVariant>
#include <QCoreApplication>

#include "trainingwidget.h"
#include "def/errordefines.h"
#include "errormessage.h"
#include "def/defines.h"

TrainingWidget::TrainingWidget(int lesson, int type, QString name, QWidget *parent) : QWidget(parent) {
	// Default values
    isStarted = false;
    isPaused = false;
	currentChar = ' ';
	currentStrokes = 0;
	currentChars = 0;
	currentErrors = 0;
	currentSeconds = 0;
	currentLesson = lesson;
	currentType = type;
	currentName = name;
	errorCorrectFlag = false;
	startTime = QDateTime::currentDateTime();
	counterToNewLine = 0;
	oneErrorFlag = false;

    // Init sound file
    #if APP_MAC
	// Mac-Version
    // -----------
    bells = new QSound(QCoreApplication::applicationDirPath() + "/error.aif");
    metronomeSound = new QSound(QCoreApplication::applicationDirPath() + "/metronome.aif");
	#else
	// Win-Version
    // -----------
    bells = new QSound("error.wav");
    metronomeSound = new QSound("metronome.wav");
	#endif

    // Initialise timer
    timer = new QTimer(this);
    metronomeTimer = new QTimer(this);

	// Set the logo
	companyLogo = new CompanyLogo(this, true);

	// Create all type trainer objects
	// 1. Ticker
	tickerBoard = new TickerBoard(this);
	// 2. Keyboard (set focus!)
    if ((currentLesson % 100) >= NUMPAD_LESSON_START && currentType == 0) {
		numPad = new NumPad(this);
	} else {
		keyBoard = new KeyBoard(this);
	}

    readSettings();

	// 3. Statusbar
	statusBar = new StatusBar(this);
    trainingSql = new TrainingSql(replaceSetting, regexpSetting, layoutSetting);
	// Create Buttons "Cancel" and "Pause"
	createButtons();
	// Create layout of widgets
	createLayout();
	// Create connections
    createConnections();

	keyboardSql = new KeyboardSql(opSystem);

	// Create lesson text
	//createLesson();
	if (!showHelpers) {
        if ((currentLesson % 100) >= NUMPAD_LESSON_START && currentType == 0) {
			numPad->setVisible(false);
		} else {
			keyBoard->setVisible(false);
		}
        //statusBar->setVisible(false);
        //companyLogo->setVisible(false);
		parent->setMinimumSize(APP_WIDTH_SMALL, APP_HEIGHT_SMALL);
		if (!parent->isMaximized()) {
			parent->resize(APP_WIDTH_SMALL, APP_HEIGHT_SMALL);
		}
	}
	tickerBoard->setFocus();
	// Start all
	startSession();
}

void TrainingWidget::createButtons() {
	// Pause button
	buttonPause = new QPushButton(tr("&Pause"));
    buttonPause->setEnabled(false);
	buttonPause->setFocusPolicy(Qt::NoFocus);
    #if APP_MAC
        buttonPause->setShortcut(Qt::ALT + Qt::Key_P);
    #endif
	// Cancel button
	buttonCancel = new QPushButton(tr("A&bbrechen"));
	buttonCancel->setFocusPolicy(Qt::NoFocus);
    #if APP_MAC
        buttonCancel->setShortcut(Qt::ALT + Qt::Key_B);
    #endif
	// Help button
	buttonHelp = new QPushButton(tr("&Hilfe"));
	buttonHelp->setFocusPolicy(Qt::NoFocus);
}

void TrainingWidget::createLayout() {
	// Button layout horizontal
	QHBoxLayout *buttonLayoutHorizontal = new QHBoxLayout;
    buttonLayoutHorizontal->addWidget(buttonCancel);
    buttonLayoutHorizontal->addSpacing(10);
    buttonLayoutHorizontal->addWidget(buttonHelp);
    buttonLayoutHorizontal->addWidget(buttonPause);
	// Button layout vertical
	QVBoxLayout *buttonLayoutVertical = new QVBoxLayout;
    buttonLayoutVertical->addSpacing(11);
    buttonLayoutVertical->addLayout(buttonLayoutHorizontal);
	// Logo layout vertical
	QVBoxLayout *logoLayout = new QVBoxLayout;
    logoLayout->addSpacing(1);
    logoLayout->addWidget(companyLogo);
	// Logo layout + button layout horizontal
	QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addLayout(logoLayout);
    bottomLayout->addStretch(1);
    bottomLayout->addLayout(buttonLayoutVertical);
	// Ticker layout horizontal
	QHBoxLayout *tickerboardLayout = new QHBoxLayout;
    tickerboardLayout->addStretch(1);
    tickerboardLayout->addWidget(tickerBoard);
    tickerboardLayout->addStretch(1);
	// Keyboard layout horizontal
	QHBoxLayout *keyboardLayout = new QHBoxLayout;
    keyboardLayout->addStretch(1);
    if ((currentLesson % 100) >= NUMPAD_LESSON_START && currentType == 0) {
    	keyboardLayout->addWidget(numPad);
	} else {
    	keyboardLayout->addWidget(keyBoard);
	}
    keyboardLayout->addStretch(1);
	// Statusbar layout horizontal
	QHBoxLayout *statusLayout = new QHBoxLayout;
    statusLayout->addStretch(1);
    statusLayout->addWidget(statusBar);
    statusLayout->addStretch(1);
	// Full layout of all widgets vertical
	QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addStretch(1);
    mainLayout->addLayout(tickerboardLayout);
    mainLayout->addLayout(keyboardLayout);
    mainLayout->addLayout(statusLayout);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(bottomLayout);
    mainLayout->setMargin(15);
    mainLayout->setSpacing(15);
    // Pass layout to parent widget (this)
	this->setLayout(mainLayout);
}

void TrainingWidget::createConnections() {
    connect(timer, SIGNAL(timeout()), this, SLOT(secondsUpdate()));
    connect(metronomeTimer, SIGNAL(timeout()), this, SLOT(metronomeOutput()));
	// Incoming connections
	// Incoming connection from TickerBoard object
	connect(tickerBoard, SIGNAL(charChanged(QChar)), this,
		SLOT(setChar(QChar)));
	connect(tickerBoard, SIGNAL(updateRequired()), this, SLOT(updateLesson()));
	// Incoming connection from KeyBoard object
	connect(tickerBoard, SIGNAL(keyPressed(QChar)), this, SLOT(setKey(QChar)));
	//connect(tickerBoard, SIGNAL(isReady()), this, SLOT(exitTraining()));
	// Button connections
	connect(buttonPause, SIGNAL(clicked()), this, SLOT(pauseSession()));
	connect(buttonPause, SIGNAL(clicked()), tickerBoard, SLOT(pauseTicker()));
    connect(buttonCancel, SIGNAL(clicked()), this, SLOT(cancelSession()));
    connect(buttonHelp, SIGNAL(clicked()), this, SLOT(pauseSession()));
	connect(buttonHelp, SIGNAL(clicked()), tickerBoard, SLOT(pauseTicker()));
    connect(buttonHelp, SIGNAL(clicked()), this, SLOT(showHelp()));
    if ((currentLesson % 100) >= NUMPAD_LESSON_START && currentType == 0) {
		//Verbindung zwischen Laufschrift- und Tastaturklasse
		connect(tickerBoard, SIGNAL(isReady()), numPad, SLOT(stopBoard()));
		connect(numPad, SIGNAL(statusRefreshed(QString)), this,
			SLOT(updateStatusText(QString)));
		connect(buttonPause, SIGNAL(clicked()), numPad, SLOT(pauseBoard()));
		connect(buttonHelp, SIGNAL(clicked()), numPad, SLOT(pauseBoard()));
	} else {
		//Verbindung zwischen Laufschrift- und Tastaturklasse
		connect(tickerBoard, SIGNAL(isReady()), keyBoard, SLOT(stopBoard()));
		connect(keyBoard, SIGNAL(statusRefreshed(QString)), this,
			SLOT(updateStatusText(QString)));
		connect(buttonPause, SIGNAL(clicked()), keyBoard, SLOT(pauseBoard()));
		connect(buttonHelp, SIGNAL(clicked()), keyBoard, SLOT(pauseBoard()));
	}
}

void TrainingWidget::startSession() {
	timer->start(1000);
    if (metronomeClock != 0) {
        metronomeTimer->start(metronomeClock);
    }
	createLesson();
	tickerBoard->pauseTicker(tr("Leertaste startet das Diktat"));
	statusBar->setCenterText(tr("Grundstellung einnehmen"));
}

void TrainingWidget::pauseSession() {
    buttonPause->setEnabled(false);
	isPaused = true;
}

void TrainingWidget::cancelSession() {
	isPaused = true;
	// Ask only is lesson is started and if anything was typed at all
	if (isStarted && currentStrokes > 0) {
		// Ask if user wants to quit really
		switch (QMessageBox::question(this, APP_NAME,
			tr("Wollen Sie die Lektion wirklich vorzeitig "
			"beenden?\n\n"), tr("&Ja"), tr("&Abbrechen"),
			0, 1)) {
			case 0:
				// User wants to quit
				break;
			case 1:
				// User canceled the message
				// -> go back to the training
				isPaused = false;
				return;
		}
		// Ask if user want to save data
		switch (QMessageBox::question(this, APP_NAME,
			tr("Sollen die Ergebnisse der Lektion gespeichert "
			"werden?\n\n"), tr("&Ja"), tr("&Nein"), 0, 0)) {
			case 0:
				// Save and exit
				exitTraining();
				return;
			case 1:
				// Don't save and exit (cancel like on the beginning)
				// -> go back to start widget
				emit lessonCanceled();
				return;
		}
	} else {
		// Nothing done yet
		// -> go back to start widget
		emit lessonCanceled();
	}
}

void TrainingWidget::createLesson() {
	QString lessonString;
	if ((lessonString = trainingSql->createLesson(currentLesson, currentType,
		lessonUnit, useIntelligence, useEszett, useUmlaut)) == "") {
		// No lesson created
		// -> error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_LESSONS_CREATION, TYPE_CRITICAL,
			CANCEL_OPERATION);
		return;
	}
	switch (currentType) {
		case 0:
            // Training lesson
			counterToNewLine = lessonString.length() + 1;
            if ((currentLesson % 100) < BORDER_LESSON_IS_SENTENCE) {
				lessonString.append(" ");
			} else {
				lessonString.append(QChar(TOKEN_NEW_LINE));
			}
            break;
		case 1:
		case 2:
			// Open or own lesson
			if (limitType != 2) {
				// Exit lesson or unit with newline
				counterToNewLine = lessonString.length() + 1;
				if (lessonUnit == 1) {
					lessonString.append(" ");
				} else {
					lessonString.append(QChar(TOKEN_NEW_LINE));
				}
			} else {
				// Hole lesson
				counterToNewLine = lessonString.length() - 2;
			}
			break;
	}
	counterChars = lessonString.length();
	tickerBoard->setTicker(lessonString);
}

void TrainingWidget::updateLesson() {
	QString lessonString;

	if (limitType == 2) {
		return;
	}
	if ((lessonString = trainingSql->updateLesson(currentLesson, currentType,
		useIntelligence, useEszett, useUmlaut)) == "") {
		// No update lesson created
		// -> error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_LESSONS_UPDATE, TYPE_CRITICAL,
			CANCEL_OPERATION);
		return;
	}
	counterToNewLine += lessonString.length();
    if ((currentType == 0 && (currentLesson % 100) < BORDER_LESSON_IS_SENTENCE &&
		counterToNewLine > NUM_TOKEN_UNTIL_NEW_LINE) ||
        (currentType == 0 && (currentLesson % 100) >= BORDER_LESSON_IS_SENTENCE) ||
		(currentType != 0 && (lessonUnit == 0 ||
		(lessonUnit == 1 &&
		counterToNewLine > NUM_TOKEN_UNTIL_NEW_LINE)))) {
		lessonString.append(QChar(TOKEN_NEW_LINE));
		counterToNewLine = 0;
	} else {
		counterToNewLine++;
		lessonString.append(" ");
	}
	counterChars += lessonString.length();
	tickerBoard->extendTicker(lessonString);
}

// Slot: Aktuellen Buchstaben setzen
void TrainingWidget::setChar(QChar newchar) {
	currentChars++;
	currentChar = newchar;
    if ((currentLesson % 100) >= NUMPAD_LESSON_START && currentType == 0) {
		numPad->setKey(currentChar);
	} else {
		keyBoard->setKey(currentChar);
	}
	if (!trainingSql->updateUsertable(currentChar, "user_char_occur_num")) {
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_USER_ERRORS_REFRESH, TYPE_CRITICAL,
			CANCEL_OPERATION);
	}
}

// Slot: Aktuellen Buchstaben setzen
void TrainingWidget::setKey(QChar key) {
	if (isStarted && !isPaused) {
		if (errorCorrectFlag) {
			// Ruecklauftaste
			if (key.unicode() == 8) {
				errorCorrectFlag = false;
                if ((currentLesson % 100) >= NUMPAD_LESSON_START && currentType == 0) {
					numPad->setKey(currentChar);
				} else {
					keyBoard->setKey(currentChar);
				}
				tickerBoard->clearErrorSelection();
				oneErrorFlag = false;
			}
		} else {
			// Check if correct key was pressed OR
			// key was enter and a line break was required
			// (char and unicode then are different)
			if (key == currentChar || ((key.unicode() == 13 || key.unicode() == 3) &&
				currentChar == QChar(TOKEN_NEW_LINE)) ||
				(key.unicode() == 9 &&
				currentChar == QChar(TOKEN_TAB))) {
				//currentChar.unicode() == 182)) {
				// Correct key was pressed
				oneErrorFlag = false;
				currentStrokes++;
				tickerBoard->getNewChar();

				charList << key;
				mistakeList << 0;
			} else {
				// Wrong key was pressed
				if (!oneErrorFlag) {
					if (beepOnError) {
						if (beepSoundcard) {
							bells->play();
						} else {
							QApplication::beep();
						}
					}
					currentErrors++;
                    tickerBoard->setErrorSelection();
					update();

					if (key.unicode() == 13 || key.unicode() == 3) {
						charList << QChar(TOKEN_NEW_LINE);
					} else {
						if (key.unicode() == 9) {
							charList << QChar(TOKEN_TAB);
						} else {
							charList << key;
						}
					}
					mistakeList << 1;

					if (!trainingSql->updateUsertable(currentChar,
						"user_char_target_errornum")) {
						// Error message
						ErrorMessage *errorMessage = new ErrorMessage(this);
						errorMessage->showMessage(ERR_USER_ERRORS_REFRESH,
							TYPE_CRITICAL, CANCEL_OPERATION);
					}
					if (!trainingSql->updateUsertable(key,
						"user_char_mistake_errornum")) {
						// Error message
						ErrorMessage *errorMessage = new ErrorMessage(this);
						errorMessage->showMessage(ERR_USER_ERRORS_REFRESH,
							TYPE_CRITICAL, CANCEL_OPERATION);
					}
					oneErrorFlag = true;
				}
				if (!stopOnError) {
					oneErrorFlag = false;
					currentStrokes++;
					tickerBoard->getNewChar();
				}
				//statusBar->setCenterText(QString::number(key.unicode()));
				if (correctOnError) {
					errorCorrectFlag = true;
                    if ((currentLesson % 100) >= NUMPAD_LESSON_START && currentType == 0) {
						numPad->setKey(QChar(TOKEN_BACKSPACE));
					} else {
						keyBoard->setKey(QChar(TOKEN_BACKSPACE));
					}
					//statusBar->setCenterText(tr("Kleiner Finger "
					//	"rechts - Ruecklauftaste!"));
				}
				//update();
			}
		}
		updateStatusValues();
	} else {
		if (key == ' ') {
            bool wasPaused = isPaused;
			isStarted = true;
			isPaused = false;
            buttonPause->setEnabled(true);
			buttonCancel->setText(tr("Lektion vorzeitig &beenden"));
			// Start board first, cause starting the ticker
			// emits a signal which need a started keyBoard
            if ((currentLesson % 100) >= NUMPAD_LESSON_START && currentType == 0) {
				numPad->startBoard();
			} else {
				keyBoard->startBoard();
			}
            tickerBoard->startTicker(wasPaused);
			return;
		}
	}
}

void TrainingWidget::secondsUpdate() {
    if (isStarted && !isPaused) {
        currentSeconds++;
        if ((limitType == 0 && limitValue <= (currentSeconds / 60)) ||
            (limitType == 1 && limitValue <= currentChars) ||
            limitType == 2 && (counterChars + 1) <= currentChars) {
            exitTraining();
        } else {
            updateStatusValues();
        }
    }
}

void TrainingWidget::metronomeOutput() {
    if (isStarted && !isPaused) {
        if (beepSoundcard) {
            metronomeSound->play();
        } else {
            QApplication::beep();
        }
    }
}

void TrainingWidget::updateStatusValues() {
	double strokesPerMinute;
	double minutes;
	int timeMinutes;
	int timeSeconds;
	QString time;
	// CPM
	minutes = (double) currentSeconds / 60.0;
	if (minutes != 0) {
		strokesPerMinute = ((double) currentStrokes / minutes);
	} else {
		strokesPerMinute = 0;
	}
	// Time
	timeMinutes = currentSeconds / 60;
	timeSeconds = currentSeconds % 60;
	time = QString::number(timeMinutes) + tr(":");
	if (timeSeconds < 10) {
		time.append("0" + QString::number(timeSeconds));
	} else {
		time.append(QString::number(timeSeconds));
	}
	statusBar->setLeftLeftText(tr("Fehler: ")
		+ QString::number(currentErrors));
	statusBar->setLeftText(tr("A/min: ")
		+ QString::number((unsigned int)strokesPerMinute));
	statusBar->setRightText(tr("Zeit: ") + time);
	statusBar->setRightRightText(tr("Zeichen: ")
		+ QString::number(currentChars));
}

void TrainingWidget::updateStatusText(QString statustext) {
	if (!showStatusInformation) {
		statustext = "";
	}
	statusBar->setCenterText(statustext);
}

void TrainingWidget::exitTraining() {
	QVariant lastRowId;
	lastRowId = trainingSql->saveLesson(currentLesson, currentSeconds,
		currentChars - 1, currentStrokes, currentErrors, startTime,
		currentType, currentName);
	if (!lastRowId.isValid()) {
		// No lesson created
		// -> error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_USER_LESSONS_REFRESH, TYPE_CRITICAL,
			CANCEL_OPERATION);
		lastRowId = 0;
	}
	emit lessonReady(lastRowId.toInt(), currentType, charList, mistakeList);
}

void TrainingWidget::showHelp() {
	helpBrowser = new HelpBrowser("training.html", 0);
	helpBrowser->show();
}

void TrainingWidget::readSettings() {
	// Read settings of the startwiget
	// (uses the default constructor of QSettings, passing
	// the application and company name see main function)
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
    settings.beginGroup("general");
    if (settings.value("language_layout",
                       APP_STD_LANGUAGE_LAYOUT).toString().right(3) == "win") {
        opSystem = "win";
    } else {
        opSystem = "mac";
    }
    useEszett = true;
    if (settings.value("language_layout",
                       APP_STD_LANGUAGE_LAYOUT).toString().left(2) == "ch") {
        useEszett = false;
    }
    useUmlaut = true;
    if (settings.value("language_layout",
                       APP_STD_LANGUAGE_LAYOUT).toString().left(2) == "us") {
        useUmlaut = false;
    }
    layoutSetting = settings.value("language_layout",
                                           APP_STD_LANGUAGE_LAYOUT).toString();
    replaceSetting = settings.value("layout_replace",
                                           "NULL").toString();
    regexpSetting = settings.value("layout_regexp",
                                           "NULL").toString();

    settings.endGroup();
    /*settings.beginGroup("settings");
    if (settings.value("radio_win", true).toBool()) {
		opSystem = "win";
	} else {
		opSystem = "mac";
    }
	useEszett = true;
	if (settings.value("combo_layout", KEYBOARD_LAYOUT_STANDARD).toInt() == 1) {
		useEszett = false;
	}
	useUmlaut = true;
	if (settings.value("combo_layout", KEYBOARD_LAYOUT_STANDARD).toInt() == 5) {
		useUmlaut = false;
	}
    settings.endGroup();*/
	settings.beginGroup("duration");
	if (settings.value("radio_time", true).toBool()) {
		// Time limit selected
		limitType = 0;
		limitValue = settings.value("spin_time", LESSON_TIMELEN_STANDARD).toInt();
	} else {
		if (settings.value("radio_token", true).toBool()) {
			// Token limit selected
			limitType = 1;
			limitValue = settings.value("spin_token", LESSON_TOKENLEN_STANDARD).toInt();
		} else {
			// Lesson limit selected
			limitType = 2;
			limitValue = 0;
		}
	}
	settings.endGroup();
	settings.beginGroup("error");
	stopOnError = settings.value("check_stop", true).toBool();
	correctOnError = settings.value("check_correct", false).toBool();
    beepOnError = settings.value("check_beep", false).toBool();
    settings.endGroup();
	settings.beginGroup("support");
	showHelpers = settings.value("check_helpers", true).toBool();
	showStatusInformation = settings.value("check_status", true).toBool();
	settings.endGroup();
	settings.beginGroup("sound");
	#if APP_X11
	bool beepDefault = false;
	#else
	bool beepDefault = true;
	#endif
	beepSoundcard = settings.value("radio_soundcard", beepDefault).toBool();
    metronomeClock = settings.value("spin_metronome", METRONOM_STANDARD).toInt();
    metronomeClock = (int) ((double) metronomeClock * 60000.0) /
                     ((double) metronomeClock * (double) metronomeClock);
    if (!settings.value("check_metronome", false).toBool()) {
        metronomeClock = 0;
    }
	settings.endGroup();

    if (currentType != 0) {
		settings.beginGroup("intelligence");
		useIntelligence = settings.value("check_intelligence", false).toBool();
		settings.endGroup();
	} else {
		useIntelligence = true;
	}
    if (currentType != 0) {
		lessonUnit = trainingSql->getLessonUnit(currentLesson, currentType);
	} else {
		lessonUnit = 0;
	}
}
