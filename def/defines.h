/*
Copyright (c) 2006-2011, Tom Thielicke IT Solutions

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
** File name: defines.h
**
****************************************************************/

#ifndef DEFINES_H
#define DEFINES_H

// OS constants
#define APP_WIN						false
#define APP_MAC						false
#define APP_X11						true
#define APP_PORTABLE				false //at least one of the 3 OS must be true too!

// Languages
#define APP_EXISTING_LANGUAGES_GUI	"de" // "de;en"
#define APP_STD_LANGUAGE_GUI		"de"
#define APP_STD_LANGUAGE_LAYOUT		"de_qwertz_win"
#define APP_STD_LANGUAGE_LESSON		"de_de_qwertz"

// Common program constants
#define APP_ORGANIZATION 			"tipp10"

#define APP_COPYRIGHT 				"(c) 2006-2011, Tom Thielicke IT Solutions"
#define APP_NAME_INTERN				"TIPP10"
#define APP_NAME 					"TIPP10"
#define APP_URL 					"https://github.com/KarlZeilhofer/Tipp10"
#define APP_DB 						"tipp10v2.template"
#define APP_USER_DB					"tipp10v2.db"

// Update constants
#define UPDATE_URL 					"www.tipp10.com"
#define UPDATE_URL_VERSION 			"/update/version.tipp10v210"
#define UPDATE_URL_SQL 				"/update/sql.tipp10v210.utf"
#define LESSON_DOWNLOAD_URL			"http://www.tipp10.de/beispiel.txt"
#define COMPILED_UPDATE_VERSION		33
#define COMPILED_UPDATE_FILENAME	"tipp10v2.template"

// Lesson text constants
#define NUM_TOKEN_UNTIL_REFRESH 	25
#define NUM_TOKEN_UNTIL_NEW_LINE 	35
#define NUM_INTELLIGENT_QUERYS 		2
#define TOKEN_NEW_LINE 				0x00b6
#define TOKEN_TAB					0x2192
#define TOKEN_BACKSPACE				0x00b9 //0x00ac

// Constants for dynamic training
#define NUM_TEXT_UNTIL_REPEAT 		10
#define BORDER_LESSON_IS_SENTENCE 	7
#define LAST_LESSON				 	18 // lesson with training of all characters
#define NUMPAD_LESSON_START			19
#define SYNCHRON_DB_WHILE_TRAINING 	false

// Standard constants
#define DATE_TIME_FORMAT 			"dd.MM.yyyy hh:mm"
#define DATE_FORMAT					"dd.MM.yyyy"
#define KEYBOARD_LAYOUT_STANDARD 	0 // 0=DE, 1=CH, 2=NEO etc.
#if APP_MAC
#define KEYBOARD_LAYOUT_MAC		 	1 // 0=WIN, 1=MAC
#else
#define KEYBOARD_LAYOUT_MAC		 	0 // 0=WIN, 1=MAC
#endif
#define LESSON_TIMELEN_STANDARD 	5
#define LESSON_TOKENLEN_STANDARD	500
#define TICKERSPEED_STANDARD 		2
#define TICKER_COLOR_FONT    		"#000000"
#define TICKER_COLOR_BG     		"#FFFFFF"
#define TICKER_COLOR_CURSOR    		"#CDCDCD"
#define METRONOM_STANDARD           60

// Font format
#define FONT_STANDARD 				"Arial"
#if APP_WIN
#define FONT_SIZE_TICKER			18
#define FONT_SIZE_TICKER_PAUSE		16
#define FONT_SIZE_STATUS			8
#define FONT_SIZE_PROGRESS			8
#define FONT_SIZE_PROGRESS_LESSON	6
#define FONT_SIZE_FINGER        	10
#endif
#if APP_MAC
#define FONT_SIZE_TICKER			22
#define FONT_SIZE_TICKER_PAUSE		20
#define FONT_SIZE_STATUS			10
#define FONT_SIZE_PROGRESS			10
#define FONT_SIZE_PROGRESS_LESSON	8
#define FONT_SIZE_FINGER        	12
#endif
#if APP_X11
#define FONT_SIZE_TICKER			18
#define FONT_SIZE_TICKER_PAUSE		16
#define FONT_SIZE_STATUS			8
#define FONT_SIZE_PROGRESS			8
#define FONT_SIZE_PROGRESS_LESSON	6
#define FONT_SIZE_FINGER        	10
#endif

// Window dimensions and icon filename
#if APP_WIN
#define APP_WIDTH_STANDARD			640
#define APP_HEIGHT_STANDARD			470
#define APP_WIDTH_SMALL				640
#define APP_HEIGHT_SMALL			200
#define ICON_FILENAME				"icon.ico"
#endif
#if APP_MAC
#define APP_WIDTH_STANDARD			720
#define APP_HEIGHT_STANDARD			500
#define APP_WIDTH_SMALL				680
#define APP_HEIGHT_SMALL			210
#define ICON_FILENAME				"icon.png"
#endif
#if APP_X11
#define APP_WIDTH_STANDARD			700
#define APP_HEIGHT_STANDARD			520
#define APP_WIDTH_SMALL				640
#define APP_HEIGHT_SMALL			200
#define ICON_FILENAME				"icon.png"
#endif


#endif // DEFINES_H
