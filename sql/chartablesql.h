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
** Definition of the CharTableSql class
** File name: chartablesql.h
**
****************************************************************/


#include <QWidget>
#include <QSqlQueryModel>
#include <QHeaderView>
#include <QVariant>
#include <QModelIndex>
#include <QString>
#include <QPushButton>

#ifndef CHARSQLMODEL_H
#define CHARSQLMODEL_H

//! The CharSqlModel class provides a table model to format cells.
/*!
	@author Tom Thielicke, s712715
	@version 0.1.9
	@date 21.06.2006
*/
class CharSqlModel : public QSqlQueryModel {
	Q_OBJECT

	public:
		CharSqlModel(QWidget *parent = 0);

		QVariant data(const QModelIndex &item, int role) const;
		QWidget *parentWidget;
};

#endif // CHARSQLMODEL_H


#ifndef CHARTABLESQL_H
#define CHARTABLESQL_H

//! The CharTableSql class provides a table widget with all typed chars.
/*!
	@author Tom Thielicke, s712715
	@version 0.1.0
	@date 21.06.2006
*/
class CharTableSql : public QWidget {
	Q_OBJECT

	public:
		CharTableSql(QWidget *parent = 0);

	private slots:
		void sortColumn(int i);
        void deleteUserChars();

	private:
		CharSqlModel *model;
		QHeaderView *headerview;
		QVariant data(const QModelIndex &item, int role) const;
        void setQueryOrder(QString columnname, int isdesc);
		int previousColumnIndex;
        QPushButton *buttonReset;
        QString sql;

};

#endif // CHARTABLESQL_H
