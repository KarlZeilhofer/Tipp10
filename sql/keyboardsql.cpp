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
** Implementation of the KeyboardSql class
** File name: keyboardsql.cpp
**
****************************************************************/

#include <QSqlQuery>
#include <QSqlDriver>
#include <QVariant>
#include <QFile>
#include <QTextStream>
#include <QString>

#include "keyboardsql.h"

KeyboardSql::KeyboardSql(QString keyboardLayout) {

	// Set the keyboard layout
	layout = keyboardLayout;
	
	// Get finger text
	getFingerDescription();
}

void KeyboardSql::getFingerDescription() {
	
	fingers << QObject::tr("Kleiner Finger links")
		 	<< QObject::tr("Ringfinger links")
		 	<< QObject::tr("Mittelfinger links")
		 	<< QObject::tr("Zeigefinger links")
		 	<< QObject::tr("Zeigefinger rechts")
		 	<< QObject::tr("Mittelfinger rechts")
		 	<< QObject::tr("Ringfinger rechts")
		 	<< QObject::tr("Kleiner Finger rechts")
		 	<< QObject::tr("Daumen");
}

bool KeyboardSql::getKeyLayout(QChar givenchar, int *left, int *top,
	int *color, int *form, int *modifier, int *modifier2, int *finger,
	QString *status) {

	// Convert given char into properties of a key
	QString tableName = "keyboard_grids";

	QSqlQuery query;
	if (query.exec("SELECT " +
		tableName +	".left, " +
		tableName + ".top, " +
		tableName + ".color, " +
		tableName + ".form, " +
		"keyboard_layouts.modifier1, " +
		"keyboard_layouts.modifier2, " +
		tableName + ".finger " +
		"FROM keyboard_layouts, " +
		tableName + " " +
		"WHERE keyboard_layouts.layout = '" + layout + "' " +
		"AND " + tableName + ".layout_country = '" + layout.left(2) + "' " +
		"AND " + tableName + ".key = keyboard_layouts.grid " +
		"AND keyboard_layouts.unicode = " +
		QString::number(givenchar.unicode()) + ";")) {
		if (query.first()) {
			*left = query.value(0).toInt();
			*top = query.value(1).toInt();
			*color = query.value(2).toInt();
			*form = query.value(3).toInt();
			*modifier = query.value(4).toInt();
			*modifier2 = query.value(5).toInt();
			*finger = query.value(6).toInt();
			*status = fingers.at(query.value(6).toInt());
			return true;
		}
	}
	return false;
}

bool KeyboardSql::getModifierLayout(int givenmodifier, int *left, int *top,
	int *color, int *form, int *finger, QString *status) {

	// Convert given id into properties of a key
	QString tableName = "keyboard_grids";

	QSqlQuery query;
	if (query.exec("SELECT " + tableName +
		".left, " + tableName + ".top, "
		+ tableName + ".color, " + tableName +
		".form, " + tableName + ".finger FROM " + tableName +
		" WHERE " + tableName + ".layout_country = '" + layout.left(2) + "' " +
		"AND " + tableName + ".key = "
		+ QString::number(givenmodifier) + ";")) {
		if (query.first()) {
			*left = query.value(0).toInt();
			*top = query.value(1).toInt();
			*color = query.value(2).toInt();
			*form = query.value(3).toInt();
			*finger = query.value(4).toInt();
			&status->prepend(fingers.at(query.value(4).toInt()) + " + ");
			return true;
		}
	}
	return false;
}

bool KeyboardSql::getNumLayout(QChar givenchar, int *left, int *top,
	int *color, int *form, int *finger, QString *status) {

	// Convert given char into properties of a key
	QString tableName = "numboard_grids";

	QSqlQuery query;
	if (query.exec("SELECT " + tableName +
		".left, " + tableName + ".top, "
		"" + tableName + ".color, " + tableName +
		".form, " + tableName + ".finger "
		"FROM numboard_layouts, " + tableName + " " +
		"WHERE numboard_layouts.layout_country = '" + layout.left(2) + "' " +
		"AND numboard_layouts.layout_os = '" + layout.right(3) + "' " +
		"AND " + tableName + ".layout_country = '" + layout.left(2) + "' " +
		"AND " + tableName + ".layout_os = '" + layout.right(3) + "' " +
		"AND " + tableName + ".key = numboard_layouts.grid " +
		"AND numboard_layouts.unicode = " +
		QString::number(givenchar.unicode()) + ";")) {
		if (query.first()) {
			*left = query.value(0).toInt();
			*top = query.value(1).toInt();
			*color = query.value(2).toInt();
			*form = query.value(3).toInt();
			*finger = query.value(4).toInt();
			*status = fingers.at(query.value(4).toInt());
			return true;
		}
	}
	return false;
}
