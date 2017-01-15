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
** Definition of the LessonTableSql class
** File name: lessontablesql.h
**
****************************************************************/


#include <QWidget>
#include <QSqlQueryModel>
#include <QHeaderView>
#include <QTableView>
#include <QVariant>
#include <QModelIndex>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QList>
#include <QChar>
#include <QSqlQuery>

#ifndef LESSONSQLMODEL_H
#define LESSONSQLMODEL_H


//! The LessonSqlModel class provides a table model to format cells.
/*!
	@author Tom Thielicke, s712715
	@version 0.0.7
	@date 21.06.2006
*/

class LessonSqlModel : public QSqlQueryModel {
	Q_OBJECT

	public:
		LessonSqlModel(int row, int type, QWidget *parent = 0);
		int lastIdInserted;

	private:
		int lastTypeInserted;
		QVariant data(const QModelIndex &item, int role) const;
		QWidget *parentWidget;
		QString language;
};

#endif // LESSONSQLMODEL_H


#ifndef LESSONTABLESQL_H
#define LESSONTABLESQL_H

//! The LessonTableSql class provides a table widget with lessons.
/*!
	@author Tom Thielicke, s712715
	@version 0.0.2
	@date 16.06.2006
*/
class LessonTableSql : public QWidget {
	Q_OBJECT

	public:
		LessonTableSql(int row, int type, QList<QChar> charlist,
			QList<int> mistakelist, QWidget *parent = 0);

	private slots:
		void sortColumn(int i);
		void changeFilter(int rowindex);

	private:
        void setModelHeader();
		QLabel *labelFilter;
		QComboBox *comboFilter;
		LessonSqlModel *model;
		QTableView *view;
		QHeaderView *headerview;
		QVariant data(const QModelIndex &item, int role) const;
		void setQueryOrder(QString columnname, int isdesc);
		int previousColumnIndex;
		QString columnName;
		QString whereClausel;
		int isDesc;
		bool printVisible;
		QList<QChar> charList;
		QList<int> mistakeList;
		int lessonRow;

};

#endif // LESSONTABLESQL_H
