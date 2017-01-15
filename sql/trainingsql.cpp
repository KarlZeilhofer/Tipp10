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
** Implementation of the TrainingSql class
** File name: trainingsql.cpp
**
****************************************************************/

#include <QSqlQuery>
#include <QSqlDriver>
#include <QVariant>
#include <QSqlDatabase>
#include <QDateTime>
#include <QRegExp>
#include <QStringList>
#include <QRegExp>

#include "trainingsql.h"
#include "def/defines.h"

// Konstruktor
TrainingSql::TrainingSql(QString replace, QString regexp, QString layout) {
	setSynchron(SYNCHRON_DB_WHILE_TRAINING);
	queryCounter = 0;
    replaceSetting = replace;
    if (replaceSetting == "NULL") {
        replaceSetting = getKeyboardLayoutReplaceRoutine(layout);
    }
    regexpSetting = regexp;
    if (regexpSetting == "NULL") {
        regexpSetting = getKeyboardLayoutRegexpRoutine(layout);
    }
}

TrainingSql::~TrainingSql() {
	setSynchron(true);
}

int TrainingSql::getLessonUnit(int lesson, int type) {
	QSqlQuery query;
	int lessonUnit;
	QString tableName;
	QString sqlString;

	switch (type) {
		case 0:
			tableName = "lesson";
			break;
		case 1:
			tableName = "open";
			break;
		case 2:
			tableName = "own";
			break;
	}
	sqlString = "SELECT " + tableName + "_list." + tableName + "_id, " +
		tableName + "_list." + tableName + "_unit FROM " + tableName + "_list WHERE " +
		tableName + "_list." + tableName + "_id = " + QString::number(lesson) + ";";
	// Standard query to get the first lesson
	if (!query.exec(sqlString)) {
        return 0;
	}
	if (!query.first()) {
		return 0;
	}
	lessonUnit = query.value(1).toInt();
	return lessonUnit;
}

void TrainingSql::setSynchron(bool synchron) {
	QSqlQuery query;
	QString queryString;
	if (synchron) {
		queryString = "PRAGMA synchronous=FULL;";
	} else {
		queryString = "PRAGMA synchronous=OFF;";
	}
	query.exec(queryString);
}

bool TrainingSql::updateUsertable(QChar unicodechar, QString columnname) {
	QSqlQuery query;
	//int sqlNum = 0;
	// First a simple select statement to check whether the unicode exists
	// already in table
	/*if (!query.exec("SELECT COUNT(uerrorunicode) FROM usererrors WHERE "
		"uerrorunicode = " + QString::number(unicodechar.unicode()) + ";")) {
        return false;
	}*/

	// Now check the number of rows returned
	// Below commented is the full code to test wheter the current database
	// supports the feature "size()".
	// At the Moment a SQLite Driver Version 3.3.4 is in use - it doesn't
	// support the feature "size()". So we have to go to the end of the table
	// to count the number of rows returned as listet below after the commented
	// code.
	// This is slow - so change it if using an other database driver which
	// supports the feature!

	/*QSqlDatabase defaultDB = QSqlDatabase::database();

	//defaultDB.driver()->hasFeature(QSqlDriver::Transactions);
	if (defaultDB.driver()->hasFeature(QSqlDriver::QuerySize)) {
		sqlNum = query.size();
	} else {
		query.last();
		sqlNum = query.at() + 1;
	}*/

	// Slow function (look at the comment above)

	//sqlNum = query.at() + 1;
	/*query.first();
    if (query.value(0).toInt() == 0) {
		// There is no data record with the current unicode
		// -> insert new data record
		if (!query.exec("INSERT INTO usererrors VALUES("
			+ QString::number(unicodechar.unicode()) + ",0,0,0);")) {
			return false;
		}
	}*/
	// Now we keep save there is already a data record with the current unicode
	// -> update (increment) current data record
	QString charToString = QString::number(unicodechar.unicode());
	//query.exec("INSERT INTO usererrors VALUES(" + temp + ",0,0,0);");
	query.exec("BEGIN;");
	query.exec("INSERT OR IGNORE INTO user_chars VALUES(" + charToString +
		",0,0,0);");
	query.exec("UPDATE user_chars SET " + columnname + "="
		+ columnname + "+1 WHERE user_char_unicode = " + charToString + ";");
	if (!query.exec("COMMIT;")) {
		return false;
	}
	return true;
}

QString TrainingSql::createLesson(int lesson, int type, int unit,
	bool intelligence, bool useEszett, bool useUmlaut) {
	QSqlQuery query;
	QString sqlString = "";
	QString tableName = "";
	QString textOutput = "";
	QString textOutputSub = "";
	int counterToNewLine;

	switch (type) {
		case 0:
			tableName = "lesson_content";
			break;
		case 1:
			tableName = "open_content";
			break;
		case 2:
			tableName = "own_content";
			break;
	}
	sqlString = "SELECT " + tableName + ".content_id, " +
		tableName + ".content_text FROM " + tableName + " WHERE " +
		tableName + ".content_lesson = " + QString::number(lesson) +
		" ORDER BY " + tableName + ".content_id;";
	// Standard query to get the first lesson
	if (!query.exec(sqlString)) {
        return "";
	}
	if (intelligence) {
		if (!query.first()) {
			return "";
		}
		if (query.isNull(0)) {
			return "";
		}
		lessonsDoneList.prepend(query.value(0).toInt());
		textOutput = query.value(1).toString();
		textOutput = replaceRoutine(textOutput, useEszett, useUmlaut, lesson,
			type);
	} else {
		counterToNewLine = 0;
		// Without intelligence
		while (query.next()) {
			textOutputSub = query.value(1).toString();
			counterToNewLine += textOutputSub.length();
			if (unit == 0 || (unit == 1 &&
				counterToNewLine > NUM_TOKEN_UNTIL_NEW_LINE)) {
				textOutputSub.append(QChar(TOKEN_NEW_LINE));
				counterToNewLine = 0;
			} else {
				textOutputSub.append(" ");
				counterToNewLine++;
			}
			textOutput.append(replaceRoutine(textOutputSub, useEszett,
				useUmlaut, lesson, type));
		}
	}
	// return new lesson
	return textOutput;
}

QString TrainingSql::updateLesson(int lesson, int type, bool intelligence,
	bool useEszett, bool useUmlaut) {
	QSqlQuery query;
	QString errorUnicodeMaxFirst = "";
	QString errorUnicodeMaxSecond = "";
	QString errorUnicodeMaxThird = "";
	QString errorUnicodeMaxFourth = "";
	QString sqlString = "";
	bool errorsExist = false;
	QString tableName = "";
	QString analysisName = "";
	QString textOutput = "";
	QString textOutputSub = "";

	queryCounter++;

	switch (type) {
		case 0:
			tableName = "lesson_content";
			analysisName = "lesson_analysis";
			break;
		case 1:
			tableName = "open_content";
			analysisName = "open_analysis";
			break;
		case 2:
			tableName = "own_content";
			analysisName = "own_analysis";
			break;
	}
	if (intelligence) {
		// Check if user errors exist and take the unicode with the
		// highest error ratio
		if (query.exec("SELECT user_char_unicode, (user_char_target_errornum * "
			"100) / user_char_occur_num AS user_char_weighted "
			"FROM user_chars "
			"WHERE user_char_unicode > 31 AND user_char_unicode < 255 "
			"ORDER BY user_char_weighted DESC;")) {
			if (query.first()) {
				errorsExist = true;
				errorUnicodeMaxFirst = query.value(0).toString();
				if (query.next()) {
					errorUnicodeMaxSecond = query.value(0).toString();
					if (query.next()) {
						errorUnicodeMaxThird = query.value(0).toString();
						if (query.next()) {
							errorUnicodeMaxFourth = query.value(0).toString();
						}
					}
				}
			}
		} else {
			// SQL exec error
			return "";
		}
		sqlString = "SELECT " + tableName + ".content_id, " +
				tableName + ".content_text FROM " + tableName;

		if (errorsExist && (queryCounter % NUM_INTELLIGENT_QUERYS) != 0) {
			// User error exists
			// -> create lesson SQL string sorted by errorUnicodeMax
			sqlString.append(" LEFT JOIN " +
				analysisName + " ON " + tableName + ".content_id = " +
                analysisName + ".analysis_content AND ");
            if ((lesson % 100) != LAST_LESSON || type != 0) {
                sqlString.append(tableName + ".content_lesson = " + QString::number(lesson));
			} else {
				// Last training lesson over all previous lessons
                sqlString.append("(" + tableName + ".content_lesson % 100) >= " + QString::number(BORDER_LESSON_IS_SENTENCE) +
                    " AND (" +
                    tableName + ".content_lesson % 100) <= " + QString::number(LAST_LESSON));
			}
            sqlString.append(" AND (" + tableName + ".content_id % 1000) != 1 "
				"GROUP BY " + tableName + ".content_id");
			if (errorUnicodeMaxFirst != "") {
				sqlString.append(" ORDER BY analysis_char_"
					+ errorUnicodeMaxFirst + " DESC");
				if (errorUnicodeMaxSecond != "") {
					sqlString.append(", analysis_char_" + errorUnicodeMaxSecond
						+ " DESC");
				}
				if (errorUnicodeMaxThird != "") {
					sqlString.append(", analysis_char_" + errorUnicodeMaxThird
						+ " DESC");
				}
				if (errorUnicodeMaxFourth != "") {
					sqlString.append(", analysis_char_" + errorUnicodeMaxFourth
						+ " DESC");
				}
			}
			sqlString.append(", Random();");
		} else {
			// No user error exists
			// -> create standard lesson SQL string sorted by id
            sqlString.append(" WHERE");
            sqlString.append(" (" + tableName + ".content_id % 1000) != 0");
             if ((lesson % 100) != LAST_LESSON || type != 0) {
                 sqlString.append(" AND " + tableName + ".content_lesson = " + QString::number(lesson));
             } else {
                 // Last training lesson over all previous lessons
                 sqlString.append(" AND (" + tableName + ".content_lesson % 100) >= " + QString::number(BORDER_LESSON_IS_SENTENCE) +
                     " AND (" +
                     tableName + ".content_lesson % 100) <= " + QString::number(LAST_LESSON));
             }
            sqlString.append(" ORDER BY Random();");
		}
        //qDebug() << sqlString;
		// Execute SQL string created above
		if (query.exec(sqlString)) {
			// Find a lesson not occurs last time
			while (query.next()) {
				if (!lessonsDoneList.contains(query.value(0).toInt())) {
					// New unused lesson found
					// -> append to lessonsDoneList
					lessonsDoneList.prepend(query.value(0).toInt());
					// Check if lessonsDoneList exceeds its maximum
					if (lessonsDoneList.size() > NUM_TEXT_UNTIL_REPEAT) {
						// lessonsDoneList exceeded -> remove last item
						lessonsDoneList.removeLast();
					}
					textOutput = query.value(1).toString();
					// return new lesson
					return replaceRoutine(textOutput, useEszett, useUmlaut,
						lesson, type);
				}
			}
		} else {
			// SQL exec error
			return "";
		}
		// No unused lesson found
		// -> delete all items expect the first of lessonsDoneList
		while (lessonsDoneList.size() > 1) {
			lessonsDoneList.removeLast();
		}
		return updateLesson(lesson, type, intelligence, useEszett, useUmlaut);
	} else {
		// Without intelligence
		sqlString = "SELECT " + tableName + ".content_id, " +
			tableName + ".content_text FROM " + tableName + " WHERE " +
			tableName + ".content_lesson = " + QString::number(lesson) +
			" ORDER BY " + tableName + ".content_id;";
		// Standard query to get the first lesson
		if (!query.exec(sqlString)) {
			return "";
		}
		while (query.next()) {
			textOutputSub = query.value(1).toString();
			textOutputSub.append(QChar(TOKEN_NEW_LINE));

			textOutput.append(replaceRoutine(textOutputSub, useEszett,
				useUmlaut, lesson, type));
		}
		return textOutput;
	}
}

QVariant TrainingSql::saveLesson(int lesson, int timelen, int tokenlen,
	int charnum, int errornum, QDateTime timestamp, int type, QString name) {
	QSqlQuery query;
	QVariant lastRowId;
	QString lessonName;
    lessonName = name;
    if (!query.exec("INSERT INTO user_lesson_list VALUES(NULL," +
		QString::number(lesson) + "," + QString::number(timelen) + "," +
		QString::number(tokenlen) + "," + QString::number(charnum) + "," +
		QString::number(errornum) + ", '" +
		timestamp.toString("yyyyMMddhhmmss") + "', " +
		QString::number(type) + ", '" + lessonName + "');")) {
		return QVariant::Invalid;
	}
	lastRowId = query.lastInsertId();
	return lastRowId;
}

QString TrainingSql::replaceRoutine(QString content, bool useEszett,
	bool useUmlaut, int lesson, int type) {

	// Tab char (an arrow)
	content.replace(QChar(0x9), QChar(TOKEN_TAB), Qt::CaseSensitive);

	// Eszett
    /*if (!useEszett) {
		content.replace(QChar(0x00df), "ss", Qt::CaseSensitive);
	}

	// Umlaut
	if (!useUmlaut) {
		content.replace(QChar(0x00df), "ss", Qt::CaseSensitive);
		content.replace(QChar(0x00e4), "ae", Qt::CaseSensitive);
		content.replace(QChar(0x00f6), "oe", Qt::CaseSensitive);
		content.replace(QChar(0x00fc), "ue", Qt::CaseSensitive);
		content.replace(QChar(0x00c4), "Ae", Qt::CaseSensitive);
		content.replace(QChar(0x00d6), "Oe", Qt::CaseSensitive);
		content.replace(QChar(0x00dc), "Ue", Qt::CaseSensitive);
		content.replace(QChar(0x20ac), "Eur", Qt::CaseSensitive);
		content.replace(QChar(0x00a7), "Par.", Qt::CaseSensitive);
        if ((lesson % 100) > LAST_LESSON && type == 0) {
			content.replace(QChar(0x002c), ".", Qt::CaseSensitive);
		}
    }*/
    if (!useUmlaut) {
        if ((lesson % 100) > LAST_LESSON && type == 0) {
            content.replace(QChar(0x002c), ".", Qt::CaseSensitive);
        }
    }
    if (replaceSetting != "") {
        QStringList replaceList = replaceSetting.split(",", QString::SkipEmptyParts);
        QStringList searchReplace;

        for (int i = 0; i < replaceList.size(); ++i) {
             searchReplace = replaceList.at(i).split("=", QString::SkipEmptyParts);
             if (searchReplace.size() >= 2) {
                 content.replace(searchReplace.at(0), searchReplace.at(1), Qt::CaseSensitive);
             }
        }
    }
    if (regexpSetting.left(2) == "[^") {
        regexpSetting = regexpSetting.left(2) + QChar(TOKEN_NEW_LINE) +
                         QChar(TOKEN_TAB) + regexpSetting.mid(2);
        content.replace(QRegExp(regexpSetting), "");
    }
	return content;

}

QString TrainingSql::getKeyboardLayoutReplaceRoutine(QString layout) {

    QSqlQuery query;
    QString replaceRoutine = "";
    if (!query.exec("SELECT layout_replace "
        "FROM language_layouts "
        "WHERE layout_key = '" + layout + "';")) {
        return "";
    }
    if (query.first()) {
        replaceRoutine = query.value(0).toString();
    }
    return replaceRoutine;
}

QString TrainingSql::getKeyboardLayoutRegexpRoutine(QString layout) {

    QSqlQuery query;
    QString regexpRoutine = "";

    if (!query.exec("SELECT layout_regexp "
        "FROM language_layouts "
        "WHERE layout_key = '" + layout + "';")) {
        return "";
    }
    if (query.first()) {
        regexpRoutine = query.value(0).toString();
    }
    return regexpRoutine;
}

