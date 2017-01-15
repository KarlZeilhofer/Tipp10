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
** Implementation of the ErrorMessage class
** File name: errormessage.cpp
**
****************************************************************/

#include <QMessageBox>

#include "errormessage.h"
#include "def/errordefines.h"
#include "def/defines.h"

ErrorMessage::ErrorMessage(QWidget *parent) : QWidget(parent) {
}

void ErrorMessage::showMessage(int errorNo, int errorType, int cancelProcedure,
	QString addon) {
	QString message;
	// First generate the error text
	message = getErrorText(errorNo);
	// If exist, append an additional text
	if (addon != "") {
		message.append("\n" + addon);
	}
	/*if (cancelProcedure != CANCEL_NO) {
		// Append fix text
		message.append(tr("\n\nBitte melden Sie den Fehler "
			"(Fehlernummer) und die Umstaende unter denen er aufgetreten "
			"ist im Internet unter: ") + APP_URL
			+ tr("\nSo kann ") + APP_NAME
			+ tr(" verbessert werden. Danke!"));
	}*/

	// Append, what the program do now
	message.append("\n\n" + getCancelText(cancelProcedure));

	// Choose a message style
	switch (errorType) {
		case TYPE_INFO:
			QMessageBox::information(0, APP_NAME, message);
			break;
		case TYPE_WARNING:
			QMessageBox::warning(0, APP_NAME, message);
			break;
		case TYPE_CRITICAL:
		default:
			QMessageBox::critical(0, APP_NAME, message);
			break;
	}
}

QString ErrorMessage::getCancelText(int number) {
	QString cancelText = "";
	switch (number) {
		case CANCEL_NO:
			cancelText = "";
			break;
		case CANCEL_OPERATION:
			cancelText = tr("Der Vorgang wird abgebrochen.");
			break;
		case CANCEL_UPDATE:
			cancelText = tr("Das Update wird abgebrochen.");
			break;
		case CANCEL_PROGRAM:
		default:
			cancelText = tr("Die Anwendung wird beendet.");
			break;
	}
	return cancelText;
}

QString ErrorMessage::getErrorText(int number) {
	QString errorText = "";
	switch (number) {
		case ERR_LOGO_PIC:
			errorText = tr("Das Programmlogo konnte nicht geladen "
				"werden.");
			break;
		case ERR_KEY_PIC:
			errorText = tr("Ein Tastatur-Bitmap konnte nicht geladen "
				"werden.");
			break;
		case ERR_TICKER_PIC:
			errorText = tr("Der Lauftext-Hintergund konnte nicht "
				"geladen werden.");
			break;
		case ERR_STATUS_PIC:
			errorText = tr("Der Statusleisten-Hintergund konnte "
				"nicht geladen werden.");
			break;

		case ERR_SQL_DB_APP_EXIST:
			errorText = tr("Die Datenbank ") + APP_DB +
				tr(" im Programmverzeichnis konnte "
				"leider nicht gefunden werden.\n"
				"Sie muss existieren, um die Software starten zu koennen.");
			break;
		case ERR_SQL_DB_USER_EXIST:
			errorText = tr("Im angegebenen Verzeichnis konnte keine "
				"Datenbank gefunden werden. Es wird nun "
				"versucht, eine neue, leere Datenbank in diesem "
				"Verzeichnis anzulegen.\n\n"
				"Den Verzeichnispfad zur Datenbank koennen Sie "
				"in den Einstellungen veraendern.\n");
			break;
		case ERR_SQL_DB_APP_COPY:
			errorText = tr("Die Benutzer-Datenbank konnte nicht in "
				"Ihrem HOME-Verzeichnis angelegt werden. Eventell fehlen die Schreibrechte.\n"
				"Es wird nun versucht, die Original-Datenbank im Programmverzeichnis zu verwenden.\n\n"
				"Den Verzeichnispfad zur Datenbank koennen Sie anschliessend in den "
				"Einstellungen veraendern.\n");
			break;
		case ERR_SQL_DB_USER_COPY:
			errorText = tr("Die Benutzer-Datenbank konnte nicht im "
				"angegebenen Verzeichnis angelegt werden. "
				"Eventell fehlt das Verzeichnis oder es "
				"sind keine Schreibrechte vorhanden.\n"
				"Es wird nun versucht, eine Datenbank in Ihrem HOME-Verzeichnis anzulegen.\n\n"
				"Den Verzeichnispfad zur Datenbank koennen Sie anschliessend in den "
				"Einstellungen veraendern.\n");
			break;
		case ERR_SQL_CONNECTION:
			errorText = tr("Die Verbindung zur Datenbank ist leider "
				"fehlgeschlagen.");
			break;

		case ERR_DB_VERSION_EXIST:
			errorText = tr("Die Verbindung zur Datenbank ist leider "
				"fehlgeschlagen.");
			break;
		case ERR_USER_LESSONS_FLUSH:
			errorText = tr(
				"Die Benutzertabelle mit den Lektionendaten kann nicht\n"
				"geleert werden.\nSQL-Statement fehlgeschlagen.");
			break;
		case ERR_USER_ERRORS_FLUSH:
			errorText = tr(
				"Die Benutzertabelle mit den Fehlerdaten kann nicht\n"
				"geleert werden.\nSQL-Statement fehlgeschlagen.");
			break;
		case ERR_LESSONS_EXIST:
			errorText = tr("Keine Lektionen vorhanden.");
			break;
		case ERR_LESSONS_SELECTED:
			errorText = tr("Es wurde keine Lektion selektiert.\n"
				"Bitte waehlen Sie eine Lektion aus.");
			break;
		case ERR_LESSONS_CREATION:
			errorText = tr("Lektion konnte nicht erstellt werden.\n"
				"SQL-Statement fehlgeschlagen.");
			break;
		case ERR_LESSONS_UPDATE:
			errorText = tr(
				"Die Lektion konnte nicht aktualisiert werden, weil kein\n"
				"Zugriff auf die Datenbank moeglich ist.\n\n"
				"Falls dieses Problem erst auftrat, nachem der Schreibtrainer\n"
				"zuvor einige Zeit anstandslos lief, ist voraussichtlich die\n"
				"Datenbank beschaedigt worden (z.B. durch einen Absturz des\n"
				"Computers).\n"
				"Um zu ueberpruefen ob die Datenbank beschaedigt wurde, koennen\n"
				"Sie die Datenbank-Datei testweise einmal umbenennen und die\n"
				"Software dann neu starten (es sollte dann auomatisch eine\n"
				"neue, leere Datenbank angelegt werden). Den Pfad zur Datenbank\n"
				"\"") + APP_USER_DB + tr("\" koennen Sie den Grundeinstellungen entnehmen.\n\n"
				"Wenn dieses Problem gleich nach dem ersten Start der Software\n"
				"auftrat, fehlen voraussichtlich die Schreibrechte auf die\n"
				"Datenbank-Datei. Bitte ueberpruefen Sie diese.\n");
			break;
		case ERR_USER_ERRORS_REFRESH:
			errorText = tr(
				"Die Benutzertabelle mit den Fehlerdaten konnte nicht\n"
				"aktualisiert werden, weil kein Zugriff auf die Datenbank\n"
				"moeglich ist.\n\n"
				"Falls dieses Problem erst auftrat, nachem der Schreibtrainer\n"
				"zuvor einige Zeit anstandslos lief, ist voraussichtlich die\n"
				"Datenbank beschaedigt worden (z.B. durch einen Absturz des\n"
				"Computers).\n"
				"Um zu ueberpruefen ob die Datenbank beschaedigt wurde, koennen\n"
				"Sie die Datenbank-Datei testweise einmal umbenennen und die\n"
				"Software dann neu starten (es sollte dann auomatisch eine\n"
				"neue, leere Datenbank angelegt werden). Den Pfad zur Datenbank\n"
				"\"") + APP_USER_DB + tr("\" koennen Sie den Grundeinstellungen entnehmen.\n\n"
				"Wenn dieses Problem gleich nach dem ersten Start der Software\n"
				"auftrat, fehlen voraussichtlich die Schreibrechte auf die\n"
				"Datenbank-Datei. Bitte ueberpruefen Sie diese.\n");
			break;
		case ERR_USER_LESSONS_REFRESH:
			errorText = tr(
				"Die Benutzertabelle mit den Lektionendaten konnte nicht\n"
				"aktualisiert werden, weil kein Zugriff auf die Datenbank\n"
				"moeglich ist.\n\n"
				"Falls dieses Problem erst auftrat, nachem der Schreibtrainer\n"
				"zuvor einige Zeit anstandslos lief, ist voraussichtlich die\n"
				"Datenbank beschaedigt worden (z.B. durch einen Absturz des\n"
				"Computers).\n"
				"Um zu ueberpruefen ob die Datenbank beschaedigt wurde, koennen\n"
				"Sie die Datenbank-Datei testweise einmal umbenennen und die\n"
				"Software dann neu starten (es sollte dann auomatisch eine\n"
				"neue, leere Datenbank angelegt werden). Den Pfad zur Datenbank\n"
				"\"") + APP_USER_DB + tr("\" koennen Sie den Grundeinstellungen entnehmen.\n\n"
				"Wenn dieses Problem gleich nach dem ersten Start der Software\n"
				"auftrat, fehlen voraussichtlich die Schreibrechte auf die\n"
				"Datenbank-Datei. Bitte ueberpruefen Sie diese.\n");
			break;
		case ERR_USER_LESSON_ADD:
			errorText = tr("Die Lektion konnte nicht gespeichert "
				"werden.\n"
				"SQL-Statement fehlgeschlagen.");
			break;
		case ERR_USER_LESSON_GET:
			errorText = tr("Die Lektion konnte nicht angefordert "
				"werden.\n"
				"SQL-Statement fehlgeschlagen.");
			break;
		case ERR_USER_LESSON_ANALYZE:
			errorText = tr("Die Lektion konnte nicht analysiert "
				"werden.\n"
				"SQL-Statement fehlgeschlagen.");
			break;
		case ERR_USER_IMPORT_READ:
			errorText = tr("Die Datei konnte leider nicht importiert "
				"werden.\n"
				"Bitte ueberpruefen Sie, ob es sich um eine lesbare Textdatei "
				"handelt.\n");
			break;
		case ERR_USER_IMPORT_EMPTY:
			errorText = tr("Die Datei konnte leider nicht importiert "
				"werden, weil sie leer ist.\n"
				"Bitte ueberpruefen Sie, ob es sich um eine lesbare Textdatei "
				"mit\nInhalt handelt.\n");
			break;
		case ERR_USER_DOWNLOAD_EXECUTION:
			errorText = tr(
				"Die Datei konnte leider nicht importiert werden.\n"
				"Ueberpruefen Sie bitte die Schreibweise der Internetadresse,\n"
				"es muss sich um eine lesbare Textdatei und um eine gueltige\n"
				"URL handeln.\n"
				"Ueberpruefen Sie bitte ausserdem Ihre Internetverbindung.\n");
			break;
		case ERR_USER_EXPORT_WRITE:
			errorText = tr("Die Datei konnte leider nicht exportiert "
				"werden.\n"
				"Bitte ueberpruefen Sie, ob es sich um eine beschreibbare Textdatei "
				"handelt.\n");
			break;
		case ERR_TEMP_FILE_CREATION:
			errorText = tr("Temporaere Datei konnte nicht erzeugt "
				"werden.");
			break;
		case ERR_UPDATE_EXECUTION:
			errorText = tr("Das Update konnte leider nicht "
				"durchgefuehrt werden.\nBitte ueberpruefen Sie Ihre "
				"Internetverbindung und die Proxyeinstellung.");
			break;
		case ERR_ONLINE_VERSION_READABLE:
			errorText = tr("Die Online-Versionsinformation konnte "
				"nicht gelesen werden.");
			break;
		case ERR_DB_VERSION_READABLE:
			errorText = tr("Die Datenbank-Versionsinformation konnte "
				"nicht gelesen werden.");
			break;
		case ERR_UPDATE_SQL_EXECUTION:
			errorText = tr("SQL-String konnte nicht verarbeitet "
				"werden");
			break;
		case ERR_ERROR_DEFINES_EXIST:
			errorText = tr("Keine Tippfehler-Definitonen vorhanden.");
			break;
		case ERR_LESSON_CONTENT_EXIST:
			errorText = tr("Temporaere Datei konnte nicht erzeugt "
				"werden.\nDas Update wird abgebrochen.");
			break;
		case ERR_ANALYZE_TABLE_CREATION:
			errorText = tr("Analyse-Tabelle kann nicht erstellt "
				"werden.");
			break;
		case ERR_ANALYZE_INDEX_CREATION:
			errorText = tr("Analyse-Index kann nicht erstellt "
				"werden.");
			break;
		case ERR_ANALYZE_TABLE_FILL:
			errorText = tr("Analyse-Tabelle kann nicht mit "
				"Inhalten gefuellt werden");
			break;

		default:
			errorText = tr("Ein Fehler ist aufgetreten.");
			break;
	}
	// Append the error number
	errorText.append(tr("\n(Fehlernummer: ")
		+ QString::number(number) + tr(")\n"));
	return errorText;
}
