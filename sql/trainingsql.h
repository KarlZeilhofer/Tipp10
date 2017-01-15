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
** Definition of the TrainingSql class
** File name: trainingsql.h
**
****************************************************************/

#ifndef TRAININGSQL_H
#define TRAININGSQL_H

#include <QString>
#include <QList>
#include <QVariant>

//! The TrainingSql class provides lesson handling over database.
/*!
	The TickerBoard class is an database interface for handling lessons while
	training. It can update the user tables and handles the lesson text by
	creating or updating it.

	@author Tom Thielicke, s712715
	@version 0.0.6
	@date 27.06.2006
*/
class TrainingSql {

	public:

		//! Constructor, sets the synchron value of the database.
        TrainingSql(QString replace = "", QString regexp = "", QString layout = "");

		//! Constructor, sets the database to synchron.
		~TrainingSql();

		int getLessonUnit(int lesson, int type);

		//! Increments the counter of the given char and column.
		/*!
			@param unicodechar The char which value is incremented
			@param columnname The column name which value is incremented
			@return bool Operation successful true/false
		*/
        bool updateUsertable(QChar unicodechar, QString columnname);

		//! Creates the first text of the given lesson number.
		/*!
			@param lesson Lesson number
			@param type Lesson type (0=Training, 1=Open, 2=Own)
			@param intelligence Use intelligence bool
			@param useEszett Use Eszett bool
			@return QString The lesson text
			@see lessonsDoneList
		*/
		QString createLesson(int lesson, int type, int unit,
			bool intelligence, bool useEszett, bool useUmlaut);

		//! Creates a new text of the given lesson number.
		/*!
			@param lesson Lesson number
			@param type Lesson type (0=Training, 1=Open, 2=Own)
			@param intelligence Use intelligence bool
			@param useEszett Use Eszett bool
			@return QString The lesson text
			@see lessonsDoneList
		*/
		QString updateLesson(int lesson, int type, bool intelligence,
			bool useEszett, bool useUmlaut);

		//! Saves lesson results of the user.
		/*!
			@param lesson Lesson number
			@param timelen Time length of lesson
			@param tokenlen Token length of lesson
			@param charnum Character number of lesson
			@param errornum Type error number of lesson
			@param timestamp Time stmp of lesson
			@return QVariant The row id that was insert
		*/
		QVariant saveLesson(int lesson, int timelen, int tokenlen,
			int charnum, int errornum, QDateTime timestamp,
			int type, QString name);

		//! Replaces varios chars in the text.
		QString replaceRoutine(QString content, bool useEszett, bool useUmlaut,
            int lesson, int type);

        QString getKeyboardLayoutReplaceRoutine(QString layout);
        QString getKeyboardLayoutRegexpRoutine(QString layout);

	private:

		//! Sets the synchron value of the database.
		/*!
			@param synchron Synchron true or false
		*/
        void setSynchron(bool synchron);

		//! Queue of text that was updated.
		QList<int> lessonsDoneList;

        int queryCounter;

        QString replaceSetting;

        QString regexpSetting;
};

#endif // TRAININGSQL_H
