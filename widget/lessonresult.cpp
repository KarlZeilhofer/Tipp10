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
** Implementation of the LessonResult class
** File name: lessonresult.h
**
****************************************************************/

#include <QObject>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QColor>
#include <QFont>
#include <QDateTime>
#include <QSettings>
#include <QTextEdit>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextCursor>
#include <QTextFrame>
#include <QTextTable>
#include <QTextFrameFormat>
#include <QTextCharFormat>
#include <QTextTableFormat>
#include <QCoreApplication>
#include <QString>
#include <QList>
#include <QChar>
#include <QPen>
#include <QColor>
#include <QUrl>
#include <QDesktopServices>
#include <math.h>

#include "lessonresult.h"
#include "lessonprintdialog.h"
#include "def/defines.h"

LessonResult::LessonResult(int row, int type, QList<QChar> charlist,
	QList<int> mistakelist, QWidget *parent) : QWidget(parent) {

	mistakeList = mistakelist;
	charList = charlist;
	lessonRow = row;

	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
    settings.beginGroup("general");
	// Language
	language = settings.value("language_gui", "en").toString();
	settings.endGroup();

	// Create print push button
	buttonPrintLesson = new QPushButton(tr("Drucken"));
    buttonPrintLesson->setFixedHeight(20);
    buttonPublishFacebook = new QPushButton(tr("Teilen auf Facebook"));
    buttonPublishFacebook->setFixedHeight(20);

    createOutput();
}

void LessonResult::publishFacebook() {
    QDesktopServices::openUrl(QString("http://www.facebook.com/sharer.php"
                                      "?u="
                                      "http://www.tipp10.com/" + languageGui + "/facebook/" +
                                      QUrl::toPercentEncoding(
                                              facebookComment)));
}

void LessonResult::createOutput() {

	QString userName = "";


	// Read settings

	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
	settings.beginGroup("duration");
	QString settingsDuration;
	if (settings.value("radio_time", true).toBool()) {
		// Time limit selected
		settingsDuration = settings.value("spin_time", LESSON_TIMELEN_STANDARD).toString() +
			tr(" Minuten");
	} else {
		if (settings.value("radio_token", true).toBool()) {
			// Token limit selected
			settingsDuration = settings.value("spin_token", LESSON_TOKENLEN_STANDARD).toString() +
				tr(" Zeichen");
		} else {
			// Lesson limit selected
			settingsDuration = tr("Gesamte Lektion");
		}
	}
	settings.endGroup();
	settings.beginGroup("error");
	QString settingsError;
	if (settings.value("check_correct", true).toBool()) {
		settingsError = tr("Fehler korrigieren");
	} else {
		if (settings.value("check_stop", true).toBool()) {
			settingsError = tr("Diktat blockieren");
		} else {
			settingsError = tr("Tippfehler uebergehen");
		}
	}
	settings.endGroup();
	QString settingsHelp;
	settings.beginGroup("support");
	if (!settings.value("check_helpers", true).toBool()) {

		settingsHelp = tr("Keine");
	} else {
		if (settings.value("check_selection", true).toBool() &&
			settings.value("check_selection_start", true).toBool() &&
			settings.value("check_border", true).toBool() &&
			settings.value("check_path", true).toBool() &&
			settings.value("check_status", true).toBool()) {

			settingsHelp = tr("Alle");
		} else {

			settingsHelp = "";
			if (settings.value("check_selection", true).toBool()) {
				if (settingsHelp != "") {
					settingsHelp.append("\n");
				}
				settingsHelp.append(tr("- Farbige Tasten"));
			}
			if (settings.value("check_selection_start", true).toBool()) {
				if (settingsHelp != "") {
					settingsHelp.append("\n");
				}
				settingsHelp.append(tr("- Grundstellung"));
			}
			if (settings.value("check_path", true).toBool()) {
				if (settingsHelp != "") {
					settingsHelp.append("\n");
				}
				settingsHelp.append(tr("- Tastpfade"));
			}
			if (settings.value("check_border", true).toBool()) {
				if (settingsHelp != "") {
					settingsHelp.append("\n");
				}
				settingsHelp.append(tr("- Trennlinie"));
			}
			if (settings.value("check_status", true).toBool()) {
				if (settingsHelp != "") {
					settingsHelp.append("\n");
				}
				settingsHelp.append(tr("- Hilfetext"));
			}
		}
	}
	settings.endGroup();
    settings.beginGroup("general");
    languageGui = settings.value("language_gui",
        APP_STD_LANGUAGE_GUI).toString();
    settings.endGroup();
	// Read lesson results

	QString lessonName;
	QString lessonTimestamp;
	QString lessonTimeLen;
	QString lessonTokenLen;
	QString lessonErrorNum;
    QString lessonCpm;
    QString lessonGrade;
    QString lessonGradeSimple;
    QString lessonRate;

	QSqlQuery query;
	if (!query.exec("SELECT user_lesson_name, user_lesson_timestamp, "
		"user_lesson_timelen, user_lesson_tokenlen, "
        "user_lesson_errornum, (user_lesson_strokesnum / "
        "(user_lesson_timelen / 60.0)) AS user_lesson_cpm, "
        "(((user_lesson_strokesnum - (20.0 * user_lesson_errornum)) / "
        "(user_lesson_timelen / 60.0)) * 0.4) AS user_lesson_grade, "
        "user_lesson_id, "
        "((user_lesson_errornum * 100.0) / "
        "user_lesson_strokesnum) AS user_lesson_rate "
        "FROM user_lesson_list WHERE "
		"user_lesson_id = " + QString::number(lessonRow) + ";")) {
		return;
	}
	if (query.first()) {
		lessonName = query.value(0).toString();
		QDateTime timeStampTemp = QDateTime::fromString(query.value(1).toString(),
			"yyyyMMddhhmmss");
		lessonTimestamp = timeStampTemp.toString(
            (language == "de" ? "dd.MM.yyyy hh:mm" : "MMM d, yyyy hh:mm ap")) +
			(language == "de" ? tr(" Uhr") : "");
		int timeSecTemp;
		double timeMinTemp;
		if ((timeSecTemp = query.value(2).toInt()) < 60) {
			lessonTimeLen = QString::number(timeSecTemp) + tr(" sek.");
		} else {
			timeMinTemp = floor((timeSecTemp / 60.0) / 0.1 + 0.5) * 0.1;
			lessonTimeLen = QString::number(timeMinTemp) + tr(" min.");
		}
		lessonTokenLen = query.value(3).toString();
		lessonErrorNum = query.value(4).toString();
        double lessonRateTemp = query.value(8).toDouble();
        lessonRate.sprintf("%.0f", lessonRateTemp);
        lessonRate.append(" %");
        double lessonCpmTemp = query.value(5).toDouble();
        lessonCpm.sprintf("%.0f", lessonCpmTemp);
        double lessonGradeTemp;
		if ((lessonGradeTemp = query.value(6).toDouble()) < 0) {
			lessonGradeTemp = 0;
		}
        lessonGrade.sprintf("%.0f", lessonGradeTemp);
        lessonGradeSimple.sprintf("%.0f", lessonGradeTemp);
        lessonGrade.append(lessonGradeTemp == 1 ? tr(" Punkt") :  tr(" Punkte"));
	}

	// Output the results

	QTextEdit *editor = new QTextEdit;

	QTextCursor cursor(editor->textCursor());
	cursor.movePosition(QTextCursor::Start);
	QTextFrame *topFrame = cursor.currentFrame();
	QTextFrameFormat topFrameFormat = topFrame->frameFormat();
	topFrameFormat.setLeftMargin(16);
	topFrameFormat.setTopMargin(3);
	topFrameFormat.setRightMargin(16);
	topFrameFormat.setBottomMargin(16);
	topFrameFormat.setBorder(0);
	topFrameFormat.setPadding(0);
	topFrame->setFrameFormat(topFrameFormat);

	QTextCharFormat h1;
	QTextCharFormat h2;
	QTextCharFormat h3;
	QTextCharFormat p;
	QTextCharFormat p_bold;
	QTextCharFormat p_mistake;
	QTextCharFormat p_no_mistake;

	#if APP_MAC || APP_X11
	h1.setFontPointSize(22);
	h2.setFontPointSize(19);
	h3.setFontPointSize(13);
	p.setFontPointSize(13);
	p_mistake.setFontPointSize(13);
	p_no_mistake.setFontPointSize(13);
	#else
	h1.setFontPointSize(18);
	h2.setFontPointSize(14);
	h3.setFontPointSize(9);
	p.setFontPointSize(9);
	p_mistake.setFontPointSize(9);
	p_no_mistake.setFontPointSize(9);
	#endif

	h1.setFontFamily("Arial");
	h2.setFontFamily("Arial");
	h3.setFontFamily("Arial");
	h3.setFontWeight(QFont::Bold);
	p.setFontFamily("Arial");
	p_mistake.setFontFamily("Arial");
	p_mistake.setFontUnderline(true);
	p_mistake.setForeground(QBrush(QColor(255, 0, 0)));
	p_mistake.setFontWeight(QFont::Bold);
	p_no_mistake.setFontFamily("Arial");

	cursor.setPosition(topFrame->firstPosition());

	QTextFrameFormat headFrameFormat;
	headFrameFormat.setWidth(QTextLength(QTextLength::PercentageLength,  100));
	headFrameFormat.setBorder(0);
	headFrameFormat.setMargin(0);
	headFrameFormat.setPosition(QTextFrameFormat::FloatLeft);
    cursor.insertFrame(headFrameFormat);

	cursor.insertText(tr("Sie haben ") + lessonGrade +
		tr(" erreicht, bei einer Schreibgeschwindigkeit von ") +
		lessonCpm + tr(" A/min und ") + lessonErrorNum +
		(lessonErrorNum == "1" ? tr(" Tippfehler") :  tr(" Tippfehlern")) +
		tr("."), h2);

    facebookComment = lessonGradeSimple + "-" + lessonCpm + "-" + lessonErrorNum;
                      /*tr("Ich habe ") + lessonGrade +
                      tr(" erreicht, bei einer Schreibgeschwindigkeit von ") +
                      lessonCpm + tr(" A/min und ") + lessonErrorNum +
                      (lessonErrorNum == "1" ? tr(" Tippfehler") :  tr(" Tippfehlern")) +
                      tr(".") + " * * * * " +
                      tr("Mit TIPP10 kostenlos das Zehnfingersystem lernen: ") +
                      QString(APP_URL);*/

	cursor.setPosition(topFrame->lastPosition());

	QTextFrameFormat referenceFrameFormat;
	referenceFrameFormat.setBorder(1);
	referenceFrameFormat.setTopMargin(13);
	referenceFrameFormat.setPadding(8);
	referenceFrameFormat.setPosition(QTextFrameFormat::FloatRight);
	referenceFrameFormat.setWidth(QTextLength(QTextLength::PercentageLength, 50));
	cursor.insertFrame(referenceFrameFormat);

	cursor.insertText(tr("Einstellungen"), h3);


	QTextTableFormat settingsTableFormat;
	settingsTableFormat.setAlignment(Qt::AlignLeft);
	QTextTable *settingsTable = cursor.insertTable(3, 2, settingsTableFormat);

	QTextFrameFormat settingsFrameFormat = cursor.currentFrame()->frameFormat();
	settingsFrameFormat.setBorder(0);
	cursor.currentFrame()->setFrameFormat(settingsFrameFormat);

	cursor = settingsTable->cellAt(0, 0).firstCursorPosition();
	cursor.insertText(tr("Dauer: "), p);
	cursor = settingsTable->cellAt(0, 1).firstCursorPosition();
	cursor.insertText(settingsDuration, p);
	cursor = settingsTable->cellAt(1, 0).firstCursorPosition();
	cursor.insertText(tr("Tippfehler: "), p);
	cursor = settingsTable->cellAt(1, 1).firstCursorPosition();
	cursor.insertText(settingsError, p);
	cursor = settingsTable->cellAt(2, 0).firstCursorPosition();
	cursor.insertText(tr("Hilfestellungen: "), p);
	cursor = settingsTable->cellAt(2, 1).firstCursorPosition();
	cursor.insertText(settingsHelp, p);


	cursor.setPosition(topFrame->lastPosition());


	QTextFrameFormat resultsFrameFormat;
	resultsFrameFormat.setBorder(1);
	resultsFrameFormat.setTopMargin(13);
	resultsFrameFormat.setPadding(8);
	resultsFrameFormat.setPosition(QTextFrameFormat::FloatLeft);
	resultsFrameFormat.setWidth(QTextLength(QTextLength::PercentageLength, 48));
	cursor.insertFrame(resultsFrameFormat);

	cursor.insertText(tr("Auswertung"), h3);

	QTextTableFormat evaluationTableFormat;
	evaluationTableFormat.setAlignment(Qt::AlignLeft);
    QTextTable *evaluationTable = cursor.insertTable(7, 2, evaluationTableFormat);

	QTextFrameFormat evaluationFrameFormat = cursor.currentFrame()->frameFormat();
	evaluationFrameFormat.setBorder(0);
	cursor.currentFrame()->setFrameFormat(evaluationFrameFormat);

	/*cursor = evaluationTable->cellAt(0, 0).firstCursorPosition();
	cursor.insertText(tr("Name: "), p);
	cursor = evaluationTable->cellAt(0, 1).firstCursorPosition();
	cursor.insertText(userName, p);*/
	cursor = evaluationTable->cellAt(0, 0).firstCursorPosition();
	cursor.insertText(tr("Lektion: "), p);
	cursor = evaluationTable->cellAt(0, 1).firstCursorPosition();
	cursor.insertText(lessonName, p);
	cursor = evaluationTable->cellAt(1, 0).firstCursorPosition();
	cursor.insertText(tr("Zeitpunkt: "), p);
	cursor = evaluationTable->cellAt(1, 1).firstCursorPosition();
	cursor.insertText(lessonTimestamp, p);
	cursor = evaluationTable->cellAt(2, 0).firstCursorPosition();
	cursor.insertText(tr("Dauer: "), p);
	cursor = evaluationTable->cellAt(2, 1).firstCursorPosition();
	cursor.insertText(lessonTimeLen, p);
	cursor = evaluationTable->cellAt(3, 0).firstCursorPosition();
	cursor.insertText(tr("Zeichen: "), p);
	cursor = evaluationTable->cellAt(3, 1).firstCursorPosition();
	cursor.insertText(lessonTokenLen, p);
	cursor = evaluationTable->cellAt(4, 0).firstCursorPosition();
	cursor.insertText(tr("Fehler: "), p);
	cursor = evaluationTable->cellAt(4, 1).firstCursorPosition();
	cursor.insertText(lessonErrorNum, p);
    cursor = evaluationTable->cellAt(5, 0).firstCursorPosition();
    cursor.insertText(tr("Fehlerquote: "), p);
    cursor = evaluationTable->cellAt(5, 1).firstCursorPosition();
    cursor.insertText(lessonRate, p);
    cursor = evaluationTable->cellAt(6, 0).firstCursorPosition();
	cursor.insertText(tr("A/min: "), p);
    cursor = evaluationTable->cellAt(6, 1).firstCursorPosition();
	cursor.insertText(lessonCpm, p);


	cursor.setPosition(topFrame->lastPosition());

	QTextFrameFormat bottomFrameFormat;
	bottomFrameFormat.setBorder(1);
	bottomFrameFormat.setPadding(8);
	bottomFrameFormat.setTopMargin(12);
	bottomFrameFormat.setWidth(QTextLength(QTextLength::PercentageLength, 100));
	bottomFrameFormat.setPosition(QTextFrameFormat::FloatLeft);
    cursor.insertFrame(bottomFrameFormat);

	cursor.insertText(tr("Diktat"), h3);
	cursor.insertBlock();

	for (int i = 0; i < charList.size(); ++i) {
		if (mistakeList.at(i) == 0) {
			cursor.insertText(QString(charList.at(i)), p_no_mistake);
		} else {
			cursor.insertText(QString(charList.at(i)), p_mistake);
		}
	}

	cursor.setPosition(QTextCursor::Start);
	editor->setReadOnly(true);

	QHBoxLayout *filterLayout = new QHBoxLayout;
    filterLayout->addStretch(1);
    filterLayout->addWidget(buttonPublishFacebook);
    filterLayout->addWidget(buttonPrintLesson);
    QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(filterLayout);
	mainLayout->addWidget(editor);
	// Pass layout to parent widget (this)
	this->setLayout(mainLayout);

    connect(buttonPrintLesson, SIGNAL(clicked()), this, SLOT(createPrintOutput()));
    connect(buttonPublishFacebook, SIGNAL(clicked()), this, SLOT(publishFacebook()));

}

void LessonResult::createPrintOutput() {

	QString userName = "";

	LessonPrintDialog lessonPrintDialog(&userName, this);
	if (lessonPrintDialog.exec() == 0) {
		return;
	}

	// Read settings

	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
	settings.beginGroup("duration");
	QString settingsDuration;
	if (settings.value("radio_time", true).toBool()) {
		// Time limit selected
		settingsDuration = settings.value("spin_time", LESSON_TIMELEN_STANDARD).toString() +
			tr(" Minuten");
	} else {
		if (settings.value("radio_token", true).toBool()) {
			// Token limit selected
			settingsDuration = settings.value("spin_token", LESSON_TOKENLEN_STANDARD).toString() +
				tr(" Zeichen");
		} else {
			// Lesson limit selected
			settingsDuration = tr("Gesamte Lektion");
		}
	}
	settings.endGroup();
	settings.beginGroup("error");
	QString settingsError;
	if (settings.value("check_correct", true).toBool()) {
		settingsError = tr("Fehler korrigieren");
	} else {
		if (settings.value("check_stop", true).toBool()) {
			settingsError = tr("Diktat blockieren");
		} else {
			settingsError = tr("Tippfehler uebergehen");
		}
	}
	settings.endGroup();
	QString settingsHelp;
	settings.beginGroup("support");
	if (!settings.value("check_helpers", true).toBool()) {

		settingsHelp = tr("Keine");
	} else {
		if (settings.value("check_selection", true).toBool() &&
			settings.value("check_selection_start", true).toBool() &&
			settings.value("check_border", true).toBool() &&
			settings.value("check_path", true).toBool() &&
			settings.value("check_status", true).toBool()) {

			settingsHelp = tr("Alle");
		} else {

			settingsHelp = "";
			if (settings.value("check_selection", true).toBool()) {
				if (settingsHelp != "") {
					settingsHelp.append("\n");
				}
				settingsHelp.append(tr("- Farbige Tasten"));
			}
			if (settings.value("check_selection_start", true).toBool()) {
				if (settingsHelp != "") {
					settingsHelp.append("\n");
				}
				settingsHelp.append(tr("- Grundstellung"));
			}
			if (settings.value("check_path", true).toBool()) {
				if (settingsHelp != "") {
					settingsHelp.append("\n");
				}
				settingsHelp.append(tr("- Tastpfade"));
			}
			if (settings.value("check_border", true).toBool()) {
				if (settingsHelp != "") {
					settingsHelp.append("\n");
				}
				settingsHelp.append(tr("- Trennlinie"));
			}
			if (settings.value("check_status", true).toBool()) {
				if (settingsHelp != "") {
					settingsHelp.append("\n");
				}
				settingsHelp.append(tr("- Hilfetext"));
			}
		}
	}
	settings.endGroup();

	// Read lesson results

	QString lessonName;
	QString lessonTimestamp;
	QString lessonTimeLen;
	QString lessonTokenLen;
	QString lessonErrorNum;
	QString lessonCpm;
	QString lessonGrade;
    QString lessonRate;

	QSqlQuery query;
	if (!query.exec("SELECT user_lesson_name, user_lesson_timestamp, "
		"user_lesson_timelen, user_lesson_tokenlen, "
		"user_lesson_errornum, ROUND(user_lesson_strokesnum / "
		"(user_lesson_timelen / 60.0), 0) AS user_lesson_cpm, "
		"ROUND(((user_lesson_strokesnum - (20 * user_lesson_errornum)) / "
		"(user_lesson_timelen / 60.0)) * 0.4, 0) AS user_lesson_grade, "
        "user_lesson_id, "
        "((user_lesson_errornum * 100.0) / "
        "user_lesson_strokesnum) AS user_lesson_rate "
        "FROM user_lesson_list WHERE "
		"user_lesson_id = " + QString::number(lessonRow) + ";")) {
		return;
	}
	if (query.first()) {
		lessonName = query.value(0).toString();
		QDateTime timeStampTemp = QDateTime::fromString(query.value(1).toString(),
			"yyyyMMddhhmmss");
		lessonTimestamp = timeStampTemp.toString(
            (language == "de" ? "dd.MM.yyyy hh:mm" : "MMM d, yyyy hh:mm ap")) +
			(language == "de" ? tr(" Uhr") : "");
		int timeSecTemp;
		double timeMinTemp;
		if ((timeSecTemp = query.value(2).toInt()) < 60) {
			lessonTimeLen = QString::number(timeSecTemp) + tr(" sek.");
		} else {
			timeMinTemp = floor((timeSecTemp / 60.0) / 0.1 + 0.5) * 0.1;
			lessonTimeLen = QString::number(timeMinTemp) + tr(" min.");
		}
		lessonTokenLen = query.value(3).toString();
		lessonErrorNum = query.value(4).toString();
        double lessonRateTemp = query.value(8).toDouble();
        lessonRate.sprintf("%.0f", lessonRateTemp);
        lessonRate.append(" %");
        double lessonCpmTemp = query.value(5).toDouble();
        lessonCpm.sprintf("%.0f", lessonCpmTemp);
        double lessonGradeTemp;
        if ((lessonGradeTemp = query.value(6).toDouble()) < 0) {
            lessonGradeTemp = 0;
        }
        lessonGrade.sprintf("%.0f", lessonGradeTemp);
        lessonGrade.append(lessonGradeTemp == 1 ? tr(" Punkt") :  tr(" Punkte"));
	}

	// Output the results

	QTextEdit *editor = new QTextEdit;

	QTextCursor cursor(editor->textCursor());
	cursor.movePosition(QTextCursor::Start);
	QTextFrame *topFrame = cursor.currentFrame();
	QTextFrameFormat topFrameFormat = topFrame->frameFormat();
	topFrameFormat.setPadding(16);
	topFrameFormat.setBorder(0);
	topFrame->setFrameFormat(topFrameFormat);

	QTextCharFormat h1;
	QTextCharFormat h2;
	QTextCharFormat h3;
	QTextCharFormat p;
	QTextCharFormat p_bold;
	QTextCharFormat p_mistake;
	QTextCharFormat p_no_mistake;

	h1.setFontPointSize(20);
	h1.setFontFamily("Arial");
	h2.setFontPointSize(15);
	h2.setFontFamily("Arial");
	h2.setFontWeight(QFont::Bold);
	h3.setFontPointSize(11);
	h3.setFontFamily("Arial");
	h3.setFontWeight(QFont::Bold);
	p.setFontPointSize(11);
	p.setFontFamily("Arial");
	p_mistake.setFontPointSize(11);
	p_mistake.setFontFamily("Arial");
	p_mistake.setFontUnderline(true);
	p_mistake.setForeground(QBrush(QColor(255, 0, 0)));
	p_mistake.setFontWeight(QFont::Bold);
	p_no_mistake.setFontPointSize(11);
	p_no_mistake.setFontFamily("Arial");

	cursor.setPosition(topFrame->firstPosition());

	QTextFrameFormat headFrameFormat;
	headFrameFormat.setWidth(QTextLength(QTextLength::PercentageLength, 100));
	headFrameFormat.setBorder(0);
	headFrameFormat.setPadding(0);
	headFrameFormat.setPosition(QTextFrameFormat::FloatLeft);
    cursor.insertFrame(headFrameFormat);

    cursor.insertText(tr("10-Finger-Schreibtrainer TIPP10"), h1);
    cursor.insertBlock();
	cursor.insertText(tr("Bericht") +
		(userName != "" ? tr(" von ") + userName : ""), h1);
	cursor.insertBlock();
	cursor.insertBlock();
	cursor.insertText(tr("Sie haben ") + lessonGrade +
		tr(" erreicht, bei einer Schreibgeschwindigkeit von ") +
		lessonCpm + tr(" A/min und ") + lessonErrorNum +
		(lessonErrorNum == "1" ? tr(" Tippfehler") :  tr(" Tippfehlern")) +
		tr("."), h2);
	cursor.insertBlock();

	cursor.setPosition(topFrame->lastPosition());

	QTextFrameFormat referenceFrameFormat;
	referenceFrameFormat.setBorder(1);
	referenceFrameFormat.setPadding(8);
	referenceFrameFormat.setPosition(QTextFrameFormat::FloatRight);
	referenceFrameFormat.setWidth(QTextLength(QTextLength::PercentageLength, 50));
	cursor.insertFrame(referenceFrameFormat);

	cursor.insertText(tr("Einstellungen"), h3);
	cursor.insertBlock();

	QTextTableFormat settingsTableFormat;
	settingsTableFormat.setAlignment(Qt::AlignLeft);
	QTextTable *settingsTable = cursor.insertTable(3, 2, settingsTableFormat);

	QTextFrameFormat settingsFrameFormat = cursor.currentFrame()->frameFormat();
	settingsFrameFormat.setBorder(0);
	cursor.currentFrame()->setFrameFormat(settingsFrameFormat);

	cursor = settingsTable->cellAt(0, 0).firstCursorPosition();
	cursor.insertText(tr("Dauer: "), p);
	cursor = settingsTable->cellAt(0, 1).firstCursorPosition();
	cursor.insertText(settingsDuration, p);
	cursor = settingsTable->cellAt(1, 0).firstCursorPosition();
	cursor.insertText(tr("Tippfehler: "), p);
	cursor = settingsTable->cellAt(1, 1).firstCursorPosition();
	cursor.insertText(settingsError, p);
	cursor = settingsTable->cellAt(2, 0).firstCursorPosition();
	cursor.insertText(tr("Hilfestellungen: "), p);
	cursor = settingsTable->cellAt(2, 1).firstCursorPosition();
	cursor.insertText(settingsHelp, p);


	cursor.setPosition(topFrame->lastPosition());


	QTextFrameFormat resultsFrameFormat;
	resultsFrameFormat.setBorder(1);
	resultsFrameFormat.setPadding(8);
	resultsFrameFormat.setPosition(QTextFrameFormat::FloatLeft);
	resultsFrameFormat.setWidth(QTextLength(QTextLength::PercentageLength, 48));
	cursor.insertFrame(resultsFrameFormat);

	cursor.insertText(tr("Auswertung"), h3);

	QTextTableFormat evaluationTableFormat;
	evaluationTableFormat.setAlignment(Qt::AlignLeft);
    QTextTable *evaluationTable = cursor.insertTable(7, 2, evaluationTableFormat);

	QTextFrameFormat evaluationFrameFormat = cursor.currentFrame()->frameFormat();
	evaluationFrameFormat.setBorder(0);
	cursor.currentFrame()->setFrameFormat(evaluationFrameFormat);

	/*cursor = evaluationTable->cellAt(0, 0).firstCursorPosition();
	cursor.insertText(tr("Name: "), p);
	cursor = evaluationTable->cellAt(0, 1).firstCursorPosition();
	cursor.insertText(userName, p);*/
	cursor = evaluationTable->cellAt(0, 0).firstCursorPosition();
	cursor.insertText(tr("Lektion: "), p);
	cursor = evaluationTable->cellAt(0, 1).firstCursorPosition();
	cursor.insertText(lessonName, p);
	cursor = evaluationTable->cellAt(1, 0).firstCursorPosition();
	cursor.insertText(tr("Zeitpunkt: "), p);
	cursor = evaluationTable->cellAt(1, 1).firstCursorPosition();
	cursor.insertText(lessonTimestamp, p);
	cursor = evaluationTable->cellAt(2, 0).firstCursorPosition();
	cursor.insertText(tr("Dauer: "), p);
	cursor = evaluationTable->cellAt(2, 1).firstCursorPosition();
	cursor.insertText(lessonTimeLen, p);
	cursor = evaluationTable->cellAt(3, 0).firstCursorPosition();
	cursor.insertText(tr("Zeichen: "), p);
	cursor = evaluationTable->cellAt(3, 1).firstCursorPosition();
	cursor.insertText(lessonTokenLen, p);
	cursor = evaluationTable->cellAt(4, 0).firstCursorPosition();
	cursor.insertText(tr("Fehler: "), p);
	cursor = evaluationTable->cellAt(4, 1).firstCursorPosition();
    cursor.insertText(lessonErrorNum, p);
    cursor = evaluationTable->cellAt(5, 0).firstCursorPosition();
    cursor.insertText(tr("Fehlerquote: "), p);
    cursor = evaluationTable->cellAt(5, 1).firstCursorPosition();
    cursor.insertText(lessonRate, p);
    cursor = evaluationTable->cellAt(6, 0).firstCursorPosition();
    cursor.insertText(tr("A/min: "), p);
    cursor = evaluationTable->cellAt(6, 1).firstCursorPosition();
    cursor.insertText(lessonCpm, p);


	cursor.setPosition(topFrame->lastPosition());

	QTextFrameFormat bottomFrameFormat;
	bottomFrameFormat.setWidth(QTextLength(QTextLength::PercentageLength, 100));
	bottomFrameFormat.setPosition(QTextFrameFormat::FloatLeft);
    cursor.insertFrame(bottomFrameFormat);

	cursor.insertBlock();
	cursor.insertBlock();
	cursor.insertText(tr("Diktat"), h3);
	cursor.insertBlock();

	for (int i = 0; i < charList.size(); ++i) {
		if (mistakeList.at(i) == 0) {
			cursor.insertText(QString(charList.at(i)), p_no_mistake);
		} else {
			cursor.insertText(QString(charList.at(i)), p_mistake);
		}
	}

	cursor.insertBlock();
	cursor.setPosition(QTextCursor::Start);


	QPrinter printer;

	QPrintDialog *dialog = new QPrintDialog(&printer, this);
	dialog->setWindowTitle(tr("Bericht drucken"));

	if (dialog->exec() != QDialog::Accepted)
		return;

	editor->print(&printer);

}

