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
** Implementation of the LessonDialog class
** File name: lessondialog.cpp
**
****************************************************************/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSqlQuery>
#include <QMessageBox>
#include <QFontDialog>
#include <QFileDialog>
#include <QString>

#include "lessondialog.h"
#include "sql/startsql.h"
#include "def/errordefines.h"
#include "errormessage.h"
#include "def/defines.h"

LessonDialog::LessonDialog(QString lessonid, QStringList *data,
	QWidget *parent) : QDialog(parent) {

	setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

	currentLessonId = lessonid;

	lessonData = data;

    // Create buttons
    createButtons();

    // Create controls
    createControls();

	// Set the layout of all widgets created above
	createLayout();

	// Widget connections
    connect(buttonSave, SIGNAL(clicked()), this, SLOT(clickSave()));
    connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(buttonHelp, SIGNAL(clicked()), this, SLOT(showHelp()));

    setWindowIcon(QIcon(":/img/" + QString(ICON_FILENAME)));

    setMinimumSize(420, 470);

    buttonSave->setFocus();

    updateContent();
}

void LessonDialog::updateContent() {
	StartSql *startSql;
	// Dialog is in edit modus (lesson id exist)
	if (currentLessonId == "-1") {
		// New lesson
		lineLessonName->selectAll();
		lineLessonName->setFocus();
	} else {
		if (currentLessonId == "-2") {
			// Import lesson
			lineLessonName->setText(lessonData->at(0).left(20));
			lineLessonDescription->setText("");
			lessonData->removeFirst();
			lineLessonContent->setText(lessonData->join("\n"));

			// Split lesson content to lines
			*lessonData = lineLessonContent->toPlainText().split("\n", QString::SkipEmptyParts);
			// Delete empty lines
			for (int i = 0; i < lessonData->size(); i++) {
				if (QString::QString(lessonData->at(i).toLocal8Bit().constData()).simplified() == "") {
					lessonData->removeAt(i);
				}
			}
			lineLessonContent->setText(lessonData->join("\n"));
		} else {
			// Edit lesson
			startSql = new StartSql();
			if (!startSql->getOwnLesson(currentLessonId, lineLessonName,
				lineLessonDescription, lineLessonContent,
				radioUnitSentence, radioUnitWord)) {
				// No selected lesson found in combo box
				// -> error message
				ErrorMessage *errorMessage = new ErrorMessage(this);
				errorMessage->showMessage(ERR_USER_LESSON_GET, TYPE_INFO,
					CANCEL_OPERATION);
				return;
			}
		}
	}
}

void LessonDialog::createButtons() {
	// Buttons
	buttonCancel = new QPushButton(tr("&Abbrechen"));
	buttonSave = new QPushButton(tr("&Speichern"));
	buttonHelp = new QPushButton(tr("&Hilfe"));
	buttonSave->setDefault(true);
}

void LessonDialog::createControls() {
	// Labels
	labelLessonName = new QLabel(tr("Name der Lektion (maximal 20 "
		"Zeichen):"));
	labelLessonDescription = new QLabel(tr("Kurzbeschreibung (maximal "
		"120 Zeichen):"));
    labelLessonContent = new QLabel(tr("Diktat (mindestens zwei "
		"Zeilen):"));
    labelLessonNotices = new QLabel(tr("<u>Erlaeuterung</u>"
		"<br>&nbsp;<br>Jede Zeile (Zeilenumbruch "
		"am Ende) entspricht einer Einheit fuer das Diktat. "
		"Das Diktat kann auf zwei Arten gefuehrt werden:<br>&nbsp;<br>"
		"<b>Satzdiktat</b> - die "
		"einzelnen Zeilen (Saetze) werden wie hier eingegeben mit einem Zeilenumbruch am "
		"Ende diktiert.<br>&nbsp;<br>"
		"<b>Wortdiktat</b> - die einzelnen Zeilen (Worte) werden mit Leerzeichen getrennt "
		"diktiert, ein Zeilenumbruch erfolgt automatisch nach mindestens ") +
		QString::number(NUM_TOKEN_UNTIL_NEW_LINE) + tr(" diktierten "
		"Schriftzeichen."));
	labelLessonNotices->setWordWrap(true);
	labelLessonNotices->setMaximumWidth(180);
	labelLessonUnit = new QLabel(tr("<b>Was passiert bei aktivierter "
		"Intelligenz?</b><br>Bei "
		"aktivierter Intelligenz werden die Zeilen nicht der Reihenfolge nach "
		"diktiert, sondern abhaengig von den Tippfehler-Quoten die aktuell "
		"fuer den Lernerfolg sinnvollste Zeile in das Diktat einbezogen. Die "
		"Intelligenz zu aktivieren macht nur bei Lektionen Sinn, die aus sehr "
		"vielen Zeilen bestehen.<br>"));
	labelLessonUnit->setWordWrap(true);
	labelLessonUnit->setMaximumWidth(180);
	labelLessonUnitRadio = new QLabel(tr("Das Diktat soll gefuehrt werden als:"));
	labelLessonUnitRadio->setWordWrap(true);
	// Lines (text input)
	lineLessonName = new QLineEdit();
	lineLessonName->setMaxLength(20);
	lineLessonDescription = new QLineEdit();
	lineLessonDescription->setMaxLength(120);
	lineLessonContent = new QTextEdit();
	lineLessonContent->setLineWrapMode(QTextEdit::NoWrap);
	lineLessonContent->setAcceptRichText(false);
	// Radiobutton sentence unit
	radioUnitSentence = new QRadioButton(tr("Satzdiktat"));
	radioUnitSentence->setToolTip(tr("Die einzelnen "
		"Einheiten werden mit\neinem Zeilenumbruch am Ende diktiert"));
	radioUnitSentence->setChecked(true);
	// Radiobutton word unit
	radioUnitWord = new QRadioButton(tr("Wortdiktat"));
	radioUnitWord->setToolTip(tr("Die einzelnen "
		"Einheiten werden mit\nLeerzeichen getrennt "
		"diktiert,\nein Zeilenumbruch erfolgt automatisch\nnach mindestens ") +
		QString::number(NUM_TOKEN_UNTIL_NEW_LINE) + tr(" "
		"Schriftzeichen"));
	radioUnitWord->setChecked(false);

	// Dialog is in edit modus (lesson id exist)
	if (currentLessonId != "-1" && currentLessonId != "-2") {
		setWindowTitle(tr("Eigene Lektion editieren"));
		// Change text of lesson name label
		labelLessonName->setText(tr("Name der Lektion:"));
		// Disable text field "Name of lesson"
		lineLessonName->setEnabled(false);
	} else {
		setWindowTitle(tr("Eigene Lektion erstellen"));
		// Select text field "Name of lesson"
		//lineLessonName->selectAll();
		//lineLessonName->setCursorPosition(0);
	}
}

void LessonDialog::createLayout() {
	// Button layout horizontal
	QHBoxLayout *buttonLayoutHorizontal = new QHBoxLayout;
    buttonLayoutHorizontal->addStretch(1);
    buttonLayoutHorizontal->addWidget(buttonCancel);
    buttonLayoutHorizontal->addSpacing(10);
    buttonLayoutHorizontal->addWidget(buttonHelp);
    buttonLayoutHorizontal->addWidget(buttonSave);
    // Group layout vertical
	QHBoxLayout *boxesLayout = new QHBoxLayout;
    boxesLayout->addStretch(1);
    boxesLayout->addWidget(radioUnitSentence);
    boxesLayout->addSpacing(30);
    boxesLayout->addWidget(radioUnitWord);
    boxesLayout->addStretch(1);
	QVBoxLayout *helpLayout = new QVBoxLayout;
    helpLayout->addWidget(labelLessonNotices);
    helpLayout->addSpacing(14);
    helpLayout->addWidget(labelLessonUnit);
    helpLayout->addStretch(1);
	QVBoxLayout *controlLayout = new QVBoxLayout;
    controlLayout->addWidget(labelLessonName);
    controlLayout->addWidget(lineLessonName);
    controlLayout->addSpacing(10);
    controlLayout->addWidget(labelLessonDescription);
    controlLayout->addWidget(lineLessonDescription);
    controlLayout->addSpacing(10);
    controlLayout->addWidget(labelLessonContent);
    controlLayout->addWidget(lineLessonContent);
    controlLayout->addSpacing(10);
    controlLayout->addWidget(labelLessonUnitRadio);
    controlLayout->addLayout(boxesLayout);
    // Full layout of all widgets vertical
	QHBoxLayout *preLayout = new QHBoxLayout;
    preLayout->addLayout(controlLayout);
    preLayout->addSpacing(20);
    preLayout->addLayout(helpLayout);
    // Full layout of all widgets vertical
	QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(preLayout);
    mainLayout->addSpacing(1);
    mainLayout->addLayout(buttonLayoutHorizontal);
    mainLayout->setMargin(15);
    mainLayout->setSpacing(5);
    // Pass layout to parent widget (this)
	this->setLayout(mainLayout);
}

void LessonDialog::clickSave() {
	StartSql *startSql;
	QStringList contentList;
	int lessonUnit = 0;
	int i;

	// Check whether lesson name is filled out
	if (lineLessonName->text() == "") {
		QMessageBox::information(this, APP_NAME,
			tr("Bitte geben Sie der Lektion einen Namen\n"));
		return;
	}
	// Check whether lesson content is filled out
	if (lineLessonContent->toPlainText() == "") {
		QMessageBox::information(this, APP_NAME,
			tr("Bitte geben Sie ein vollstaendiges Diktat ein\n"));
		return;
	}

	// Split lesson content to lines
	contentList = lineLessonContent->toPlainText().split("\n", QString::SkipEmptyParts);
	// Delete empty lines
	for (i = 0; i < contentList.size(); i++) {
		if (QString::QString(contentList.at(i).toLocal8Bit().constData()).simplified() == "") {
			contentList.removeAt(i);
		}
	}
	// Check whether there is enough lesson content
    if (contentList.size() < 2) {
		QMessageBox::information(this, APP_NAME,
            tr("Bitte geben Sie mindestens zwei Zeilen Diktat ein\n"));
		return;
	}
    // Check whether there is too much lesson content
	if (contentList.size() > 400) {
		QMessageBox::information(this, APP_NAME,
			tr("Bitte geben Sie maximal 400 Zeilen Diktat ein\n"));
		return;
	}
	// Save the lesson
	startSql = new StartSql();
	if ((currentLessonId == "-1" || currentLessonId == "-2") &&
		startSql->ownLessonExist(lineLessonName->text())) {
		QMessageBox::information(this, APP_NAME,
			tr("Der Name der Lektion existiert bereits. Bitte geben "
				"Sie der Lektion einen anderen Namen.\n"));
		return;
	}
	if (radioUnitWord->isChecked()) {
		lessonUnit = 1;
	}
	if (!startSql->updateOwnLesson(currentLessonId, lineLessonName->text(),
		lineLessonDescription->text(), contentList, lessonUnit)) {
		// No selected lesson found in combo box
		// -> error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_USER_LESSON_ADD, TYPE_INFO,
			CANCEL_OPERATION);
		return;
	}
    if (!startSql->analyzeOwnLessons()) {
		// No selected lesson found in combo box
		// -> error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_USER_LESSON_ANALYZE, TYPE_INFO,
			CANCEL_OPERATION);
		return;
	}
    this->accept();
}

void LessonDialog::showHelp() {
	helpBrowser = new HelpBrowser("lessons.html#ownlesson", this);
	helpBrowser->show();
}
