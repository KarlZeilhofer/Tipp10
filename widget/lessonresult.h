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
** Definition of the LessonResult class
** File name: lessonresult.h
**
****************************************************************/


#include <QWidget>
#include <QPushButton>
#include <QList>
#include <QChar>
#include <QSqlQuery>


#ifndef LESSONRESULT_H
#define LESSONRESULT_H

//! The LessonTableSql class provides a table widget with lessons.
/*!
	@author Tom Thielicke, s712715
	@version 0.0.2
	@date 16.06.2006
*/
class LessonResult : public QWidget {
	Q_OBJECT

	public:
		LessonResult(int row, int type, QList<QChar> charlist,
			QList<int> mistakelist, QWidget *parent = 0);

    private slots:
        void createPrintOutput();
        void publishFacebook();

	private:
		void createOutput();
		QPushButton *buttonPrintLesson;
        QPushButton *buttonPublishFacebook;
		QList<QChar> charList;
		QList<int> mistakeList;
		int lessonRow;
		QString language;
        QString facebookComment;
        QString languageGui;

};

#endif // LESSONRESULT_H
