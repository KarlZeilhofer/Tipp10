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
** Definition of the StartSql class
** File name: startsql.h
**
****************************************************************/

#ifndef STARTSQL_H
#define STARTSQL_H

#include <QListWidget>
#include <QList>
#include <QString>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QProgressBar>
#include <QRadioButton>
#include <QString>

//! The StartSql class fills a list widget and resets user data .
/*!
	The StartSql class provides 3 functions. The first, fillLessonList(),
	is to fill a list widget with a lesson list. The other two functions,
	deleteUserLessonList and deleteUserChars, are to reset user data tables.

	@author Tom Thielicke, s712715
	@version 0.0.2
	@date 27.06.2006
*/
class StartSql {

	public:

		//! Constructor is empty.
		StartSql();

		//! Converts an unicode char into layout values of a key.
		/*!
			This function fills a list widget with items of the table
			"lesson_list". If field "user_lesson_lesson" is bigger than 1
			icon "lesson_done_multiple.png" is shown, otherwise
			icon "lesson_done_none.png" is shown.

			@param listLesson QListWidget which is filled
			@return bool Operation successful true/false
			@see opSystem
		*/
		int fillLessonList(QListWidget *listLesson,
            QList<QString> *arrayTraining, QString languageLesson);

		int fillOpenList(QListWidget *listOpen, QList<QString> *arrayOpen,
            QString themeId, QString languageLesson);

        int fillOwnList(QListWidget *listOwn, QList<QString> *arrayOwn);

        int fillThemes(QComboBox *comboTheme, QList<QString> *arrayThemes,
                       QString languageLesson, QString textAll);

        void fillLanguage(QComboBox *combo, QString table, QString field);

		//! Deletes the complete content of table "user_lesson_list".
		bool deleteUserLessonList();

		//! Deletes the complete content of table "user_lesson_list".
		bool deleteUserChars();

		//! Deletes the complete content of table "user_lesson_list".
		bool deleteOwnLesson(QString lessonnumber);

		//! Updates the complete content of table "user_lesson_list".
		bool updateOwnLesson(QString lessonnumber, QString lessonname,
			QString description, QStringList content, int unit);

		bool getOwnLesson(QString lessonnumber, QLineEdit *lineLessonName,
			QLineEdit *lineLessonDescription, QTextEdit *lineLessonContent,
			QRadioButton *radioUnitSentence, QRadioButton *radioUnitWord);

		bool ownLessonExist(QString lessonname);

		bool analyzeOwnLessons();

		bool analyzeLessons(QString lessonType);

	private:

		QString trim(QString s);
};

#endif // STARTSQL_H
