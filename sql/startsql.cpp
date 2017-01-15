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
** Implementation of the StartSql class
** File name: startsql.cpp
**
****************************************************************/

#include <QSqlQuery>
#include <QSqlDriver>
#include <QVariant>
#include <QSqlDatabase>
#include <QDateTime>
#include <QIcon>
#include <QListWidgetItem>
#include <QChar>
#include <QEventLoop>
#include <QApplication>
#include <QHash>
#include <QHashIterator>

#include "startsql.h"
#include "def/defines.h"

// Konstruktor
StartSql::StartSql() {
}

int StartSql::fillLessonList(QListWidget *listLesson, QList<QString> *arrayTraining,
                             QString languageLesson) {
	// Clear the list first because this function is called not only once in
	// the contructor but also after a successful online update
	listLesson->clear();
	arrayTraining->clear();
	// Fill lesson list from SQLite lesson table
	QSqlQuery query;
	QString sqlString;
	QString lessonId;
	QString lessonName;
	QString lessonDescription;
	QListWidgetItem *currentItem;
	QIcon lessonIcon;
	int lessonCounter = 0;
	// SQL: all lessons sorted by id and a left joint to the number of
	// lessons done by the user
	sqlString = "SELECT lesson_list.lesson_id, lesson_list.lesson_name, "
		"lesson_list.lesson_description, "
		"COUNT(user_lesson_list.user_lesson_lesson) FROM lesson_list "
		"LEFT JOIN user_lesson_list ON lesson_list.lesson_id = "
        "user_lesson_list.user_lesson_lesson AND user_lesson_list.user_lesson_type = 0 "
        "WHERE lesson_list.lesson_language = '" + languageLesson + "' ";
    sqlString = sqlString.append("GROUP BY lesson_list.lesson_id;");
    if (!query.exec(sqlString)) {
		return -1;
	}
	// Read all datasets to list items
	while (query.next()) {
		// ID of the lesson
		lessonId = query.value(0).toString();
		// Name of the lesson
		lessonName = query.value(1).toString();
		// Maybe a additional description
		lessonDescription = query.value(2).toString();
		// Maybe a additional description -> show it in brackets
        if (query.value(2).toString() != "") {
			lessonName.append(" (" + lessonDescription + ")");
		}
        // Show different icons depending on number of lessons done
		if (query.value(3).toInt() > 2) {
			// Lesson done multiple
			lessonIcon = QIcon(":/img/lesson_done_three.png");
		} else {
			if (query.value(3).toInt() == 2) {
				// Lesson done multiple
				lessonIcon = QIcon(":/img/lesson_done_two.png");
			} else {
				if (query.value(3).toInt() == 1) {
					// Lesson done once
					lessonIcon = QIcon(":/img/lesson_done_one.png");
				} else {
					// Lesson never done
					lessonIcon = QIcon(":/img/lesson_done_none.png");
				}
			}
		}
		// Add Item to the list
		currentItem = new QListWidgetItem(lessonIcon, lessonName, listLesson);
		currentItem->setToolTip(lessonDescription);
		arrayTraining->append(lessonId);
		lessonCounter++;
	}
	return lessonCounter;
}

int StartSql::fillOpenList(QListWidget *listOpen, QList<QString> *arrayOpen,
                           QString themeId, QString languageLesson) {
	// Clear the list first because this function is called not only once in
	// the contructor but also after a successful online update
	listOpen->clear();
	arrayOpen->clear();
	// Fill lesson list from SQLite lesson table
	QSqlQuery query;
	QString lessonName;
	QString lessonDescription;
	QListWidgetItem *currentItem;
	QString lessonId;
	QIcon openIcon;
	int lessonCounter = 0;
	QString themeAll = "WHERE open_list.open_theme = " + themeId + " ";
	if (themeId == "0" || themeId == "") {
		themeAll = "";
	}
	// SQL: all lessons sorted by id and a left joint to the number of
	// lessons done by the user
	if (!query.exec("SELECT open_list.open_id, open_list.open_name, "
		"open_list.open_description, "
		"COUNT(user_lesson_list.user_lesson_lesson) FROM open_list "
		"LEFT JOIN user_lesson_list ON open_list.open_id = "
		"user_lesson_list.user_lesson_lesson AND user_lesson_list.user_lesson_type = 1 "
		+ themeAll + "GROUP BY open_list.open_name;")) {
		return -1;
	}
	// Read all datasets to list items
	while (query.next()) {
		// ID of the lesson
		lessonId = query.value(0).toString();
		// Name of the lesson
		lessonName = query.value(1).toString();
		// Maybe a additional description
		lessonDescription = query.value(2).toString();
		// Show different icons depending on number of lessons done
		if (query.value(3).toInt() > 2) {
			// Lesson done multiple
			openIcon = QIcon(":/img/lesson_done_three.png");
		} else {
			if (query.value(3).toInt() == 2) {
				// Lesson done multiple
				openIcon = QIcon(":/img/lesson_done_two.png");
			} else {
				if (query.value(3).toInt() == 1) {
					// Lesson done once
					openIcon = QIcon(":/img/lesson_done_one.png");
				} else {
					// Lesson never done
					openIcon = QIcon(":/img/lesson_done_none.png");
				}
			}
		}
		// Add Item to the list
		currentItem = new QListWidgetItem(openIcon, lessonName, listOpen);
		currentItem->setToolTip(lessonDescription);
		arrayOpen->append(lessonId);
		lessonCounter++;
	}
	return lessonCounter;
}

int StartSql::fillOwnList(QListWidget *listOwn, QList<QString> *arrayOwn) {
	// Clear the list first because this function is called not only once in
	// the contructor but also after a successful online update
	listOwn->clear();
	arrayOwn->clear();
	// Fill lesson list from SQLite lesson table
	QSqlQuery query;
	QString lessonName;
	QString lessonDescription;
	QListWidgetItem *currentItem;
	QString lessonId;
	QIcon ownIcon;
	int lessonCounter = 0;
	// SQL: all lessons sorted by id and a left joint to the number of
	// lessons done by the user
	if (!query.exec("SELECT own_list.own_id, own_list.own_name, "
		"own_list.own_description, "
		"COUNT(user_lesson_list.user_lesson_lesson) FROM own_list "
		"LEFT JOIN user_lesson_list ON own_list.own_id = "
		"user_lesson_list.user_lesson_lesson AND user_lesson_list.user_lesson_type = 2 "
		"GROUP BY own_list.own_name;")) {
		return -1;
	}
	// Read all datasets to list items
	while (query.next()) {
		// ID of the lesson
		lessonId = query.value(0).toString();
		// Name of the lesson
		lessonName = query.value(1).toString();
		// Maybe a additional description
		lessonDescription = query.value(2).toString();
		// Show different icons depending on number of lessons done
		if (query.value(3).toInt() > 2) {
			// Lesson done multiple
			ownIcon = QIcon(":/img/lesson_done_three.png");
		} else {
			if (query.value(3).toInt() == 2) {
				// Lesson done multiple
				ownIcon = QIcon(":/img/lesson_done_two.png");
			} else {
				if (query.value(3).toInt() == 1) {
					// Lesson done once
					ownIcon = QIcon(":/img/lesson_done_one.png");
				} else {
					// Lesson never done
					ownIcon = QIcon(":/img/lesson_done_none.png");
				}
			}
		}
		// Add Item to the list
		currentItem = new QListWidgetItem(ownIcon, lessonName, listOwn);
		currentItem->setToolTip(lessonDescription);
		arrayOwn->append(lessonId);
		lessonCounter++;
	}
	return lessonCounter;
}

int StartSql::fillThemes(QComboBox *comboTheme, QList<QString> *arrayThemes,
                         QString languageLesson, QString textAll) {
	// Clear the list first because this function is called not only once in
	// the contructor but also after a successful online update
	comboTheme->clear();
	arrayThemes->clear();
	// Fill lesson list from SQLite lesson table
	QSqlQuery query;
	QString themeName;
	QString themeId;
    int themeCounter = 1;
	// SQL: all lessons sorted by id and a left joint to the number of
	// lessons done by the user
	if (!query.exec("SELECT open_themes.theme_id, open_themes.theme_name, "
		"open_themes.theme_description FROM open_themes "
		"GROUP BY open_themes.theme_id;")) {
		return -1;
	}
    comboTheme->insertItem(0, textAll);
    arrayThemes->append(0);
	// Read all datasets to list items
	while (query.next()) {
		// ID of the lesson
		themeId = query.value(0).toString();
		// Name of the lesson
		themeName = query.value(1).toString();
		// Add Item to the list
		comboTheme->insertItem(themeCounter, themeName);
		arrayThemes->append(themeId);
		themeCounter++;
	}
	return themeCounter;
}

bool StartSql::deleteUserLessonList() {
	QSqlQuery query;
	if (!query.exec("DELETE FROM user_lesson_list;")) {
		return false;
	}
	return true;
}

bool StartSql::deleteUserChars() {
	QSqlQuery query;
	if (!query.exec("DELETE FROM user_chars;")) {
		return false;
	}
	return true;
}

bool StartSql::deleteOwnLesson(QString lessonnumber) {
	QSqlQuery query;
	if (!query.exec("DELETE FROM user_lesson_list WHERE user_lesson_lesson = " +
		lessonnumber + " AND user_lesson_type = 2;")) {
		return false;
	}
	if (!query.exec("DELETE FROM own_content WHERE content_lesson = " +
		lessonnumber + ";")) {
		return false;
	}
	if (!query.exec("DELETE FROM own_list WHERE own_id = " +
		lessonnumber + ";")) {
		return false;
	}
	return true;
}

bool StartSql::updateOwnLesson(QString lessonnumber, QString lessonname,
	QString description, QStringList content, int unit) {
	QVariant lastLessonId;
	QSqlQuery query;
	QString lessonid = "0";
	QString simplifiedContent = "";
	int i;

	if (content.size() > 0) {
		if (lessonnumber != "-1" && lessonnumber != "-2") {
			// Update existing lesson
			// ----------------------
			// First, delete all content of the lesson
			if (!query.exec("DELETE FROM own_content WHERE content_lesson = " +
				lessonnumber + ";")) {
				return false;
			}
			lessonid = lessonnumber;
			// Insert content name in the database
			if (!query.exec("UPDATE own_list SET own_name = '" +
				lessonname.replace(QChar(0x27), "''", Qt::CaseSensitive) + "', "
				"own_description = '" +
				description.replace(QChar(0x27), "''", Qt::CaseSensitive) + "', "
				"own_unit = " + QString::number(unit) + " WHERE own_id = " +
				lessonnumber + ";")) {
				return false;
			}
		} else {
			// Create new lesson
			// -----------------
			// Insert content name in the database
			if (!query.exec("INSERT INTO own_list VALUES(NULL,'" +
				lessonname.replace(QChar(0x27), "''", Qt::CaseSensitive) +
				"','" +
				description.replace(QChar(0x27), "''", Qt::CaseSensitive) +
				"', " +
				QString::number(unit) + ");")) {
				return false;
			}
			lastLessonId = query.lastInsertId();
			lessonid = QString::number(lastLessonId.toInt());
		}
		// Write every line of lesson content to database
		for (i = 0; i < content.size(); i++) {
			//simplifiedContent = QString::QString(
			//	content.at(i)).replace(QChar(0x27), "''", Qt::CaseSensitive).simplified();
			simplifiedContent = trim(QString::QString(
				content.at(i)).replace(QChar(0x27), "''", Qt::CaseSensitive));

			if (!query.exec("INSERT INTO own_content VALUES(NULL,'" +
				simplifiedContent + "'," +
				lessonid + ");")) {
				return false;
			}
		}
	}

	return true;
}

QString StartSql::trim(QString s) {
    if (s.size() == 0) {
        return "";
    }
    int start = 0;
    int end = s.size() - 1;
    QChar *c = s.data();
    if (c[start] != ' ' && c[end] != ' ') {
        return s;
	}
    while (start<=end && c[start] == ' ') {
        start++;
	}
    if (start <= end) {
        while (end && c[end] == ' ') {
            end--;
		}
    }
    int l = end - start + 1;
    if (l <= 0) {
        return "";
    }
    return QString(c + start, l);
}

bool StartSql::ownLessonExist(QString lessonname) {
	QSqlQuery query;
	if (!query.exec("SELECT own_name "
		"FROM own_list "
		"WHERE own_name = '" + lessonname + "';")) {
		return false;
	}
	if (!query.first()) {
		return false;
	} else {
		return true;
	}
}


bool StartSql::getOwnLesson(QString lessonnumber, QLineEdit *lineLessonName,
	QLineEdit *lineLessonDescription, QTextEdit *lineLessonContent,
	QRadioButton *radioUnitSentence, QRadioButton *radioUnitWord) {

	QSqlQuery query;
	QString lessonName = "";
	QString lessonDescription = "";
	QString lessonContent = "";
	int lessonUnit = 0;

	if (!query.exec("SELECT own_name, own_description, own_unit "
		"FROM own_list "
		"WHERE own_id = " + lessonnumber + ";")) {
		return false;
	}
	if (query.first()) {
		lessonName = query.value(0).toString();
		lessonDescription = query.value(1).toString();
		lessonUnit = query.value(2).toInt();
	}

	if (!query.exec("SELECT content_text "
		"FROM own_content "
		"WHERE content_lesson = " + lessonnumber + " "
		"ORDER BY content_id;")) {
		return false;
	}
	// Read all datasets to list items
	while (query.next()) {
		// ID of the lesson
		lessonContent.append(query.value(0).toString() + "\n");
	}

	// Fill out text lines
	lineLessonName->setText(lessonName);
	lineLessonDescription->setText(lessonDescription);
	lineLessonContent->setText(lessonContent);
	if (lessonUnit == 0) {
		radioUnitSentence->setChecked(true);
		radioUnitWord->setChecked(false);
	} else {
		radioUnitSentence->setChecked(false);
		radioUnitWord->setChecked(true);
	}
	return true;
}

bool StartSql::analyzeOwnLessons() {
    QSqlQuery mainQuery;
	QSqlQuery subQuery;
	QString queryString;
	QString indexString;
	QString lessonString;
	int lessonLen;
	QHash<int, int> unicodeErrorHash;
	QHash<int, int> unicodeErrorHashClean;
	int errorNum = 0;
	int rowsRead = 0;
	int totalRows = 0;
	int i;

	if (!mainQuery.exec("SELECT char_unicode FROM lesson_chars;")) {
		return false;
	}

	while (mainQuery.next()) {
		unicodeErrorHashClean[mainQuery.value(0).toInt()] = 0;
		errorNum++;
	}
	// 1. Delete current lessonanalysis table
	// (no error message, table could not exist)
	mainQuery.exec("DROP TABLE own_analysis;");

	// 2a. Create new analysis table query with new error definitions as columns
	queryString = "CREATE TABLE own_analysis (analysis_content INTEGER primary key";
	indexString = "CREATE INDEX own_analysis_index ON own_analysis (analysis_content";

	QHashIterator<int, int> hashIteratorClean(unicodeErrorHashClean);
    while (hashIteratorClean.hasNext()) {
        hashIteratorClean.next();
        queryString += ", analysis_char_"
        	+ QString::number(hashIteratorClean.key()) + " INTEGER";
        indexString += ", analysis_char_"
        	+ QString::number(hashIteratorClean.key());
        //cout << i.key() << ": " << i.value() << endl;
    }

	queryString += ");";
	indexString += ");";

	// 2b. Execute new analysis table query
	if (!mainQuery.exec(queryString)) {
		return false;
	}

	// 2c. Execute new analysis table query
	if (!mainQuery.exec(indexString)) {
		return false;
	}

	// 3. Count number of lesson text
	if (!mainQuery.exec("SELECT COUNT(content_id) FROM own_content;")) {
		return false;
	}
	mainQuery.first();
	totalRows = mainQuery.value(0).toInt();

	// 4. Query all lesson text and analyze it
	if (!mainQuery.exec("SELECT content_id, content_text FROM own_content "
		"ORDER BY content_id;")) {
		return false;
	}

	while (mainQuery.next()) {
		unicodeErrorHash = unicodeErrorHashClean;
		lessonString = mainQuery.value(1).toString();
		lessonLen = lessonString.length();

        // Now count all error chars and put them into the hash table
		for (i = 0; i < lessonString.size(); i++) {
			if (unicodeErrorHash.contains(lessonString[i].unicode())) {
				unicodeErrorHash[lessonString[i].unicode()]++;
			}
		}

		queryString = "INSERT INTO own_analysis VALUES(";
		queryString += mainQuery.value(0).toString();
		QHashIterator<int, int> hashIterator(unicodeErrorHash);
		//hashIterator.toFront();
		while (hashIterator.hasNext()) {
			hashIterator.next();
			// Weighted number of chars (char ratio) to SQL string
			queryString += ", " + QString::number(((double) hashIterator.value() /
				(double) lessonLen) * 100.0, 'f', 2);
		}
		queryString += ");";

		// Execute new analysis table query
		if (!subQuery.exec(queryString)) {
			return false;
		}
		rowsRead++;
	}
    return true;
}

bool StartSql::analyzeLessons(QString lessonType) {
    QSqlQuery mainQuery;
	QSqlQuery subQuery;
	QString queryString;
	QString indexString;
	QString lessonString;
	int lessonLen;
	QHash<int, int> unicodeErrorHash;
	QHash<int, int> unicodeErrorHashClean;
	int errorNum = 0;
	int rowsRead = 0;
	int totalRows = 0;
	int i;

	if (!mainQuery.exec("SELECT char_unicode FROM lesson_chars;")) {
		// Error message
		return false;
	}

	while (mainQuery.next()) {
		unicodeErrorHashClean[mainQuery.value(0).toInt()] = 0;
		errorNum++;
	}

	// 1. Delete current lessonanalysis table
	// (no error message, table could not exist)
	mainQuery.exec("DROP TABLE " + lessonType + "_analysis;");

	// 2a. Create new analysis table query with new error definitions as columns
	queryString = "CREATE TABLE " + lessonType + "_analysis (analysis_content "
		"INTEGER primary key";
	indexString = "CREATE INDEX " + lessonType + "_analysis_index ON " +
		lessonType + "_analysis (analysis_content";

	QHashIterator<int, int> hashIteratorClean(unicodeErrorHashClean);
    while (hashIteratorClean.hasNext()) {
        hashIteratorClean.next();
        queryString += ", analysis_char_"
        	+ QString::number(hashIteratorClean.key()) + " INTEGER";
        indexString += ", analysis_char_"
        	+ QString::number(hashIteratorClean.key());
        //cout << i.key() << ": " << i.value() << endl;
    }

	queryString += ");";
	indexString += ");";

	// 2b. Execute new analysis table query
	if (!mainQuery.exec(queryString)) {
		return false;
	}

	// 2c. Execute new analysis table query
	if (!mainQuery.exec(indexString)) {
		return false;
	}

	// 3. Count number of lesson text
	if (!mainQuery.exec("SELECT COUNT(content_id) FROM " + lessonType +
		"_content;")) {
		return false;
	}
	mainQuery.first();
	totalRows = mainQuery.value(0).toInt();

	// 4. Query all lesson text and analyze it
	if (!mainQuery.exec("SELECT content_id, content_text FROM " + lessonType +
		"_content ORDER BY content_id;")) {
		return false;
	}

	while (mainQuery.next()) {
		unicodeErrorHash = unicodeErrorHashClean;
		lessonString = mainQuery.value(1).toString();
		lessonLen = lessonString.length();

        // Now count all error chars and put them into the hash table
		for (i = 0; i < lessonString.size(); i++) {
			if (unicodeErrorHash.contains(lessonString[i].unicode())) {
				unicodeErrorHash[lessonString[i].unicode()]++;
			}
		}

		queryString = "INSERT INTO " + lessonType + "_analysis VALUES(";
		queryString += mainQuery.value(0).toString();
		QHashIterator<int, int> hashIterator(unicodeErrorHash);
		//hashIterator.toFront();
		while (hashIterator.hasNext()) {
			hashIterator.next();
			// Weighted number of chars (char ratio) to SQL string
			queryString += ", " + QString::number(((double) hashIterator.value() /
				(double) lessonLen) * 100.0, 'f', 2);
		}
		queryString += ");";

		// Execute new analysis table query
		if (!subQuery.exec(queryString)) {
			return false;
		}
		rowsRead++;
	}

    return true;
}

void StartSql::fillLanguage(QComboBox *combo, QString table, QString field) {

    combo->clear();

    QSqlQuery query;
    if (!query.exec("SELECT " + table + "." + field + "_key, "
        "" + table + "." + field + "_label "
        "FROM " + table + " "
        "GROUP BY " + table + "." + field + "_id;")) {
        return;
    }

    int counter = 0;
    while (query.next()) {
        combo->insertItem(counter,
                                 query.value(1).toString(),
                                 QVariant(query.value(0).toString()));
        counter++;
    }
}

