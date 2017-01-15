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
** Definition of the KeyboardSql class
** File name: keyboardsql.h
**
****************************************************************/

#ifndef KEYBOARDSQL_H
#define KEYBOARDSQL_H

#include <QChar>
#include <QString>
#include <QStringList>

//! The KeyboardSql class provides a key to layout convertion.
/*!
	The KeyboardSql class is an database interface for a virtual keyboard. It
	can manage different layouts, standard is a windows keyboard layout.
	Over two functions, getKeyLayout() and getModifierLayout() it converts
	an unicode char or a given id into different layout values like
	coordinates, color and form data.

	@author Tom Thielicke, s712715
	@version 0.1.1
	@date 23.06.2006
*/
class KeyboardSql {

	public:

		//! Constructor: sets the keyboard layout.
		/*!
			In this contructor the private QString variable opSystem is set.
			Standard value is "win", but other layouts are possible if
			corresponding tables exist. To change it, transmit the parameter
			op.Tables used for the keyboard layout are named
			character_list_<layout>, i.e. character_list_win.

			@param op The code of the keyboard layout table
			@see opSystem
		*/
		KeyboardSql(QString keyboardLayout = "us_qwerty_win");

		//! Converts an unicode char into layout values of a key.
		/*!
			This function receives an unicode char, pointers to layout
			values which represent a key and a status text.
			The givenchar is selected in a sql query and the corresponding
			key layout values and a status text are transfered to the passed
			pointers.

			@param givenchar The char which is converted
			@param left The x ccordinate of the key
			@param top The y ccordinate of the key
			@param color The color of the key
			@param form The form of the key
			@param finger The finger code of the key
			@param modifier The id of the corresponding modifier key
			@param modifier2 The id of the corresponding second modifier key
			@param status The text for a status bar
			@see opSystem
		*/
		bool getKeyLayout(QChar givenchar, int *left, int *top, int *color,
			int *form, int *modifier, int *modifier2, int *finger, QString *status);

		//! Converts an table id into layout values of a key.
		/*!
			This function receives an id, pointers to layout
			values which represent a key and a status text.
			The id is selected in a sql query and the corresponding
			key layout values and a status text are transfered to the passed
			pointers.

			@param givenmodifier The id which is converted
			@param left The x ccordinate of the key
			@param top The y ccordinate of the key
			@param color The color of the key
			@param form The form of the key
			@param finger The finger code of the key
			@param status The text for a status bar
			@see opSystem
		*/
		bool getModifierLayout(int givenmodifier, int *left, int *top,
			int *color, int *form, int *finger, QString *status);

		//! Converts an unicode char into layout values of a key.
		/*!
			This function receives an unicode char, pointers to layout
			values which represent a key and a status text.
			The givenchar is selected in a sql query and the corresponding
			key layout values and a status text are transfered to the passed
			pointers.

			@param givenchar The char which is converted
			@param left The x ccordinate of the key
			@param top The y ccordinate of the key
			@param color The color of the key
			@param form The form of the key
			@param finger The finger code of the key
			@param status The text for a status bar
			@see opSystem
		*/
		bool getNumLayout(QChar givenchar, int *left, int *top, int *color,
			int *form, int *finger, QString *status);

		void getFingerDescription();

	private:

		//! Holds identification string of the keyboard layout table.
		QString layout;
		QStringList fingers;
};

#endif // KEYBOARDSQL_H
