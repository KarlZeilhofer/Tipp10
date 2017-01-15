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
** File name: errordefines.h
**
****************************************************************/

#ifndef ERRORDEFINES_H
#define ERRORDEFINES_H

// Error message text
//********************
// Failed pic operations error numbers
#define ERR_LOGO_PIC 					101
#define ERR_KEY_PIC 					102
#define ERR_TICKER_PIC 					103
#define ERR_STATUS_PIC 					104

// SQL connection error numbers
#define ERR_SQL_DB 						201
#define ERR_SQL_CONNECTION 				202
#define ERR_SQL_DB_APP_EXIST			203
#define ERR_SQL_DB_USER_EXIST			204
#define ERR_SQL_DB_APP_COPY				205
#define ERR_SQL_DB_USER_COPY			206

// Lesson operation error numbers
#define ERR_DB_VERSION_EXIST 			301
#define ERR_USER_LESSONS_FLUSH 			302
#define ERR_USER_ERRORS_FLUSH 			303
#define ERR_LESSONS_EXIST 				304
#define ERR_LESSONS_SELECTED 			305
#define ERR_LESSONS_CREATION 			306
#define ERR_LESSONS_UPDATE 				307
#define ERR_USER_ERRORS_REFRESH 		308
#define ERR_USER_LESSONS_REFRESH 		309
#define ERR_USER_LESSON_ADD		 		310
#define ERR_USER_LESSON_GET		 		311
#define ERR_USER_LESSON_ANALYZE			312
#define ERR_USER_IMPORT_READ			313
#define ERR_USER_IMPORT_EMPTY			314
#define ERR_USER_DOWNLOAD_EXECUTION		315
#define ERR_USER_EXPORT_WRITE			316

// Update operations error numbers
#define ERR_TEMP_FILE_CREATION 			401
#define ERR_UPDATE_EXECUTION 			402
#define ERR_ONLINE_VERSION_READABLE 	403
#define ERR_DB_VERSION_READABLE 		404
#define ERR_UPDATE_SQL_EXECUTION 		405
#define ERR_ERROR_DEFINES_EXIST 		406
#define ERR_LESSON_CONTENT_EXIST 		407
#define ERR_ANALYZE_TABLE_CREATION 		408
#define ERR_ANALYZE_INDEX_CREATION 		409
#define ERR_ANALYZE_TABLE_FILL 			410

// Error message addons
//**********************
// Error message type numbers
#define TYPE_INFO 						901
#define TYPE_WARNING 					902
#define TYPE_CRITICAL 					903
// Error message cancel numbers
#define CANCEL_NO 						911
#define CANCEL_OPERATION 				912
#define CANCEL_UPDATE 					913
#define CANCEL_PROGRAM 					914

#endif // ERRORDEFINES_H

